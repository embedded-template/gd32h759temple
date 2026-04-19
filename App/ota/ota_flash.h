#ifndef OTA_FLASH_H
#define OTA_FLASH_H

#include <stdint.h>

#define SECONDARY_SLOT_ADDR   0x080C0000U
#define SECONDARY_SLOT_SIZE   0x000A0000U  /* 640 KB */
#define FLASH_SECTOR_SIZE     0x00001000U  /* 4 KB */
#define FLASH_WRITE_ALIGN     8U

int ota_flash_erase_sector(uint32_t sector_index);
int ota_flash_write(uint32_t offset, const uint8_t *data, uint32_t len);
uint32_t ota_flash_crc32(uint32_t offset, uint32_t len);

#endif /* OTA_FLASH_H */
