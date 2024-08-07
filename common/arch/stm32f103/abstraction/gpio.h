#ifndef GPIO_H
#define GPIO_H
#include "igpio.h"
#include "stm32f1xx_hal.h"

class Gpio: public IGpio
{
public:
Gpio(GPIO_TypeDef* port, uint16_t pin, uint32_t mode, uint8_t pull=0, uint8_t alternative=0);
~Gpio();
    virtual void set(bool state);
    virtual bool get() const;
private:
GPIO_TypeDef* mPort;
uint16_t mPin;
};

#endif