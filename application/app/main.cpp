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


  Ws2812<6> leds(*bsp.ledDataPin);
  sleep(100);

  
  for(size_t i=0; i<6; i++) {
    leds.setColor(i, 0xFF, 0x00, 0x00);
  }
  leds.update();
  ControlChannel controlChannel(settings, *bsp.i2cBus);
  LOG << "Application BS";
  Application app(bsp);

  bool dir = true;
  uint32_t color = 0xFF;
  while (true)
  {
    for(size_t i=0; i<5; i++) {
    leds.setColor(i, color);
  color = color << 8;
  if(color == 0xFF000000) {
    color = 0xFF;
  }

  }

    leds.update();
    bsp.ledPin->toggle();
    controlChannel.setMotor(false, 1, dir);
    dir = !dir;
    controlChannel.setMotor(true, 1, dir);
    LOG << "Set motor:" << dir;
    sleep(100);
    controlChannel.getPowerSensorStatus();
    sleep(50000);
    app.spin();
  }

  return 0;
}