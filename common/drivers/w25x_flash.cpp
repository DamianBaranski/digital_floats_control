#include "w25x_flash.h"

W25xFlash::W25xFlash(ISpi &spi, IGpio &csPin)
    : mCsPin(csPin), mSpi(spi) {
        mCsPin.set();
    }

void W25xFlash::writeEnable(void) {
    mCsPin.reset();                                      // Assert CS
    mSpi.transmit(&cWriteEnable, sizeof(cWriteEnable));  // Send Write Enable command
    mCsPin.set();                                        // Deassert CS
}

void W25xFlash::writeDisable(void) {
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(&cWriteDisable, sizeof(cWriteDisable)); // Send Write Disable command
    mCsPin.set();                                         // Deassert CS
}

uint8_t W25xFlash::readStatus(void) {
    uint8_t status = 0;
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(&cReadStatusReg, sizeof(cReadStatusReg)); // Send Read Status Register command
    mSpi.receive(&status, sizeof(status));                // Receive the status register
    mCsPin.set();                                         // Deassert CS
    return status;
}

void W25xFlash::writeStatus(uint8_t status) {
    uint8_t data[] = {cWriteStatusReg, status};
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(data, sizeof(data));                    // Send Write Status Register command followed by status value
    mCsPin.set();                                         // Deassert CS
}

void W25xFlash::readData(uint32_t addr, uint8_t *data, uint16_t len) {
    uint8_t cmd[] = {
        cReadData,
        static_cast<uint8_t>(addr >> 16),  // Address MSB
        static_cast<uint8_t>(addr >> 8),   // Address
        static_cast<uint8_t>(addr)         // Address LSB
    };
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(cmd, sizeof(cmd));                      // Send Read Data command followed by address
    mSpi.receive(data, len);                              // Receive data into buffer
    mCsPin.set();                                         // Deassert CS
}

void W25xFlash::pageProgram(uint32_t addr, const uint8_t *data, uint16_t len) {
    uint8_t cmd[] = {
        cPageProgram,
        static_cast<uint8_t>(addr >> 16),  // Address MSB
        static_cast<uint8_t>(addr >> 8),   // Address
        static_cast<uint8_t>(addr)         // Address LSB
    };
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(cmd, sizeof(cmd));                      // Send Page Program command followed by address
    mSpi.transmit(data, len);                             // Transmit data to be programmed
    mCsPin.set();                                         // Deassert CS
}

void W25xFlash::sectorErase(uint32_t addr) {
    uint8_t cmd[] = {
        cSectorErase,
        static_cast<uint8_t>(addr >> 16),  // Address MSB
        static_cast<uint8_t>(addr >> 8),   // Address
        static_cast<uint8_t>(addr)         // Address LSB
    };
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(cmd, sizeof(cmd));                      // Send Sector Erase command followed by address
    mCsPin.set();                                         // Deassert CS
}

void W25xFlash::chipErase(void) {
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(&cChipErase, sizeof(cChipErase));       // Send Chip Erase command
    mCsPin.set();                                         // Deassert CS
}

void W25xFlash::powerDown(void) {
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(&cPowerDown, sizeof(cPowerDown));       // Send Power Down command
    mCsPin.set();                                         // Deassert CS
}

void W25xFlash::releasePowerDown(void) {
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(&cReleasePowerDown, sizeof(cReleasePowerDown)); // Send Release Power Down command
    mCsPin.set();                                         // Deassert CS
}

uint16_t W25xFlash::readID(void) {
    uint16_t id = 0;
    uint8_t data[] = {cReadID, 0x00, 0x00, 0x00};         // Command followed by 3 dummy bytes
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(data, sizeof(data));                    // Send the Read ID command
    mSpi.receive(reinterpret_cast<uint8_t*>(&id), sizeof(id));  // Receive the 16-bit ID
    mCsPin.set();                                         // Deassert CS
    return id;
}

uint32_t W25xFlash::readJEDECID(void) {
    uint32_t id = 0;
    mCsPin.reset();                                       // Assert CS
    mSpi.transmit(&cReadJEDECID, sizeof(cReadJEDECID));   // Send Read JEDEC ID command
    mSpi.receive(reinterpret_cast<uint8_t*>(&id), 3);     // Receive 3-byte JEDEC ID
    mCsPin.set();                                         // Deassert CS
    return id;
}

    bool W25xFlash::erase(uint32_t address, size_t no_sectors) {
        sectorErase(address);
        return true;
    }

    bool W25xFlash::write(uint32_t address, const uint8_t* data, size_t size) {
        pageProgram(address, data, size);
        return true;
    }

    bool W25xFlash::read(uint32_t address, uint8_t* data, size_t size) {
        readData(address, data, size);
        return true;
    }  
    
    size_t W25xFlash::getSectorSize() {
        return 1024;
    }
