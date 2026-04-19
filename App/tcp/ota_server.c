#include "ota_server.h"
#include "ota_flash.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <string.h>
#include <stdio.h>
#include "log.h"
#include "gd32h7xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "bsp_lan8720a_rtos.h"

static const uint8_t ota_magic[8] = {'G', 'D', '3', '2', 'O', 'T', 'A', '1'};
static const uint32_t ota_resp_magic = 0xACACACACU;

/* StreamBuffer passes received bytes from LwIP callback to OTA task */
#define OTA_STREAM_BUF_SIZE   8192
#define OTA_STREAM_TRIG_LEVEL 1

static StreamBufferHandle_t g_ota_stream;
static struct tcp_pcb *g_ota_listen_pcb;
static struct tcp_pcb *g_ota_data_pcb;

/* --------- LwIP callbacks (run in tcpip thread, NO blocking) --------- */

static void ota_cleanup_connection(void)
{
    if (g_ota_data_pcb != NULL) {
        tcp_arg(g_ota_data_pcb, NULL);
        tcp_recv(g_ota_data_pcb, NULL);
        tcp_err(g_ota_data_pcb, NULL);
        tcp_abort(g_ota_data_pcb);
        g_ota_data_pcb = NULL;
    }
}

static err_t ota_send_response(struct tcp_pcb *pcb, uint8_t status)
{
    uint8_t resp[OTA_RESP_SIZE];
    uint32_t magic = ota_resp_magic;

    memcpy(&resp[0], &magic, 4);
    resp[4] = status;
    resp[5] = 0;
    resp[6] = 0;
    resp[7] = 0;

    err_t err = tcp_write(pcb, resp, OTA_RESP_SIZE, TCP_WRITE_FLAG_COPY);
    if (err == ERR_OK) {
        tcp_output(pcb);
    } else {
        Log_error("OTA: tcp_write response failed: %d", err);
    }
    return err;
}

static err_t ota_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    (void)arg;

    if (p == NULL) {
        Log_info("OTA: connection closed by client");
        ota_cleanup_connection();
        return ERR_OK;
    }

    if (err != ERR_OK) {
        pbuf_free(p);
        return err;
    }

    tcp_recved(pcb, p->tot_len);

    /* Forward all received bytes into the StreamBuffer (non-blocking in task context) */
    struct pbuf *q;
    for (q = p; q != NULL; q = q->next) {
        xStreamBufferSend(g_ota_stream, q->payload, q->len, 0);
    }

    pbuf_free(p);
    return ERR_OK;
}

static void ota_err(void *arg, err_t err)
{
    (void)arg;
    Log_info("OTA: connection error %d", err);
    g_ota_data_pcb = NULL;
}

static err_t ota_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    (void)arg;

    if (err != ERR_OK || newpcb == NULL) {
        return ERR_VAL;
    }

    /* Only accept if we have no active connection */
    if (g_ota_data_pcb != NULL) {
        return ERR_ABRT;
    }

    /* Reset stream buffer for new session */
    xStreamBufferReset(g_ota_stream);

    g_ota_data_pcb = newpcb;
    tcp_arg(newpcb, NULL);
    tcp_recv(newpcb, ota_recv);
    tcp_err(newpcb, ota_err);

    Log_info("OTA: client connected");
    return ERR_OK;
}

/* --------- OTA task (runs in its own FreeRTOS task context) --------- */

static int ota_flush_sector(uint8_t *buf, uint32_t buf_pos, uint32_t sector_idx)
{
    if (buf_pos == 0) {
        return 0;
    }

    if (ota_flash_erase_sector(sector_idx) != 0) {
        Log_error("OTA: erase sector %lu failed", sector_idx);
        return -1;
    }

    uint32_t write_offset = sector_idx * OTA_FLASH_SECTOR_SIZE;
    if (ota_flash_write(write_offset, buf, buf_pos) != 0) {
        Log_error("OTA: write sector %lu failed", sector_idx);
        return -1;
    }

    return 0;
}

void ota_server_task(void *pvParameters)
{
    (void)pvParameters;

    uint8_t header_buf[OTA_HEADER_SIZE];
    uint32_t header_received = 0;
    uint8_t sector_buf[OTA_FLASH_SECTOR_SIZE];
    uint32_t sector_buf_pos = 0;
    uint32_t current_sector = 0;
    uint32_t received = 0;
    uint32_t total_size = 0;
    uint32_t expected_crc = 0;
    int header_valid = 0;

    /* Initialize network stack (must be in task context, not before scheduler) */
    TCPIP_Init();

    /* Create stream buffer for LwIP callback -> task data transfer */
    g_ota_stream = xStreamBufferCreate(OTA_STREAM_BUF_SIZE, OTA_STREAM_TRIG_LEVEL);
    if (g_ota_stream == NULL) {
        Log_error("OTA: failed to create stream buffer");
        return;
    }

    /* Wait for network to be ready */
    vTaskDelay(pdMS_TO_TICKS(3000));

    /* Start TCP server */
    g_ota_listen_pcb = tcp_new();
    if (g_ota_listen_pcb == NULL) {
        Log_error("OTA: failed to allocate listen PCB");
        return;
    }

    if (tcp_bind(g_ota_listen_pcb, IP_ADDR_ANY, OTA_SERVER_PORT) != ERR_OK) {
        Log_error("OTA: failed to bind port %d", OTA_SERVER_PORT);
        memp_free(MEMP_TCP_PCB, g_ota_listen_pcb);
        g_ota_listen_pcb = NULL;
        return;
    }

    g_ota_listen_pcb = tcp_listen(g_ota_listen_pcb);
    if (g_ota_listen_pcb == NULL) {
        Log_error("OTA: failed to listen");
        return;
    }

    tcp_accept(g_ota_listen_pcb, ota_accept);
    Log_info("OTA: server listening on port %d", OTA_SERVER_PORT);

    /* Main processing loop */
    while (1) {
        /* Phase 1: Accumulate header bytes */
        if (!header_valid) {
            size_t got = xStreamBufferReceive(g_ota_stream,
                                              header_buf + header_received,
                                              OTA_HEADER_SIZE - header_received,
                                              portMAX_DELAY);
            header_received += got;

            if (header_received < OTA_HEADER_SIZE) {
                continue;
            }

            /* Parse header */
            if (memcmp(header_buf, ota_magic, 8) != 0) {
                Log_error("OTA: invalid magic");
                ota_send_response(g_ota_data_pcb, OTA_STATUS_ERROR);
                header_received = 0;
                continue;
            }

            memcpy(&total_size, &header_buf[8], 4);
            memcpy(&expected_crc, &header_buf[12], 4);

            if (total_size == 0 || total_size > OTA_MAX_FIRMWARE_SIZE) {
                Log_error("OTA: invalid size %lu", total_size);
                ota_send_response(g_ota_data_pcb, OTA_STATUS_SIZE_MISMATCH);
                header_received = 0;
                continue;
            }

            header_valid = 1;
            received = 0;
            sector_buf_pos = 0;
            current_sector = 0;

            Log_info("OTA: receiving firmware, size=%lu, CRC=0x%08lX",
                     total_size, expected_crc);
            continue;
        }

        /* Phase 2: Receive firmware data into sector buffer */
        uint32_t space = OTA_FLASH_SECTOR_SIZE - sector_buf_pos;
        size_t got = xStreamBufferReceive(g_ota_stream,
                                          sector_buf + sector_buf_pos,
                                          space,
                                          portMAX_DELAY);
        if (got == 0) {
            continue;
        }

        sector_buf_pos += got;
        received += got;

        /* Flush full sectors to flash */
        while (sector_buf_pos >= OTA_FLASH_SECTOR_SIZE) {
            if (ota_flush_sector(sector_buf, OTA_FLASH_SECTOR_SIZE, current_sector) != 0) {
                ota_send_response(g_ota_data_pcb, OTA_STATUS_FLASH_ERROR);
                header_valid = 0;
                header_received = 0;
                sector_buf_pos = 0;
                break;
            }
            current_sector++;
            sector_buf_pos -= OTA_FLASH_SECTOR_SIZE;
            if (sector_buf_pos > 0) {
                memcpy(sector_buf, sector_buf + OTA_FLASH_SECTOR_SIZE, sector_buf_pos);
            }
        }

        if (!header_valid) {
            continue;
        }

        /* Check if transfer is complete */
        if (received >= total_size) {
            /* Flush last partial sector */
            if (ota_flush_sector(sector_buf, sector_buf_pos, current_sector) != 0) {
                ota_send_response(g_ota_data_pcb, OTA_STATUS_FLASH_ERROR);
                header_valid = 0;
                header_received = 0;
                sector_buf_pos = 0;
                continue;
            }

            /* Verify CRC32 */
            uint32_t crc = ota_flash_crc32(0, total_size);
            if (crc != expected_crc) {
                Log_error("OTA: CRC mismatch (expected 0x%08lX, got 0x%08lX)",
                          expected_crc, crc);
                ota_send_response(g_ota_data_pcb, OTA_STATUS_CRC_FAIL);
                header_valid = 0;
                header_received = 0;
                sector_buf_pos = 0;
                continue;
            }

            Log_info("OTA: firmware received OK, size=%lu, CRC=0x%08lX",
                     total_size, crc);

            /* Send OK response, wait for it to be transmitted, then reboot */
            ota_send_response(g_ota_data_pcb, OTA_STATUS_OK);
            vTaskDelay(pdMS_TO_TICKS(500));
            __disable_irq();
            NVIC_SystemReset();
        }
    }
}
