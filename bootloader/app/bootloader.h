#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "protocol.h"
#include "flash.h"
#include "logger.h"
#include "bsp.h"
#include "version.h"
#include <cstring>

#define ETX_APP_START_ADDRESS 0x08005000
#define BOOTLOADER_VER "BootBS v1.0_" VERSION

/// @class Bootloader
/// @brief A class that manages bootloader functionalities, including command registration, 
///        firmware updating, and application jumping.
class Bootloader
{
public:
    /// @union InProtocolData
    /// @brief A union representing different input data types for protocol commands.
    union InProtocolData
    {
        struct
        {
            uint16_t ptr;    ///< Pointer for firmware update.
            uint16_t len;    ///< Length of firmware data.
            uint8_t data[512]; ///< Firmware data buffer.
        } updateFirmware;
        uint8_t raw[1024]; ///< Raw input data buffer.
    };

    /// @union OutProtocolData
    /// @brief A union representing different output data types for protocol commands.
    union OutProtocolData
    {
        struct
        {
            char string[32]; ///< Application version string.
        } appVersion;
        uint8_t result;      ///< Result code of the command.
        uint8_t raw[32];     ///< Raw output data buffer.
    };

    /// @brief Registers commands with the protocol.
    /// @param protocol The protocol instance for command registration.
    void registerCommands(Protocol<InProtocolData, OutProtocolData, 10> &protocol);

    /// @brief Jumps to the main application.
    void gotoApplication();

    /// @brief Sends the bootloader version information.
    /// @param in Input protocol data.
    /// @param out Output protocol data.
    /// @param outlen Output data length.
    /// @return True if successful, false otherwise.
    bool sendBootloaderVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

    /// @brief Updates the firmware.
    /// @param in Input protocol data.
    /// @param out Output protocol data.
    /// @param outlen Output data length.
    /// @return True if successful, false otherwise.
    bool updateFirmware(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

    /// @brief Checks if the bootloader is waiting to proceed.
    /// @return True if in wait mode, false otherwise.
    bool isWaiting();

private:
    Flash mFlash;            ///< Flash memory handler.
    uint32_t mTime;          ///< Timestamp for wait checks.
    static constexpr uint32_t cWaitTime = 1000; ///< Wait time constant in milliseconds.
};

#endif // BOOTLOADER_H
