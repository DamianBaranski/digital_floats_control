#include "control_channel.h"
#include "bsp.h"

ControlChannel::ControlChannel(II2cMaster &i2c) : mSettings{}, mCurrentSensor(i2c), mExpanderIO(i2c)
{
    // Default initialization with empty settings
    // Actual configuration happens in setSettings()
}

bool ControlChannel::setSettings(const ControlChannelSettings &settings)
{
    mSettings = settings;
    mCurrentSensor.setAddress(mSettings.ina_addr);
    mExpanderIO.setAddress(mSettings.pcf_addr);

    // Verify hardware connections before applying configuration
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
    // Configure I/O expander: lower 4 bits as inputs (limit switches), upper 4 bits as outputs (motor control)
    return mExpanderIO.write(cPcfCfg);
}

bool ControlChannel::connectionTest()
{
    bool result = true;

    // Skip test for disabled channels
    if (mSettings.enable == false)
    {
        return true;
    }

    // Test INA219 current/voltage sensor connection
    if (mCurrentSensor.connectionTest())
    {
        mErrors.clr(Errors::INA_COMMUNICATION_ISSUE);
    }
    else
    {
        result = false;
        mErrors.set(Errors::INA_COMMUNICATION_ISSUE);
    }

    // Test PCF8574 I/O expander connection
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

    // Skip test for disabled channels
    if(!mSettings.enable) {
        return true;
    }

    // Select appropriate mask based on assigned PCF channel
    if(mSettings.pcf_channel == 0) {
        // For channel 0, use motor 1 direction pins
        mask = cMotor1RightDirMask | cMotor1LeftDirMask;
    } else if(mSettings.pcf_channel == 1) {
        // For channel 1, use motor 2 direction pins
        mask = cMotor2RightDirMask | cMotor2LeftDirMask;
    }

    // If we're just verifying and not setting, clear the mask
    if(!set) {
        mask = 0;
    }

    // Apply configuration with appropriate mask
    if(!mExpanderIO.write(mask | cPcfCfg)) {
        return false;
    }
    return true;
}

bool ControlChannel::relaysTest() {
    bool result = true;
    uint8_t mask = 0;

    // Skip test for disabled channels
    if(!mSettings.enable) {
        return true;
    }

    // Select appropriate mask based on assigned PCF channel
    if(mSettings.pcf_channel == 0) {
        // For channel 0, use motor 1 direction pins for testing both relays
        mask = cMotor1RightDirMask | cMotor1LeftDirMask;
    } else if(mSettings.pcf_channel == 1) {
        // For channel 1, use motor 2 direction pins for testing both relays
        mask = cMotor2RightDirMask | cMotor2LeftDirMask;
    }

    // Activate both relays to test their functionality
    if(!mExpanderIO.write(mask | cPcfCfg)) {
        return false;
    }

    // Allow relays time to settle and then measure electrical parameters
    sleep(500);
    uint16_t voltage = mCurrentSensor.readBusVoltage();
    uint16_t current = mCurrentSensor.readCurrent();

    // Verify voltage is within configured limits
    // Note: Unit conversion between 0.1V settings and mV measurements
    if(mSettings.max_voltage_limit*100 < voltage) { //max_voltage_limit [0.1V], voltage [1mV]
        result = false;
        mWarnings.set(Warnings::HIGH_POWER_VOLTAGE);
    }
    if(mSettings.min_voltage_limit*100 > voltage) { //min_voltage_limit [0.1V], voltage [1mV]
        result = false;
        mWarnings.set(Warnings::LOW_POWER_VOLTAGE);
    }
    
    // When both relays are active, we expect no current flow (open circuit)
    // Current flow would indicate a short circuit or relay failure
    if(current != 0) {
        result = false;
        mErrors.set(Errors::RELAYS_ISSUE);
    }

    // Reset relay states to default configuration
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

    // Read current I/O expander state and maintain input configuration bits
    uint8_t data = mExpanderIO.read() | cPcfCfg;
    uint8_t mask;

    // Clear both directional bits for the selected motor before setting new direction
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

    // Only apply direction mask if the motor is being enabled
    if (enable)
    {
        data |= mask;
    }

    return mExpanderIO.write(data);
}

bool ControlChannel::setMotor(bool dir) {
    // Check if motor should be activated based on limit switches:
    // - If moving up (dir=true) and the DOWN limit switch is not active, allow movement
    // - If moving down (dir=false) and the UP limit switch is not active, allow movement
    // This prevents driving the motor against an already reached limit
    if((dir && !getLimitSwitchState(LimitSwitch::DOWN)) || (!dir && !getLimitSwitchState(LimitSwitch::UP))) {
        return setMotor(true, mSettings.pcf_channel, dir);
    } else {
        // Disable motor if attempting to drive against an active limit switch
        return setMotor(false, mSettings.pcf_channel, dir);
    }
}

bool ControlChannel::getPowerSensorStatus(uint16_t &voltage, uint16_t &current)
{
    // Verify sensor connection before reading values
    if(!mCurrentSensor.connectionTest()) {
        return false;
    }

    voltage = mCurrentSensor.readBusVoltage();
    current = mCurrentSensor.readCurrent();
    return true;
}

State ControlChannel::getChannelState()
{
    // Check communication with I/O expander before trying to determine state
    if(!mExpanderIO.connectionTest()) {
        return State::ERROR;
    }

    // Read current limit switch states
    bool upSwitch = getLimitSwitchState(LimitSwitch::UP);
    bool downSwitch = getLimitSwitchState(LimitSwitch::DOWN);
    
    // Motor impedance check when no limit switches are active (channel is moving)
    // This allows detection of motor circuit issues while in motion
    if(!upSwitch && !downSwitch) {
        int16_t current = mCurrentSensor.readCurrent();
        
        // Check for abnormal current conditions which may indicate issues
        if(abs(current) > mSettings.max_current_limit) {
            mWarnings.set(Warnings::LOW_MOTOR_IMPEDANCE);
        }

        if(abs(current) < mSettings.min_current_limit) {
            mWarnings.set(Warnings::HIGH_MOTOR_IMPEDANCE);
        }
    }

    // Determine channel state based on limit switch positions
    // Only one limit switch should be active at a time in normal operation
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
        // Both limit switches active simultaneously indicates a wiring error
        return State::ERROR;
    }
}

bool ControlChannel::isRudder() const {
    return mSettings.rudder;
}

bool ControlChannel::getLimitSwitchState(LimitSwitch limit_switch)
{
    // Read current state of all inputs from I/O expander
    uint8_t data = mExpanderIO.read();
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

    // Check if input pin is active (1) or inactive (0)
    result = data & mask;

    // Apply switch-specific inversion if configured
    // This allows using either normally-open or normally-closed switches
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