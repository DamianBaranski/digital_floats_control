#include <cstdint>
#include "ii2c_master.h"
#include "logger.h"

class Ina219
{
public:
    Ina219(II2cMaster &i2c, uint8_t addr);
    bool connectionTest() const;
    uint16_t readBusVoltage();
    int16_t readCurrnet();

private:
    static constexpr uint8_t cShuntVoltageRegister = 0x01;
    static constexpr uint8_t cBusVoltageRegister = 0x02;

    II2cMaster &mI2c;
    uint8_t mAddr;
};

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
    rawData = (tmp >>8) & 0xFF;
    rawData |= ((tmp & 0xFF) << 8);
    return (rawData >> 3) * 4;
}

int16_t Ina219::readCurrnet()
{
    int16_t shuntVoltage = 0;
    int16_t tmp;
    mI2c.readRegister(mAddr, cShuntVoltageRegister, reinterpret_cast<uint8_t*>(&shuntVoltage), sizeof(shuntVoltage));
    tmp = shuntVoltage;
    shuntVoltage = tmp >>8;
    shuntVoltage |= (tmp << 8) &0xFF;
    float voltage = shuntVoltage * 100; //1mV
    float current = voltage/50; //5mOhm
    return current;
}

class Pcf8574
{
public:
    Pcf8574(II2cMaster &i2c, uint8_t addr);
    bool connectionTest() const;
    uint8_t read();
    bool write(uint8_t data);

private:
    II2cMaster &mI2c;
    uint8_t mAddr;
};

Pcf8574::Pcf8574(II2cMaster &i2c, uint8_t addr) : mI2c(i2c), mAddr(addr)
{
}

bool Pcf8574::connectionTest() const
{
    return mI2c.isDeviceReady(mAddr);
}

uint8_t Pcf8574::read()
{
    uint8_t data = 0;
    mI2c.read(mAddr, &data, sizeof(data));
    LOG << "r:" << (data & 0xF);
    return data;
}

bool Pcf8574::write(uint8_t data)
{
    return mI2c.write(mAddr, &data, sizeof(data));
}

typedef struct
{
    uint8_t enable : 1;
    uint8_t bridge : 1;
    uint8_t inverse_motor : 1;
    uint8_t inverse_up_limit_switch : 1;
    uint8_t inverse_down_limit_switch : 1;
    uint8_t inverse_limit_switch : 1;
    uint8_t ina_addr;
    uint16_t ina_callibration; // 0.01 Ohm
    uint8_t pcf_addr;
    uint8_t pcf_channel;        // 0 or 1
    uint16_t max_voltage_limit; // 0.1V
    uint16_t min_voltage_limit; // 0.1V
    uint16_t max_current_limit; // 0.1A
    uint16_t min_current_limit; // 0.1A
} ControlChannelSettings;

class ControlChannel
{
public:
    ControlChannel(const ControlChannelSettings &settings, II2cMaster &i2c);
    bool connectionTest() const;
    bool setMotor(bool enable, uint8_t channel, bool dir);
    bool getPowerSensorStatus();

private:
    static constexpr uint8_t cPcfCfg = 0x0F;
    static constexpr uint8_t cMotor1RightDirMask = 0x10;
    static constexpr uint8_t cMotor1LeftDirMask = 0x20;
    static constexpr uint8_t cMotor2RightDirMask = 0x40;
    static constexpr uint8_t cMotor2LeftDirMask = 0x80;
    static constexpr uint8_t cMotor1UpSensorMask = 0x01;
    static constexpr uint8_t cMotor1DownSensorMask = 0x02;
    static constexpr uint8_t cMotor2UpSensorMask = 0x04;
    static constexpr uint8_t cMotor2DownSensorMask = 0x08;

    bool configure();
    const ControlChannelSettings &mSettings;
    Ina219 mCurrentSensor;
    Pcf8574 mExpanderIO;
};

ControlChannel::ControlChannel(const ControlChannelSettings &settings, II2cMaster &i2c) : mSettings(settings), mCurrentSensor(i2c, settings.ina_addr),
                                                                                          mExpanderIO(i2c, settings.pcf_addr)
{
    configure();
}

bool ControlChannel::configure()
{
    return mExpanderIO.write(cPcfCfg);
}

bool ControlChannel::connectionTest() const
{
    if (mSettings.enable == false)
    {
        return true;
    }

    return mCurrentSensor.connectionTest() && mExpanderIO.connectionTest();
}

bool ControlChannel::setMotor(bool enable, uint8_t channel, bool dir)
{
    uint8_t data = mExpanderIO.read() | cPcfCfg;
    uint8_t mask;

    if (channel == 0)
    {
        if (dir)
        {
            mask = cMotor1RightDirMask;
        }
        else
        {
            mask = cMotor1LeftDirMask;
        }
    }
    else if (channel == 1)
    {
        if (dir)
        {
            mask = cMotor2RightDirMask;
        }
        else
        {
            mask = cMotor2LeftDirMask;
        }
    }

    if (enable)
    {
        data |= mask;
    }
    else
    {
        data &= ~mask;
    }

    return mExpanderIO.write(data);
}

bool ControlChannel::getPowerSensorStatus() {
    LOG << "Voltage: " << mCurrentSensor.readBusVoltage() << "mv";
    LOG << "Current: " << mCurrentSensor.readCurrnet() << "mA";
    return true;
}