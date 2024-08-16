#include "application.h"

Application::Application(Bsp &bsp) : mBsp(bsp)
  {
    mProtocol.registerCmd('v', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                          { return this->sendAppVersion(in, out, outlen); });
    mProtocol.registerCmd('r', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                          { return this->resetDevice(in, out, outlen); });
    mProtocol.registerCmd('s', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                          { return this->scanI2cDevices(in, out, outlen); });
  }

  void Application::spin()
  {
    char inBuff[128] = {};
    char outBuff[128] = {};
    if (UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0))
    {
      if (mProtocol.process(inBuff, outBuff, sizeof(outBuff)))
      {
        Logger() << outBuff;
      }
    }
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
    if (mBsp.i2cBus->isDeviceReady(in.i2cScan.i2cAddress)) {
      result = true;
    }
    out.i2cScan.result = result;
    outlen = sizeof(out.i2cScan);
    return true;
  }