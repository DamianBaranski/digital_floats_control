#include "adc.h"

Adc::Adc(ADC_TypeDef* adcInstance)
{
    mAdcHandle.Instance = adcInstance;
    mAdcHandle.Init.ScanConvMode = ADC_SCAN_DISABLE;
    mAdcHandle.Init.ContinuousConvMode = ENABLE;
    mAdcHandle.Init.DiscontinuousConvMode = DISABLE;
    mAdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    mAdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    mAdcHandle.Init.NbrOfConversion = 1;

    if (HAL_ADC_Init(&mAdcHandle) != HAL_OK)
    {
        // Initialization Error
        // Handle error appropriately (e.g., log it, assert, etc.)
    }

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_0; // Default channel, can be changed later
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

    if (HAL_ADC_ConfigChannel(&mAdcHandle, &sConfig) != HAL_OK)
    {
        // Channel Configuration Error
        // Handle error appropriately (e.g., log it, assert, etc.)
    }
}

Adc::~Adc()
{
    HAL_ADC_DeInit(&mAdcHandle);
}

void Adc::startConversion()
{
    if (HAL_ADC_Start(&mAdcHandle) != HAL_OK)
    {
        // Start Error
        // Handle error appropriately (e.g., log it, assert, etc.)
    }
}

uint16_t Adc::readValue() const
{
    uint16_t adcValue = 0;
    adcValue = HAL_ADC_GetValue(const_cast<ADC_HandleTypeDef*>(&mAdcHandle));
    return adcValue;
}