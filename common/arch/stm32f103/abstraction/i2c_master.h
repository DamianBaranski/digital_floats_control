#ifndef I2CMASTER_H
#define I2CMASTER_H

#include "ii2c_master.h"
#include "stm32f1xx_hal.h"

/// @brief I2C Master class for handling I2C communication using STM32 HAL.
/// 
/// This class provides an implementation of the II2cMaster interface, using
/// the STM32 HAL library to perform I2C transactions such as device readiness checks,
/// data transmission, and register read/write operations.
class I2cMaster : public II2cMaster {
public:
    /// @brief Constructs an I2cMaster object.
    /// 
    /// Initializes the I2C peripheral with the specified instance.
    /// 
    /// @param instance A pointer to the I2C peripheral instance (e.g., I2C1, I2C2).
    I2cMaster(I2C_TypeDef *instance);

    /// @brief Destroys the I2cMaster object.
    /// 
    /// Deinitializes the I2C peripheral and releases any resources.
    ~I2cMaster();

    /// @brief Checks if an I2C device is ready for communication.
    /// 
    /// This method checks whether the device with the specified address is responding.
    /// 
    /// @param addr The 7-bit I2C address of the device.
    /// @return True if the device is ready, false otherwise.
    virtual bool isDeviceReady(uint8_t addr) const override;

    /// @brief Writes data to an I2C device.
    /// 
    /// Sends a sequence of bytes to the specified I2C address.
    /// 
    /// @param addr The 7-bit I2C address of the device.
    /// @param data Pointer to the data buffer to be sent.
    /// @param len The number of bytes to send.
    /// @return True if the data was successfully transmitted, false otherwise.
    virtual bool write(uint8_t addr, const uint8_t *data, uint8_t len) override;

    /// @brief Reads data from an I2C device.
    /// 
    /// Receives a sequence of bytes from the specified I2C address.
    /// 
    /// @param addr The 7-bit I2C address of the device.
    /// @param data Pointer to the buffer where the received data will be stored.
    /// @param len The number of bytes to read.
    /// @return True if the data was successfully received, false otherwise.
    virtual bool read(uint8_t addr, uint8_t *data, uint8_t len) override;

    /// @brief Writes data to a specific register of an I2C device.
    /// 
    /// Sends a register address followed by a sequence of bytes to the specified I2C address.
    /// 
    /// @param addr The 7-bit I2C address of the device.
    /// @param reg The register address to write to.
    /// @param data Pointer to the data buffer to be sent.
    /// @param len The number of bytes to send.
    /// @return True if the data was successfully transmitted, false otherwise.
    virtual bool writeRegister(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t len) override;

    /// @brief Reads data from a specific register of an I2C device.
    /// 
    /// Sends a register address and then reads a sequence of bytes from the specified I2C address.
    /// 
    /// @param addr The 7-bit I2C address of the device.
    /// @param reg The register address to read from.
    /// @param data Pointer to the buffer where the received data will be stored.
    /// @param len The number of bytes to read.
    /// @return True if the data was successfully received, false otherwise.
    virtual bool readRegister(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) override;

private:
    /// @brief I2C handler structure used by the HAL library.
    /// 
    /// This structure holds the configuration and status information for the I2C peripheral.
    mutable I2C_HandleTypeDef mI2cHandler;
};

#endif
