#ifndef W25X_FLASH_H
#define W25X_FLASH_H

#include "ispi.h"
#include "igpio.h"
#include "iflash.h"
#include <cstdint>

class W25xFlash: public IFlash {
  public:
    W25xFlash(ISpi &spi, IGpio &csPin);
    bool read(uint32_t addr, uint8_t *data, size_t len) override;
    bool sectorErase(uint32_t addr);
    bool chipErase();
    bool powerDown();
    bool releasePowerDown();
    uint16_t readID();
    uint32_t readJEDECID();

    bool erase(uint32_t address, size_t no_sectors) override;
    bool write(uint32_t address, const uint8_t* data, size_t size) override;
    size_t getSectorSize() override;

private:
    bool writeEnable();
    bool writeDisable();
    uint8_t readStatus();
    bool waitForWrite();
    bool writeStatus(uint8_t status);
    template<typename T>
    bool transmit(const T &data);
    template<typename T_tx, typename T_rx>
    bool transmitReceive(const T_tx &tx_data, T_rx &rx_data);
    template<typename T_tx>
    bool transmitReceive(const T_tx &tx_data, uint8_t* rx_data, size_t len);

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

template<typename T>
bool W25xFlash::transmit(const T &data) {
 if(!mSpi.start()) {
        return false;
    }
    mCsPin.reset();
    if(!mSpi.transmit(reinterpret_cast<const uint8_t*>(&data), sizeof(data))){
        return false;
    }
    mCsPin.set();
    if(!mSpi.stop()){
        return false;
    }
    return true;
}

template<typename T_tx, typename T_rx>
bool W25xFlash::transmitReceive(const T_tx &tx_data, T_rx &rx_data) {
 if(!mSpi.start()) {
        return false;
    }
    mCsPin.reset();
    if(!mSpi.transmit(reinterpret_cast<const uint8_t*>(&tx_data), sizeof(tx_data))){
        return false;
    }
    if(!mSpi.receive(reinterpret_cast<uint8_t*>(&rx_data), sizeof(rx_data))){
        return false;
    }
    mCsPin.set();
    if(!mSpi.stop()){
        return false;
    }
    return true;
}

template<typename T_tx>
bool W25xFlash::transmitReceive(const T_tx &tx_data, uint8_t *rx_data, size_t len) {
 if(!mSpi.start()) {
        return false;
    }
    mCsPin.reset();
    if(!mSpi.transmit(reinterpret_cast<const uint8_t*>(&tx_data), sizeof(tx_data))){
        return false;
    }
    if(!mSpi.receive(rx_data, len)){
        return false;
    }
    mCsPin.set();
    if(!mSpi.stop()){
        return false;
    }
    return true;
}
#endif