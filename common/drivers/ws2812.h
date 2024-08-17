#ifndef WS2812_H
#define WS2812_H

#include <cstddef>
#include <igpio.h>

/// @brief A template class to control a WS2812 LED strip.
///
/// This class provides methods to set the color of individual LEDs on a WS2812 LED strip
/// and to send the updated color data to the strip using bit-banging through a GPIO interface.
///
/// @tparam S Number of LEDs in the strip.
template <size_t S>
class Ws2812
{
public:
    /// @brief Construct a new Ws2812 object.
    ///
    /// @param gpio A reference to an IGpio object to control the data line of the WS2812 strip.
    Ws2812(IGpio &gpio);

    /// @brief Sends the current color data to the WS2812 LED strip.
    ///
    /// This method iterates through the color array and sends each LED's color data
    /// (in GRB format) to the LED strip, followed by a reset signal to latch the data.
    void update();

    /// @brief Sets the color of a specific LED on the strip.
    ///
    /// @param led_id The index of the LED to set (0-based).
    /// @param red The red component of the color (0-255).
    /// @param green The green component of the color (0-255).
    /// @param blue The blue component of the color (0-255).
    void setColor(size_t led_id, uint8_t red, uint8_t green, uint8_t blue);

    /// @brief Sets the color of a specific LED on the strip using a 24-bit color value.
    ///
    /// @param led_id The index of the LED to set (0-based).
    /// @param color A 24-bit color value in the format 0xRRGGBB.
    void setColor(size_t led_id, uint32_t color);

private:
    /// @brief A rough delay function calibrated for nanoseconds.
    ///
    /// This function creates a delay by performing a busy-wait loop, with each iteration
    /// corresponding to approximately one clock cycle.
    ///
    /// @param ns The delay duration in nanoseconds.
    void delay_ns(uint32_t ns);

    /// @brief Sends a single bit to the WS2812 LED strip.
    ///
    /// This method sets the GPIO high for a specific duration based on the bit value (0 or 1),
    /// then pulls it low for the remaining period.
    ///
    /// @param bit The bit to send (0 or 1).
    void sendBit(uint8_t bit);

    /// @brief Sends a single byte to the WS2812 LED strip.
    ///
    /// This method sends each bit of the byte to the LED strip using the `sendBit` method,
    /// starting with the most significant bit.
    ///
    /// @param byte The byte to send.
    void sendByte(uint8_t byte);

    /// @brief Sends a reset signal to the WS2812 LED strip.
    ///
    /// This method sends a reset signal to the LED strip by holding the GPIO pin low
    /// for a period longer than 50 µs, which latches the color data into the LEDs.
    void sendReset();

    /// @brief Structure to represent the color of an LED.
    struct Color
    {
        uint8_t red;   ///< Red component (0-255).
        uint8_t green; ///< Green component (0-255).
        uint8_t blue;  ///< Blue component (0-255).
    };

    Color mColors[S]; ///< Array to store the colors for all LEDs in the strip.
    IGpio &mGpio;     ///< Reference to the GPIO interface used to control the WS2812 strip.
};

template <size_t S>
Ws2812<S>::Ws2812(IGpio &gpio) : mColors{}, mGpio(gpio) {}

template <size_t S>
void Ws2812<S>::update()
{
    for (size_t i = 0; i < S; i++)
    {
        sendByte(mColors[i].red);
        sendByte(mColors[i].green);
        sendByte(mColors[i].blue);
    }
    sendReset();
}

template <size_t S>
void Ws2812<S>::setColor(size_t led_id, uint8_t red, uint8_t green, uint8_t blue)
{
    mColors[led_id].red = red;
    mColors[led_id].green = green;
    mColors[led_id].blue = blue;
}

template <size_t S>
void Ws2812<S>::setColor(size_t led_id, uint32_t color)
{
    mColors[led_id].red = color & 0xFF;
    mColors[led_id].green = (color >> 8) & 0xFF;
    mColors[led_id].blue = (color >> 16) & 0xFF;
}

template <size_t S>
void Ws2812<S>::delay_ns(uint32_t ns)
{
    uint32_t cycles = (72 * ns) / 1000; // Convert ns to clock cycles
    while (cycles--)
        ;
}

template <size_t S>
void Ws2812<S>::sendBit(uint8_t bit)
{
    if (bit)
    {
        mGpio.set();
        delay_ns(900); // WS2812B_DELAY_T1H
        mGpio.reset();
        delay_ns(350); // WS2812B_DELAY_T1L
    }
    else
    {
        mGpio.set();
        delay_ns(350); // WS2812B_DELAY_T0H
        mGpio.reset();
        delay_ns(900); // WS2812B_DELAY_T0L
    }
}

template <size_t S>
void Ws2812<S>::sendByte(uint8_t byte)
{
    for (int i = 7; i >= 0; i--)
    {
        sendBit((byte >> i) & 0x01);
    }
}

template <size_t S>
void Ws2812<S>::sendReset()
{
    mGpio.reset();
    delay_ns(100000); // Hold low for more than 50 µs to reset the LED strip
}

#endif
