#include "bsp.h"
#include "logger.h"
#include "protocol.h"

#define ETX_APP_START_ADDRESS 0x08004000

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
      i=0;
      //Logger()<< "Reecived:" << buff;
      protocol.handleData(buff);
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

extern "C"
{
  void SysTick_Handler()
  {
  }

  void NMI_Handler(void)
  {
    while (true)
      ;
  }
  void HardFault_Handler(void)
  {
    while (true)
      ;
  }
  void MemManage_Handler(void)
  {
    while (true)
      ;
  }
  void BusFault_Handler(void)
  {
    while (true)
      ;
  }
  void UsageFault_Handler(void)
  {
    while (true)
      ;
  }
  void SVC_Handler(void)
  {
    while (true)
      ;
  }
  void DebugMon_Handler(void)
  {
    while (true)
      ;
  }
  void PendSV_Handler(void)
  {
    while (true)
      ;
  }
  void WWDG_IRQHandler(void)
  {
    while (true)
      ;
  }
  void PVD_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TAMPER_IRQHandler(void)
  {
    while (true)
      ;
  }
  void RTC_IRQHandler(void)
  {
    while (true)
      ;
  }
  void FLASH_IRQHandler(void)
  {
    while (true)
      ;
  }
  void RCC_IRQHandler(void)
  {
    while (true)
      ;
  }
  void EXTI0_IRQHandler(void)
  {
    while (true)
      ;
  }
  void EXTI1_IRQHandler(void)
  {
    while (true)
      ;
  }
  void EXTI2_IRQHandler(void)
  {
    while (true)
      ;
  }
  void EXTI3_IRQHandler(void)
  {
    while (true)
      ;
  }
  void EXTI4_IRQHandler(void)
  {
    while (true)
      ;
  }
  void DMA1_Channel1_IRQHandler(void)
  {
    while (true)
      ;
  }
  void DMA1_Channel2_IRQHandler(void)
  {
    while (true)
      ;
  }
  void DMA1_Channel3_IRQHandler(void)
  {
    while (true)
      ;
  }
  void DMA1_Channel4_IRQHandler(void)
  {
    while (true)
      ;
  }
  void DMA1_Channel5_IRQHandler(void)
  {
    while (true)
      ;
  }
  void DMA1_Channel6_IRQHandler(void)
  {
    while (true)
      ;
  }
  void DMA1_Channel7_IRQHandler(void)
  {
    while (true)
      ;
  }
  void ADC1_2_IRQHandler(void)
  {
    while (true)
      ;
  }
  void USB_HP_CAN1_TX_IRQHandler(void)
  {
    while (true)
      ;
  }
  void USB_LP_CAN1_RX0_IRQHandler(void)
  {
    while (true)
      ;
  }
  void CAN1_RX1_IRQHandler(void)
  {
    while (true)
      ;
  }
  void CAN1_SCE_IRQHandler(void)
  {
    while (true)
      ;
  }
  void EXTI9_5_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TIM1_BRK_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TIM1_UP_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TIM1_TRG_COM_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TIM1_CC_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TIM2_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TIM3_IRQHandler(void)
  {
    while (true)
      ;
  }
  void TIM4_IRQHandler(void)
  {
    while (true)
      ;
  }
  void I2C1_EV_IRQHandler(void)
  {
    while (true)
      ;
  }
  void I2C1_ER_IRQHandler(void)
  {
    while (true)
      ;
  }
  void I2C2_EV_IRQHandler(void)
  {
    while (true)
      ;
  }
  void I2C2_ER_IRQHandler(void)
  {
    while (true)
      ;
  }
  void SPI1_IRQHandler(void)
  {
    while (true)
      ;
  }
  void SPI2_IRQHandler(void)
  {
    while (true)
      ;
  }
  void USART2_IRQHandler(void)
  {
    while (true)
      ;
  }
  void USART3_IRQHandler(void)
  {
    while (true)
      ;
  }
  void EXTI15_10_IRQHandler(void)
  {
    while (true)
      ;
  }
  void RTC_Alarm_IRQHandler(void)
  {
    while (true)
      ;
  }
  void USBWakeUp_IRQHandler(void)
  {
    while (true)
      ;
  }
}
