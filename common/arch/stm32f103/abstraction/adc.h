#ifndef ADC_H
#define ADC_H
#include "iadc.h"
#include "stm32f1xx_hal.h"

/// @class Adc
/// @brief A class for controlling the ADC peripheral on STM32 microcontrollers.
/// This class provides methods to start an ADC conversion and read the conversion result.
/// It is derived from the IAdc interface, ensuring compatibility with other components that rely on ADC functionality.
class Adc : public IAdc
{
public:
    /// @brief Constructs a new Adc object for the specified ADC instance.
    ///
    /// Initializes the ADC peripheral with default settings.
    /// @param adcInstance The ADC instance to use (e.g., ADC1, ADC2).
    Adc(ADC_TypeDef* adcInstance);

    /// @brief Destructor for the Adc object.
    /// Cleans up any resources used by the ADC object.
    ~Adc();

    /// @brief Starts an ADC conversion.
    /// This method triggers the start of an ADC conversion process.
    virtual void startConversion() override;

    /// @brief Reads the result of the ADC conversion.
    /// This method retrieves the digital value resulting from the most recent ADC conversion.
    /// @return The digital value from the ADC conversion.
    virtual uint16_t readValue() const override;

private:
    ADC_HandleTypeDef mAdcHandle; ///< The ADC handle used for managing the ADC peripheral.
};

#endif // ADC_H