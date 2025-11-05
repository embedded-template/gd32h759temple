/*!
    \file    tcp_client.c
    \brief   TCP client demo program

    \version 2023-03-31, V1.0.0, firmware for GD32H7xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "tcp_client.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <string.h>
#include <stdio.h>
#include "gd32h7xx.h"
#include "main.h"
#include "log.h"
#include "bsp_lan8720a_rtos.h"


#define MAX_BUF_SIZE    1000

struct recev_packet {
    int length;
    char bytes[MAX_BUF_SIZE];
};

/* Connection state tracking */
static volatile uint8_t g_tcp_connected = 0;
static uint32_t g_send_counter = 0;

static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static void tcp_client_err(void *arg, err_t err);

/*!
    \brief      called when a data is received on the tcp connection
    \param[in]  arg: the user argument
    \param[in]  pcb: the tcp_pcb that has received the data
    \param[in]  p: the packet buffer
    \param[in]  err: the error value linked with the received data
    \param[out] none
    \retval     err_t: error value
*/
static err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    struct recev_packet *recev_packet = (struct recev_packet *)arg;
    int buf_full;
    char *c;
    int i;

    /* we perform here any necessary processing on the pbuf */
    if(p != NULL) {
        /* we call this function to tell the LwIp that we have processed the data */
        /* this lets the stack advertise a larger window, so more data can be received*/
        tcp_recved(pcb, p->tot_len);

        /* check the name if NULL, no data passed, return withh illegal argument error */
        if(!recev_packet) {
            pbuf_free(p);
            return ERR_ARG;
        }

        buf_full = 0;

        for(q = p; q != NULL; q = q->next) {

            c = q->payload;
            for(i = 0; i < q->len && !buf_full; i++) {
                /* if the received data size is larger than the size we want to get */
                if(recev_packet->length < MAX_BUF_SIZE) {
                    recev_packet->bytes[recev_packet->length] = c[i];
                    recev_packet->length++;
                } else {
                    buf_full = 1;
                }
            }
        }

        /* send out the message */
        tcp_write(pcb, recev_packet->bytes, recev_packet->length, 1);

        /* null-terminate the received data for logging */
        if(recev_packet->length < MAX_BUF_SIZE) {
            recev_packet->bytes[recev_packet->length] = '\0';
        } else {
            recev_packet->bytes[MAX_BUF_SIZE - 1] = '\0';
        }

        /* log the received data */
        Log_info("TCP Received: %s", recev_packet->bytes);

        recev_packet->length = 0;

        pbuf_free(p);

    } else if(ERR_OK == err) {
        /* connection closed by remote host */
        Log_info("TCP connection closed by server");
        g_tcp_connected = 0;
        mem_free(recev_packet);
        return tcp_close(pcb);
    }

    return ERR_OK;
}

/*!
    \brief      this function when the connection is established
    \param[in]  arg: user supplied argument
    \param[in]  pcb: the tcp_pcb which accepted the connection
    \param[in]  err: error value
    \param[out] none
    \retval     err_t: error value
*/

struct tcp_pcb *g_pcb;

/*!
    \brief      error callback function
    \param[in]  arg: user supplied argument
    \param[in]  err: error value
    \param[out] none
    \retval     none
*/
static void tcp_client_err(void *arg, err_t err)
{
    Log_info("TCP connection error: %d", err);
    g_tcp_connected = 0;
    g_pcb = NULL;
    if(arg != NULL) {
        mem_free(arg);
    }
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    if(err == ERR_OK) {
        g_pcb = pcb;
        g_tcp_connected = 1;
        Log_info("TCP connected to server successfully");

        tcp_arg(pcb, mem_calloc(sizeof(struct recev_packet), 1));
        /* configure LwIP to use our call back functions */
        tcp_recv(pcb, tcp_client_recv);
        tcp_err(pcb, tcp_client_err);
    } else {
        Log_info("TCP connection failed: %d", err);
        g_tcp_connected = 0;
    }

    return ERR_OK;
}

/*!
    \brief      initialize the tcp_client application
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tcp_client_init(void)
{
    ip_addr_t ipaddr;
    IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);

    /* create a new TCP control block  */
    g_pcb = tcp_new();

    if(g_pcb != NULL) {
        err_t err;

        /* set error callback before connecting */
        tcp_err(g_pcb, tcp_client_err);

        err = tcp_connect(g_pcb, &ipaddr, IP_S_PORT, tcp_client_connected);
        Log_info("TCP connecting to %d.%d.%d.%d:%d, result: %d",
                 IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3, IP_S_PORT, err);

        if(err != ERR_OK) {
            Log_info("TCP connect failed immediately: %d", err);
            memp_free(MEMP_TCP_PCB, g_pcb);
            g_pcb = NULL;
        }
    } else {
        Log_info("TCP pcb allocation failed");
    }
}

void tcp_client_task(void* pvParameters)
{
    char send_buf[64];
    err_t err;
    uint32_t period_ms = *(uint32_t*)pvParameters;

    TCPIP_Init();

    /* Wait for network to be ready */
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* Initialize TCP client connection */
    tcp_client_init();

    while(1)
    {
        /* Check if connected and send test data */
        if(g_tcp_connected && g_pcb != NULL) {
            /* Prepare test data */
            snprintf(send_buf, sizeof(send_buf), "Test message #%u\n", g_send_counter++);

            /* Send data to server */
            err = tcp_write(g_pcb, send_buf, strlen(send_buf), TCP_WRITE_FLAG_COPY);
            if(err == ERR_OK) {
                /* Trigger sending */
                tcp_output(g_pcb);
                Log_info("TCP sent: %s", send_buf);
            } else {
                Log_info("TCP send failed: %d", err);
            }
        } else if(!g_tcp_connected && g_pcb == NULL) {
            /* Try to reconnect if disconnected */
            Log_info("TCP not connected, attempting to reconnect...");
            tcp_client_init();
        }

        /* Wait for next period */
        vTaskDelay(pdMS_TO_TICKS(period_ms));
    }
}
