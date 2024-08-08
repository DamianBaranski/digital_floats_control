#include "bsp.h"
#include "logger.h"
#include "protocol2.h"

#define APP_VER "Application BS v1.0"
UartStream *UartStream::mInstance = nullptr;

void sleep(size_t time)
{
  for (; time > 0; time--)
  {
    for (size_t i = 0; i < 1000; i++)
    {
      asm("nop");
    }
  }
}

class Application
{
private:
  union InProtocolData
  {
    size_t fileSize;
  };

  union OutProtocolData
  {
    char string[32];
  };

public:
  Application()
  {
    mProtocol.registerCmd('v', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                          { return this->sendAppVersion(in, out, outlen); });
    mProtocol.registerCmd('r', [this](const InProtocolData &in, OutProtocolData &out, size_t &outlen)
                          { return this->resetDevice(in, out, outlen); });
  }

  void spin() {
    char inBuff[128] = {};
    char outBuff[128] = {};
    if (UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0))
    {
      if(mProtocol.process(inBuff, outBuff, sizeof(outBuff))) {
        Logger() << outBuff;
      }
    }
  }

private:
  bool sendAppVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen)
  {
    LOG << "Getting app version";
    strcpy(out.string, APP_VER);
    outlen = strlen(APP_VER);
    return true;
  }

  bool resetDevice(const InProtocolData &in, OutProtocolData &out, size_t &outlen)
  {
    LOG << "Reset device";
    return true;
  }

private:
  Protocol<InProtocolData, OutProtocolData, 10> mProtocol;
};

int main()
{
  Bsp bsp;
  UartStream logStream(*bsp.uartBus);
  Logger() << "Application BS";
  Application app;

  while (true)
  {
    bsp.ledPin->toggle();
    sleep(1000);
    app.spin();
  }

  return 0;
}

extern "C"
{
  void SysTick_Handler()
  {
  }
}
