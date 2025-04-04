#include <string.h>
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
#define SERIALFLASH_CMD_CHIP_ERASE 0x60

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
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_SetPowerDown(const struct SerialFlash_Platform *platform, bool powerDown) {
    uint8_t cmd[1] = { powerDown ? SERIALFLASH_CMD_POWER_DOWN : SERIALFLASH_CMD_RELEASE_POW_DOWN };
    int ret = platform->spiWrite(cmd, sizeof(cmd));
    platform->delayUs(3);
    return ret == 0;
}

bool SerialFlash_ReadManufDevId(const struct SerialFlash_Platform *platform, uint8_t *manufId, uint8_t *devId) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_MANUF_DEV_ID, 0, 0, 0 };
    uint8_t response[2] = { 0 };
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    *manufId = response[0];
    *devId = response[1];
    return ret == 0;
}

bool SerialFlash_ReadUniqueId(const struct SerialFlash_Platform *platform, uint8_t *uniqueId64) {
    uint8_t cmd[5] = { SERIALFLASH_CMD_UNIQUE_ID, 0, 0, 0, 0 };
    return platform->spiWriteRead(cmd, sizeof(cmd), uniqueId64, 8) == 0;
}

bool SerialFlash_ReadData(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *data, uint32_t length) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_READ_DATA, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };
    return platform->spiWriteRead(cmd, sizeof(cmd), data, length) == 0;
}

bool SerialFlash_FastRead(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *data, uint32_t length) {
    uint8_t cmd[5] = { SERIALFLASH_CMD_FAST_READ, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address, 0 };
    return platform->spiWriteRead(cmd, sizeof(cmd), data, length) == 0;
}

bool SerialFlash_PageProgram(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *data, uint32_t length) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_PAGE_PROGRAN, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };
    return platform->spiWriteWrite(cmd, sizeof(cmd), data, length) == 0;
}

bool SerialFlash_SectorErase(const struct SerialFlash_Platform *platform, uint32_t address) {
    uint8_t cmd[4] = { SERIALFLASH_CMD_SECTOR_ERASE, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_BlockErase(const struct SerialFlash_Platform *platform, uint32_t address, bool block64k) {
    uint8_t cmd[4] = { block64k ? SERIALFLASH_CMD_BLOCK64K_ERASE : SERIALFLASH_CMD_BLOCK32K_ERASE,
         (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_ChipErase(const struct SerialFlash_Platform *platform) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_CHIP_ERASE };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_ReadStatusRegister1(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister1 *status1) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_READ_STATUS1 };
    uint8_t response[1] = { 0 };
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    *status1 = *(struct SerialFlash_StatusRegister1 *)(&response[0]);
    return ret == 0;
}

bool SerialFlash_WriteStatusRegister1(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister1 *status1) {
    uint8_t cmd[2] = { SERIALFLASH_CMD_WRITE_STATUS1, *(const uint8_t *)status1 };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_ReadStatusRegister2(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister2 *status2) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_READ_STATUS2 };
    uint8_t response[1] = { 0 };
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    *status2 = *(struct SerialFlash_StatusRegister2 *)(&response[0]);
    return ret == 0;
}

bool SerialFlash_WriteStatusRegister2(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister2 *status2) {
    uint8_t cmd[2] = { SERIALFLASH_CMD_WRITE_STATUS2, *(const uint8_t *)status2 };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_ReadStatusRegister3(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister3 *status3) {
    uint8_t cmd[1] = { SERIALFLASH_CMD_READ_STATUS3 };
    uint8_t response[1] = { 0 };
    int ret = platform->spiWriteRead(cmd, sizeof(cmd), response, sizeof(response));
    *status3 = *(struct SerialFlash_StatusRegister3 *)(&response[0]);
    return ret == 0;
}

bool SerialFlash_WriteStatusRegister3(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister3 *status3) {
    uint8_t cmd[2] = { SERIALFLASH_CMD_WRITE_STATUS3, *(const uint8_t *)status3 };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_SetGlobalBlockLock(const struct SerialFlash_Platform *platform, bool lock) {
    uint8_t cmd[1] = { lock ? SERIALFLASH_CMD_GLOBAL_BLOCK_LOCK : SERIALFLASH_CMD_GLOBAL_BLOCK_UNLOCK };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_SetBlockLock(const struct SerialFlash_Platform *platform, uint32_t address, bool lock) {
    uint8_t cmd[4] = {
        lock ? SERIALFLASH_CMD_INDIV_BLOCK_LOCK : SERIALFLASH_CMD_INDIV_BLOCK_UNLOCK,
        (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address
    };
    return platform->spiWrite(cmd, sizeof(cmd)) == 0;
}

bool SerialFlash_Reset(const struct SerialFlash_Platform *platform) {
    uint8_t cmd1[1] = { SERIALFLASH_CMD_ENABLE_RESET };
    int ret = platform->spiWrite(cmd1, sizeof(cmd1));

    uint8_t cmd2[1] = { SERIALFLASH_CMD_RESET };
    ret |= platform->spiWrite(cmd2, sizeof(cmd2));

    platform->delayUs(30);

    return ret == 0;
}
