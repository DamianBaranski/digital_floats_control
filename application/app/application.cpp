#include "application.h"
#include "colors.h"

ControlChannelSettings settings;

Application::Application(Bsp &bsp) : mBsp(bsp), mLeds(*mBsp.leds),
                                     mChannels{ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus)},
                                     mChannelsSettings(*mBsp.extFlash, 1024), mUserSettings(*mBsp.extFlash, 0) {
    mProtocol.registerCmd('v', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->sendAppVersion(in, out, outlen); });
    mProtocol.registerCmd('r', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->resetDevice(in, out, outlen); });
    mProtocol.registerCmd('s', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen) { return this->scanI2cDevices(in, out, outlen); });

    LOG << "I2C scan";
    for (size_t i = 0; i < 127; i++) {
        if (mBsp.i2cBus->isDeviceReady(i)) {
            LOG << i;
        }
    }

    loadSettings();
    setBrightness();
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
    sleep(1);
}

bool Application::sendAppVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    LOG << "Getting app version";
    strcpy(out.appVersion.string, APP_VER);
    outlen = sizeof(out.appVersion);
    return true;
}

bool Application::resetDevice(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    LOG << "Reset device";
    return true;
}

bool Application::scanI2cDevices(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    bool result = mBsp.i2cBus->isDeviceReady(in.i2cScan.i2cAddress);
    out.i2cScan.result = result;
    outlen = sizeof(out.i2cScan);
    return result;
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
    mChannelsSettings.get().channelSettings[0] = {true, true, false, false, false, false, true, 64, 50, 38, 0, 160, 100, 50, 5};
    mChannelsSettings.get().channelSettings[1] = {true, false, false, false, false, false, true, 64, 50, 38, 0, 160, 100, 50, 5};
    mChannelsSettings.get().channelSettings[2] = {true, false, false, false, false, false, false, 65, 50, 39, 0, 160, 100, 50, 5};
    mChannelsSettings.get().channelSettings[3] = {true, false, false, false, false, false, false, 79, 50, 39, 1, 160, 100, 50, 5};
    mChannelsSettings.get().channelSettings[4] = {true, false, false, false, false, false, false, 76, 50, 37, 0, 160, 100, 50, 5};
    mChannelsSettings.get().channelSettings[5] = {true, false, false, false, false, false, false, 77, 50, 37, 1, 160, 100, 50, 5};
    //mChannelsSettings.save();

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
            color = ldgGearSwitchState ? Colors::ORANGE : Colors::BLUE;
        } else {
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        }
    } else {
        if (ldgGearSwitchState) {
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        } else {
            color = Colors::GREEN;
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
            color = Colors::GREEN;
        }
    } else {
        if (ldgGearSwitchState) {
            color = Colors::BLUE;
        } else {
            color = getColorForMovingState(isRudder, rudderSwitchState, ldgGearSwitchState, time);
        }
    }
    return color;
}

uint32_t Application::getColorForMovingState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time) {
    uint32_t color = 0;
    if (isRudder) {
        if (rudderSwitchState) {
            color = ldgGearSwitchState ? Colors::ORANGE : Colors::BLUE;
        } else {
            color = Colors::GREEN;
        }
    } else {
        color = ldgGearSwitchState ? Colors::GREEN : Colors::BLUE;
    }
    return Colors::blinking(500, time, color);
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