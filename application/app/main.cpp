#include "bsp.h"
#include "logger.h"
#include "protocol2.h"
#include "control_channel.h"
#include "stm32f1xx_hal.h"

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

void i2c_scanner(const II2cMaster &i2c) {
 	for (uint8_t i=1; i<128; i++)
 	{
    if(i2c.isDeviceReady(i)) {
      Logger() << "\r\nFound:" << i << "\r\n";
    } else {
      Logger() << ".";
    }
    HAL_Delay(100);
  }
  Logger() << "\r\n";
}

int main()
{
  Bsp bsp;
  UartStream logStream(*bsp.uartBus);
  ControlChannelSettings settings = {};
  settings.enable = true;
  //settings.ina_addr = 0x41;
  settings.ina_addr = 0x44;
  settings.pcf_addr = 0x27;
  settings.pcf_channel = 1;
  settings.ina_callibration = 50; //50mOhm
  settings.max_current_limit = 50; //5A
  settings.min_current_limit = 5;  //0.5A
  settings.max_voltage_limit = 160; //16V
  settings.min_voltage_limit = 100; //10V
  
  ControlChannel controlChannel(settings, *bsp.i2cBus);
  LOG << "Application BS";
  //i2c_scanner(*bsp.i2cBus);
  Application app;

  /*if(controlChannel.connectionTest()) {
    LOG << "Connetion ok";
  } else {
    LOG << "Connetion fail";  
  }*/

bool dir = true;
  while (true)
  {
    bsp.ledPin->toggle();
    controlChannel.setMotor(false, 1, dir);
    dir = !dir;
    controlChannel.setMotor(true, 1, dir);
    LOG << "Set motor:" << dir;
    sleep(100);
    controlChannel.getPowerSensorStatus();
    sleep(5000);
    app.spin();
  
  }

  return 0;
}