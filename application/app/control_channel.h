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
#include "ina219.h"
#include "pcf8574.h"

/**
 * @enum Warnings
 * @brief Enumeration representing possible warning conditions for a control channel.
 *
 * Warnings indicate potential issues that don't prevent operation but may require attention.
 */
enum class Warnings {
    LOW_MOTOR_IMPEDANCE,   /**< Motor impedance is below expected range, indicating potential fault */
    HIGH_MOTOR_IMPEDANCE,  /**< Motor impedance is above expected range, indicating potential fault */
    LOW_POWER_VOLTAGE,     /**< Power supply voltage is below recommended threshold */
    HIGH_POWER_VOLTAGE,    /**< Power supply voltage is above recommended threshold */
};

/**
 * @enum Errors
 * @brief Enumeration representing possible error conditions for a control channel.
 *
 * Errors indicate conditions that prevent normal operation of the control channel.
 */
enum class Errors {
    NONE,                   /**< No error present */
    OPEN_CIRCUIT,           /**< Open circuit detected in motor wiring */
    SHORT_CIRCUIT,          /**< Short circuit detected in motor wiring */
    TIME_EXCEEDED,          /**< Operation time limit exceeded (motor may be stuck) */
    PCF_COMMUNICATION_ISSUE, /**< Communication failure with PCF8574 I/O expander */
    INA_COMMUNICATION_ISSUE, /**< Communication failure with INA219 current/voltage sensor */
    ENDSTOP_SHORT_CIRCUIT,   /**< Short circuit detected in limit switch wiring */
    RELAYS_ISSUE,            /**< Problem detected with relay operation or control */
};

/**
 * @enum State
 * @brief Enumeration representing the operational state of a control channel.
 *
 * These states indicate the current position and movement status of the controlled mechanism.
 */
enum class State {
    UP,      /**< Mechanism is in fully raised position */
    DOWN,    /**< Mechanism is in fully lowered position */
    MOVING,  /**< Mechanism is in motion between positions */
    ERROR,   /**< An error condition prevents normal operation */
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

    /** @name Hardware Configuration */
    /**@{*/
    uint8_t ina_addr;                      /**< I2C address of the INA219 current/voltage sensor (7-bit address) */
    uint16_t ina_callibration;             /**< Calibration value for INA219 (in units of 0.01 Ohm shunt resistance) */
    uint8_t pcf_addr;                      /**< I2C address of the PCF8574 I/O expander (7-bit address) */
    uint8_t pcf_channel;                   /**< Channel of the PCF8574 (0 or 1) for this control channel */
    /**@}*/

    /** @name Safety Limits */
    /**@{*/
    uint16_t max_voltage_limit;            /**< Maximum voltage limit in units of 0.1V (e.g. 280 = 28.0V) */
    uint16_t min_voltage_limit;            /**< Minimum voltage limit in units of 0.1V (e.g. 80 = 8.0V) */
    uint16_t max_current_limit;            /**< Maximum current limit in units of 0.1A (e.g. 50 = 5.0A) */
    uint16_t min_current_limit;            /**< Minimum current limit in units of 0.1A (e.g. 1 = 0.1A) */
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
    ControlChannel(II2cMaster &i2c);

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
     * @brief Tests the communication with all associated sensors and I/O expanders
     * @return true if all connections are successful, false if any component cannot be reached
     * 
     * Performs a communication test with the INA219 current/voltage sensor and PCF8574 I/O expander
     * to verify that they are properly connected and responding.
     */
    bool connectionTest();

    /**
     * @brief Tests the I2C address configuration
     * @param set Whether to set (true) or verify (false) the device addresses
     * @return true if addresses were successfully set or verified, false otherwise
     * 
     * This function can either set the I2C addresses according to the channel settings
     * or verify that the currently configured addresses match the expected values.
     */
    bool addressTest(bool set);

    /**
     * @brief Tests the relay functionality
     * @return true if all relays are functioning correctly, false if any issues were detected
     * 
     * Performs a comprehensive test of the relay control system by cycling through different
     * relay states and verifying proper operation. This is typically called during system 
     * initialization to detect hardware faults.
     */
    bool relaysTest();

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

    /**
     * @brief Retrieves the current power sensor readings
     * @param[out] voltage Output parameter for voltage reading (in millivolts)
     * @param[out] current Output parameter for current reading (in milliamps)
     * @return true if readings were successfully acquired, false if a sensor error occurred
     * 
     * Gets the latest voltage and current measurements from the INA219 sensor.
     * These values can be used for monitoring power consumption and detecting abnormal conditions.
     */
    bool getPowerSensorStatus(uint16_t &voltage, uint16_t &current);

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
    State getChannelState();

private:
    /**
     * @brief Configures the PCF8574 I/O expander with the necessary settings
     * @return true if configuration was successful, false if a communication error occurred
     * 
     * Sets up the I/O expander pins for motor control and limit switch monitoring
     * according to the channel's configuration.
     */
    bool configure();

    /**
     * @brief Gets the state of the specified limit switch
     * @param limit_switch The limit switch to check (UP or DOWN)
     * @return true if the limit switch is active, false if inactive
     * 
     * Reads the current state of the specified limit switch from the I/O expander,
     * taking into account the inversion settings from the channel configuration.
     */
    bool getLimitSwitchState(LimitSwitch limit_switch);

    /**
     * @brief Sets the motor state and direction
     * @param enable Whether to enable (true) or disable (false) the motor
     * @param channel The channel number on the PCF8574 (0 or 1)
     * @param dir The direction of the motor (true = forward/up, false = reverse/down)
     * @return true if the motor state was successfully set, false if an error occurred
     * 
     * Low-level function that controls the H-bridge relays via the PCF8574 I/O expander
     * to enable/disable the motor and set its direction.
     */
    bool setMotor(bool enable, uint8_t channel, bool dir);
    
    /** @name Instance Variables */
    /**@{*/
    ControlChannelSettings mSettings; /**< Current configuration settings for this channel */
    Ina219 mCurrentSensor;            /**< INA219 current/voltage sensor interface */
    Pcf8574 mExpanderIO;              /**< PCF8574 I/O expander interface for motor control and limit switches */
    BitMask<Errors> mErrors;          /**< Bitmask tracking active error conditions */
    BitMask<Warnings> mWarnings;      /**< Bitmask tracking active warning conditions */
    /**@}*/

    /** @name PCF8574 Configuration Constants */
    /**@{*/
    /** 
     * @brief Configuration value for the PCF8574
     * 
     * The PCF8574 configuration sets the lower 4 bits (0-3) as inputs for limit switches
     * and the upper 4 bits (4-7) as outputs for relay control.
     * Value 0x0F configures pins 0-3 as inputs (1) and pins 4-7 as outputs (0).
     */
    static constexpr uint8_t cPcfCfg = 0x0F; 

    /** @name Motor Direction Control Masks */
    /**@{*/
    static constexpr uint8_t cMotor1RightDirMask = 0x10; /**< Bit mask for Motor 1 right/up direction (pin 4) */
    static constexpr uint8_t cMotor1LeftDirMask = 0x20;  /**< Bit mask for Motor 1 left/down direction (pin 5) */
    static constexpr uint8_t cMotor2RightDirMask = 0x40; /**< Bit mask for Motor 2 right/up direction (pin 6) */
    static constexpr uint8_t cMotor2LeftDirMask = 0x80;  /**< Bit mask for Motor 2 left/down direction (pin 7) */
    /**@}*/

    /** @name Limit Switch Input Masks */
    /**@{*/
    static constexpr uint8_t cMotor1UpLimitSwitchMask = 0x01;   /**< Bit mask for Motor 1 up limit switch (pin 0) */
    static constexpr uint8_t cMotor1DownLimitSwitchMask = 0x02; /**< Bit mask for Motor 1 down limit switch (pin 1) */
    static constexpr uint8_t cMotor2UpLimitSwitchMask = 0x04;   /**< Bit mask for Motor 2 up limit switch (pin 2) */
    static constexpr uint8_t cMotor2DownLimitSwitchMask = 0x08; /**< Bit mask for Motor 2 down limit switch (pin 3) */
    /**@}*/
    /**@}*/
};

#endif // CONTROL_CHANNEL_H