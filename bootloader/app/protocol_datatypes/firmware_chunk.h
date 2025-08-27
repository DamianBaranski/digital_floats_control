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
struct UpdateFirmwareRequest {
    uint32_t ptr;      /**< Pointer to the location in the firmware (offset from start) */
    uint8_t data[256]; /**< Array holding the firmware data bytes */
    uint16_t len;      /**< Length of the data in bytes */
};

/**
 * @struct UpdateFirmwareResponse
 * @brief Structure representing the response to a firmware update request.
 * 
 * This structure contains the result of the firmware write operation,
 * indicating whether the operation was successful or not.
 */
struct UpdateFirmwareResponse {
    bool result;       /**< Result of the firmware write operation (true if successful) */
};

#endif // FIRMWARE_CHUNK_H