#ifndef STATUS_DATA_H
#define STATUS_DATA_H
#include <cstdint>

typedef struct __attribute__ ((packed)) {
    uint16_t power_voltage;/**< Power supply voltage in millivolts */
    uint16_t memory_usage;  /**< Memory usage percentage (0-100%) */
    uint32_t uptime;       /**< System uptime in seconds */
    uint8_t ldg_gear_switch: 1; /**< Landing gear switch state (1=deployed, 0=retracted) */
    uint8_t rudder_switch: 1;      /**< Rudder switch state (1=deployed, 0=retracted) */
    uint8_t test_button: 1;  /**< Test button state (1=pressed, 0=not pressed) */
    uint8_t remote_control_status: 1; /**< Remote control connection state (1=connected, 0=disconnected) */
} StatusData;

#endif // STATUS_DATA_H