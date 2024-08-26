#include <cstdint>

/// @brief A utility class for working with colors.
///
/// This class provides static methods and constants for managing colors,
/// including predefined colors, brightness adjustment, and color blinking effects.
class Colors
{
public:
    /// @brief Predefined white color (0xFFFFFF).
    static constexpr uint32_t WHITE = 0xFFFFFF;

    /// @brief Predefined red color (0xFF0000).
    static constexpr uint32_t RED = 0xFF0000;

    /// @brief Predefined green color (0x00FF00).
    static constexpr uint32_t GREEN = 0x00FF00;

    /// @brief Predefined blue color (0x0000FF).
    static constexpr uint32_t BLUE = 0x0000FF;

    /// @brief Predefined orange color (0xFF7F00).
    static constexpr uint32_t ORANGE = 0xFF7F00;

    /// @brief Predefined violet color (0x7F00FF).
    static constexpr uint32_t VIOLET = 0x7F00FF;

    /// @brief Predefined yellow color (0xFFFF00).
    static constexpr uint32_t YELLOW = 0xFFFF00;

    /// @brief Adjust the brightness of a color.
    ///
    /// This method scales the RGB components of the color based on the provided brightness level.
    ///
    /// @param color The original color value (0xRRGGBB).
    /// @param brightness The brightness level (0-255).
    /// @return The color with adjusted brightness.
    static uint32_t setBrightness(uint32_t color, uint8_t brightness)
    {
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;

        r = r * brightness / 0xFF;
        g = g * brightness / 0xFF;
        b = b * brightness / 0xFF;

        return (r << 16) | (g << 8) | b;
    }

    /// @brief Create a blinking effect between two colors.
    ///
    /// This method returns one of the two colors based on the given time and period, creating a blinking effect.
    ///
    /// @param period The blinking period (in time units).
    /// @param time The current time (in the same units as period).
    /// @param color1 The first color (displayed in the first half of the period).
    /// @param color2 The second color (displayed in the second half of the period, default is 0x000000).
    /// @return The current color based on the blinking effect.
    static uint32_t blinking(uint32_t period, uint32_t time, uint32_t color1, uint32_t color2 = 0)
    {
        return ((time % period) < (period / 2)) ? color1 : color2;
    }
};