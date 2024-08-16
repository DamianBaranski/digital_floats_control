#ifndef INA219_H
#define INA219_H

#include "ii2c_master.h"

/// @class Ina219
/// @brief A driver class for the INA219 sensor, which provides methods to read bus voltage and current.
class Ina219
{
public:
    /// @brief Constructor for the Ina219 class.
    /// @param i2c Reference to an I2C master interface.
    /// @param addr The I2C address of the INA219 device.
    Ina219(II2cMaster &i2c, uint8_t addr);

    /// @brief Tests the connection to the INA219 device.
    /// @return True if the device is ready, false otherwise.
    bool connectionTest() const;

    /// @brief Reads the bus voltage from the INA219 sensor.
    /// @return The bus voltage in millivolts.
    uint16_t readBusVoltage();

    /// @brief Reads the current flowing through the shunt resistor.
    /// @return The current in milliamps.
    int16_t readCurrnet();

private:
    static constexpr uint8_t cShuntVoltageRegister = 0x01; ///< Register address for the shunt voltage.
    static constexpr uint8_t cBusVoltageRegister = 0x02;   ///< Register address for the bus voltage.

    II2cMaster &mI2c; ///< Reference to the I2C master interface.
    uint8_t mAddr;    ///< I2C address of the INA219 device.
};

#endif