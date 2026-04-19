#ifndef OTA_SERVER_H
#define OTA_SERVER_H

#include <stdint.h>

#define OTA_SERVER_PORT           9090
#define OTA_MAX_FIRMWARE_SIZE     0xA0000U  /* 640 KB */
#define OTA_FLASH_SECTOR_SIZE     0x1000U   /* 4 KB */
#define OTA_HEADER_SIZE           16
#define OTA_RESP_SIZE             8

#define OTA_STATUS_OK             0x00
#define OTA_STATUS_ERROR          0x01
#define OTA_STATUS_SIZE_MISMATCH  0x02
#define OTA_STATUS_CRC_FAIL       0x03
#define OTA_STATUS_FLASH_ERROR    0x04

void ota_server_init(void);
void ota_server_task(void *pvParameters);

#endif /* OTA_SERVER_H */
