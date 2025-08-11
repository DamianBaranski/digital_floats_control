#include "pwm.h"

Pwm::Pwm(TIM_TypeDef *timer, uint32_t channel, uint32_t period) : mChannel(channel)
{
    __HAL_RCC_TIM3_CLK_ENABLE();
    mHtim.Instance = timer;
    mHtim.Init.Prescaler = 100;
    mHtim.Init.CounterMode = TIM_COUNTERMODE_UP;
    mHtim.Init.Period = period;
    mHtim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    mHtim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&mHtim);

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&mHtim, &sClockSourceConfig);
    HAL_TIM_PWM_Init(&mHtim);

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&mHtim, &sMasterConfig);

    mConfigOC.OCMode = TIM_OCMODE_PWM1;
    mConfigOC.Pulse = 0;
    mConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    mConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&mHtim, &mConfigOC, mChannel);
}

bool Pwm::start()
{
    HAL_StatusTypeDef result = HAL_TIM_PWM_Start(&mHtim, mChannel);
    return result == HAL_OK;
}

bool Pwm::stop()
{
    HAL_StatusTypeDef result = HAL_TIM_PWM_Stop(&mHtim, mChannel);
    return result == HAL_OK;
}

void Pwm::setPulse(uint16_t pulse)
{
    mConfigOC.Pulse=pulse;
    HAL_TIM_PWM_ConfigChannel(&mHtim, &mConfigOC, mChannel);
}