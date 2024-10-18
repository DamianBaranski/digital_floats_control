#include "application.h"
#include "colors.h"
#include "version.h"

Application::Application(Bsp &bsp) : mBsp(bsp), mLeds(*mBsp.leds),
                                     mChannels{ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus)},
                                     mChannelsSettings(*mBsp.extFlash, 4096), mUserSettings(*mBsp.extFlash, 0) {
    mProtocol.registerCmd('v', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendAppVersion(in, out, outlen); });
    mProtocol.registerCmd('r', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->resetDevice(in, out, outlen); });
    mProtocol.registerCmd('s', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->scanI2cDevices(in, out, outlen); });
    mProtocol.registerCmd('u', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendUserSettings(in, out, outlen); });
    mProtocol.registerCmd('U', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->updateUserSettings(in, out, outlen); });
    mProtocol.registerCmd('c', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendChannelSettings(in, out, outlen); });
    mProtocol.registerCmd('C', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->updateChannelSettings(in, out, outlen); });
    mProtocol.registerCmd('m', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendMonitoringData(in, out, outlen); });
    mProtocol.registerCmd('t', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->setTestChannel(in, out, outlen); });

    loadSettings();
    setBrightness();
    relaysTest();
}

bool waitForPushRelease(uint32_t time_ms, IGpio &pin, bool expectedState) {
    uint32_t dt = 10;
    for (uint32_t t = 0; t < time_ms; t += dt) {
        if (pin.get() == expectedState) {
            return true;
        }
        sleep(dt);
    }
    return false;
}

void Application::spin() {
    if (!mBsp.testSwitch->get()) {
        testSwitchProcedure();
        sleep(10);
        return;
    }

    uint32_t time = getTime();
    bool ldgGearSwitchState = getLdgGearSwitch();
    bool rudderSwitchState = getRudderSwitch();

    for (size_t channel = 0; channel < NO_CHANNELS; ++channel) {
        processChannel(channel, rudderSwitchState, ldgGearSwitchState, time);
    }

    mLeds.update();

    handleUartCommunication();
    sleep(100);
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
    if(channel>=NO_CHANNELS) {
        return false;
    }
    outlen = sizeof(out.controlChannelSettings.settings);
    memcpy(&out.raw, &mChannelsSettings.get().channelSettings[channel], outlen);
    out.controlChannelSettings.channel = channel;
    return true;
}

bool Application::updateChannelSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    uint8_t channel = in.controlChannelSettings.channel;
    if(channel>=NO_CHANNELS) {
        return false;
    }
    mChannelsSettings.get().channelSettings[channel] = in.controlChannelSettings.settings;
    bool result = mChannelsSettings.save();
    out.result = result;
    outlen = sizeof(out.result);
    for (size_t i = 0; i < NO_CHANNELS; ++i) {
        mChannels[i].setSettings(mChannelsSettings.get().channelSettings[i]);
    }
    return true;
}

bool Application::sendMonitoringData(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    uint8_t channel_id = in.channel_id;
    uint16_t current=0, voltage=0;
    mChannels[channel_id].getPowerSensorStatus(voltage, current);
    out.monitoringData.current = current;
    out.monitoringData.voltage = voltage;
    out.monitoringData.state = static_cast<uint8_t>(mChannels[channel_id].getChannelState());
    outlen = sizeof(out.monitoringData);
    return true;
}

bool Application::setTestChannel(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    ControlChannel testChannel(*mBsp.i2cBus.get());
    ControlChannelSettings settings = {};
    settings.enable = true;
    settings.ina_addr = in.channelTest.ina_addr;
    settings.ina_callibration = 500;
    settings.pcf_addr = in.channelTest.pcf_addr;
    settings.pcf_channel = in.channelTest.pcf_channel;
    settings.max_voltage_limit = 280;
    settings.min_voltage_limit = 80;
    testChannel.setSettings(settings);
    bool ret = testChannel.relaysTest();
    out.result = ret;
    outlen = sizeof(ret);
    return true;
}

void Application::testSwitchProcedure() {
    const uint32_t colors[] = {Colors::RED, Colors::GREEN, Colors::BLUE};

    for (uint32_t color : colors) {
        mLeds.setColor(color);
        mLeds.update();
        if (waitForPushRelease(500, *mBsp.testSwitch, true)) {
            return;
        }
    }
}

void Application::loadSettings() {
    mUserSettings.load();
    mChannelsSettings.load();
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
    State state = mChannels[channel].getChannelState();
    uint32_t color = 0;

    bool isRudder = mChannels[channel].isRudder();
    if (isRudder) {
        mChannels[channel].setMotor(rudderSwitchState);
    } else {
        mChannels[channel].setMotor(ldgGearSwitchState);
    }

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
        color = Colors::blinking(500, time, Colors::RED);
        break;
    }

    color = Colors::setBrightness(color, mUserSettings.get().brightness);
    mLeds.setColor(channel, color);
}

uint32_t Application::getColorForDownState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    if (isRudder) {
        if (rudderSwitchState) {
            color = ldgGearSwitchState ? mUserSettings.get().rudderInactiveColor : mUserSettings.get().rudderDownColor;
        } else {
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        }
    } else {
        if (ldgGearSwitchState) {
            color = mUserSettings.get().ldgDownColor;
        } else {
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        }
    }
    return color;
}

uint32_t Application::getColorForUpState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    if (isRudder) {
        if (rudderSwitchState) {
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);            
        } else {
            color = mUserSettings.get().rudderUpColor;
        }
    } else {
        if (ldgGearSwitchState) {
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        } else {
            color = mUserSettings.get().ldgUpColor;
        }
    }
    return color;
}

uint32_t Application::getColorForMovingState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    if (isRudder) {
        if (rudderSwitchState) {
            color = ldgGearSwitchState ? mUserSettings.get().rudderInactiveColor : mUserSettings.get().rudderDownColor;
        } else {
            color = mUserSettings.get().rudderUpColor;
        }
    } else {
        color = ldgGearSwitchState ? mUserSettings.get().ldgDownColor : mUserSettings.get().ldgUpColor;
    }
    return Colors::blinking(500, time, color);
}

bool Application::relaysTest() {
    bool result = true;
    for(size_t i=0; i<NO_CHANNELS; ++i) {
        result &= mChannels[i].relaysTest();
    }
    return result;
}

void Application::handleUartCommunication() {
    char inBuff[128] = {};
    char outBuff[128] = {};

    if (UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0)) {
        if (mProtocol.process(inBuff, outBuff, sizeof(outBuff))) {
            Logger() << outBuff;
        }
    }
}

void Application::setBrightness() {
    if (waitForPushRelease(5000, *mBsp.testSwitch, true)) {
        return;
    }

    for (size_t i = 0; i < 10; i++) {
        for (uint8_t b = 0xF; b < 0xFF; b += 0x18) {
            mLeds.setColor(Colors::setBrightness(Colors::WHITE, b));
            mLeds.update();
            if (waitForPushRelease(500, *mBsp.testSwitch, true)) {
                mUserSettings.get().brightness = b;
                mUserSettings.save();
                return;
            }
        }
    }
}