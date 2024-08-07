#include "bsp.h"
#include "logger.h"

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
  Logger() << "Application BS";
  while(true) {
    bsp.ledPin->toggle();
    sleep(1000);
  }

  return 0;
}

extern "C"
{
  void SysTick_Handler()
  {
  }
}
