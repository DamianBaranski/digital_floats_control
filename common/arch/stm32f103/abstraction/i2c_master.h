#ifndef I2CMASTER_H
#define I2CMASTER_H

#include "ii2c_master.h"
#include "stm32f1xx_hal.h"

class I2cMaster: public II2cMaster {
    public:
    I2cMaster(I2C_TypeDef *instance);
    ~I2cMaster();

    virtual bool transmit() override {return false;}
    virtual bool isDeviceReady(uint8_t addr) const override;
    virtual bool write(uint8_t addr, const uint8_t *data, uint8_t len) override;
    virtual bool read(uint8_t addr, uint8_t *data, uint8_t len) override;
    virtual bool writeRegister(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t len) override;
    virtual bool readRegister(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) override;

    private:
    mutable I2C_HandleTypeDef mI2cHandler;
};

#endif