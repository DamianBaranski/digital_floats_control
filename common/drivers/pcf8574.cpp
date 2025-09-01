#include "pcf8574.h"
#include "logger.h"

Pcf8574::Pcf8574(II2cMaster &i2c) : mI2c(i2c), mAddr(0)
{
}

void Pcf8574::setAddress(uint8_t address) {
    mAddr = address;
}

bool Pcf8574::connectionTest() const
{
    return mI2c.isDeviceReady(mAddr);
}

bool Pcf8574::read(uint8_t &data)
{
    bool result = mI2c.read(mAddr, &data, sizeof(data));
    return result;
}

bool Pcf8574::write(uint8_t data)
{
    return mI2c.write(mAddr, &data, sizeof(data));
}
