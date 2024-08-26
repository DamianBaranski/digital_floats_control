#include "application.h"
#include "colors.h"

ControlChannelSettings settings;
Application::Application(Bsp &bsp) : mBsp(bsp), mLeds(*mBsp.leds),
                                     mChannels{ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus),
                                               ControlChannel(*mBsp.i2cBus)}
{
  mProtocol.registerCmd('v', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                        { return this->sendAppVersion(in, out, outlen); });
  mProtocol.registerCmd('r', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                        { return this->resetDevice(in, out, outlen); });
  mProtocol.registerCmd('s', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                        { return this->scanI2cDevices(in, out, outlen); });


  LOG << "I2C scan";
  for (size_t i = 0; i < 127; i++)
  {
    if (mBsp.i2cBus->isDeviceReady(i))
    {
      LOG << i;
    }
  }
    loadSettings();
}

bool waitForPushRelease(uint32_t time_ms, IGpio &pin, bool expectedState)
{
  uint32_t dt = 10;
  for (uint32_t t = 0; t < time_ms; t++)
  {
    if (pin.get() == expectedState)
    {
      return true;
    }
    sleep(dt);
  }
  return false;
}

void Application::spin()
{
  if (!mBsp.testSwitch->get())
  {
    testSwitchProcedure();
    sleep(10);
  }


  uint32_t time = getTime();
  uint32_t moving_ldg_color = 0;
  uint32_t moving_rudder_color = 0;
  if(getLdgGearSwitch()) {
    moving_ldg_color = Colors::blinking(500, time, Colors::setBrightness(Colors::GREEN, 10));
  } else {
    moving_ldg_color = Colors::blinking(500, time, Colors::setBrightness(Colors::BLUE, 10));
  }

  if(getRudderSwitch()) {
    moving_rudder_color = Colors::blinking(500, time, Colors::setBrightness(Colors::GREEN, 10));
  } else {
    moving_rudder_color = Colors::blinking(500, time, Colors::setBrightness(Colors::BLUE, 10));
  }

  for(size_t channel=0; channel<NO_CHANNELS; channel++) {
    State state = mChannels[channel].getChannelState();
    uint32_t color = 0;
    switch(state) {
      case State::DOWN:
      color = Colors::GREEN;
      break;
      case State::UP:
      color = Colors::BLUE;
      break;
      case State::MOVING:
      if(mChannels[channel].isRudder()) {
        color = moving_rudder_color;
      } else {
        color = moving_ldg_color;
      }
      break;
      case State::ERROR:
      color = Colors::blinking(500, time, Colors::RED);
      break;
    }
    mLeds.setColor(channel, color);
  }
  mLeds.update();

  char inBuff[128] = {};
  char outBuff[128] = {};
  if (UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0))
  {
    if (mProtocol.process(inBuff, outBuff, sizeof(outBuff)))
    {
      Logger() << outBuff;
    }
  }
  sleep(10);
}

bool Application::sendAppVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen)
{
  LOG << "Getting app version";
  strcpy(out.appVersion.string, APP_VER);
  outlen = sizeof(out.appVersion);
  return true;
}

bool Application::resetDevice(const InProtocolData &in, OutProtocolData &out, size_t &outlen)
{
  LOG << "Reset device";
  return true;
}

bool Application::scanI2cDevices(const InProtocolData &in, OutProtocolData &out, size_t &outlen)
{
  bool result = false;
  if (mBsp.i2cBus->isDeviceReady(in.i2cScan.i2cAddress))
  {
    result = true;
  }
  out.i2cScan.result = result;
  outlen = sizeof(out.i2cScan);
  return true;
}

void Application::testSwitchProcedure()
{
  mLeds.setColor(Colors::RED);
  mLeds.update();
  if (waitForPushRelease(50, *mBsp.testSwitch, true))
  {
    return;
  }

  mLeds.setColor(Colors::GREEN);
  mLeds.update();
  if (waitForPushRelease(50, *mBsp.testSwitch, true))
  {
    return;
  }

  mLeds.setColor(Colors::BLUE);
  mLeds.update();
  if (waitForPushRelease(50, *mBsp.testSwitch, true))
  {
    return;
  }
}

// Relays 1
// LOG: 38
// LOG: 64
// LOG: 68
//
// Relays 2
// LOG: 39
// LOG: 65
// LOG: 79
//
// Relays 3
// LOG: 37
// LOG: 76
// LOG: 77
 void Application::loadSettings() {
  Settings settings;
  settings.channelSettings[0] = {true, true, false, false, false, false, true, 64, 50, 38, 0, 160, 100, 50, 5};
  settings.channelSettings[1] = {true, false, false, false, false, false, true, 64, 50, 38, 0, 160, 100, 50, 5};
  settings.channelSettings[2] = {true, false, false, false, false, false, false, 65, 50, 39, 0, 160, 100, 50, 5};
  settings.channelSettings[3] = {true, false, false, false, false, false, false, 79, 50, 39, 1, 160, 100, 50, 5};
  settings.channelSettings[4] = {true, false, false, false, false, false, false, 76, 50, 37, 0, 160, 100, 50, 5};
  settings.channelSettings[5] = {true, false,  false, false, false, false, false, 77, 50, 37, 1, 160, 100, 50, 5};

  for(size_t i=0; i<NO_CHANNELS; i++) {
    mChannels[i].setSettings(settings.channelSettings[i]);
  }
 }

bool Application::getLdgGearSwitch() {
  return mBsp.ldgSwitch->get();
}

bool Application::getRudderSwitch() {
  return mBsp.rudSwitch->get();
}