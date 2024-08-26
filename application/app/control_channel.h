#ifndef CONTROL_CHANNEL_H
#define CONTROL_CHANNEL_H

#include <cstdint>
#include "bit_mask.h"
#include "ii2c_master.h"
#include "logger.h"
#include "ina219.h"
#include "pcf8574.h"

enum class Warnings {
  LOW_MOTOR_IMPEDANCE,
  HIGH_MOTOR_IMPEDANCE,
  LOW_POWER_VOLTAGE,
  HIGH_POWER_VOLTAGE,
};

enum class Errors {
  NONE,
  OPEN_CIRCUIT,
  SHORT_CIRCUIT,
  TIME_EXCEEDED,
  PCF_COMMUNICATION_ISSUE,
  INA_COMMUNICATION_ISSUE,
  ENDSTOP_SHORT_CIRCUIT,
};

enum class State {
  UP,
  DOWN,
  MOVING,
  ERROR,
};

enum class LimitSwitch {
  UP,
  DOWN
};

typedef struct
{
    uint8_t enable : 1;
    uint8_t bridge : 1;
    uint8_t inverse_motor : 1;
    uint8_t inverse_up_limit_switch : 1;
    uint8_t inverse_down_limit_switch : 1;
    uint8_t inverse_limit_switch : 1;
    uint8_t rudder : 1;
    uint8_t ina_addr;
    uint16_t ina_callibration; // 0.01 Ohm
    uint8_t pcf_addr;
    uint8_t pcf_channel;        // 0 or 1
    uint16_t max_voltage_limit; // 0.1V
    uint16_t min_voltage_limit; // 0.1V
    uint16_t max_current_limit; // 0.1A
    uint16_t min_current_limit; // 0.1A
} ControlChannelSettings;

class ControlChannel
{
public:
    ControlChannel(II2cMaster &i2c);
    bool setSettings(const ControlChannelSettings &settings);
    bool connectionTest();
    bool setMotor(bool enable, uint8_t channel, bool dir);
    bool getPowerSensorStatus();
    bool isRudder() const;
    State getChannelState();

private:
    static constexpr uint8_t cPcfCfg = 0x0F;
    static constexpr uint8_t cMotor1RightDirMask = 0x10;
    static constexpr uint8_t cMotor1LeftDirMask = 0x20;
    static constexpr uint8_t cMotor2RightDirMask = 0x40;
    static constexpr uint8_t cMotor2LeftDirMask = 0x80;
    static constexpr uint8_t cMotor1UpLimitSwitchMask = 0x01;
    static constexpr uint8_t cMotor1DownLimitSwitchMask = 0x02;
    static constexpr uint8_t cMotor2UpLimitSwitchMask = 0x04;
    static constexpr uint8_t cMotor2DownLimitSwitchMask = 0x08;

    bool configure();
    bool getLimitSwitchState(LimitSwitch limit_switch);
    ControlChannelSettings mSettings;
    Ina219 mCurrentSensor;
    Pcf8574 mExpanderIO;
    BitMask<Errors> mErrors;
    BitMask<Warnings> mWarnings;
};

#endif