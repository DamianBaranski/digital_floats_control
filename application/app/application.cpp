#include "application.h"
#include "colors.h"
#include "version.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

/**
 * System constants namespace
 * Contains configuration values and constants used throughout the application
 */
namespace SysConst {
    // Memory storage addresses and sizes
    constexpr uint32_t kUserSettingsFlashAddress = 0;
    constexpr uint32_t kChannelSettingsFlashAddress = 4096;

    // Timing constants
    constexpr uint32_t kDefaultSleepMs = 100;
    constexpr uint32_t kTestSwitchSleepMs = 10;
    constexpr uint32_t kPollIntervalMs = 10;
    constexpr uint32_t kColorChangeIntervalMs = 500;
    constexpr uint32_t kBlinkingIntervalMs = 500;
    constexpr uint32_t kMaxBrightnessWaitTimeMs = 5000;
    constexpr uint32_t kSimulationTimeout = 1000;

    // Buffer sizes
    constexpr size_t kUartBufferSize = 200;

    // Channel test settings
    constexpr uint16_t kDefaultInaCalibration = 500;
    constexpr uint16_t kMaxVoltageLimitMv = 280;
    constexpr uint16_t kMinVoltageLimitMv = 80;

    // Brightness settings
    constexpr uint8_t kMinBrightnessValue = 0x0F;
    constexpr uint8_t kMaxBrightnessValue = 0xFF;
    constexpr uint8_t kBrightnessStep = 0x18;
    constexpr size_t kBrightnessCalibrationCycles = 10;
}

Application::Application(Bsp &bsp) : mBsp(bsp), mLeds(*mBsp.leds),
                                     mExpanders{Expander(*mBsp.i2cBusRelays), Expander(*mBsp.i2cBusRelays), Expander(*mBsp.i2cBusRelays)}, mState{},
                                     mChannels{ControlChannel(mExpanders[0], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[0], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[1], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[1], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[2], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[2], *mBsp.i2cBusCurrent)},
                                     mChannelsSettings(*mBsp.extFlash, SysConst::kChannelSettingsFlashAddress, mState.mChannelSettings),
                                     mCurrentSensors{},
                                     mUartCommunication(mState, *mBsp.uartBus)
                                      {
    mExpanders[0].setAddress(0x22);
    mExpanders[1].setAddress(0x21);
    mExpanders[2].setAddress(0x20);

    mCurrentSensors[0] = Adc121c(mBsp.i2cBusCurrent.get(), 0x55);
    mCurrentSensors[1] = Adc121c(mBsp.i2cBusCurrent.get(), 0x55);
    mCurrentSensors[2] = Adc121c(mBsp.i2cBusCurrent.get(), 0x54);
    mCurrentSensors[3] = Adc121c(mBsp.i2cBusCurrent.get(), 0x52);
    mCurrentSensors[4] = Adc121c(mBsp.i2cBusCurrent.get(), 0x51);
    mCurrentSensors[5] = Adc121c(mBsp.i2cBusCurrent.get(), 0x50);
    
    mChannels[0].setExpanderChannel(0);
    mChannels[1].setExpanderChannel(0);
    mChannels[2].setExpanderChannel(1);
    mChannels[3].setExpanderChannel(0);
    mChannels[4].setExpanderChannel(1);
    mChannels[5].setExpanderChannel(0);

    // Initialize application state
    loadSettings();
    setBrightness();
    mBsp.pwr_voltage->startConversion();
}

/**
 * Helper function for waiting on GPIO pin state change with timeout
 * Used in multiple places for switch detection with timeout capability
 */
bool waitForPushRelease(uint32_t time_ms, IGpio &pin, bool expectedState) {
    for (uint32_t t = 0; t < time_ms; t += SysConst::kPollIntervalMs) {
        if (pin.get() == expectedState) {
            return true;
        }
        sleep(SysConst::kPollIntervalMs);
    }
    return false;
}

void Application::spin() {
    // Get current time
    mState.mSystem.mUptime = getTime();

    // Handle requested actions
    processStateRequests();

    mState.mDebugData.request_time = getTime() - mState.mSystem.mUptime;
    
    // Update I/O expander states from hardware
    updateExpanderState();

    mState.mDebugData.expander_update_time = getTime() - mState.mDebugData.request_time - mState.mSystem.mUptime;
    
    // Before updating, store old states
    bool prevLdgGear = mState.mSwitches.mLdgGearSwitchState;
    bool prevRudder  = mState.mSwitches.mRudderSwitchState;
    bool prevTest    = mState.mSwitches.mTestSwitchState;
    // Read current switch states, considering remote control simulation
    if(mState.mRemoteControl.mSimulationTimeout!=0 && mState.mRemoteControl.mSimulationTimeout+SysConst::kSimulationTimeout>mState.mSystem.mUptime) {
        mState.mSwitches.mLdgGearSwitchState = mState.mRemoteControl.mLdgGearSwitchState;
        mState.mSwitches.mRudderSwitchState = mState.mRemoteControl.mRudderSwitchState;
        mState.mSwitches.mTestSwitchState = mState.mRemoteControl.mTestSwitchState;
    } else {
        mState.mRemoteControl.mSimulationTimeout = 0;
        mState.mSwitches.mLdgGearSwitchState = getLdgGearSwitch();
        mState.mSwitches.mRudderSwitchState = getRudderSwitch();
        mState.mSwitches.mTestSwitchState = !mBsp.testSwitch->get();
    }
    // Check if any button state changed
    mState.mMovementTimings.buttons_changed = (prevLdgGear != mState.mSwitches.mLdgGearSwitchState)
                                           || (prevRudder != mState.mSwitches.mRudderSwitchState) 
                                           || (prevTest != mState.mSwitches.mTestSwitchState);

    for(size_t i=0; i<Application::NO_CHANNELS; ++i) {
        if(mChannels[i].isRudder() && prevRudder != mState.mSwitches.mRudderSwitchState) {
            mState.mMovementTimings.start_time[i] = mState.mSystem.mUptime;
        }
        if(!mChannels[i].isRudder() && prevLdgGear != mState.mSwitches.mLdgGearSwitchState) {
            mState.mMovementTimings.start_time[i] = mState.mSystem.mUptime;
        }
    }

    mState.mDebugData.switch_read_time = getTime() - mState.mDebugData.expander_update_time - mState.mSystem.mUptime;

    // Read current sensors
    updateMonitoringData();

    mState.mDebugData.current_read_time = getTime() - mState.mDebugData.switch_read_time - mState.mSystem.mUptime;

    // Process all channels with current switch states
    processChannels();


    mState.mDebugData.channel_process_time = getTime() - mState.mDebugData.current_read_time  - mState.mSystem.mUptime;

    // Write expander states to hardware
    mExpanders[0].write();
    mExpanders[1].write();
    mExpanders[2].write();

    mState.mDebugData.expander_write_time = getTime() - mState.mDebugData.channel_process_time  - mState.mSystem.mUptime;

    // Update LED indicators
    animateLeds();
    mLeds.update();

    mState.mSystem.mPowerVoltage = mBsp.pwr_voltage->readValue()*33/4096*9.11; //in mV
    
    mState.mDebugData.led_update_time = getTime() - mState.mDebugData.expander_write_time - mState.mSystem.mUptime;

    mUartCommunication.spin();
    mState.mDebugData.uart_time = getTime() - mState.mDebugData.led_update_time - mState.mSystem.mUptime;
    // Process communication
    while(mState.mSystem.mUptime + SysConst::kPollIntervalMs > getTime()) {
        mUartCommunication.spin();
    }
    mState.mDebugData.total_time = getTime() - mState.mSystem.mUptime;


    if(mState.mDebugData.total_time>SysConst::kPollIntervalMs) {
        // Something went wrong with timing
        LOG << "Loop time exceeded: " << mState.mDebugData.total_time << " ms\r\n";
        LOG << "  Request time: " << mState.mDebugData.request_time << " ms\r\n";
        LOG << "  Expander update time: " << mState.mDebugData.expander_update_time << " ms\r\n"; 
        LOG << "  Switch read time: " << mState.mDebugData.switch_read_time << " ms\r\n";
        LOG << "  Current read time: " << mState.mDebugData.current_read_time << " ms\r\n";
        LOG << "  Channel process time: " << mState.mDebugData.channel_process_time << " ms\r\n";
        LOG << "  Expander write time: " << mState.mDebugData.expander_write_time << " ms\r\n";
        LOG << "  LED update time: " << mState.mDebugData.led_update_time << " ms\r\n";
        LOG << "  UART time: " << mState.mDebugData.uart_time << " ms\r\n";
        sleep(SysConst::kPollIntervalMs);
    }
}

void Application::loadSettings() {
    // Load saved settings from non-volatile storage
    
    for (int i =0; i < NO_CHANNELS; ++i) {
        mState.mChannelSettings[i].enable = true;
        mState.mChannelSettings[i].rudder = false;
        mState.mChannelSettings[i].inverse_motor = false;
        mState.mChannelSettings[i].bridge = false;                    
        mState.mChannelSettings[i].inverse_up_limit_switch = false;   
        mState.mChannelSettings[i].inverse_down_limit_switch = false; 
        mState.mChannelSettings[i].inverse_limit_switch = false; 
        mState.mChannelSettings[i].max_current_error_limit = 1000;
        mState.mChannelSettings[i].max_current_warning_limit = 500;
        mState.mChannelSettings[i].min_current_limit = 0;
        mState.mChannelSettings[i].timeout = 15; //seconds
    }    
    
    mState.mChannelSettings[0].bridge = true;
    mState.mChannelSettings[0].rudder = true;
    mState.mChannelSettings[1].rudder = true;
    
    
    // Apply channel settings to all channels
    for (size_t i = 0; i < NO_CHANNELS; ++i) {
        mChannels[i].setSettings(mState.mChannelSettings[i]);
    }
}

void Application::processStateRequests() {
    if(mState.mActionRequest.mSaveSettings) {
        mChannelsSettings.save();
        mState.mActionRequest.mSaveSettings = false;
    }
    
    if(mState.mActionRequest.mDeviceReset) {
        mBsp.reset();
    }
}

void Application::updateExpanderState() {
    if(mExpanders[0].update()) {
        mState.mErrors.clear(0, ChannelError::RELAY_COMMUNICATION_ERROR);
        mState.mErrors.clear(1, ChannelError::RELAY_COMMUNICATION_ERROR);
    } else {
        mState.mErrors.set(0, ChannelError::RELAY_COMMUNICATION_ERROR);
        mState.mErrors.set(1, ChannelError::RELAY_COMMUNICATION_ERROR);
    }
    if(mExpanders[1].update()) {
        mState.mErrors.clear(2, ChannelError::RELAY_COMMUNICATION_ERROR);
        mState.mErrors.clear(3, ChannelError::RELAY_COMMUNICATION_ERROR);
    } else {
        mState.mErrors.set(2, ChannelError::RELAY_COMMUNICATION_ERROR);
        mState.mErrors.set(3, ChannelError::RELAY_COMMUNICATION_ERROR);
    }
    if(mExpanders[2].update()) {
        mState.mErrors.clear(4, ChannelError::RELAY_COMMUNICATION_ERROR);
        mState.mErrors.clear(5, ChannelError::RELAY_COMMUNICATION_ERROR);
    } else {
        mState.mErrors.set(4, ChannelError::RELAY_COMMUNICATION_ERROR);
        mState.mErrors.set(5, ChannelError::RELAY_COMMUNICATION_ERROR);
    }
}

void Application::updateMonitoringData() {
    for(int i=0; i<NO_CHANNELS; ++i) {
        float current = -1;
        bool result = mCurrentSensors[i].read(current);

        if(!result) {
            mState.mErrors.set(i, ChannelWarning::ADC_COMMUNICATION_ERROR);
        } else {
            mState.mErrors.clear(i, ChannelWarning::ADC_COMMUNICATION_ERROR);
            mState.mMonitoringData[i].timestamp = getTime();
            mState.mMonitoringData[i].current = current*1000; //in mA
            mState.mMonitoringData[i].state = static_cast<uint8_t>(mChannels[i].getChannelState());
            mState.mMonitoringData[i].switches = mChannels[i].getLimitSwitchState(LimitSwitch::UP) ? 0x01 : 0x00;
            mState.mMonitoringData[i].switches |= mChannels[i].getLimitSwitchState(LimitSwitch::DOWN) ? 0x02 : 0x00;
        }

        if(mState.mMonitoringData[i].current > mState.mChannelSettings[i].max_current_error_limit) {
            mState.mErrors.set(i, ChannelError::OVER_CURRENT_ERROR);
        } else if(mState.mMovementTimings.buttons_changed){
            mState.mErrors.clear(i, ChannelError::OVER_CURRENT_ERROR);
        }

        if(mState.mMonitoringData[i].current > mState.mChannelSettings[i].max_current_warning_limit) {
            mState.mErrors.set(i, ChannelWarning::OVER_CURRENT_WARNING);
        } else {
            mState.mErrors.clear(i, ChannelWarning::OVER_CURRENT_WARNING);
        }

        if(mState.mMonitoringData[i].current < mState.mChannelSettings[i].min_current_limit) {
            mState.mErrors.set(i, ChannelWarning::UNDER_CURRENT_WARNING);
        } else {
            mState.mErrors.clear(i, ChannelWarning::UNDER_CURRENT_WARNING);
        }

        if(mState.mMonitoringData[i].switches == 3) {
            mState.mErrors.set(i, ChannelError::ENDSTOP_SHORT_CIRCUIT);
        } else if(mState.mMovementTimings.buttons_changed) {
            mState.mErrors.clear(i, ChannelError::ENDSTOP_SHORT_CIRCUIT);
        }

        if(mState.mSystem.mUptime - mState.mMovementTimings.start_time[i] > mState.mChannelSettings[i].timeout * 1000 && mState.mMonitoringData[i].state == static_cast<uint8_t>(GearState::MOVING)) {
            mState.mErrors.set(i, ChannelWarning::MOVEMENT_TIMEOUT);
        } else {
            mState.mErrors.clear(i, ChannelWarning::MOVEMENT_TIMEOUT);
        }
    }
}

bool Application::getLdgGearSwitch() {
    return mBsp.ldgSwitch->get();
}

bool Application::getRudderSwitch() {
    return mBsp.rudSwitch->get();
}

void Application::processChannels() {
    bool rudderSwitchState = mState.mSwitches.mRudderSwitchState;
    bool ldgGearSwitchState = mState.mSwitches.mLdgGearSwitchState;
    for(size_t channel = 0; channel < NO_CHANNELS; ++channel) {

        // Set motor state based on channel type (rudder vs landing gear)
        bool isRudder = mChannels[channel].isRudder();
        if(mState.mErrors.getChannelErrors(channel) != 0) {
            // In case of error disable motor
            mChannels[channel].disableMotor();
        } else if (isRudder) {
            mChannels[channel].setMotor(rudderSwitchState);
        } else {
            mChannels[channel].setMotor(ldgGearSwitchState);
        }
    }
}

void Application::animateLeds() {
    for(size_t channel=0; channel<NO_CHANNELS; ++channel) {
        GearState state = mChannels[channel].getChannelState();
        bool isRudder = mChannels[channel].isRudder();
        bool error = mState.mErrors.getChannelErrors(channel)!=0;
        bool warning = mState.mErrors.getChannelWarnings(channel)!=0;

        uint32_t color = 0;

        if(error) {
            color = Colors::blinking(SysConst::kBlinkingIntervalMs, mState.mSystem.mUptime, Colors::RED);
            mLeds.setColor(channel, color);
            continue;
        }

        uint32_t rudder_down_color = !mState.mSwitches.mLdgGearSwitchState?cRudderDownColor:cRudderInactiveColor;
        switch (state)
        {
            case GearState::UP:
                color = isRudder ? cRudderUpColor : cLdgGearUpColor;
            break;
            case GearState::DOWN:
                color = isRudder ? rudder_down_color : cLdgGearDownColor;
            break;
            case GearState::MOVING:
                uint32_t rudder_color = !mState.mSwitches.mRudderSwitchState ? cRudderUpColor : rudder_down_color;
                uint32_t ldg_color = !mState.mSwitches.mLdgGearSwitchState ? cLdgGearUpColor : cLdgGearDownColor;
                uint32_t color1 = warning ? Colors::ORANGE : 0;
                uint32_t color2 = isRudder ? rudder_color : ldg_color;
                color = Colors::blinking(SysConst::kBlinkingIntervalMs, mState.mSystem.mUptime, color1, color2);
            break;
        }
        mLeds.setColor(channel, color);
    }
}

void Application::setBrightness() {
    // Exit if test switch not held long enough
    if (waitForPushRelease(SysConst::kMaxBrightnessWaitTimeMs, *mBsp.testSwitch, true)) {
        return;
    }

    // Brightness calibration routine - cycles through brightness levels
    // and lets user select by releasing the test switch
    for (size_t i = 0; i < SysConst::kBrightnessCalibrationCycles; i++) {
        for (uint8_t b = SysConst::kMinBrightnessValue; b < SysConst::kMaxBrightnessValue; b += SysConst::kBrightnessStep) {
            // Display current brightness level
            mLeds.setColor(Colors::setBrightness(Colors::WHITE, b));
            mLeds.update();
            
            // If user selects this brightness level, save and exit
            if (waitForPushRelease(SysConst::kColorChangeIntervalMs, *mBsp.testSwitch, true)) {
                return;
            }
        }
    }
}

#pragma GCC pop_options