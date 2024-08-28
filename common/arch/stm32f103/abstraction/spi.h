#ifndef SPI_H
#define SPI_H

#include "ispi.h"
#include "stm32f1xx_hal.h"

class Spi : public ISpi {
    public:
    Spi(SPI_TypeDef *spi);
    virtual bool transmit(const uint8_t *data, size_t len) override;
    virtual bool receive(uint8_t *data, size_t len) override;
    virtual bool start() override;
    virtual bool stop() override;

};

#endif