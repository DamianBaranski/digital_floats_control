/**
 * @file control_channel.h
 * @brief Control channel implementation for digital float control system
 *
 * This file defines the ControlChannel class and related types that manage individual
 * control channels in the digital float control system. Each channel represents a motor
 * control unit with limit switches, current/voltage sensing, and error detection.
 */

#ifndef CONTROL_CHANNEL_H
#define CONTROL_CHANNEL_H

#include <cstdint>
#include "bit_mask.h"
#include "ii2c_master.h"
#include "logger.h"
#include "adc121c.h"
#include "pcf8574.h"
#include "expander.h"

/**
 * @enum GearState
 * @brief Enumeration representing the operational state of a control channel.
 *
 * These states indicate the current position and movement status of the controlled mechanism.
 */
enum class GearState {
    UP,      /**< Mechanism is in fully raised position */
    DOWN,    /**< Mechanism is in fully lowered position */
    MOVING,  /**< Mechanism is in motion between positions */
};

/**
 * @enum LimitSwitch
 * @brief Enumeration representing the different limit switch types.
 *
 * Limit switches indicate when the mechanism has reached its end positions.
 */
enum class LimitSwitch {
    UP,    /**< Upper limit switch that triggers at the fully raised position */
    DOWN   /**< Lower limit switch that triggers at the fully lowered position */
};

/**
 * @struct ControlChannelSettings
 * @brief Configuration settings for a control channel.
 *
 * This structure holds all configurable parameters that define the behavior and 
 * hardware configuration of a control channel. These settings can be persisted
 * to non-volatile memory.
 */
typedef struct {
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
} ControlChannelSettings;

/**
 * @class ControlChannel
 * @brief Class to control a motor channel with current/voltage sensing and limit switches.
 *
 * The ControlChannel class manages the interaction with hardware components for one channel:
 * - Motor control (via H-bridge)
 * - Current and voltage sensing (via INA219)
 * - Limit switch monitoring (via PCF8574)
 * - Error detection and handling
 * 
 * Each control channel can be configured for either landing gear or rudder operation
 * with appropriate settings for limits, sensors, and behavior.
 */
class ControlChannel {
public:
    /**
     * @brief Constructs a new ControlChannel object
     * @param i2c Reference to an I2C master interface for communicating with sensors and expanders
     * 
     * Initializes the control channel in a safe inactive state. The channel will not
     * be operational until setSettings() is called with valid configuration parameters.
     */
    ControlChannel(Expander &expander, II2cMaster &i2c);

    void setExpanderChannel(uint8_t channel) {
        mPcfChannel = channel;
    }

    /**
     * @brief Sets the configuration settings for the control channel
     * @param settings The settings structure containing all configuration parameters
     * @return true if settings were successfully applied, false if validation failed or hardware initialization failed
     * 
     * Applies the provided settings to the control channel and initializes the hardware
     * components accordingly. This must be called before the channel can be used.
     */
    bool setSettings(const ControlChannelSettings &settings);

    /**
     * @brief Sets the motor direction
     * @param dir The direction to set (true = forward/up, false = reverse/down)
     * @return true if the motor direction was successfully set, false if an error occurred
     * 
     * Activates the motor in the specified direction. The actual direction of movement
     * depends on the channel configuration (may be inverted) and the current position
     * of the mechanism.
     */
    bool setMotor(bool dir);

    bool disableMotor();

    /**
     * @brief Checks if the current control channel is configured as a rudder
     * @return true if the channel is configured for rudder control, false if it's for landing gear
     * 
     * This function indicates whether the channel is set up to control a rudder mechanism
     * or a landing gear mechanism, which affects its behavior and how it responds to commands.
     */
    bool isRudder() const;

    /**
     * @brief Gets the current operational state of the control channel
     * @return The current state (UP, DOWN, MOVING, or ERROR)
     * 
     * Returns the current state of the controlled mechanism based on limit switch positions,
     * movement commands, and error conditions. This state determines the visual indication
     * and available commands for the channel.
     */
    GearState getChannelState();

    /**
     * @brief Gets the state of the specified limit switch
     * @param limit_switch The limit switch to check (UP or DOWN)
     * @return true if the limit switch is active, false if inactive
     * 
     * Reads the current state of the specified limit switch from the I/O expander,
     * taking into account the inversion settings from the channel configuration.
     */
    bool getLimitSwitchState(LimitSwitch limit_switch);

private:
    /**
     * @brief Configures the PCF8574 I/O expander with the necessary settings
     * @return true if configuration was successful, false if a communication error occurred
     * 
     * Sets up the I/O expander pins for motor control and limit switch monitoring
     * according to the channel's configuration.
     */
    bool configure();
    
    /** @name Instance Variables */
    /**@{*/
    ControlChannelSettings mSettings; /**< Current configuration settings for this channel */
    Expander &mExpander;        /**< Reference to the PCF8574 I/O expander for relay control */
    uint8_t mPcfChannel;              /**< Channel number on the PCF8574 (0 or 1) */

    static constexpr uint8_t cMotor1UpLimitSwitchMask = 0x01;   /**< Bit mask for Motor 1 up limit switch (pin 0) */
    static constexpr uint8_t cMotor1DownLimitSwitchMask = 0x02; /**< Bit mask for Motor 1 down limit switch (pin 1) */
    static constexpr uint8_t cMotor2UpLimitSwitchMask = 0x04;   /**< Bit mask for Motor 2 up limit switch (pin 2) */
    static constexpr uint8_t cMotor2DownLimitSwitchMask = 0x08; /**< Bit mask for Motor 2 down limit switch (pin 3) */

    /**@}*/


};

#endif // CONTROL_CHANNEL_H