#ifndef PWM_H
#define PWM_H

#include "ipwm.h"
#include "stm32f1xx_hal.h"

/// @brief Pwm class is responsible for handling PWM.
/// This class inherits from the IPwm interface and provides an implementation
/// for initializing and starting PWM.
class Pwm : public IPwm
{
public:
    /// @brief Constructor for the PwmDma class.
    /// @param timer Pointer to the timer instance to be used for PWM.
    /// @param channel Timer channel to be used for PWM.
    /// @param period Period value for the PWM signal.
    Pwm(TIM_TypeDef *timer, uint32_t channel, uint32_t period);

    /// @brief Starts the PWM.
    virtual bool start() override;

    /// @brief Starts the PWM.
    virtual bool stop() override;

    virtual void setPulse(uint16_t pulse) override;

private:
    TIM_HandleTypeDef mHtim;
    TIM_OC_InitTypeDef mConfigOC;
    uint32_t mChannel; ///< Timer channel used for PWM.
};

#endif