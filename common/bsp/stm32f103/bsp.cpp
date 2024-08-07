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
