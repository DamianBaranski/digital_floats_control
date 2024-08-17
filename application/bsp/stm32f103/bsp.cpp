#include "bsp.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "i2c_master.h"
#include "uart.h"

Bsp::Bsp()
{
  HAL_Init();
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
	initClock();
	mSdaPin.reset(new Gpio(GPIOB, GPIO_PIN_6, GPIO_MODE_AF_OD, GPIO_PULLUP, 0));
	mSclPin.reset(new Gpio(GPIOB, GPIO_PIN_7, GPIO_MODE_AF_OD, GPIO_PULLUP, 0));
	i2cBus.reset(new I2cMaster(I2C1));

	mRxPin.reset(new Gpio(GPIOA, GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));
	mTxPin.reset(new Gpio(GPIOA, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_NOPULL, 0));
  uartBus.reset(new Uart(USART1, 115200));

  ledPin.reset(new Gpio(GPIOC, GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0));
  ledPin->reset();
}
	

void Bsp::initClock()
{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

extern "C"
{
  void SysTick_Handler()
  {
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
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
