#include "control_channel.h"

ControlChannel::ControlChannel(II2cMaster &i2c) : mSettings{}, mCurrentSensor(i2c), mExpanderIO(i2c)
{
}

bool ControlChannel::setSettings(const ControlChannelSettings &settings)
{
    mSettings = settings;
    mCurrentSensor.setAddress(mSettings.ina_addr);
    mExpanderIO.setAddress(mSettings.pcf_addr);

    if (!connectionTest())
    {
        return false;
    }
    if (!configure())
    {
        return false;
    }
    return true;
}

bool ControlChannel::configure()
{
    return mExpanderIO.write(cPcfCfg);
}

bool ControlChannel::connectionTest()
{
    bool result = true;

    if (mSettings.enable == false)
    {
        return true;
    }

    if (mCurrentSensor.connectionTest())
    {
        mErrors.clr(Errors::INA_COMMUNICATION_ISSUE);
    }
    else
    {
        result = false;
        mErrors.set(Errors::INA_COMMUNICATION_ISSUE);
    }

    if (mExpanderIO.connectionTest())
    {
        mErrors.clr(Errors::PCF_COMMUNICATION_ISSUE);
    }
    else
    {
        result = false;
        mErrors.set(Errors::PCF_COMMUNICATION_ISSUE);
    }
    return result;
}

bool ControlChannel::setMotor(bool enable, uint8_t channel, bool dir)
{
    if(!mSettings.enable) {
        return true;
    }

    uint8_t data = mExpanderIO.read() | cPcfCfg;
    uint8_t mask;

    if (channel == 0)
    {
        data &= ~(cMotor1RightDirMask | cMotor1LeftDirMask);
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
        data &= ~(cMotor2RightDirMask | cMotor2LeftDirMask);
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

    return mExpanderIO.write(data);
}

 bool ControlChannel::setMotor(bool dir) {
    if((dir && !getLimitSwitchState(LimitSwitch::DOWN)) || (!dir && !getLimitSwitchState(LimitSwitch::UP))) {
        return setMotor(true, mSettings.pcf_channel, dir);
    } else {
        return setMotor(false, mSettings.pcf_channel, dir);
    }
 }

bool ControlChannel::getPowerSensorStatus()
{
    LOG << "Voltage: " << mCurrentSensor.readBusVoltage() << "mv";
    LOG << "Current: " << mCurrentSensor.readCurrnet() << "mA";
    return true;
}

State ControlChannel::getChannelState()
{
    bool upSwitch = getLimitSwitchState(LimitSwitch::UP);
    bool downSwitch = getLimitSwitchState(LimitSwitch::DOWN);

    if (upSwitch && !downSwitch)
    {
        return State::UP;
    }
    else if (!upSwitch && downSwitch)
    {
        return State::DOWN;
    }
    else if (!upSwitch && !downSwitch)
    {
        return State::MOVING;
    }
    else
    {
        return State::ERROR;
    }
}

bool ControlChannel::isRudder() const {
    return mSettings.rudder;
}

bool ControlChannel::getLimitSwitchState(LimitSwitch limit_switch)
{
    uint8_t data = mExpanderIO.read();
    bool result = false;
    uint8_t mask = 0;

    if (mSettings.inverse_limit_switch)
    {
        if (limit_switch == LimitSwitch::DOWN)
        {
            limit_switch = LimitSwitch::UP;
        }
        else
        {
            limit_switch = LimitSwitch::DOWN;
        }
    }

    if (limit_switch == LimitSwitch::DOWN && mSettings.pcf_channel == 0)
    {
        mask = cMotor1DownLimitSwitchMask;
    }
    else if (limit_switch == LimitSwitch::UP && mSettings.pcf_channel == 0)
    {
        mask = cMotor1UpLimitSwitchMask;
    }
    else if (limit_switch == LimitSwitch::DOWN && mSettings.pcf_channel == 1)
    {
        mask = cMotor2DownLimitSwitchMask;
    }
    else if (limit_switch == LimitSwitch::UP && mSettings.pcf_channel == 1)
    {
        mask = cMotor2UpLimitSwitchMask;
    }

    result = data & mask;

    if (!mSettings.inverse_down_limit_switch && limit_switch == LimitSwitch::DOWN)
    {
        result = !result;
    }

    if (!mSettings.inverse_up_limit_switch && limit_switch == LimitSwitch::UP)
    {
        result = !result;
    }

    return result;
}