#ifndef W25X_FLASH_H
#define W25X_FLASH_H

#include "ispi.h"
#include "igpio.h"
#include "iflash.h"
#include <cstdint>

class W25xFlash: public IFlash {
  public:
    W25xFlash(ISpi &spi, IGpio &csPin);
    void writeEnable(void);
    void writeDisable(void);
    uint8_t readStatus(void);
    void writeStatus(uint8_t status);
    void readData(uint32_t addr, uint8_t *data, uint16_t len);
    void pageProgram(uint32_t addr, const uint8_t *data, uint16_t len);
    void sectorErase(uint32_t addr);
    void chipErase(void);
    void powerDown(void);
    void releasePowerDown(void);
    uint16_t readID(void);
    uint32_t readJEDECID(void);

    bool erase(uint32_t address, size_t no_sectors) override;
    bool write(uint32_t address, const uint8_t* data, size_t size) override;
    bool read(uint32_t address, uint8_t* data, size_t size) override;
    size_t getSectorSize() override;

  private:
    static constexpr uint8_t cWriteEnable = 0x06;
    static constexpr uint8_t cWriteDisable = 0x04;
    static constexpr uint8_t cReadStatusReg = 0x05;
    static constexpr uint8_t cWriteStatusReg = 0x01;
    static constexpr uint8_t cReadData = 0x03;
    static constexpr uint8_t cFastRead = 0x0B;
    static constexpr uint8_t cPageProgram = 0x02;
    static constexpr uint8_t cSectorErase = 0x20;
    static constexpr uint8_t cBlockErase32K = 0x52;
    static constexpr uint8_t cBlockErase64K = 0xD8;
    static constexpr uint8_t cChipErase = 0xC7;
    static constexpr uint8_t cPowerDown = 0xB9;
    static constexpr uint8_t cReleasePowerDown = 0xAB;
    static constexpr uint8_t cReadID = 0x90;
    static constexpr uint8_t cReadJEDECID = 0x9F;

    IGpio &mCsPin;
    ISpi &mSpi;
};

#endif