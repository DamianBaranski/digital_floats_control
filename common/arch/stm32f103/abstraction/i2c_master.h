#ifndef I2CMASTER_H
#define I2CMASTER_H

#include "ii2c_master.h"
#include "stm32f1xx_hal.h"

class I2cMaster: public II2cMaster {
    public:
    I2cMaster(I2C_TypeDef *instance);
    ~I2cMaster();

    virtual bool transmit() override {return false;};

    private:
    I2C_HandleTypeDef mI2cHandler;
};

#endif