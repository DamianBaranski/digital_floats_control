#include "pwm_dma.h"

TIM_HandleTypeDef htim;
DMA_HandleTypeDef hdma;

PwmDma::PwmDma(TIM_TypeDef *timer, uint32_t channel, DMA_Channel_TypeDef *dma_channel, uint32_t period) : mChannel(channel)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    __HAL_RCC_TIM2_CLK_ENABLE();
    htim.Instance = timer;
    htim.Init.Prescaler = 0;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = period;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim);

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig);
    HAL_TIM_PWM_Init(&htim);

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, mChannel);

    hdma.Instance = dma_channel;
    hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma.Init.MemInc = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma.Init.Mode = DMA_NORMAL;
    hdma.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma);

    switch (mChannel)
    {
    case TIM_CHANNEL_1:
        __HAL_LINKDMA(&htim, hdma[TIM_DMA_ID_CC1], hdma);
        break;
    case TIM_CHANNEL_2:
        __HAL_LINKDMA(&htim, hdma[TIM_DMA_ID_CC2], hdma);
        break;
    case TIM_CHANNEL_3:
        __HAL_LINKDMA(&htim, hdma[TIM_DMA_ID_CC3], hdma);
        break;
    case TIM_CHANNEL_4:
        __HAL_LINKDMA(&htim, hdma[TIM_DMA_ID_CC4], hdma);
        break;
    }
}

bool PwmDma::start(uint16_t *data, size_t len)
{
    HAL_StatusTypeDef result = HAL_TIM_PWM_Start_DMA(&htim, mChannel, (uint32_t *)data, len);
    return result == HAL_OK;
}

extern "C"
{
    void DMA1_Channel7_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&hdma);
    }

    void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2)
        {
            HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_2);
        }
    }
};