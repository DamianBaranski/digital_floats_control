#include "i2c_master.h"
#include "cassert"

I2cMaster::I2cMaster(I2C_TypeDef *instance)
{
    if (instance == I2C1)
    {
        __I2C1_CLK_ENABLE();
    }
    else if (instance == I2C2)
    {
        __I2C2_CLK_ENABLE();
    }
    else
    {
        assert("Unsuported I2C instance");
    }

    mI2cHandler.Instance = instance;
    mI2cHandler.Init.ClockSpeed = 100000;
    mI2cHandler.Init.DutyCycle = I2C_DUTYCYCLE_2;
    mI2cHandler.Init.OwnAddress1 = 0;
    mI2cHandler.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    mI2cHandler.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    mI2cHandler.Init.OwnAddress2 = 0;
    mI2cHandler.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    mI2cHandler.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    HAL_I2C_Init(&mI2cHandler);
}

I2cMaster::~I2cMaster()
{
    if (mI2cHandler.Instance == I2C1)
    {
        __I2C1_CLK_DISABLE();
    }
    else if (mI2cHandler.Instance == I2C2)
    {
        __I2C2_CLK_DISABLE();
    }
    else
    {
        assert("Unsuported I2C instance");
    }
}

bool I2cMaster::isDeviceReady(uint8_t addr) const
{
    HAL_StatusTypeDef result;
    result = HAL_I2C_IsDeviceReady(&mI2cHandler, (uint16_t)(addr << 1), 1, 1);
    return result == HAL_OK;
}

bool I2cMaster::write(uint8_t addr, const uint8_t *data, uint8_t len) {
    HAL_StatusTypeDef result;
    result = HAL_I2C_Master_Transmit(&mI2cHandler, addr<<1, const_cast<uint8_t*>(data), len, 100);
    return result == HAL_OK;
}

bool I2cMaster::read(uint8_t addr, uint8_t *data, uint8_t len) {
    HAL_StatusTypeDef result;
    result = HAL_I2C_Master_Receive(&mI2cHandler, addr<<1, data, len, 100);
    return result == HAL_OK;
}

bool I2cMaster::writeRegister(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t len) {
    HAL_StatusTypeDef result;
    result = HAL_I2C_Mem_Write(&mI2cHandler, addr<<1, reg, sizeof(reg), const_cast<uint8_t*>(data), len, 100);
    return result == HAL_OK;
}

bool I2cMaster::readRegister(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) {
    HAL_StatusTypeDef result;
    result = HAL_I2C_Mem_Read(&mI2cHandler, addr<<1, reg, sizeof(reg), data, len, 100);
    return result == HAL_OK;
}
