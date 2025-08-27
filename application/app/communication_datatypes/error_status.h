#ifndef ERROR_STATUS_H
#define ERROR_STATUS_H
#include <cstdint>

typedef struct __attribute__ ((packed)) {
    uint8_t errors[6]; /**< Error codes for each of the 6 channels */
    uint8_t warnings[6]; /**< Warning codes for each of the 6 channels */
    uint8_t system_warnings; /**< System-wide warning codes */
} ErrorStatus;

#endif // ERROR_STATUS_H