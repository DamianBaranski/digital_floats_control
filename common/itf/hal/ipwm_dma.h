#ifndef IPWM_DMA_H
#define IPWM_DMA_H

#include <cstdint>
#include <cstddef>

/// @brief Interface for PWM with DMA functionality.
/// This interface provides a contract for starting a PWM signal using DMA transfer.
class IPwmDma {
public:
    /// @brief Starts the PWM signal with DMA using the provided data buffer.
    /// @param data Pointer to the data buffer to be transferred via DMA.
    /// @param len Length of the data buffer.
    /// @return Returns true if the PWM with DMA starts successfully, otherwise false.
    virtual bool start(uint16_t *data, size_t len) = 0;
};

#endif