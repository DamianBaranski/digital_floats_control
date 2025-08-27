#ifndef CHANNEL_SETTINGS_H
#define CHANNEL_SETTINGS_H

#include <cstdint>

typedef struct __attribute__ ((packed)) {
    /** @name Identification */
    /**@{*/
    uint8_t channel_id;                /**< Unique identifier for the channel (0-5) */
    /**@}*/
} ChannelSettingsRequest;

typedef struct __attribute__ ((packed)) {
    /** @name Identification */
    /**@{*/
    uint8_t channel_id;                /**< Unique identifier for the channel (0-5) */
    /**@}*/

    /** @name Configuration Flags */
    /**@{*/
    uint8_t enable : 1;                    /**< Enable flag (1=enabled, 0=disabled) */
    uint8_t bridge : 1;                    /**< Bridge mode flag for H-bridge configuration (1=enabled, 0=disabled) */
    uint8_t inverse_motor : 1;             /**< Inverse motor direction flag (1=inverted, 0=normal) */
    uint8_t inverse_up_limit_switch : 1;   /**< Inverse up limit switch flag (1=normally closed, 0=normally open) */
    uint8_t inverse_down_limit_switch : 1; /**< Inverse down limit switch flag (1=normally closed, 0=normally open) */
    uint8_t inverse_limit_switch : 1;      /**< Global inverse limit switch flag (overrides individual switch settings) */
    uint8_t rudder : 1;                    /**< Rudder control flag (1=rudder channel, 0=landing gear channel) */
    /**@}*/

    uint8_t bridge_channel;            /**< Number of channel to bridge */
    uint8_t timeout;                   /**< Timeout in seconds for motor operation before error */

    /** @name Safety Limits */
    /**@{*/
    uint16_t max_current_warning_limit;   /**< Warning current limit in units of 0.1A (e.g. 50 = 5.0A) */
    uint16_t max_current_error_limit;     /**< Error current limit in units of 0.1A (e.g. 100 = 10.0A) */
    uint16_t min_current_limit;           /**< Minimum current limit in units of 0.1A (e.g. 1 = 0.1A) */
    /**@}*/
} ChannelSettingsResponse;

#endif // CHANNEL_SETTINGS_H