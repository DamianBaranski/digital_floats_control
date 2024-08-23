#include "bsp.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "i2c_master.h"
#include "uart.h"
#include "pwm_dma.h"

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

  mLedDataPin.reset(new Gpio(GPIOA, GPIO_PIN_1, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));

  testSwitch.reset(new Gpio(GPIOC, GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));
  ldgSwitch.reset(new Gpio(GPIOC, GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_NOPULL, 0));
  rudSwitch.reset(new Gpio(GPIOC, GPIO_PIN_4, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));

  leds.reset(new PwmDma(TIM2, TIM_CHANNEL_2, DMA1_Channel7, 79));
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

extern "C"
{
  void defaultHandler() {
    while(true);
  }

  void SysTick_Handler()
  {
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
  }
  
  void NMI_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void HardFault_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void MemManage_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void BusFault_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void UsageFault_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void SVC_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void DebugMon_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void PendSV_Handler(void) __attribute__((weak, alias("defaultHandler")));
  void WWDG_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void PVD_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TAMPER_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void RTC_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void FLASH_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void RCC_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void EXTI0_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void EXTI1_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void EXTI2_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void EXTI3_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void EXTI4_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void ADC1_2_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void EXTI9_5_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TIM1_BRK_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TIM1_UP_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TIM2_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TIM3_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void TIM4_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void I2C1_EV_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void I2C1_ER_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void I2C2_EV_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void I2C2_ER_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void SPI1_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void SPI2_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void USART2_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void USART3_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void RTC_Alarm_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
  void USBWakeUp_IRQHandler(void) __attribute__((weak, alias("defaultHandler")));
}
