#include "w25x_flash.h"
#include <cstring>

W25xFlash::W25xFlash(ISpi &spi, IGpio &csPin)
    : mCsPin(csPin), mSpi(spi) {
}

bool W25xFlash::writeEnable() {
    uint8_t data = cWriteEnable;

    return transmit(data);
}

bool W25xFlash::writeDisable() {
    uint8_t data[] = {cWriteDisable};
    return transmit(data);
}

uint8_t W25xFlash::readStatus() {
    uint8_t status = 0;
    uint8_t data = cReadStatusReg;
    transmitReceive(data, status);
    return status;
}

bool W25xFlash::writeStatus(uint8_t status) {
    uint8_t data[] = {cWriteStatusReg, status};
    return transmit(data);
}

bool W25xFlash::waitForWrite() {
    for (size_t i = 0; i < 10000; i++) {
        uint8_t status = readStatus();
        if (!(status & 0x01)) {
            return true;
        }
    }
    return false;
}

bool W25xFlash::read(uint32_t addr, uint8_t *data, size_t len) {
    uint8_t cmd[] = {
        cReadData,
        static_cast<uint8_t>(addr >> 16), // Address MSB
        static_cast<uint8_t>(addr >> 8),  // Address
        static_cast<uint8_t>(addr)        // Address LSB
    };
    memset(data, 0xFF, len);

    return transmitReceive(cmd, data, len);
}

bool W25xFlash::write(uint32_t addr, const uint8_t *data, size_t len) {
    uint8_t cmd[] = {
        cPageProgram,
        static_cast<uint8_t>(addr >> 16), // Address MSB
        static_cast<uint8_t>(addr >> 8),  // Address
        static_cast<uint8_t>(addr)        // Address LSB
    };

    if (!writeEnable()) {
        return false;
    }

    mSpi.start();
    mCsPin.reset();
    mSpi.transmit(cmd, sizeof(cmd)); // Send Page Program command followed by address
    mSpi.transmit(data, len);        // Transmit data to be programmed
    mCsPin.set();
    mSpi.stop();

    return waitForWrite();
}

bool W25xFlash::sectorErase(uint32_t addr) {
    uint8_t cmd[] = {
        cSectorErase,
        static_cast<uint8_t>(addr >> 16), // Address MSB
        static_cast<uint8_t>(addr >> 8),  // Address
        static_cast<uint8_t>(addr)        // Address LSB
    };
    if (!writeEnable()) {
        return false;
    }

    if (!transmit(cmd)) {
        return false;
    }

    return waitForWrite();
}

bool W25xFlash::chipErase() {
    uint8_t data[] = {cChipErase};
    if (!writeEnable()) {
        return false;
    }
    if (!transmit(data)) {
        return false;
    }
    return waitForWrite();
}

bool W25xFlash::powerDown() {
    uint8_t data[] = {cPowerDown};
    return transmit(data);
}

bool W25xFlash::releasePowerDown() {
    uint8_t data[] = {cReleasePowerDown};
    return transmit(data);
}

uint16_t W25xFlash::readID() {
    uint16_t id = 0;
    uint8_t data[] = {cReadID, 0x00, 0x00, 0x00}; // Command followed by 3 dummy bytes
    transmitReceive(data, id);
    return id;
}

uint32_t W25xFlash::readJEDECID() {
    uint32_t id = 0;
    uint8_t data[] = {cReadJEDECID};
    transmitReceive(data, id);
    return id;
}

bool W25xFlash::erase(uint32_t address, size_t no_sectors) {
    sectorErase(address);
    return true;
}

size_t W25xFlash::getSectorSize() {
    return 4092;
}
