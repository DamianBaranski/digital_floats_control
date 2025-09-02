#ifndef ADC121C_H
#define ADC121C_H

#include "ii2c_master.h"
#include <cstdint>
#include <functional>

/// @brief Driver for the ADC121C I2C 12-bit ADC.
class Adc121c {
public:
    /// @brief Constructs the ADC121C object.
    /// @param i2c I2C master used to communicate with the ADC.
    /// @param address 7-bit I2C address of the ADC (default is 0x50).
    Adc121c(II2cMaster* i2c=nullptr, uint8_t address = 0x50);

    void setI2c(II2cMaster* i2c);

    void setAddress(uint8_t address);

    /// @brief Reads a 12-bit ADC value from the device.
    /// @param value Output variable to store the ADC value (0-4095).
    /// @return True if the read was successful, false otherwise.
    //void read(std::function<void(bool, float)> ret);
    bool read(float &value);

    /// @brief Reads the configuration register.
    bool readConfig(uint8_t& config);

    /// @brief Writes to the configuration register.
    bool writeConfig(uint8_t config);

    bool connectionTest() const;

private:
    float calculateCurrent(uint16_t adc);
    II2cMaster* mI2c;
    uint8_t mAddress;
};

#endif