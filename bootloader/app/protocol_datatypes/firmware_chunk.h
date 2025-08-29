#ifndef FIRMWARE_CHUNK_H
#define FIRMWARE_CHUNK_H
#include <cstdint>

/**
 * @struct UpdateFirmwareRequest
 * @brief Structure representing a firmware update request.
 * 
 * This structure contains the necessary information to perform a firmware update,
 * including the pointer to the location in the firmware, the data bytes to be written,
 * and the length of the data.
 */
typedef struct __attribute__ ((packed)) {
    uint32_t ptr;      /**< Pointer to the location in the firmware (offset from start) */
    uint16_t len;      /**< Length of the data in bytes */
    uint8_t data[256]; /**< Array holding the firmware data bytes */
} UpdateFirmwareRequest;

/**
 * @struct UpdateFirmwareResponse
 * @brief Structure representing the response to a firmware update request.
 * 
 * This structure contains the result of the firmware write operation,
 * indicating whether the operation was successful or not.
 */
typedef struct __attribute__ ((packed)) {
    bool result;       /**< Result of the firmware write operation (true if successful) */
} UpdateFirmwareResponse;

#endif // FIRMWARE_CHUNK_H