#ifndef GPIO_H
#define GPIO_H

#include "igpio.h"
#include "stm32f1xx_hal.h"

/// @brief A class for controlling GPIO pins on STM32 microcontrollers.
///
/// This class provides methods to set and get the state of a GPIO pin. It is derived from the IGpio interface,
/// ensuring compatibility with other components that rely on GPIO control.
class Gpio : public IGpio
{
public:
    /// @brief Constructs a new Gpio object with the specified parameters.
    ///
    /// Initializes the GPIO pin with the provided port, pin number, mode, pull-up/pull-down configuration,
    /// and alternative function.
    ///
    /// @param port The GPIO port (e.g., GPIOA, GPIOB).
    /// @param pin The GPIO pin number (e.g., GPIO_PIN_0).
    /// @param mode The mode of the GPIO pin (e.g., GPIO_MODE_OUTPUT_PP).
    /// @param pull The pull-up/pull-down configuration (e.g., GPIO_NOPULL). Default is 0 (no pull).
    /// @param alternative The alternative function configuration. Default is 0 (no alternative function).
    Gpio(GPIO_TypeDef* port, uint16_t pin, uint32_t mode, uint8_t pull = 0, uint8_t alternative = 0);

    /// @brief Destructor for the Gpio object.
    ///
    /// Cleans up any resources used by the GPIO object.
    ~Gpio();

    /// @brief Sets the state of the GPIO pin.
    ///
    /// @param state The desired state of the GPIO pin (true for high, false for low).
    virtual void set(bool state);

    /// @brief Gets the current state of the GPIO pin.
    ///
    /// @return true if the GPIO pin is high, false if it is low.
    virtual bool get() const;

private:
    GPIO_TypeDef* mPort; ///< The GPIO port associated with this pin.
    uint16_t mPin;       ///< The GPIO pin number.
};

#endif // GPIO_H
