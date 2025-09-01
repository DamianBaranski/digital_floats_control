#ifndef IADC_H
#define IADC_H
#include <cstdint>

/// @class IAdc
/// @brief Interface class for Analog-to-Digital Converter (ADC) operations.
/// This class provides an abstract interface for controlling ADC peripherals. It 
/// includes methods to initialize the ADC, start conversions, and read conversion results. 
/// Derived classes must implement the pure virtual methods.
class IAdc
{
public:
    /// @brief Starts an ADC conversion.
    /// This pure virtual method must be implemented by derived classes to trigger 
    /// the start of an ADC conversion process.
    virtual void startConversion() = 0;

    /// @brief Reads the result of the ADC conversion.
    /// This pure virtual method must be implemented by derived classes to retrieve 
    /// the digital value resulting from the most recent ADC conversion.
    /// @return The digital value from the ADC conversion.
    virtual uint16_t readValue() const = 0;
};

#endif // IADC_H