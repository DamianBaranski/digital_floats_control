#include "gpio.h"

Gpio::Gpio(GPIO_TypeDef *port, uint16_t pin, uint32_t mode, uint8_t pull, uint8_t alternative) : mPort(port), mPin(pin)
{
    if (port == GPIOA)
    {
        __GPIOA_CLK_ENABLE();
    }
    else if (port == GPIOB)
    {
        __GPIOB_CLK_ENABLE();
    }
    else if (port == GPIOC)
    {
        __GPIOC_CLK_ENABLE();
    }
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = mPin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pull;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    //GPIO_InitStruct.Alternate = alternative;
    HAL_GPIO_Init(mPort, &GPIO_InitStruct);
}

Gpio::~Gpio()
{
    HAL_GPIO_DeInit(mPort, mPin);
}

void Gpio::set(bool state)
{
    HAL_GPIO_WritePin(mPort, mPin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool Gpio::get() const
{
    return HAL_GPIO_ReadPin(mPort, mPin) == GPIO_PIN_SET;
}