#ifndef IGPIO_H
#define IGPIO_H

/// @class IGpio
/// @brief Interface class for General Purpose Input/Output (GPIO) operations.
/// 
/// This class provides an abstract interface for controlling GPIO pins. It 
/// includes methods to set and reset the pin state, as well as to get the 
/// current pin state. Derived classes must implement the pure virtual methods.
class IGpio
{
public:
    /// @brief Sets the GPIO pin to high state.
    /// 
    /// This method sets the GPIO pin to a high state (true). It calls the pure 
    /// virtual method set() with true as the argument.
    virtual void set() { set(true); }

    /// @brief Resets the GPIO pin to low state.
    /// 
    /// This method resets the GPIO pin to a low state (false). It calls the pure 
    /// virtual method set() with false as the argument.
    virtual void reset() { set(false); }

    /// @brief Toggles the GPIO pin state.
    ///
    /// This method toggles the GPIO pin state from high to low or from low to high.
    /// It calls the pure virtual methods set() and get() to achieve this.
    virtual void toggle() { set(!get()); }

    /// @brief Sets the GPIO pin to the specified state.
    /// 
    /// This pure virtual method must be implemented by derived classes to set 
    /// the GPIO pin to the specified state.
    /// 
    /// @param state The desired state of the GPIO pin. True for high, false for low.
    virtual void set(bool state) = 0;

    /// @brief Gets the current state of the GPIO pin.
    /// 
    /// This pure virtual method must be implemented by derived classes to get 
    /// the current state of the GPIO pin.
    /// 
    /// @return The current state of the GPIO pin. True for high, false for low.
    virtual bool get() const = 0;


};

#endif // IGPIO_H
