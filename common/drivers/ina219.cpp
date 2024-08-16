#include "ina219.h"

Ina219::Ina219(II2cMaster &i2c, uint8_t addr) : mI2c(i2c), mAddr(addr)
{
}

bool Ina219::connectionTest() const
{
    return mI2c.isDeviceReady(mAddr);
}

uint16_t Ina219::readBusVoltage()
{
    int16_t rawData = 0, tmp; 
    mI2c.readRegister(mAddr, cBusVoltageRegister, reinterpret_cast<uint8_t*>(&rawData), sizeof(rawData));
    tmp = rawData;
    rawData = (tmp >> 8) & 0xFF;
    rawData |= ((tmp & 0xFF) << 8);
    return (rawData >> 3) * 4;
}

int16_t Ina219::readCurrnet()
{
    int16_t shuntVoltage = 0;
    int16_t tmp;
    mI2c.readRegister(mAddr, cShuntVoltageRegister, reinterpret_cast<uint8_t*>(&shuntVoltage), sizeof(shuntVoltage));
    tmp = shuntVoltage;
    shuntVoltage = tmp >> 8;
    shuntVoltage |= (tmp << 8) & 0xFF;
    float voltage = shuntVoltage * 100; // 1mV per LSB
    float current = voltage / 50; //50mOhm shunt resistor
    return current;
}
