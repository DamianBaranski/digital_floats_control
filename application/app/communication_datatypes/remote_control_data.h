#ifndef REMOTE_CONTROL_DATA_H
#define REMOTE_CONTROL_DATA_H
#include <cstdint>

typedef struct __attribute__ ((packed)) {
    uint8_t ldg_gear_switch: 1;      /**< Landing gear switch state (1=deployed, 0=retracted) */
    uint8_t rudder_switch: 1;        /**< Rudder switch state (1=deployed, 0=retracted) */
    uint8_t test_button: 1;          /**< Test button state (1=pressed, 0=not pressed) */
} RemoteControlData;

#endif // REMOTE_CONTROL_DATA_H