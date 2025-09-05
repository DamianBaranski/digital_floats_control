#ifndef STATE_H
#define STATE_H

#include <cstdint>
#include "errors.h"
#include "control_channel.h"
#include "settings.h"

// State machine for blinking
enum class BlinkState : uint8_t {
    Idle,   /**< Waiting / ready to start a new sequence */
    On,     /**< LED ON phase */
    Off,    /**< LED OFF phase */
    Delay   /**< Pause between sequences */
};

enum class DisplayMode : uint8_t {
    ERRORS,   /**< Show errors */
    WARNINGS  /**< Show warnings */
};
struct State {
    struct RemoteControlState {
        uint32_t mSimulationTimeout;
        bool mLdgGearSwitchState;
        bool mRudderSwitchState;
        bool mTestSwitchState;
    } mRemoteControl;

    struct SwitchState {
        bool mLdgGearSwitchState;
        bool mRudderSwitchState;
        bool mTestSwitchState;
    } mSwitches;
    
    struct SystemState {
        uint32_t mUptime;
        uint16_t mPowerVoltage;
        uint16_t mMemoryUsage;
    } mSystem;

    struct ActionRequest {
        bool mDeviceReset;
        bool mSaveSettings;
    } mActionRequest;

    struct debugData {
        uint32_t request_time;
        uint32_t expander_update_time;
        uint32_t switch_read_time;
        uint32_t current_read_time;
        uint32_t channel_process_time;
        uint32_t expander_write_time;
        uint32_t led_update_time;
        uint32_t uart_time;
        uint32_t total_time;
    } mDebugData;
    struct MonitoringData {
      uint32_t timestamp; /**< Timestamp of the last measurement in milliseconds */
      uint16_t current;  /**< Measured current in milliamps */
      uint8_t state;     /**< Current state of the channel */
      uint8_t switches;  /**< State of switches (bit field) */
    } mMonitoringData[6];

    struct MovementTimings {
        bool buttons_changed; /**< Flag indicating if buttons changed since last check */
        uint32_t start_time[6];      /**< Timestamp when movement started */
    } mMovementTimings;

    struct ErrorsDisplay {
        DisplayMode mode;                      /**< Current display mode */ 
        uint8_t channel;             /**< Channel number (0..NO_CHANNELS-1) */
        uint8_t currentIdx;          /**< Index of the current error/warning */
        uint32_t last_led_update;    /**< Timestamp of the last LED update */
        BlinkState state;               /**< Current state in the blink sequence */
        uint8_t blinksRemaining;     /**< Number of blinks remaining in current sequence */
    } mErrorsDisplay;

    typedef ControlChannelSettings ChannelSettings[6];
    ChannelSettings mChannelSettings;

    Errors mErrors;
};

#endif // STATE_H