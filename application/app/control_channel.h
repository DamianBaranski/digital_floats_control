#ifndef CONTROL_CHANNEL_H
#define CONTROL_CHANNEL_H

#include <cstdint>
#include "bit_mask.h"
#include "ii2c_master.h"
#include "logger.h"
#include "ina219.h"
#include "pcf8574.h"

/// @brief Enumeration representing possible warning conditions.
enum class Warnings {
    LOW_MOTOR_IMPEDANCE,
    HIGH_MOTOR_IMPEDANCE,
    LOW_POWER_VOLTAGE,
    HIGH_POWER_VOLTAGE,
};

/// @brief Enumeration representing possible error conditions.
enum class Errors {
    NONE,
    OPEN_CIRCUIT,
    SHORT_CIRCUIT,
    TIME_EXCEEDED,
    PCF_COMMUNICATION_ISSUE,
    INA_COMMUNICATION_ISSUE,
    ENDSTOP_SHORT_CIRCUIT,
};

/// @brief Enumeration representing the state of the control channel.
enum class State {
    UP,
    DOWN,
    MOVING,
    ERROR,
};

/// @brief Enumeration representing the limit switch type.
enum class LimitSwitch {
    UP,
    DOWN
};

/// @brief Structure to hold the settings for a control channel.
typedef struct {
    uint8_t enable : 1;                    ///< Enable flag for the control channel.
    uint8_t bridge : 1;                    ///< Bridge mode flag.
    uint8_t inverse_motor : 1;             ///< Inverse motor direction flag.
    uint8_t inverse_up_limit_switch : 1;   ///< Inverse up limit switch flag.
    uint8_t inverse_down_limit_switch : 1; ///< Inverse down limit switch flag.
    uint8_t inverse_limit_switch : 1;      ///< Inverse limit switch flag.
    uint8_t rudder : 1;                    ///< Rudder control flag.
    uint8_t ina_addr;                      ///< I2C address of the INA219 sensor.
    uint16_t ina_callibration;             ///< Calibration value for INA219 (0.01 Ohm).
    uint8_t pcf_addr;                      ///< I2C address of the PCF8574 expander.
    uint8_t pcf_channel;                   ///< Channel of the PCF8574 (0 or 1).
    uint16_t max_voltage_limit;            ///< Maximum voltage limit (0.1V).
    uint16_t min_voltage_limit;            ///< Minimum voltage limit (0.1V).
    uint16_t max_current_limit;            ///< Maximum current limit (0.1A).
    uint16_t min_current_limit;            ///< Minimum current limit (0.1A).
} ControlChannelSettings;

/// @brief Class to control a channel with motor, current sensor and limit switches.
class ControlChannel {
public:
    /// @brief Constructs a new ControlChannel object.
    ///
    /// @param i2c Reference to an I2C master interface.
    ControlChannel(II2cMaster &i2c);

    /// @brief Sets the settings for the control channel.
    ///
    /// @param settings The settings to apply.
    /// @return true if settings were successfully applied, false otherwise.
    bool setSettings(const ControlChannelSettings &settings);

    /// @brief Tests the connection to the sensors and IO expander.
    ///
    /// @return true if all connections are successful, false otherwise.
    bool connectionTest();

    /// @brief Sets the motor state and direction.
    ///
    /// @param enable Enable or disable the motor.
    /// @param channel The channel number (0 or 1).
    /// @param dir The direction of the motor (true for forward, false for reverse).
    /// @return true if the motor state was successfully set, false otherwise.
    bool setMotor(bool enable, uint8_t channel, bool dir);

    bool getPowerSensorStatus();

    /// @brief Checks if the current control channel is configured as a rudder.
    ///
    /// @return true if the channel is a rudder, false otherwise.
    bool isRudder() const;

    /// @brief Gets the current state of the control channel.
    ///
    /// @return The current state of the channel (UP, DOWN, MOVING, or ERROR).
    State getChannelState();

private:
    /// @brief Configures the IO expander with the necessary settings.
    ///
    /// @return true if the configuration was successful, false otherwise.
    bool configure();

    /// @brief Gets the state of the specified limit switch.
    ///
    /// @param limit_switch The limit switch to check (UP or DOWN).
    /// @return true if the limit switch is active, false otherwise.
    bool getLimitSwitchState(LimitSwitch limit_switch);

    ControlChannelSettings mSettings; ///< The settings for the control channel.
    Ina219 mCurrentSensor;            ///< The current sensor (INA219).
    Pcf8574 mExpanderIO;              ///< The IO expander (PCF8574).
    BitMask<Errors> mErrors;          ///< Bitmask for tracking error states.
    BitMask<Warnings> mWarnings;      ///< Bitmask for tracking warning states.

    static constexpr uint8_t cPcfCfg = 0x0F; ///< Configuration value for the PCF8574.

    // Motor direction masks for PCF8574
    static constexpr uint8_t cMotor1RightDirMask = 0x10;
    static constexpr uint8_t cMotor1LeftDirMask = 0x20;
    static constexpr uint8_t cMotor2RightDirMask = 0x40;
    static constexpr uint8_t cMotor2LeftDirMask = 0x80;

    // Limit switch masks for PCF8574
    static constexpr uint8_t cMotor1UpLimitSwitchMask = 0x01;
    static constexpr uint8_t cMotor1DownLimitSwitchMask = 0x02;
    static constexpr uint8_t cMotor2UpLimitSwitchMask = 0x04;
    static constexpr uint8_t cMotor2DownLimitSwitchMask = 0x08;
};

#endif // CONTROL_CHANNEL_H