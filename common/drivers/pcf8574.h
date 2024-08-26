#ifndef PCF8574_H
#define PCF8574_H

#include "ii2c_master.h"

/// @class Pcf8574
/// @brief A driver class for the PCF8574 I/O expander, providing methods to read and write data.
class Pcf8574
{
public:
    /// @brief Constructor for the Pcf8574 class.
    /// @param i2c Reference to an I2C master interface.
    Pcf8574(II2cMaster &i2c);

    void setAddress(uint8_t address);

    /// @brief Tests the connection to the PCF8574 device.
    /// @return True if the device is ready, false otherwise.
    bool connectionTest() const;

    /// @brief Reads data from the PCF8574 device.
    /// @return The data read from the device.
    uint8_t read();

    /// @brief Writes data to the PCF8574 device.
    /// @param data The data to write to the device.
    /// @return True if the write operation was successful, false otherwise.
    bool write(uint8_t data);

private:
    II2cMaster &mI2c; ///< Reference to the I2C master interface.
    uint8_t mAddr;    ///< I2C address of the PCF8574 device.
};

#endif