#include "application.h"
#include "colors.h"
#include "version.h"

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

    // Buffer sizes
    constexpr size_t kUartBufferSize = 128;

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
                                     mChannels{ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus)},
                                     mChannelsSettings(*mBsp.extFlash, SysConst::kChannelSettingsFlashAddress), 
                                     mUserSettings(*mBsp.extFlash, SysConst::kUserSettingsFlashAddress) {
    // Register command handlers for protocol communications
    mProtocol.registerCmd('v', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendAppVersion(in, out, outlen); });
    mProtocol.registerCmd('r', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->resetDevice(in, out, outlen); });
    mProtocol.registerCmd('s', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->scanI2cDevices(in, out, outlen); });
    mProtocol.registerCmd('u', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendUserSettings(in, out, outlen); });
    mProtocol.registerCmd('U', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->updateUserSettings(in, out, outlen); });
    mProtocol.registerCmd('c', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendChannelSettings(in, out, outlen); });
    mProtocol.registerCmd('C', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->updateChannelSettings(in, out, outlen); });
    mProtocol.registerCmd('m', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendMonitoringData(in, out, outlen); });
    mProtocol.registerCmd('t', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->setTestChannel(in, out, outlen); });

    // Initialize application state
    loadSettings();
    setBrightness();
    relaysTest();
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
    // Test mode detection - check if test switch is active
    if (!mBsp.testSwitch->get()) {
        testSwitchProcedure();
        sleep(SysConst::kTestSwitchSleepMs);
        return;
    }

    // Get current system state
    uint32_t time = getTime();
    bool ldgGearSwitchState = getLdgGearSwitch();
    bool rudderSwitchState = getRudderSwitch();

    // Process all channels with current switch states
    for (size_t channel = 0; channel < NO_CHANNELS; ++channel) {
        processChannel(channel, rudderSwitchState, ldgGearSwitchState, time);
    }

    // Update LED indicators
    mLeds.update();

    // Process communication
    handleUartCommunication();
    sleep(SysConst::kDefaultSleepMs);
}

bool Application::sendAppVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    LOG << "Getting app version";
    strcpy(out.appVersion.string, APP_VER);
    outlen = sizeof(out.appVersion);
    return true;
}

bool Application::resetDevice(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    LOG << "Reset device";
    mBsp.reset();
    return true;
}

bool Application::scanI2cDevices(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    bool result = mBsp.i2cBus->isDeviceReady(in.i2cScan.i2cAddress);
    if(result) {
        LOG << "Found I2C device:" << in.i2cScan.i2cAddress;
    }
    out.i2cScan.result = result;
    outlen = sizeof(out.i2cScan);
    return true;
}

bool Application::sendUserSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    memcpy(&out.userSettings,&mUserSettings.get(), sizeof(UserSettings));
    outlen = sizeof(UserSettings);
    return true;
}

bool Application::updateUserSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    mUserSettings.get() = in.userSettings;
    bool result = mUserSettings.save();
    out.result = result;
    outlen = sizeof(out.result);
    return true;
}

bool Application::sendChannelSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    uint8_t channel = in.channel_id;
    if(channel >= NO_CHANNELS) {
        return false;
    }
    outlen = sizeof(out.controlChannelSettings.settings);
    memcpy(&out.raw, &mChannelsSettings.get().channelSettings[channel], outlen);
    out.controlChannelSettings.channel = channel;
    return true;
}

bool Application::updateChannelSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    uint8_t channel = in.controlChannelSettings.channel;
    if(channel >= NO_CHANNELS) {
        return false;
    }
    
    // Update settings in storage
    mChannelsSettings.get().channelSettings[channel] = in.controlChannelSettings.settings;
    bool result = mChannelsSettings.save();
    out.result = result;
    outlen = sizeof(out.result);
    
    // Apply settings to all channels
    for (size_t i = 0; i < NO_CHANNELS; ++i) {
        mChannels[i].setSettings(mChannelsSettings.get().channelSettings[i]);
    }
    return true;
}

bool Application::sendMonitoringData(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    uint8_t channel_id = in.channel_id;
    uint16_t current = 0, voltage = 0;
    
    // Read power sensor values for requested channel
    mChannels[channel_id].getPowerSensorStatus(voltage, current);
    
    // Prepare response
    out.monitoringData.current = current;
    out.monitoringData.voltage = voltage;
    out.monitoringData.state = static_cast<uint8_t>(mChannels[channel_id].getChannelState());
    outlen = sizeof(out.monitoringData);
    return true;
}

bool Application::setTestChannel(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    // Create temporary channel for testing with default settings
    ControlChannel testChannel(*mBsp.i2cBus.get());
    ControlChannelSettings settings = {};
    settings.enable = true;
    settings.ina_addr = in.channelTest.ina_addr;
    settings.ina_callibration = SysConst::kDefaultInaCalibration;
    settings.pcf_addr = in.channelTest.pcf_addr;
    settings.pcf_channel = in.channelTest.pcf_channel;
    settings.max_voltage_limit = SysConst::kMaxVoltageLimitMv;
    settings.min_voltage_limit = SysConst::kMinVoltageLimitMv;
    
    // Apply settings and run test
    testChannel.setSettings(settings);
    bool ret = testChannel.relaysTest();
    
    // Return result
    out.result = ret;
    outlen = sizeof(ret);
    return true;
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
    mUserSettings.load();
    mChannelsSettings.load();
    
    // Apply channel settings to all channels
    for (size_t i = 0; i < NO_CHANNELS; ++i) {
        mChannels[i].setSettings(mChannelsSettings.get().channelSettings[i]);
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
    State state = mChannels[channel].getChannelState();
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
    case State::DOWN:
        color = getColorForDownState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        break;

    case State::UP:
        color = getColorForUpState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        break;

    case State::MOVING:
        color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        break;

    case State::ERROR:
        // In error state, blink red regardless of other conditions
        color = Colors::blinking(SysConst::kBlinkingIntervalMs, time, Colors::RED);
        break;
    }

    // Apply brightness setting and set LED color
    color = Colors::setBrightness(color, mUserSettings.get().brightness);
    mLeds.setColor(channel, color);
}

uint32_t Application::getColorForDownState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    
    // Logic for rudder channels
    if (isRudder) {
        if (rudderSwitchState) {
            // Rudder switch active, color depends on landing gear switch
            color = ldgGearSwitchState ? mUserSettings.get().rudderInactiveColor : mUserSettings.get().rudderDownColor;
        } else {
            // Rudder switch inactive but channel is DOWN - show transition animation
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        }
    } 
    // Logic for landing gear channels
    else {
        if (ldgGearSwitchState) {
            // Landing gear switch active matches DOWN state - show normal color
            color = mUserSettings.get().ldgDownColor;
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
            color = mUserSettings.get().rudderUpColor;
        }
    } 
    // Logic for landing gear channels
    else {
        if (ldgGearSwitchState) {
            // Landing gear switch active but channel is UP - show transition animation
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        } else {
            // Landing gear switch inactive matches UP state - show normal color
            color = mUserSettings.get().ldgUpColor;
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
            color = ldgGearSwitchState ? mUserSettings.get().rudderInactiveColor : mUserSettings.get().rudderDownColor;
        } else {
            // Moving toward up position for rudder
            color = mUserSettings.get().rudderUpColor;
        }
    } else {
        // For landing gear, determine color based on switch position
        color = ldgGearSwitchState ? mUserSettings.get().ldgDownColor : mUserSettings.get().ldgUpColor;
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

void Application::handleUartCommunication() {
    // Process UART communication if available
    char inBuff[SysConst::kUartBufferSize] = {};
    char outBuff[SysConst::kUartBufferSize] = {};

    if (UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0)) {
        if (mProtocol.process(inBuff, outBuff, sizeof(outBuff))) {
            Logger() << outBuff;
        }
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
                mUserSettings.get().brightness = b;
                mUserSettings.save();
                return;
            }
        }
    }
}