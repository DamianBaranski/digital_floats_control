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
#define BOOTLOADER_VER "BootBS v1.0"

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
     * @brief Registers commands with the protocol.
     * 
     * This method sets up the command handlers to respond to different
     * protocol messages.
     *
     * @param protocol The protocol instance for command registration.
     */
    void registerCommands(Protocol<1024, 10> &protocol);

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
    bool sendBootloaderVersion(const uint8_t &in, uint8_t &out, size_t &outlen);

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
    bool updateFirmware(const uint8_t &in, uint8_t &out, size_t &outlen);

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
