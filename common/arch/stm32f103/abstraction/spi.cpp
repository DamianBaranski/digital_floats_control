#include "spi.h"
SPI_HandleTypeDef spiHandle = {};

Spi::Spi(SPI_TypeDef *spi) {
    __SPI1_CLK_ENABLE();
    spiHandle.Instance = spi;
    spiHandle.Init.Mode = SPI_MODE_MASTER;
    spiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    spiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    spiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    spiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    spiHandle.Init.NSS = SPI_NSS_SOFT;
    spiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    spiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spiHandle.Init.CRCPolynomial = 7;

    //HAL_SPI_Init(&spiHandle);
    uint8_t data = 0;
    transmit(&data, sizeof(data));
}

bool Spi::transmit(const uint8_t *data, size_t len) {

    if (HAL_SPI_Transmit(&spiHandle, const_cast<uint8_t *>(data), len, 100) != HAL_OK) {
        return false;
    }

    return true;
}

bool Spi::receive(uint8_t *data, size_t len) {

    if (HAL_SPI_Receive(&spiHandle, data, len, 100) != HAL_OK) {
        return false;
    }

    return true;
}

bool Spi::start() {
    HAL_SPI_Init(&spiHandle);
    return true;
}

bool Spi::stop() {
HAL_SPI_DeInit(&spiHandle);
return true;
}

extern "C" {
void SPI1_IRQHandler(void) {
    HAL_SPI_IRQHandler(&spiHandle);
}
}