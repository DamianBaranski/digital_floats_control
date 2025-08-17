/**
 * @file bootloader.h
 * @brief Bootloader implementation for firmware update and application control
 * @author Digital Floats Control Team
 * @date 2023
 *
 * This file contains the declaration of the Bootloader class which manages
 * bootloader functionalities including firmware updates and jumping to the main
 * application. It also defines the protocol data structures for communication.
 */

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "protocol.h"
#include "flash.h"
#include "logger.h"
#include "bsp.h"
#include "version.h"
#include <cstring>

/**
 * @def ETX_APP_START_ADDRESS
 * @brief Start address of the main application in flash memory
 */
#define ETX_APP_START_ADDRESS 0x08005000

/**
 * @def BOOTLOADER_VER
 * @brief Bootloader version string
 */
#define BOOTLOADER_VER "BootBS v1.0_" APP_VERSION

/**
 * @class Bootloader
 * @brief A class that manages bootloader functionalities, including command registration, 
 *        firmware updating, and application jumping.
 *
 * The Bootloader class implements the core functionality of the bootloader,
 * responsible for updating firmware, managing command protocols, and
 * transitioning to the main application when ready.
 */
class Bootloader
{
public:
    /**
     * @union InProtocolData
     * @brief A union representing different input data types for protocol commands.
     *
     * This union provides a flexible structure for handling various types of
     * input data in the bootloader protocol.
     */
    union InProtocolData
    {
        /**
         * @struct updateFirmware
         * @brief Structure containing firmware update data
         */
        struct
        {
            uint16_t ptr;       /**< Pointer/address offset for firmware update */
            uint16_t len;       /**< Length of firmware data chunk */
            uint8_t data[512];  /**< Firmware data buffer */
        } updateFirmware;
        
        uint8_t raw[1024];      /**< Raw input data buffer for generic access */
    };

    /**
     * @union OutProtocolData
     * @brief A union representing different output data types for protocol commands.
     *
     * This union provides a flexible structure for returning various types of
     * output data in the bootloader protocol responses.
     */
    union OutProtocolData
    {
        /**
         * @struct appVersion
         * @brief Structure containing application version information
         */
        struct
        {
            char string[32];    /**< Application version string */
        } appVersion;
        
        uint8_t result;         /**< Result code of the command */
        uint8_t raw[32];        /**< Raw output data buffer for generic access */
    };

    /**
     * @brief Registers commands with the protocol.
     * 
     * This method sets up the command handlers to respond to different
     * protocol messages.
     *
     * @param protocol The protocol instance for command registration.
     */
    void registerCommands(Protocol<InProtocolData, OutProtocolData, 10> &protocol);

    /**
     * @brief Jumps to the main application.
     * 
     * This method initiates a jump to the main application code at the
     * defined application start address.
     */
    void gotoApplication();

    /**
     * @brief Sends the bootloader version information.
     * 
     * This method handles the protocol command to retrieve the bootloader version.
     *
     * @param in Input protocol data (not used for this command).
     * @param out Output protocol data to store the version string.
     * @param outlen Output data length reference to update.
     * @return True if successful, false otherwise.
     */
    bool sendBootloaderVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

    /**
     * @brief Updates the firmware.
     * 
     * This method handles the protocol command to update firmware by writing
     * new firmware data to flash memory.
     *
     * @param in Input protocol data containing firmware update information.
     * @param out Output protocol data to store the result.
     * @param outlen Output data length reference to update.
     * @return True if successful, false otherwise.
     */
    bool updateFirmware(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

    /**
     * @brief Checks if the bootloader is waiting to proceed.
     * 
     * This method determines if the bootloader is in a waiting state 
     * before transitioning to the main application.
     *
     * @return True if in wait mode, false otherwise.
     */
    bool isWaiting();

private:
    Flash mFlash;                                 /**< Flash memory handler */
    uint32_t mTime;                               /**< Timestamp for wait checks */
    static constexpr uint32_t cWaitTime = 1000;   /**< Wait time constant in milliseconds */
};

#endif // BOOTLOADER_H
