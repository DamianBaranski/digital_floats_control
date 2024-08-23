#ifndef WS2812_H
#define WS2812_H

#include <cstddef>
#include "ipwm_dma.h"

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
    /// @param pwm A reference to an IPwmDma object to control the data line of the WS2812 strip.
    Ws2812(IPwmDma &pwm);

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

    /// @brief Structure to represent the color of an LED.
    struct Color
    {
        uint8_t red;   ///< Red component (0-255).
        uint8_t green; ///< Green component (0-255).
        uint8_t blue;  ///< Blue component (0-255).
    };

    Color mColors[S]; ///< Array to store the colors for all LEDs in the strip.
    IPwmDma &mPwm;     ///< Reference to the PwmDma interface used to control the WS2812 strip.
    uint16_t mPwmData[24*S+50];
};

template <size_t S>
Ws2812<S>::Ws2812(IPwmDma &pwm) : mColors{}, mPwm(pwm), mPwmData{}
{}

template <size_t S>
void Ws2812<S>::update()
{
    for(size_t i=0; i<S; i++) {
        for(size_t bit=0; bit<8; bit++) {
            uint16_t value = 0;
            if(mColors[i].green & (1<<bit)) {
                value=59;
            } else {
                value = 18;
            }
            mPwmData[i*24+bit] = value;
        }
        for(size_t bit=0; bit<8; bit++) {
            uint16_t value = 0;
            if(mColors[i].red & (1<<(7-bit))) {
                value=59;
            } else {
                value = 18;
            }
            mPwmData[i*24+8+bit] = value;
        }
        for(size_t bit=0; bit<8; bit++) {
            uint16_t value = 0;
            if(mColors[i].blue & (1<<bit)) {
                value=59;
            } else {
                value = 18;
            }
            mPwmData[i*24+16+bit] = value;
        }
    }
    mPwm.start(mPwmData, sizeof(mPwmData)/sizeof(mPwmData[0]));
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

#endif
