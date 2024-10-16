#include "ina219.h"
#include "logger.h"

Ina219::Ina219(II2cMaster &i2c) : mI2c(i2c), mAddr(0)
{
}

void Ina219::setAddress(uint8_t address)
{
    mAddr = address;
}

bool Ina219::connectionTest() const
{
    return mI2c.isDeviceReady(mAddr);
}

uint16_t Ina219::readBusVoltage()
{
    int16_t rawData = 0, tmp;
    mI2c.readRegister(mAddr, cBusVoltageRegister, reinterpret_cast<uint8_t *>(&rawData), sizeof(rawData));
    tmp = rawData;
    rawData = (tmp >> 8) & 0xFF;
    rawData |= ((tmp & 0xFF) << 8);
    return (rawData >> 3) * 4;
}

int16_t Ina219::readCurrent() {
    int16_t shuntVoltage = 0;
    int16_t tmp = 0;
    float voltage = 0;
    float current = 0;
    mI2c.readRegister(mAddr, cShuntVoltageRegister, reinterpret_cast<uint8_t *>(&shuntVoltage), sizeof(shuntVoltage));
    tmp = shuntVoltage;
    shuntVoltage = (tmp >> 8) & 0xFF;
    shuntVoltage |= (tmp & 0xFF) << 8;
    voltage = shuntVoltage * 10.0; // 1mV per LSB
    current = voltage / 50.0;       // 50mOhm shunt resistor
    return current;
}
