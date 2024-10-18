#include "bsp.h"
#include "logger.h"
#include "protocol2.h"
#include "flash.h"
#include "version.h"

#define ETX_APP_START_ADDRESS 0x08005000
#define BOOTLOADER_VER "BootBS v1.0_" VERSION
UartStream *UartStream::mInstance = nullptr;

/// @class Bootloader
/// @brief A class for bootloader functionalities.
class Bootloader
{
public:
  /// @union InProtocolData
  /// @brief A union representing different input data types for protocol commands.
  union InProtocolData
  {
    struct
    {
      uint16_t ptr;
      uint16_t len;
      uint8_t data[64];
    } updateFirmware;
    uint8_t raw[32];
  };

  /// @union OutProtocolData
  /// @brief A union representing different output data types for protocol commands.
  union OutProtocolData
  {
    struct
    {
      char string[32]; ///< Application version string.
    } appVersion;
    uint8_t result;
    uint8_t raw[32];
  };

  void registerCommands(Protocol<InProtocolData, OutProtocolData, 10> &protocol) {
    protocol.registerCmd('v', [&](const Bootloader::InProtocolData &in, Bootloader::OutProtocolData &out, size_t &outlen) {
        return this->sendBootloaderVersion(in, out, outlen);
    });
    protocol.registerCmd('u', [&](const Bootloader::InProtocolData &in, Bootloader::OutProtocolData &out, size_t &outlen) {
        return this->updateFirmware(in, out, outlen);
    });

  }

    void gotoApplication()
    {
        LOG << "Gonna Jump to Application...";
        sleep(100);
        void (*app_reset_handler)(void) = (void (*)())(*((volatile uint32_t *)(ETX_APP_START_ADDRESS + 4U)));

        if (app_reset_handler == (void (*)())0xFFFFFFFF)
        {
            LOG << "Invalid Application... HALT!!!";
            while (1)
                ;
        }
        app_reset_handler(); // Call the app reset handler
    }

    bool sendBootloaderVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen)
    {
        strcat(out.appVersion.string, BOOTLOADER_VER);
        outlen = strlen(BOOTLOADER_VER);
        return true;
    }

    bool updateFirmware(const InProtocolData &in, OutProtocolData &out, size_t &outlen)
    {
      if(in.updateFirmware.ptr == 0) {
        mFlash.erase(ETX_APP_START_ADDRESS,4096);
      }
      return true;
    }

    private:
    Flash mFlash;
};

int main()
{
    Bsp bsp;
    UartStream logStream(*bsp.uartBus);
    LOG << BOOTLOADER_VER;

    Bootloader bootloader;
    Protocol<Bootloader::InProtocolData, Bootloader::OutProtocolData, 10> protocol;


    char inBuff[128] = {};
    char outBuff[128] = {};

    for (size_t i = 0; i < 100; i++)
    {
        if (UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0))
        {
            if (protocol.process(inBuff, outBuff, sizeof(outBuff)))
            {
                Logger() << outBuff;
            }
        }
        sleep(10);
    }

    bootloader.gotoApplication();
    while (true)
        ;

    return 0;
}
