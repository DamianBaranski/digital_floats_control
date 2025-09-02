#include "adc121c.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

#define ADC121C_REG_RESULT     0x00
#define ADC121C_REG_ALERT      0x01
#define ADC121C_REG_CONFIG     0x02
static int a=0;
Adc121c::Adc121c(II2cMaster* i2c, uint8_t address)
    : mI2c(i2c), mAddress(address) {
    }

bool Adc121c::read(float &value) {
    if(mI2c == nullptr) {
        return false;
    }

    uint16_t buffer;
    bool result = mI2c->readRegister(mAddress, ADC121C_REG_RESULT, reinterpret_cast<uint8_t*>(&buffer), sizeof(buffer));
    if(!result) {
        return false;
    }

    value = calculateCurrent(buffer);
    return true;
}

void Adc121c::setI2c(II2cMaster* i2c){
    mI2c=i2c;
}

void Adc121c::setAddress(uint8_t address) {
    mAddress=address;
}

bool Adc121c::readConfig(uint8_t& config) {
    /*
    volatile bool done = false;
    volatile bool success = false;
    if(mI2c == nullptr) {
        return false;
    }

    if (!mI2c->readRegister(mAddress, ADC121C_REG_CONFIG, &config, sizeof(config), [&](bool ret) {
        success = ret;
        done = true;
    })) {
        return false;
    }

    while (!done) {
    }

    if (!success)
        return false;
*/
    return true;
}

bool Adc121c::connectionTest() const {
    if (mI2c == nullptr) {
        return false;
    }
    // Check if the device is ready by attempting to read the configuration register
    uint8_t config;
    return mI2c->isDeviceReady(mAddress);
}

bool Adc121c::writeConfig(uint8_t config) {
/*
    volatile bool done = false;
    volatile bool success = false;
    if(mI2c == nullptr) {
        return false;
    }

    if (!mI2c->writeRegister(mAddress, ADC121C_REG_CONFIG, &config, sizeof(config), [&](bool ret) {
        success = ret;
        done = true;
    })) {
        return false;
    }

    while (!done) {
    }

    return success;
    */
   return true;
}

float Adc121c::calculateCurrent(uint16_t adc) {
    static constexpr float curentScale = 0.2f; // 200mV/A
    static constexpr float adcScale = 3.3f/4096.0f; // 12-bit ADC scale
    static constexpr float adcToCurrent = adcScale/curentScale; // Convert ADC value to current
    float value = (((adc >> 8) & 0xFF) | ((adc & 0x0F) << 8))*0.004;//adcToCurrent; // Combine high and low byte, then scale
    return value;
}

#pragma GCC pop_options