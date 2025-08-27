#include "control_channel.h"
#include "bsp.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

ControlChannel::ControlChannel(Expander &expander, II2cMaster &i2c) : mSettings{}, mCurrentSensor(&i2c), mExpander(expander), mCurrent{}
{
}

bool ControlChannel::setSettings(const ControlChannelSettings &settings)
{
    mSettings = settings;

    if (!configure())
    {
        return false;
    }
    return true;
}

bool ControlChannel::configure()
{
    return mExpander.configure();
}

bool ControlChannel::relaysTest() {
    bool result = true;
    return result;
}

bool ControlChannel::setMotor(bool dir) {
    // Check if motor should be activated based on limit switches:
    // - If moving up (dir=true) and the DOWN limit switch is not active, allow movement
    // - If moving down (dir=false) and the UP limit switch is not active, allow movement
    // This prevents driving the motor against an already reached limit
    if((dir && !getLimitSwitchState(LimitSwitch::DOWN)) || (!dir && !getLimitSwitchState(LimitSwitch::UP))) {
        mExpander.setMotor(mPcfChannel, true, dir);
    } else {
        // Disable motor if attempting to drive against an active limit switch
        mExpander.setMotor(mPcfChannel, false, dir);
    }
    return true;
}

GearState ControlChannel::getChannelState()
{
    mCurrent.current = mCurrentSensor.read()*1000.0; // Read current in 0.1A units
    mCurrent.timestamp = getTime();

    // Read current limit switch states
    bool upSwitch = getLimitSwitchState(LimitSwitch::UP);
    bool downSwitch = getLimitSwitchState(LimitSwitch::DOWN);
    
    // Determine channel state based on limit switch positions
    // Only one limit switch should be active at a time in normal operation
    if (upSwitch && !downSwitch)
    {
        return GearState::UP;
    }
    else if (!upSwitch && downSwitch)
    {
        return GearState::DOWN;
    }
    else if (!upSwitch && !downSwitch)
    {
        return GearState::MOVING;
    }
    else
    {
        // Both limit switches active simultaneously indicates a wiring error
        return GearState::ERROR;
    }
}

CurrentStatus ControlChannel::getCurrent() {
    return mCurrent;
}

bool ControlChannel::isRudder() const {
    return mSettings.rudder;
}

bool ControlChannel::getLimitSwitchState(LimitSwitch limit_switch)
{
    // Read current state of all inputs from I/O expander
    uint8_t data = mExpander.read();
    bool result = false;
    uint8_t mask = 0;

    // Handle global switch inversion if configured
    // This can be used to swap UP and DOWN functionality
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

    // Select appropriate input pin mask based on channel and requested switch
    if (limit_switch == LimitSwitch::DOWN && mPcfChannel == 0)
    {
        mask = cMotor1DownLimitSwitchMask;
    }
    else if (limit_switch == LimitSwitch::UP && mPcfChannel == 0)
    {
        mask = cMotor1UpLimitSwitchMask;
    }
    else if (limit_switch == LimitSwitch::DOWN && mPcfChannel == 1)
    {
        mask = cMotor2DownLimitSwitchMask;
    }
    else if (limit_switch == LimitSwitch::UP && mPcfChannel == 1)
    {
        mask = cMotor2UpLimitSwitchMask;
    }

    // Check if input pin is active (1) or inactive (0)
    result = data & mask;

    // Apply switch-specific inversion if configured
    // This allows using either normally-open or normally-closed switches
    if (mSettings.inverse_down_limit_switch && limit_switch == LimitSwitch::DOWN)
    {
        result = !result;
    }

    if (mSettings.inverse_up_limit_switch && limit_switch == LimitSwitch::UP)
    {
        result = !result;
    }

    return result;
}

#pragma GCC pop_options