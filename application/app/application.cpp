#include "application.h"
#include "colors.h"
#include "version.h"

#pragma GCC push_options
#pragma GCC optimize ("Og")
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
    constexpr uint32_t kPollIntervalMs = 150;
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
                                     mExpanders{Expander(*mBsp.i2cBusRelays), Expander(*mBsp.i2cBusRelays), Expander(*mBsp.i2cBusRelays)},
                                     mChannels{ControlChannel(mExpanders[0], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[0], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[1], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[1], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[2], *mBsp.i2cBusCurrent),
                                               ControlChannel(mExpanders[2], *mBsp.i2cBusCurrent)},
                                     mChannelsSettings(*mBsp.extFlash, SysConst::kChannelSettingsFlashAddress, mState.mChannelSettings), mState(),
                                     mUartCommunication(mState, *mBsp.uartBus)
                                      {
    mExpanders[0].setAddress(0x22);
    mExpanders[1].setAddress(0x21);
    mExpanders[2].setAddress(0x20);
    
    mChannels[0].setExpanderChannel(0);
    mChannels[0].setCurrentSensorAddress(0x55);

    mChannels[1].setExpanderChannel(0);
    mChannels[1].setCurrentSensorAddress(0x55);

    mChannels[2].setExpanderChannel(1);
    mChannels[2].setCurrentSensorAddress(0x54);

    mChannels[3].setExpanderChannel(0);
    mChannels[3].setCurrentSensorAddress(0x52);    

    mChannels[4].setExpanderChannel(1);
    mChannels[4].setCurrentSensorAddress(0x51);

    mChannels[5].setExpanderChannel(0);
    mChannels[5].setCurrentSensorAddress(0x50);    

    // Initialize application state
    loadSettings();
    setBrightness();
    relaysTest();

    for(int i=0; i<127; i++) {
        if(bsp.i2cBusRelays->isDeviceReady(i)) {
            sleep(100);
            LOG << "I2C device found at address: " << i;
        }
    }
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
    if(mState.mActionRequest.mSaveSettings) {
        mChannelsSettings.save();
        mState.mActionRequest.mSaveSettings = false;
    }
    
    if(mState.mActionRequest.mDeviceReset) {
        mBsp.reset();
    }

    mExpanders[0].update();
    mExpanders[1].update();
    mExpanders[2].update();
    
    // Get current time
    mState.mSystem.mUptime = getTime();
    
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

    // Test mode detection - check if test switch is active
    if (mState.mSwitches.mTestSwitchState) {
        testSwitchProcedure();
    } else {
        // Process all channels with current switch states
        for (size_t channel = 0; channel < NO_CHANNELS; ++channel) {
            processChannel(channel, mState.mSwitches.mRudderSwitchState, mState.mSwitches.mLdgGearSwitchState, mState.mSystem.mUptime);
        }
        // Write expander states to hardware
        mExpanders[0].write();
        mExpanders[1].write();
        mExpanders[2].write();

        // Update LED indicators
        mLeds.update();
    }

    mUartCommunication.spin();
    // Process communication
    while(mState.mSystem.mUptime + SysConst::kPollIntervalMs > getTime()) {
        mUartCommunication.spin();
    }
}

void Application::testSwitchProcedure() {
    // Run through a sequence of colors to indicate test mode
    const uint32_t colors[] = {Colors::RED, Colors::GREEN, Colors::BLUE};

    for (uint32_t color : colors) {
        mLeds.setColor(color);
        mLeds.update();
        // Exit if test switch is released
        if (waitForPushRelease(SysConst::kColorChangeIntervalMs, *mBsp.testSwitch, true)) {
            return;
        }
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
        mState.mChannelSettings[i].max_current_error_limit = 100;
        mState.mChannelSettings[i].max_current_warning_limit = 100;
        mState.mChannelSettings[i].min_current_limit = 0;
    }    
    
    mState.mChannelSettings[0].bridge = true;
    mState.mChannelSettings[0].rudder = true;
    mState.mChannelSettings[1].rudder = true;
    
    
    // Apply channel settings to all channels
    for (size_t i = 0; i < NO_CHANNELS; ++i) {
        mChannels[i].setSettings(mState.mChannelSettings[i]);
    }
}

bool Application::getLdgGearSwitch() {
    return mBsp.ldgSwitch->get();
}

bool Application::getRudderSwitch() {
    return mBsp.rudSwitch->get();
}

void Application::processChannel(size_t channel, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    // Get current channel state
    GearState state = mChannels[channel].getChannelState();
    uint32_t color = 0;

    // Set motor state based on channel type (rudder vs landing gear)
    bool isRudder = mChannels[channel].isRudder();
    if (isRudder) {
        mChannels[channel].setMotor(rudderSwitchState);
    } else {
        mChannels[channel].setMotor(ldgGearSwitchState);
    }

    // Determine LED color based on channel state
    switch (state) {
    case GearState::DOWN:
        color = getColorForDownState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        break;

    case GearState::UP:
        color = getColorForUpState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        break;

    case GearState::MOVING:
        color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        break;

    case GearState::ERROR:
        // In error state, blink red regardless of other conditions
        color = Colors::blinking(SysConst::kBlinkingIntervalMs, time, Colors::RED);
        break;
    }

    // Apply brightness setting and set LED color
    color = Colors::setBrightness(color, 100);
    mLeds.setColor(channel, color);

    }

uint32_t Application::getColorForDownState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    
    // Logic for rudder channels
    if (isRudder) {
        if (rudderSwitchState) {
            // Rudder switch active, color depends on landing gear switch
            color = ldgGearSwitchState ? cRudderInactiveColor : cRudderDownColor;
        } else {
            // Rudder switch inactive but channel is DOWN - show transition animation
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        }
    } 
    // Logic for landing gear channels
    else {
        if (ldgGearSwitchState) {
            // Landing gear switch active matches DOWN state - show normal color
            color = cLdgGearDownColor;
        } else {
            // Landing gear switch inactive but channel is DOWN - show transition animation
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        }
    }
    return color;
}

uint32_t Application::getColorForUpState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    
    // Logic for rudder channels
    if (isRudder) {
        if (rudderSwitchState) {
            // Rudder switch active but channel is UP - show transition animation
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);            
        } else {
            // Rudder switch inactive matches UP state - show normal color
            color = cLdgGearUpColor;
        }
    } 
    // Logic for landing gear channels
    else {
        if (ldgGearSwitchState) {
            // Landing gear switch active but channel is UP - show transition animation
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        } else {
            // Landing gear switch inactive matches UP state - show normal color
            color = cLdgGearUpColor;
        }
    }
    return color;
}

uint32_t Application::getColorForMovingState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    
    // For moving state, determine target color based on where it's moving to
    if (isRudder) {
        if (rudderSwitchState) {
            // Moving toward down position for rudder
            color = ldgGearSwitchState ? cRudderInactiveColor : cRudderDownColor;
        } else {
            // Moving toward up position for rudder
            color = cRudderUpColor;
        }
    } else {
        // For landing gear, determine color based on switch position
        color = ldgGearSwitchState ? cLdgGearDownColor : cLdgGearUpColor;
    }
    
    // Create blinking effect to indicate motion
    return Colors::blinking(SysConst::kBlinkingIntervalMs, time, color);
}

bool Application::relaysTest() {
    // Test all relays to verify hardware functionality
    bool result = true;
    for(size_t i = 0; i < NO_CHANNELS; ++i) {
        result &= mChannels[i].relaysTest();
    }
    return result;
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