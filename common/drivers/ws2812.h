#ifndef WS2812_H
#define WS2812_H

#include <cstddef>
#include <igpio.h>

#include "stm32f1xx_hal.h"
TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch2_ch4;
void Error_Handler()
{
    while (true)
        ;
}
static void MX_TIM2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 79;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

TIM_MasterConfigTypeDef sMasterConfig = {0};
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;//58; //20
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    hdma_tim2_ch2_ch4.Instance = DMA1_Channel7;
    hdma_tim2_ch2_ch4.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim2_ch2_ch4.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch2_ch4.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch2_ch4.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim2_ch2_ch4.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim2_ch2_ch4.Init.Mode = DMA_NORMAL;
    hdma_tim2_ch2_ch4.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_tim2_ch2_ch4) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC2], hdma_tim2_ch2_ch4);
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/// @brief A template class to control a WS2812 LED strip.
///
/// This class provides methods to set the color of individual LEDs on a WS2812 LED strip
/// and to send the updated color data to the strip using bit-banging through a GPIO interface.
///
/// @tparam S Number of LEDs in the strip.
template <size_t S>
class Ws2812
{
public:
    /// @brief Construct a new Ws2812 object.
    ///
    /// @param gpio A reference to an IGpio object to control the data line of the WS2812 strip.
    Ws2812(IGpio &gpio);

    /// @brief Sends the current color data to the WS2812 LED strip.
    ///
    /// This method iterates through the color array and sends each LED's color data
    /// (in GRB format) to the LED strip, followed by a reset signal to latch the data.
    void update();

    /// @brief Sets the color of a specific LED on the strip.
    ///
    /// @param led_id The index of the LED to set (0-based).
    /// @param red The red component of the color (0-255).
    /// @param green The green component of the color (0-255).
    /// @param blue The blue component of the color (0-255).
    void setColor(size_t led_id, uint8_t red, uint8_t green, uint8_t blue);

    /// @brief Sets the color of a specific LED on the strip using a 24-bit color value.
    ///
    /// @param led_id The index of the LED to set (0-based).
    /// @param color A 24-bit color value in the format 0xRRGGBB.
    void setColor(size_t led_id, uint32_t color);

private:

    /// @brief Structure to represent the color of an LED.
    struct Color
    {
        uint8_t red;   ///< Red component (0-255).
        uint8_t green; ///< Green component (0-255).
        uint8_t blue;  ///< Blue component (0-255).
    };

    Color mColors[S]; ///< Array to store the colors for all LEDs in the strip.
    IGpio &mGpio;     ///< Reference to the GPIO interface used to control the WS2812 strip.
    uint16_t mPwmData[24*S+50];
};

template <size_t S>
Ws2812<S>::Ws2812(IGpio &gpio) : mColors{}, mGpio(gpio), mPwmData{}
{
        MX_DMA_Init();
    MX_TIM2_Init();

}

template <size_t S>
void Ws2812<S>::update()
{
    for(size_t i=0; i<S; i++) {
        for(size_t bit=0; bit<8; bit++) {
            uint16_t value = 0;
            if(mColors[i].green & (1<<bit)) {
                value=59;
            } else {
                value = 18;
            }
            mPwmData[i*24+bit] = value;
        }
        for(size_t bit=0; bit<8; bit++) {
            uint16_t value = 0;
            if(mColors[i].red & (1<<(7-bit))) {
                value=59;
            } else {
                value = 18;
            }
            mPwmData[i*24+8+bit] = value;
        }
        for(size_t bit=0; bit<8; bit++) {
            uint16_t value = 0;
            if(mColors[i].blue & (1<<bit)) {
                value=59;
            } else {
                value = 18;
            }
            mPwmData[i*24+16+bit] = value;
        }
    }

    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, (uint32_t *)mPwmData, sizeof(mPwmData)/sizeof(mPwmData[0]));
}

template <size_t S>
void Ws2812<S>::setColor(size_t led_id, uint8_t red, uint8_t green, uint8_t blue)
{
    mColors[led_id].red = red;
    mColors[led_id].green = green;
    mColors[led_id].blue = blue;
}

template <size_t S>
void Ws2812<S>::setColor(size_t led_id, uint32_t color)
{
    mColors[led_id].red = color & 0xFF;
    mColors[led_id].green = (color >> 8) & 0xFF;
    mColors[led_id].blue = (color >> 16) & 0xFF;
}

#endif
