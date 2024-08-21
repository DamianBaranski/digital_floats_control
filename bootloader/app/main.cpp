#include "bsp.h"
#include "logger.h"
#include "protocol.h"

#define ETX_APP_START_ADDRESS 0x08005000

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

static void goto_application(void)
{
  Logger() << "Gonna Jump to Application...";
  sleep(100);
  void (*app_reset_handler)(void) = (void (*)())(*((volatile uint32_t *)(ETX_APP_START_ADDRESS + 4U)));

  if (app_reset_handler == (void (*)())0xFFFFFFFF)
  {
    Logger() << "Invalid Application... HALT!!!";
    while (1)
      ;
  }
  app_reset_handler(); // call the app reset handler
}

int main()
{
  Bsp bsp;
  UartStream logStream(*bsp.uartBus);
  Logger() << "Bootloader BS v1.0";
  Protocol protocol;
  char buff[128] = {};
  for (size_t i = 0; i < 50; i++)
  {
    memset(buff, 0, sizeof(buff));
    if (UartStream::getInstance()->readLine(buff, sizeof(buff), 0))
    {
      if(protocol.handleData(buff)) {
        i=0;
      }
    }
    else
    {
      bsp.ledPin->toggle();
      sleep(20);
    }
  }

  goto_application();
  while (true)
    ;
  return 0;
}