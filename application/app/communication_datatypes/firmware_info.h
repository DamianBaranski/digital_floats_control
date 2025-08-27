#ifndef FIRMWARE_INFO_H
#define FIRMWARE_INFO_H
#include <cstdint>

typedef struct __attribute__ ((packed)) {
    char app_version[20];      /**< Application version string */
    char hardware_version[20]; /**< Hardware version string */
    char serial_number[20];   /**< Device serial number string */
    char build_date[20];       /**< Build date string */
    char build_time[20];       /**< Build time string */
    char git_commit[40];       /**< Git commit hash string */
} FirmwareInfo;

#endif // FIRMWARE_INFO_H