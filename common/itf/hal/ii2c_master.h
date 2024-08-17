#ifndef II2CMASTER_H
#define II2CMASTER_H

#include <cstdint>

/// @class II2cMaster
/// @brief Interface class for I2C Master operations.
///
/// This class provides an abstract interface for I2C master communication, 
/// defining the essential methods required for I2C data transmission and reception.
class II2cMaster {
public:
    /// @brief Writes data to an I2C device.
    ///
    /// This method sends a sequence of bytes to the specified I2C address.
    ///
    /// @param addr The 7-bit I2C address of the device.
    /// @param data Pointer to the data buffer to be sent.
    /// @param len The number of bytes to send.
    /// @return True if the data was successfully transmitted, false otherwise.
    virtual bool write(uint8_t addr, const uint8_t *data, uint8_t len) = 0;

    /// @brief Reads data from an I2C device.
    ///
    /// This method receives a sequence of bytes from the specified I2C address.
    ///
    /// @param addr The 7-bit I2C address of the device.
    /// @param data Pointer to the buffer where the received data will be stored.
    /// @param len The number of bytes to read.
    /// @return True if the data was successfully received, false otherwise.
    virtual bool read(uint8_t addr, uint8_t *data, uint8_t len) = 0;

    /// @brief Writes data to a specific register of an I2C device.
    ///
    /// This method sends a register address followed by a sequence of bytes to the specified I2C address.
    ///
    /// @param addr The 7-bit I2C address of the device.
    /// @param reg The register address to write to.
    /// @param data Pointer to the data buffer to be sent.
    /// @param len The number of bytes to send.
    /// @return True if the data was successfully transmitted, false otherwise.
    virtual bool writeRegister(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t len) = 0;

    /// @brief Reads data from a specific register of an I2C device.
    ///
    /// This method sends a register address and then reads a sequence of bytes from the specified I2C address.
    ///
    /// @param addr The 7-bit I2C address of the device.
    /// @param reg The register address to read from.
    /// @param data Pointer to the buffer where the received data will be stored.
    /// @param len The number of bytes to read.
    /// @return True if the data was successfully received, false otherwise.
    virtual bool readRegister(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) = 0;

    /// @brief Checks if an I2C device is ready for communication.
    ///
    /// This method checks whether the device with the specified address is responding.
    ///
    /// @param addr The 7-bit I2C address of the device.
    /// @return True if the device is ready, false otherwise.
    virtual bool isDeviceReady(uint8_t addr) const = 0;
};

#endif // II2CMASTER_H
