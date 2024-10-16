#include "control_channel.h"
#include "bsp.h"

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

bool ControlChannel::addressTest(bool set) {
    uint8_t mask = 0;

    if(!mSettings.enable) {
        return true;
    }

    if(mSettings.pcf_channel == 0) {
        mask = cMotor1RightDirMask | cMotor1LeftDirMask;
    } else if(mSettings.pcf_channel == 1) {
        mask = cMotor2RightDirMask | cMotor2LeftDirMask;
    }

    if(!set) {
        mask = 0;
    }

    if(!mExpanderIO.write(mask | cPcfCfg)) {
        return false;
    }
    return true;
}

bool ControlChannel::relaysTest() {
    bool result = true;
    uint8_t mask = 0;

    if(!mSettings.enable) {
        return true;
    }

    if(mSettings.pcf_channel == 0) {
        mask = cMotor1RightDirMask | cMotor1LeftDirMask;
    } else if(mSettings.pcf_channel == 1) {
        mask = cMotor2RightDirMask | cMotor2LeftDirMask;
    }

    if(!mExpanderIO.write(mask | cPcfCfg)) {
        return false;
    }

    sleep(500);
    uint16_t voltage = mCurrentSensor.readBusVoltage();
    uint16_t current = mCurrentSensor.readCurrent();

    if(mSettings.max_voltage_limit*100 < voltage) { //max_voltage_limit [0.1V], voltage [1mV]
        result = false;
        mWarnings.set(Warnings::HIGH_POWER_VOLTAGE);
    }
    if(mSettings.min_voltage_limit*100 > voltage) { //min_voltage_limit [0.1V], voltage [1mV]
        result = false;
        mWarnings.set(Warnings::LOW_POWER_VOLTAGE);
    }
    if(current != 0) {
        result = false;
        mErrors.set(Errors::RELAYS_ISSUE);
    }


    if(!mExpanderIO.write(cPcfCfg)) {
        return false;
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

bool ControlChannel::getPowerSensorStatus(uint16_t &voltage, uint16_t &current)
{
    if(!mCurrentSensor.connectionTest()) {
        return false;
    }

    voltage = mCurrentSensor.readBusVoltage();
    current = mCurrentSensor.readCurrent();
    return true;
}

State ControlChannel::getChannelState()
{
    if(!mExpanderIO.connectionTest()) {
        return State::ERROR;
    }

    bool upSwitch = getLimitSwitchState(LimitSwitch::UP);
    bool downSwitch = getLimitSwitchState(LimitSwitch::DOWN);
    if(!upSwitch && !downSwitch) { //ToDo change to motor on check
        int16_t current = mCurrentSensor.readCurrent();
        
        if(abs(current) > mSettings.max_current_limit) {
            mWarnings.set(Warnings::LOW_MOTOR_IMPEDANCE);
        }

        if(abs(current) < mSettings.min_current_limit) {
            mWarnings.set(Warnings::HIGH_MOTOR_IMPEDANCE);
        }
    }

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