#include "bsp.h"
#include "logger.h"
#include "protocol2.h"
#include "application.h"
#include "ws2812.h"

UartStream *UartStream::mInstance = nullptr;

int main()
{
  Bsp bsp;
  UartStream logStream(*bsp.uartBus);

  LOG << "Application BS";
  Application app(bsp);
  uint8_t data;
  bsp.extFlash->read(0, &data, 1);

  while(true) {
    app.spin();
  }

  return 0;
}