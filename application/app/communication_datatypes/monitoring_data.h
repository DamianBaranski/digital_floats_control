#ifndef MONITORING_DATA_H
#define MONITORING_DATA_H
#include <cstdint>

typedef struct __attribute__ ((packed)) {
    uint32_t timestamp; /**< Timestamp of the last measurement in milliseconds */
    uint16_t current;  /**< Measured current in milliamps */
    uint8_t channel;  /**< Channel identifier (0-5) */
    uint8_t state;     /**< Current state of the channel */
    uint8_t switches;  /**< State of switches (bit field) */
} MonitoringDataResponse;

typedef struct __attribute__ ((packed)) {
    uint8_t channel_id;  /**< Channel identifier (0-5) */
} MonitoringDataRequest;

#endif // MONITORING_DATA_H