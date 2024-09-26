#ifndef APPLICATION_H
#define APPLICATION_H

#include "protocol2.h"
#include "bsp.h"
#include "control_channel.h"
#include "logger.h"
#include "settings.h"
#include "ws2812.h"

#define APP_VER "Application BS v1.0"

/// @class Application
/// @brief This class handles the main application logic, including protocol command processing and device communication.
class Application
{
private:
  struct UserSettings {
    uint8_t brightness;
    uint32_t ldgUpColor;
    uint32_t ldgDownColor;
    uint32_t rudderUpColor;
    uint32_t rudderDownColor;
    uint32_t rudderInactiveColor;
    uint32_t warningColor;
    uint32_t errorColor;
  };

  /// @union InProtocolData
  /// @brief A union representing different input data types for protocol commands.
  union InProtocolData
  {
    struct
    {
      uint8_t i2cAddress; ///< I2C device address used in I2C scan command.
    } i2cScan;
    size_t fileSize; ///< File size used for file-related commands (not currently implemented).
  };

  /// @union OutProtocolData
  /// @brief A union representing different output data types for protocol commands.
  union OutProtocolData
  {
    struct
    {
      char string[32]; ///< Application version string.
    } appVersion;
    struct
    {
      bool result; ///< Result of the I2C scan (true if device is ready).
    } i2cScan;

    //struct UserSettings userSettings;
  };

public:
  /// @brief Constructor for the Application class.
  /// @param bsp Reference to a BSP (Board Support Package) object used for hardware interactions.
  Application(Bsp &bsp);

  /// @brief Main loop function that processes incoming UART commands.
  /// Reads from the UART stream and processes the command using the protocol.
  void spin();

private:
  /// @brief Handles the 'v' command to send the application version.
  /// @param in Input protocol data (unused).
  /// @param out Output protocol data containing the application version string.
  /// @param outlen Output length of the data being sent.
  /// @return true Always returns true.
  bool sendAppVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /// @brief Handles the 'r' command to reset the device.
  /// @param in Input protocol data (unused).
  /// @param out Output protocol data (unused).
  /// @param outlen Output length of the data being sent (unused).
  /// @return true Always returns true.
  bool resetDevice(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /// @brief Handles the 's' command to scan I2C devices.
  /// @param in Input protocol data containing the I2C device address to scan.
  /// @param out Output protocol data containing the result of the scan.
  /// @param outlen Output length of the data being sent.
  /// @return true Always returns true.
  bool scanI2cDevices(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  bool sendUserSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  void testSwitchProcedure();

  void loadSettings();

  bool getLdgGearSwitch();

  bool getRudderSwitch();

  void processChannel(size_t channel, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  uint32_t getColorForDownState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  uint32_t getColorForUpState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  uint32_t getColorForMovingState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  bool relaysTest();
  void handleUartCommunication();
  void setBrightness();

private:
  static constexpr size_t NO_CHANNELS = 6;
  struct ChannelsSettings
  {
    ControlChannelSettings channelSettings[NO_CHANNELS];
  };

  Protocol<InProtocolData, OutProtocolData, 10> mProtocol; ///< Protocol object for handling commands.
  Bsp &mBsp;                                               ///< Reference to the Board Support Package for hardware interactions.
  Ws2812<NO_CHANNELS> mLeds;
  ControlChannel mChannels[NO_CHANNELS];
  Settings<ChannelsSettings> mChannelsSettings;
  Settings<UserSettings> mUserSettings;
  


};

#endif
