#ifndef SERIALFLASH_H
#define SERIALFLASH_H

#include <stdbool.h>
#include <stdio.h>

#define SERIALFLASH_PAGE_SIZE 256
#define SERIALFLASH_SECTOR_SIZE (4 * 1024)
#define SERIALFLASH_BLOCK_SIZE (64 * 1024)

#define SERIALFLASH_MANUF_ID_ZBIT 0x50
#define SERIALFLASH_MANUF_ID_WINBOND 0xEF // Winbond Serial Flash

#define SERIALFLASH_DEV_ID_Q80 0x13
#define SERIALFLASH_DEV_ID_Q16 0x14
#define SERIALFLASH_DEV_ID_Q32 0x15
#define SERIALFLASH_DEV_ID_Q64 0x16
#define SERIALFLASH_DEV_ID_Q128 0x17

#define SERIALFLASH_CLOCK_FREQ_MAX_MHZ 50

#define SERIALFLASH_PAGE_PROGRAM_TIME_MS_MAX 3
#define SERIALFLASH_SECTOR_ERASE_TIME_MS_MAX 400
#define SERIALFLASH_BLOCK32K_ERASE_TIME_MS_MAX 1600
#define SERIALFLASH_BLOCK64K_ERASE_TIME_MS_MAX 2000
#define SERIALFLASH_CHIP_ERASE_TIME_MS_MAX (50 * 1000)

struct SerialFlash_Platform {
    // SPI Mode 0 and Mode 3 are supported
    // MSB first

    int (*spiWrite)(const uint8_t *data, uint32_t length);
    int (*spiRead)(uint8_t *data, uint32_t length);

    int (*spiWriteWrite)(const uint8_t *data1, uint32_t length1, const uint8_t *data2, uint32_t length2);
    int (*spiWriteRead)(const uint8_t *data1, uint32_t length1, uint8_t *data2, uint32_t length2);

    void (*spiChipSelect)(bool select);

    // TODO: add nHOLD, nWP, nRESET support

    void (*delayUs)(int us);
};

enum SerialFlash_StatusRegisterProtect0 {
    SERIALFLASH_SRP0_NWP_NO_EFFECT = 0,
    SERIALFLASH_SRP0_NWP_WRITE_PROTECT = 1
};

enum SerialFlash_SectorBlockProtect {
    SERIALFLASH_SEC_64K_BLOCKS = 0,
    SERIALFLASH_SEC_4K_SECTORS = 1
};

enum SerialFlash_TopBottomProtect {
    SERIALFLASH_TB_TOP_DOWN = 0,
    SERIALFLASH_TB_BOTTOM_UP = 1
};

enum SerialFlash_ComplementProtect {
    SERIALFLASH_CMP_NORMAL = 0,
    SERIALFLASH_CMP_COMPLEMENTARY = 1
};

enum SerialFlash_StatusRegisterProtect1 {
    SERIALFLASH_SRP1_NWP_STATUS = 0,
    SERIALFLASH_SRP1_POWER_LOCK_DOWN = 1
};

enum SerialFlash_HoldOrReset {
    SERIALFLASH_HRSW_NHOLD = 0,
    SERIALFLASH_HRSW_NRESET = 1
};

enum SerialFlash_OutputDriveStrength {
    SERIALFLASH_DRV_100 = 0,
    SERIALFLASH_DRV_75 = 1,
    SERIALFLASH_DRV_50 = 2,
    SERIALFLASH_DRV_25 = 3
};

enum SerialFlash_WriteProtectSelection {
    SERIALFLASH_WPS_CMP_SEC_TB_BP02 = 0,
    SERIALFLASH_WPS_INDIV_BLOCKS = 1
};

// Status Register-1 (SR1)
struct SerialFlash_StatusRegister1 {
    int srp0 : 1; // Status Register Protect 0
    int sec : 1; // Sector/Block Protect
    int tb : 1; // Top/Bottom Protect
    int bp0_2 : 3; // Block Protect Bits
    int wel : 1; // Write Enable Latch, Read only
    int busy : 1; // Embedded Operation Status, Read only
};

// Status Register-2 (SR2)
struct SerialFlash_StatusRegister2 {
    int sus : 1; // Suspend Status, Read only
    int cmp : 1; // Complement Protect
    int lb1_3 : 3; // Security Register Lock Bits, OTP
    
    // Note: If the WP# or HOLD# / RESET# pins are tied directly to the power supply or ground during standard SPI or Dual SPI operation, the
    // QE bit should never be set to a 1.
    int qu : 1; // Quad Enable 
    
    int srp1 : 1; // Status Register Protect 1
};

// Status Register-3 (SR3)
struct SerialFlash_StatusRegister3 {
    int hrsw : 1; // Hold/Reset Status
    int drv : 2; // Output Drive Strength
    int hfm : 1; // High Frequency Mode Enable Bit
    int wps : 1; // Write Protect Selection
};

// TODO: SFDP support

// Low level API

bool SerialFlash_SetWriteEnable(const struct SerialFlash_Platform *platform, bool enable);

bool SerialFlash_SetPowerDown(const struct SerialFlash_Platform *platform, bool powerDown);
bool SerialFlash_ReadManufDevId(const struct SerialFlash_Platform *platform, uint8_t *manufId, uint8_t *devId);
bool SerialFlash_ReadUniqueId(const struct SerialFlash_Platform *platform, uint8_t *uniqueId64);

bool SerialFlash_ReadData(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *data, uint32_t length);
bool SerialFlash_FastRead(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *data, uint32_t length);

bool SerialFlash_PageProgram(const struct SerialFlash_Platform *platform, uint32_t address, const uint8_t *data, uint32_t length);

bool SerialFlash_SectorErase(const struct SerialFlash_Platform *platform, uint32_t address);
bool SerialFlash_BlockErase(const struct SerialFlash_Platform *platform, uint32_t address, bool block64k);
bool SerialFlash_ChipErase(const struct SerialFlash_Platform *platform);

bool SerialFlash_ReadStatusRegister1(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister1 *status1);
bool SerialFlash_WriteStatusRegister1(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister1 *status1);
bool SerialFlash_ReadStatusRegister2(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister2 *status2);
bool SerialFlash_WriteStatusRegister2(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister2 *status2);
bool SerialFlash_ReadStatusRegister3(const struct SerialFlash_Platform *platform, struct SerialFlash_StatusRegister3 *status3);
bool SerialFlash_WriteStatusRegister3(const struct SerialFlash_Platform *platform, const struct SerialFlash_StatusRegister3 *status3);

bool SerialFlash_SetGlobalBlockLock(const struct SerialFlash_Platform *platform, bool lock);
bool SerialFlash_SetBlockLock(const struct SerialFlash_Platform *platform, uint32_t address, bool lock);

bool SerialFlash_Reset(const struct SerialFlash_Platform *platform);

// Hight level API

bool SerialFlash_ReadIds(const struct SerialFlash_Platform *platform, char *manufIdStr16, char *devIdStr16, char *uniqueIdStr24);
bool SerialFlash_WaitBusy(const struct SerialFlash_Platform *platform, uint32_t timeout_ms);
bool SerialFlash_Read(const struct SerialFlash_Platform *platform, uint32_t address, uint8_t *buffer, uint32_t length, uint32_t timeout_ms);
bool SerialFlash_Erase(const struct SerialFlash_Platform *platform, uint32_t address, uint32_t length, uint32_t timeout_ms);
bool SerialFlash_Write(const struct SerialFlash_Platform *platform, uint32_t address, const uint8_t *buffer, uint32_t length, uint32_t timeout_ms);

#endif // SERIALFLASH_H
