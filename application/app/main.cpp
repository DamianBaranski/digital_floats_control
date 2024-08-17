#include "bsp.h"
#include "logger.h"
#include "protocol2.h"
#include "control_channel.h"
#include "application.h"
#include "ws2812.h"

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

int main()
{
  Bsp bsp;
  UartStream logStream(*bsp.uartBus);
  ControlChannelSettings settings = {};
  settings.enable = true;
  // settings.ina_addr = 0x41;
  settings.ina_addr = 0x44;
  settings.pcf_addr = 0x27;
  settings.pcf_channel = 1;
  settings.ina_callibration = 50;   // 50mOhm
  settings.max_current_limit = 50;  // 5A
  settings.min_current_limit = 5;   // 0.5A
  settings.max_voltage_limit = 160; // 16V
  settings.min_voltage_limit = 100; // 10V

  ControlChannel controlChannel(settings, *bsp.i2cBus);
  LOG << "Application BS";
  Application app(bsp);

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