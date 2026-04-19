#include "ota_flash.h"
#include "gd32h7xx.h"
#include "gd32h7xx_fmc.h"
#include <string.h>

static void fmc_clear_all_flags(void)
{
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_PGSERR);
    fmc_flag_clear(FMC_FLAG_RPERR);
    fmc_flag_clear(FMC_FLAG_RSERR);
    fmc_flag_clear(FMC_FLAG_ECCCOR);
    fmc_flag_clear(FMC_FLAG_ECCDET);
    fmc_flag_clear(FMC_FLAG_OBMERR);
}

static int program_doubleword(uint32_t addr, uint64_t data)
{
    fmc_state_enum state = fmc_doubleword_program(addr, data);
    if (state != FMC_READY) {
        return -1;
    }

    SCB_InvalidateDCache_by_Addr((uint32_t *)addr, 8);

    if (*(__IO uint64_t *)addr != data) {
        return -1;
    }

    return 0;
}

int ota_flash_erase_sector(uint32_t sector_index)
{
    uint32_t addr = SECONDARY_SLOT_ADDR + sector_index * FLASH_SECTOR_SIZE;

    if (addr >= SECONDARY_SLOT_ADDR + SECONDARY_SLOT_SIZE) {
        return -1;
    }

    fmc_unlock();
    fmc_clear_all_flags();

    fmc_state_enum state = fmc_sector_erase(addr);
    if (state != FMC_READY) {
        fmc_clear_all_flags();
        fmc_lock();
        return -1;
    }

    SCB_InvalidateDCache_by_Addr((uint32_t *)addr, FLASH_SECTOR_SIZE);

    if (*(__IO uint32_t *)addr != 0xFFFFFFFFU) {
        fmc_clear_all_flags();
        fmc_lock();
        return -1;
    }

    fmc_clear_all_flags();
    fmc_lock();
    return 0;
}

int ota_flash_write(uint32_t offset, const uint8_t *data, uint32_t len)
{
    uint32_t addr = SECONDARY_SLOT_ADDR + offset;
    const uint8_t *src = data;
    uint32_t remaining = len;
    int rc;

    if (offset + len > SECONDARY_SLOT_SIZE) {
        return -1;
    }

    fmc_unlock();
    fmc_clear_all_flags();

    if (addr & 0x7U) {
        uint32_t aligned = addr & ~0x7U;
        uint32_t head = 8U - (addr & 0x7U);
        uint64_t buf;

        if (head > remaining) head = remaining;

        SCB_InvalidateDCache_by_Addr((uint32_t *)aligned, 8);
        buf = *(__IO uint64_t *)aligned;
        memcpy((uint8_t *)&buf + (addr & 0x7U), src, head);

        rc = program_doubleword(aligned, buf);
        if (rc != 0) {
            fmc_clear_all_flags();
            fmc_lock();
            return rc;
        }

        src += head;
        addr += head;
        remaining -= head;
    }

    while (remaining >= 8U) {
        uint64_t word;
        memcpy(&word, src, 8);

        rc = program_doubleword(addr, word);
        if (rc != 0) {
            fmc_clear_all_flags();
            fmc_lock();
            return rc;
        }

        src += 8;
        addr += 8;
        remaining -= 8;
    }

    if (remaining > 0) {
        uint64_t buf;
        SCB_InvalidateDCache_by_Addr((uint32_t *)addr, 8);
        buf = *(__IO uint64_t *)addr;
        memcpy(&buf, src, remaining);

        rc = program_doubleword(addr, buf);
        if (rc != 0) {
            fmc_clear_all_flags();
            fmc_lock();
            return rc;
        }
    }

    fmc_clear_all_flags();
    fmc_lock();
    return 0;
}

uint32_t ota_flash_crc32(uint32_t offset, uint32_t len)
{
    uint32_t addr = SECONDARY_SLOT_ADDR + offset;

    /* Invalidate D-Cache over the entire region so we read actual flash content */
    SCB_InvalidateDCache_by_Addr((uint32_t *)addr, len);

    const uint8_t *ptr = (const uint8_t *)addr;
    uint32_t crc = 0xFFFFFFFFU;

    for (uint32_t i = 0; i < len; i++) {
        crc ^= ptr[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320U;
            else
                crc = crc >> 1;
        }
    }

    return crc ^ 0xFFFFFFFFU;
}
