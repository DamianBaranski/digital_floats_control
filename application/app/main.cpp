#include "bsp.h"
#include "logger.h"
#include "application.h"

UartStream *UartStream::mInstance = nullptr;

int main()
{
  Bsp bsp;
  UartStream logStream(*bsp.uartBus);

  LOG << "Application BS";
  Application app(bsp);
  
  while(true) {
    app.spin();
  }

  return 0;
}