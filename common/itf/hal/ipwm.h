#ifndef IPWM_H
#define IPWM_H

#include <cstdint>
#include <cstddef>

/// @brief Interface for PWM.
/// This interface provides a contract for starting a PWM signal.
class IPwm {
public:
    /// @brief Starts the PWM signal.
    virtual bool start() = 0;

    /// @brief Stops the PWM signal.
    virtual bool stop() = 0;

    virtual void setPulse(uint16_t pulse) = 0;
};

#endif