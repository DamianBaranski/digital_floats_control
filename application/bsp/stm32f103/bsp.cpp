#include "bsp.h"
#include "adc.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "i2c_master.h"
#include "uart.h"
#include "pwm.h"
#include "pwm_dma.h"
#include "spi.h"
#include "w25x_flash.h"

Bsp::Bsp()
{
  HAL_Init();
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_ADC1_CLK_ENABLE();

	initClock();
  mAdcPin.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_0, GPIO_MODE_ANALOG, GPIO_NOPULL, 0));
  pwr_voltage.reset(new(std::nothrow) Adc(ADC1));
	mSdaPin1.reset(new(std::nothrow) Gpio(GPIOB, GPIO_PIN_6, GPIO_MODE_AF_OD, GPIO_PULLUP, 0));
	mSclPin1.reset(new(std::nothrow) Gpio(GPIOB, GPIO_PIN_7, GPIO_MODE_AF_OD, GPIO_PULLUP, 0));
	i2cBusRelays.reset(new(std::nothrow) I2cMaster(I2C1));

  mSdaPin2.reset(new(std::nothrow) Gpio(GPIOB, GPIO_PIN_10, GPIO_MODE_AF_OD, GPIO_PULLUP, 0));
	mSclPin2.reset(new(std::nothrow) Gpio(GPIOB, GPIO_PIN_11, GPIO_MODE_AF_OD, GPIO_PULLUP, 0));
	i2cBusCurrent.reset(new(std::nothrow) I2cMaster(I2C2));

	mRxPin.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));
	mTxPin.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_NOPULL, 0));
  uartBus.reset(new(std::nothrow) Uart(USART1, 115200));

  mLedDataPin.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_1, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));

  testSwitch.reset(new(std::nothrow) Gpio(GPIOC, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0));
  ldgSwitch.reset(new(std::nothrow) Gpio(GPIOC, GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_PULLUP, 0));
  rudSwitch.reset(new(std::nothrow) Gpio(GPIOC, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0));

  leds.reset(new(std::nothrow) PwmDma(TIM2, TIM_CHANNEL_2, DMA1_Channel7, 79));

  mSpiCsPin.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0));
  mSpiClk.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));
  mSpiMiso.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_NOPULL, 0));
  mSpiMosi.reset(new(std::nothrow) Gpio(GPIOA, GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));
  mSpi.reset(new(std::nothrow) Spi(SPI1));

  extFlash.reset(new(std::nothrow) W25xFlash(*mSpi, *mSpiCsPin));

  buzzer.reset(new(std::nothrow) Pwm(TIM3, TIM_CHANNEL_3, 1000));

  mBuzzerPin.reset(new(std::nothrow) Gpio(GPIOB, GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, 0));
}
	
void Bsp::reset()
{
  NVIC_SystemReset();
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

  /** Configure the ADC clock
  */
  RCC_PeriphCLKInitTypeDef adc_clk;
  adc_clk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  adc_clk.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  HAL_RCCEx_PeriphCLKConfig(&adc_clk);
}

void sleep(uint32_t time_ms) {
    HAL_Delay(time_ms);
}

uint32_t getTime() {
  return HAL_GetTick();
}

extern "C" {
extern "C" void defaultHandler(const char *irqName) {
    while (true)
        ;
}

void SysTick_Handler() {
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

#define DEFAULT_IRQ_HANDLER(name)                     \
    extern "C" void name(void) __attribute__((weak)); \
    extern "C" void name(void) { defaultHandler(#name); }

DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(WWDG_IRQHandler)
DEFAULT_IRQ_HANDLER(PVD_IRQHandler)
DEFAULT_IRQ_HANDLER(TAMPER_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_IRQHandler)
DEFAULT_IRQ_HANDLER(FLASH_IRQHandler)
DEFAULT_IRQ_HANDLER(RCC_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI0_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI1_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI2_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI3_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI4_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC1_2_IRQHandler)
DEFAULT_IRQ_HANDLER(USB_HP_CAN1_TX_IRQHandler)
DEFAULT_IRQ_HANDLER(USB_LP_CAN1_RX0_IRQHandler)
DEFAULT_IRQ_HANDLER(CAN1_RX1_IRQHandler)
DEFAULT_IRQ_HANDLER(CAN1_SCE_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI9_5_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_BRK_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_UP_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_TRG_COM_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_CC_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM2_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM3_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM4_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI2_IRQHandler)
DEFAULT_IRQ_HANDLER(USART2_IRQHandler)
DEFAULT_IRQ_HANDLER(USART3_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI15_10_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_Alarm_IRQHandler)
DEFAULT_IRQ_HANDLER(USBWakeUp_IRQHandler)
}
