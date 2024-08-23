#ifndef PWM_H
#define PWM_H

#include "ipwm_dma.h"
#include "stm32f1xx_hal.h"

/// @brief PwmDma class is responsible for handling PWM with DMA functionality.
/// This class inherits from the IPwmDma interface and provides an implementation
/// for initializing and starting PWM with DMA.
class PwmDma : public IPwmDma
{
public:
    /// @brief Constructor for the PwmDma class.
    /// @param timer Pointer to the timer instance to be used for PWM.
    /// @param channel Timer channel to be used for PWM.
    /// @param dma_channel DMA channel to be used for transferring data.
    /// @param period Period value for the PWM signal.
    PwmDma(TIM_TypeDef *timer, uint32_t channel, DMA_Channel_TypeDef *dma_channel, uint32_t period);

    /// @brief Starts the PWM with DMA using the provided data buffer.
    /// @param data Pointer to the data buffer to be transferred via DMA.
    /// @param len Length of the data buffer.
    /// @return Returns true if the PWM with DMA starts successfully, otherwise false.
    virtual bool start(uint16_t *data, size_t len) override;

private:
    uint32_t mChannel; ///< Timer channel used for PWM.
};

#endif