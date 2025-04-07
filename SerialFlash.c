#include <string.h>
#include "BitOps.h"
#include "SerialFlash.h"

// Standard SPI instructions

#define SERIALFLASH_CMD_WRITE_ENABLE 0x06
#define SERIALFLASH_CMD_WRITE_DISABLE 0x04

#define SERIALFLASH_CMD_RELEASE_POW_DOWN 0xAB
#define SERIALFLASH_CMD_MANUF_DEV_ID 0x90
#define SERIALFLASH_CMD_UNIQUE_ID 0x4B

#define SERIALFLASH_CMD_READ_DATA 0x03
#define SERIALFLASH_CMD_FAST_READ 0x0B

#define SERIALFLASH_CMD_PAGE_PROGRAN 0x02

#define SERIALFLASH_CMD_SECTOR_ERASE 0x20
#define SERIALFLASH_CMD_BLOCK32K_ERASE 0x52
#define SERIALFLASH_CMD_BLOCK64K_ERASE 0xD8
#define SERIALFLASH_CMD_CHIP_ERASE 0xC7 // Also 0x60

#define SERIALFLASH_CMD_READ_STATUS1 0x05
#define SERIALFLASH_CMD_WRITE_STATUS1 0x01
#define SERIALFLASH_CMD_READ_STATUS2 0x35
#define SERIALFLASH_CMD_WRITE_STATUS2 0x31
#define SERIALFLASH_CMD_READ_STATUS3 0x15
#define SERIALFLASH_CMD_WRITE_STATUS3 0x11

#define SERIALFLASH_CMD_GLOBAL_BLOCK_LOCK 0x7E
#define SERIALFLASH_CMD_GLOBAL_BLOCK_UNLOCK 0x98
#define SERIALFLASH_CMD_INDIV_BLOCK_LOCK 0x36
#define SERIALFLASH_CMD_INDIV_BLOCK_UNLOCK 0x39

#define SERIALFLASH_CMD_POWER_DOWN 0xB9

#define SERIALFLASH_CMD_ENABLE_RESET 0x66
#define SERIALFLASH_CMD_RESET 0x99

// Dual/Quad SPI instructions

// TODO

bool SerialFlash_SetWriteEnable(const struct SerialFlash_Platform *platform, bool enable) {
    uint8_t cmd[1] = { enable ? SERIALFLASH_CMD_WRITE_ENABLE : SERIALFLASH_CMD_WRITE_DISABLE };

    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_SetPowerDown(const struct SerialFlash_Platform *platform, bool powerDown) {
    uint8_t cmd[1] = { powerDown ? SERIALFLASH_CMD_POWER_DOWN : SERIALFLASH_CMD_RELEASE_POW_DOWN };

    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);
    platform->delayUs(3);

    return !ret;
}

bool SerialFlash_ReadManufDevId(const struct SerialFlash_Platform *platform, uint8_t *manufId, uint8_t *devId) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_MANUF_DEV_ID, 0, 0, 0 };
    uint8_t response[2] = { 0 };

    platform->spiChipSelect(true);
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    platform->spiChipSelect(false);

    *manufId = response[0];
    *devId = response[1];
    return !ret;
}

bool SerialFlash_ReadUniqueId(const struct SerialFlash_Platform *platform, uint8_t *uniqueId64) {
    uint8_t cmd[5] = { SERIALFLASH_CMD_UNIQUE_ID, 0, 0, 0, 0 };

    platform->spiChipSelect(true);
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), uniqueId64, 8);
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_ReadData(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *data, uint32_t length) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_READ_DATA, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };

    platform->spiChipSelect(true);
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), data, length);
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_FastRead(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *data, uint32_t length) {
    uint8_t cmd[5] = { SERIALFLASH_CMD_FAST_READ, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address, 0 };

    platform->spiChipSelect(true);
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), data, length);
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_PageProgram(const struct SerialFlash_Platform *platform, uint32_t address, const uint8_t *data, uint32_t length) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_PAGE_PROGRAN, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };

    platform->spiChipSelect(true);
    int ret = platform->spiWriteWrite(cmd, sizeof(cmd), data, length);
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_SectorErase(const struct SerialFlash_Platform *platform, uint32_t address) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_SECTOR_ERASE, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };

    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_BlockErase(const struct SerialFlash_Platform *platform, uint32_t address, bool block64k) {
    uint8_t cmd[4] = { block64k ? SERIALFLASH_CMD_BLOCK64K_ERASE : SERIALFLASH_CMD_BLOCK32K_ERASE,
         (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };

    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_ChipErase(const struct SerialFlash_Platform *platform) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_CHIP_ERASE };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_ReadStatusRegister1(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister1 *status1) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_READ_STATUS1 };
    uint8_t response[1] = { 0 };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    platform->spiChipSelect(false);

    // Decode the status register
    uint8_t sr1 = response[0];
    status1->srp0 = BITOPS_GET_BIT(sr1, 7);
    status1->sec = BITOPS_GET_BIT(sr1, 6);
    status1->tb = BITOPS_GET_BIT(sr1, 5);
    status1->bp0_2 = BITOPS_GET_BITS(sr1, 2, 3);
    status1->wel = BITOPS_GET_BIT(sr1, 1);
    status1->busy = BITOPS_GET_BIT(sr1, 0);

    return !ret;
}

bool SerialFlash_WriteStatusRegister1(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister1 *status1) {
    // Encode the status register
    uint8_t sr1 = 0;
    BITOPS_SET_BIT(&sr1, 7, status1->srp0);
    BITOPS_SET_BIT(&sr1, 6, status1->sec);
    BITOPS_SET_BIT(&sr1, 5, status1->tb);
    BITOPS_SET_BITS(&sr1, 2, 3, status1->bp0_2);
    BITOPS_SET_BIT(&sr1, 1, status1->wel);
    BITOPS_SET_BIT(&sr1, 0, status1->busy);

    uint8_t cmd[2] = { SERIALFLASH_CMD_WRITE_STATUS1, sr1 };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_ReadStatusRegister2(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister2 *status2) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_READ_STATUS2 };
    uint8_t response[1] = { 0 };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    platform->spiChipSelect(false);

    // Decode the status register
    uint8_t sr2 = response[0];
    status2->sus = BITOPS_GET_BIT(sr2, 7);
    status2->cmp = BITOPS_GET_BIT(sr2, 6);
    status2->lb1_3 = BITOPS_GET_BITS(sr2, 3, 3);
    status2->qu = BITOPS_GET_BIT(sr2, 1);
    status2->srp1 = BITOPS_GET_BIT(sr2, 0);

    return !ret;
}

bool SerialFlash_WriteStatusRegister2(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister2 *status2) {
    // Encode the status register
    uint8_t sr2 = 0;
    BITOPS_SET_BIT(&sr2, 7, status2->sus);
    BITOPS_SET_BIT(&sr2, 6, status2->cmp);
    BITOPS_SET_BITS(&sr2, 3, 3, status2->lb1_3);
    BITOPS_SET_BIT(&sr2, 1, status2->qu);
    BITOPS_SET_BIT(&sr2, 0, status2->srp1);

    uint8_t cmd[2] = { SERIALFLASH_CMD_WRITE_STATUS2, sr2 };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_ReadStatusRegister3(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister3 *status3) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_READ_STATUS3 };
    uint8_t response[1] = { 0 };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    platform->spiChipSelect(false);

    // Decode the status register
    uint8_t sr3 = response[0];
    status3->hrsw = BITOPS_GET_BIT(sr3, 7);
    status3->drv = BITOPS_GET_BITS(sr3, 5, 2);
    status3->hfm = BITOPS_GET_BIT(sr3, 4);
    status3->wps = BITOPS_GET_BIT(sr3, 2);

    return !ret;
}

bool SerialFlash_WriteStatusRegister3(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister3 *status3) {
    // Encode the status register
    uint8_t sr3 = 0;
    BITOPS_SET_BIT(&sr3, 7, status3->hrsw);
    BITOPS_SET_BITS(&sr3, 5, 2, status3->drv);
    BITOPS_SET_BIT(&sr3, 4, status3->hfm);
    BITOPS_SET_BIT(&sr3, 2, status3->wps);

    uint8_t cmd[2] = { SERIALFLASH_CMD_WRITE_STATUS3, sr3 };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_SetGlobalBlockLock(const struct SerialFlash_Platform *platform, bool lock) {
    uint8_t cmd[1] = { lock ? SERIALFLASH_CMD_GLOBAL_BLOCK_LOCK : SERIALFLASH_CMD_GLOBAL_BLOCK_UNLOCK };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_SetBlockLock(const struct SerialFlash_Platform *platform, uint32_t address, bool lock) {
    uint8_t cmd[4] = {
        lock ? SERIALFLASH_CMD_INDIV_BLOCK_LOCK : SERIALFLASH_CMD_INDIV_BLOCK_UNLOCK,
        (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address
    };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->spiChipSelect(false);

    return !ret;
}

bool SerialFlash_Reset(const struct SerialFlash_Platform *platform) {
    uint8_t cmd1[1] = { SERIALFLASH_CMD_ENABLE_RESET };
    
    platform->spiChipSelect(true);
    int ret = platform->spiWrite(cmd1, sizeof(cmd1));
    platform->spiChipSelect(false);
    
    platform->delayUs(10);

    uint8_t cmd2[1] = { SERIALFLASH_CMD_RESET };
    
    platform->spiChipSelect(true);
    ret |= platform->spiWrite(cmd2, sizeof(cmd2));
    platform->spiChipSelect(false);

    platform->delayUs(30);

    return !ret;
}


bool SerialFlash_ReadIds(const struct SerialFlash_Platform *platform, char *manufIdStr16, char *devIdStr16, char *uniqueIdStr24) {
    // Read manufacturer and device ID
    uint8_t manufId, devId;
    if (!SerialFlash_ReadManufDevId(platform, &manufId, &devId)) {
        return false;
    }

    // Built manufacturer ID string
    switch (manufId) {
    case SERIALFLASH_MANUF_ID_ZBIT:
        strcpy(manufIdStr16, "ZBIT");
        break;
    case SERIALFLASH_MANUF_ID_WINBOND:
        strcpy(manufIdStr16, "Winbond");
        break;
    default:
        return false;
    }

    // Built device ID string
    switch (devId) {
    case SERIALFLASH_DEV_ID_Q80:
        strcpy(devIdStr16, "Q80 (1 MB)");
        break;
    case SERIALFLASH_DEV_ID_Q16:
        strcpy(devIdStr16, "Q16 (2 MB)");
        break;
    case SERIALFLASH_DEV_ID_Q32:
        strcpy(devIdStr16, "Q32 (4 MB)");
        break;
    case SERIALFLASH_DEV_ID_Q64:
        strcpy(devIdStr16, "Q64 (8 MB)");
        break;
    case SERIALFLASH_DEV_ID_Q128:
        strcpy(devIdStr16, "Q128 (16 MB)");
        break;
    default:
        return false;
    }

    // Read unique ID
    if (uniqueIdStr24) {
        uint8_t uniqueId[8];
        if (!SerialFlash_ReadUniqueId(platform, uniqueId)) {
            return false;
        }

        // Built unique ID string
        sprintf(uniqueIdStr24, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
            uniqueId[0], uniqueId[1], uniqueId[2], uniqueId[3], uniqueId[4], uniqueId[5], uniqueId[6], uniqueId[7]);
    }

    return true;
}

bool SerialFlash_WaitBusy(const struct SerialFlash_Platform *platform, uint32_t timeout_ms) {
    struct SerialFlash_StatusRegister1 sr1;
    for (uint32_t timeout = 0; timeout < timeout_ms * 2; timeout++) {
        if (!SerialFlash_ReadStatusRegister1(platform, &sr1)) {
            return false;
        }

        if (!sr1.busy) {
            return true;
        }

        platform->delayUs(500);
    }

    return false;
}

bool SerialFlash_Read(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *buffer, uint32_t length, uint32_t timeout_ms) {
    // Wait for the flash to be ready
    if (!SerialFlash_WaitBusy(platform, timeout_ms)) {
        return false;
    }

    // Read data (fast)
    return SerialFlash_FastRead(platform, address, buffer, length);
}

bool SerialFlash_Erase(const struct SerialFlash_Platform *platform, uint32_t address, uint32_t length, uint32_t timeout_ms) {
    // Wait for the flash to be ready
    if (!SerialFlash_WaitBusy(platform, timeout_ms)) {
        return false;
    }

    // Set write enable
    if (!SerialFlash_SetWriteEnable(platform, true)) {
        return false;
    }

    // FIXME: check protection and locks

    bool ok = true;

    if (length < SERIALFLASH_BLOCK_SIZE) {
        // Erase by sectors
        if (address % SERIALFLASH_SECTOR_SIZE != 0 || length % SERIALFLASH_SECTOR_SIZE != 0) {
            ok = false;
        }

        for (uint32_t curAddress = address; curAddress < address + length; curAddress += SERIALFLASH_SECTOR_SIZE) {
            ok &= SerialFlash_SectorErase(platform, curAddress);
            ok &= SerialFlash_WaitBusy(platform, timeout_ms);
        }
    } else {
        // Erase by blocks
        if (address % SERIALFLASH_BLOCK_SIZE != 0 || length % SERIALFLASH_BLOCK_SIZE != 0) {
            ok = false;
        }

        for (uint32_t curAddress = address; curAddress < address + length; curAddress += SERIALFLASH_BLOCK_SIZE) {
            ok &= SerialFlash_BlockErase(platform, curAddress, true);
            ok &= SerialFlash_WaitBusy(platform, timeout_ms);
        }
    }

    // Set write disable
    if (!SerialFlash_SetWriteEnable(platform, false)) {
        return false;
    }

    return ok;
}

bool SerialFlash_Write(const struct SerialFlash_Platform *platform, uint32_t address, const uint8_t *buffer, uint32_t length, uint32_t timeout_ms) {
    // Wait for the flash to be ready
    if (!SerialFlash_WaitBusy(platform, timeout_ms)) {
        return false;
    }

    // Write by pages
    if (address % SERIALFLASH_PAGE_SIZE != 0) {
        return false;
    }

    // Set write enable
    if (!SerialFlash_SetWriteEnable(platform, true)) {
        return false;
    }

    // FIXME: check protection and locks

    bool ok = true;

    const uint8_t *curBuffer = buffer;
    uint32_t curLength = length;
    for (uint32_t curAddress = address; curAddress < address + length; curAddress += SERIALFLASH_PAGE_SIZE) {
        uint32_t curWriteLength = (curLength > SERIALFLASH_PAGE_SIZE) ? SERIALFLASH_PAGE_SIZE : curLength;
        ok &= SerialFlash_PageProgram(platform, curAddress, curBuffer, curWriteLength);
        ok &= SerialFlash_WaitBusy(platform, timeout_ms);

        curBuffer += curWriteLength;
        curLength -= curWriteLength;
    }

    // Set write disable
    if (!SerialFlash_SetWriteEnable(platform, false)) {
        return false;
    }

    return ok;
}
