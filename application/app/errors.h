#ifndef ERRORS_H
#define ERRORS_H

#include <cstdint>
#include "bit_mask.h"

enum class ChannelError {
    RELAY_COMMUNICATION_ERROR,  /**< Error communicating with relay module */
    ENDSTOP_SHORT_CIRCUIT,      /**< Short circuit detected on endstop */
    OVER_CURRENT_ERROR,         /**< Current exceeds maximum limit */
};

enum class ChannelWarning {
    MOVEMENT_TIMEOUT,       /**< Movement operation took too long */
    OVER_CURRENT_WARNING,     /**< Current exceeds warning threshold but not critical */
    UNDER_CURRENT_WARNING,    /**< Current below warning threshold but not critical */
    ADC_COMMUNICATION_ERROR, /**< Error communicating with ADC sensor */
};

enum class SystemWarning {
    LOW_VOLTAGE,            /**< Power supply voltage is below recommended level */
    HIGH_VOLTAGE,           /**< Power supply voltage is above recommended level */
    EXT_MEMORY_ERROR,      /**< Error accessing external memory */
};

class Errors {
public:
    /**
     * @brief Checks if a specific error is set
     * @param error The error to check
     * @return true if the error is set, false otherwise
     */
    bool isSet(uint8_t channel, ChannelError error) const {
        if (channel >= sizeof(mErrors) / sizeof(mErrors[0])) {
            return false; // Invalid channel index
        }
        return mErrors[channel].isSet(error);
    }

    bool isSet(uint8_t channel, ChannelWarning warning) const {
        if (channel >= sizeof(mWarnings) / sizeof(mWarnings[0])) {
            return false; // Invalid channel index
        }
        return mWarnings[channel].isSet(warning);
    }

    bool isSet(SystemWarning warning) const {
        return mSystemWarnings.isSet(warning);
    }

    /**
     * @brief Sets a specific error for a channel
     * @param channel The channel index
     * @param error The error to set
     */
    void set(uint8_t channel, ChannelError error) {
        if (channel < sizeof(mErrors) / sizeof(mErrors[0])) {
            mErrors[channel].set(error);
        }
    }

    /**
     * @brief Clears a specific error for a channel
     * @param channel The channel index
     * @param error The error to clear
     */
    void set(uint8_t channel, ChannelWarning warning) {
        if (channel < sizeof(mWarnings) / sizeof(mWarnings[0])) {
            mWarnings[channel].set(warning);
        }
    }

    /**
     * @brief Clears a specific system warning
     * @param warning The system warning to clear
     */
    void set(SystemWarning warning) {
        mSystemWarnings.set(warning);
    }

    /**
     * @brief Clears a specific error for a channel
     * @param channel The channel index
     * @param error The error to clear
     */
    void clear(uint8_t channel, ChannelError error) {
        if (channel < sizeof(mErrors) / sizeof(mErrors[0])) {
            mErrors[channel].clr(error);
        }
    }
    void clear(uint8_t channel, ChannelWarning warning) {
        if (channel < sizeof(mWarnings) / sizeof(mWarnings[0])) {
            mWarnings[channel].clr(warning);
        }
    }   
    void clear(SystemWarning warning) {
        mSystemWarnings.clr(warning);
    }   
    /**
     * @brief Clears all errors for a channel
     * @param channel The channel index
     */
    void clearAll(uint8_t channel) {
        if (channel < sizeof(mErrors) / sizeof(mErrors[0])) {
            mErrors[channel] = BitMask<ChannelError>();
            mWarnings[channel] = BitMask<ChannelWarning>();
        }
    }

    /**
     * @brief Clears all system warnings
     */
    void clearAllSystemWarnings() {
        mSystemWarnings = BitMask<SystemWarning>();
    }

    /**
     * @brief Checks if any errors are set for a channel
     * @param channel The channel index
     * @return true if any errors are set, false otherwise
     */
    bool anyError(uint8_t channel) const {
        if (channel >= sizeof(mErrors) / sizeof(mErrors[0])) {
            return false; // Invalid channel index
        }
        return mErrors[channel].any();
    }

    bool anyWarning(uint8_t channel) const {
        if (channel >= sizeof(mWarnings) / sizeof(mWarnings[0])) {
            return false; // Invalid channel index
        }
        return mWarnings[channel].any();
    }


    private:
    BitMask<ChannelError> mErrors[5];  /**< Bitmask tracking channel-specific errors */
    BitMask<ChannelWarning> mWarnings[5]; /**< Bitmask tracking channel-specific warnings */
    BitMask<SystemWarning> mSystemWarnings; /**< Bitmask tracking system-wide warnings */
};
#endif