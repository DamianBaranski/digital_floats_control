#ifndef II2CMASTER_H
#define II2CMASTER_H
#include <cstdint>

/// @class II2cMaster
/// @brief Interface class for I2C Master operations.
///
/// This class provides an abstract interface for I2C master communication.
class II2cMaster {
public:
    /// @brief Transmits data via I2C.
    ///
    /// This pure virtual method must be implemented by derived classes to handle
    /// the transmission of data over the I2C bus.
    ///
    /// @return True if the transmission was successful, false otherwise.
    virtual bool transmit() = 0;
    virtual bool write(uint8_t addr, const uint8_t *data, uint8_t len) = 0;
    virtual bool read(uint8_t addr, uint8_t *data, uint8_t len) = 0;
    virtual bool writeRegister(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t len) = 0;
    virtual bool readRegister(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) = 0;
    virtual bool isDeviceReady(uint8_t addr) const = 0;
};

#endif // II2CMASTER_H
