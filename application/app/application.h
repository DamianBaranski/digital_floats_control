/**
 * @file application.h
 * @brief Main application class for the digital float control system
 *
 * This file contains the Application class which is responsible for coordinating 
 * all functionality of the device, including handling UART commands, managing control
 * channels, LED indicators, and device settings.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "protocol.h"
#include "bsp.h"
#include "control_channel.h"
#include "logger.h"
#include "settings.h"
#include "ws2812.h"

/**
 * @brief Application version string macro
 */
#define APP_VER "AppBS v" VERSION

/**
 * @class Application
 * @brief This class handles the main application logic, including protocol command processing and device communication.
 *
 * The Application class serves as the central coordinator for the digital float control system.
 * It manages communication protocols, control channels, user settings, and hardware interactions
 * through the BSP (Board Support Package). It processes commands received via UART communication
 * and updates device state accordingly.
 */
class Application
{
private:
  /**
   * @struct UserSettings
   * @brief Structure containing configurable user preferences for LED colors and brightness
   * 
   * This structure stores color settings for different states of the landing gear and
   * rudder indicators, as well as status colors and the overall brightness level.
   */
  struct UserSettings {
    uint32_t ldgUpColor;          /**< Color for landing gear in up position */
    uint32_t ldgDownColor;        /**< Color for landing gear in down position */
    uint32_t rudderUpColor;       /**< Color for rudder in up position */
    uint32_t rudderDownColor;     /**< Color for rudder in down position */
    uint32_t rudderInactiveColor; /**< Color for rudder in inactive state */
    uint32_t warningColor;        /**< Color for warning indicators */
    uint32_t errorColor;          /**< Color for error indicators */
    uint8_t brightness;           /**< Global LED brightness (0-255) */
  };

  /**
   * @union InProtocolData
   * @brief A union representing different input data types for protocol commands.
   * 
   * This union allows the protocol handler to efficiently process different types of 
   * incoming data depending on the command being executed.
   */
  union InProtocolData
  {
    struct
    {
      uint8_t i2cAddress; /**< I2C device address used in I2C scan command */
    } i2cScan;
    
    /** @brief User settings data for color and brightness configuration */
    struct UserSettings userSettings;
    
    /** @brief Control channel settings and channel identifier */
    struct {
      ControlChannelSettings settings; /**< Configuration for a control channel */
      uint8_t channel;                 /**< Channel identifier (0-5) */
    } controlChannelSettings;
    
    /** @brief Channel identifier for operations targeting a specific channel */
    uint8_t channel_id;
    
    /** @brief Channel test configuration */
    struct {
      uint8_t ina_addr;     /**< INA219 current/voltage sensor address */
      uint8_t pcf_addr;     /**< PCF8574 I/O expander address */
      uint8_t pcf_channel;  /**< Channel on PCF8574 to test */
    } channelTest;
    
    /** @brief File size used for file-related commands (not currently implemented) */
    size_t fileSize;
    
    /** @brief Raw byte access to the union data */
    uint8_t raw[32];
  };

  /**
   * @union OutProtocolData
   * @brief A union representing different output data types for protocol commands.
   * 
   * This union allows the protocol handler to efficiently return different types of 
   * data depending on the command being processed.
   */
  union OutProtocolData
  {
    /** @brief Application version information */
    struct
    {
      char string[32]; /**< Application version string */
    } appVersion;
    
    /** @brief I2C scan result */
    struct
    {
      bool result; /**< Result of the I2C scan (true if device is ready) */
    } i2cScan;

    /** @brief User settings data for color and brightness configuration */
    struct UserSettings userSettings;
    
    /** @brief Control channel settings and channel identifier */
    struct {
      ControlChannelSettings settings; /**< Configuration for a control channel */
      uint8_t channel;                 /**< Channel identifier (0-5) */
    } controlChannelSettings;
    
    /** @brief Monitoring data for a channel */
    struct {
      uint16_t voltage;  /**< Measured voltage in millivolts */
      uint16_t current;  /**< Measured current in milliamps */
      uint8_t state;     /**< Current state of the channel */
      uint8_t switches;  /**< State of switches (bit field) */
    } monitoringData;
    
    /** @brief Generic result code */
    uint8_t result;
    
    /** @brief Raw byte access to the union data */
    uint8_t raw[32];
  };

public:
  /**
   * @brief Constructor for the Application class
   * @param bsp Reference to a BSP (Board Support Package) object used for hardware interactions
   * 
   * Initializes the application with the provided BSP instance for hardware access.
   * Sets up protocol handlers, control channels, and loads user settings.
   */
  Application(Bsp &bsp);

  /**
   * @brief Main loop function that processes incoming UART commands
   * 
   * This function should be called repeatedly in the main program loop.
   * It handles UART communication, processes protocol commands, manages control channels,
   * and updates LED indicators based on system state.
   */
  void spin();

private:
  /**
   * @brief Handles the 'v' command to send the application version
   * @param in Input protocol data (unused)
   * @param out Output protocol data containing the application version string
   * @param outlen Output length of the data being sent
   * @return true Always returns true
   */
  bool sendAppVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Handles the 'r' command to reset the device
   * @param in Input protocol data (unused)
   * @param out Output protocol data (unused)
   * @param outlen Output length of the data being sent (unused)
   * @return true Always returns true
   */
  bool resetDevice(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Handles the 's' command to scan I2C devices
   * @param in Input protocol data containing the I2C device address to scan
   * @param out Output protocol data containing the result of the scan
   * @param outlen Output length of the data being sent
   * @return true Always returns true
   */
  bool scanI2cDevices(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Sends current user settings to the client
   * @param in Input protocol data (unused)
   * @param out Output protocol data containing user settings
   * @param outlen Output length of the data being sent
   * @return true if settings were successfully sent
   */
  bool sendUserSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Updates user settings with values from client
   * @param in Input protocol data containing new user settings
   * @param out Output protocol data with result
   * @param outlen Output length of the data being sent
   * @return true if settings were successfully updated
   */
  bool updateUserSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Sends control channel settings to the client
   * @param in Input protocol data containing channel ID
   * @param out Output protocol data containing channel settings
   * @param outlen Output length of the data being sent
   * @return true if settings were successfully sent
   */
  bool sendChannelSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Updates control channel settings with values from client
   * @param in Input protocol data containing new channel settings
   * @param out Output protocol data with result
   * @param outlen Output length of the data being sent
   * @return true if settings were successfully updated
   */
  bool updateChannelSettings(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Sends monitoring data for a specific channel to the client
   * @param in Input protocol data containing channel ID
   * @param out Output protocol data containing monitoring information
   * @param outlen Output length of the data being sent
   * @return true if monitoring data was successfully sent
   */
  bool sendMonitoringData(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Configures a channel for testing
   * @param in Input protocol data containing test configuration
   * @param out Output protocol data with result
   * @param outlen Output length of the data being sent
   * @return true if test channel was successfully configured
   */
  bool setTestChannel(const InProtocolData &in, OutProtocolData &out, size_t &outlen);

  /**
   * @brief Performs a test procedure for switch functionality
   * 
   * Tests the operation of the switches by cycling through different states
   * and verifying proper operation.
   */
  void testSwitchProcedure();

  /**
   * @brief Loads saved settings from non-volatile storage
   * 
   * Retrieves both user settings and channel configuration from persistent storage
   * and applies them to the current system state.
   */
  void loadSettings();

  /**
   * @brief Gets the current state of the landing gear switch
   * @return true if the landing gear switch is active, false otherwise
   */
  bool getLdgGearSwitch();

  /**
   * @brief Gets the current state of the rudder switch
   * @return true if the rudder switch is active, false otherwise
   */
  bool getRudderSwitch();

  /**
   * @brief Processes a specific control channel based on switch states
   * @param channel Channel index to process
   * @param rudderSwitchState Current state of the rudder switch
   * @param ldgGearSwitchState Current state of the landing gear switch
   * @param time Current system time in milliseconds
   * 
   * Updates channel state and LED indicators based on switch positions and timing.
   */
  void processChannel(size_t channel, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  
  /**
   * @brief Determines the appropriate color for a channel in the down state
   * @param isRudder Flag indicating if the channel is for rudder control
   * @param rudderSwitchState Current state of the rudder switch
   * @param ldgGearSwitchState Current state of the landing gear switch
   * @param time Current system time in milliseconds
   * @return 32-bit RGB color value
   */
  uint32_t getColorForDownState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  
  /**
   * @brief Determines the appropriate color for a channel in the up state
   * @param isRudder Flag indicating if the channel is for rudder control
   * @param rudderSwitchState Current state of the rudder switch
   * @param ldgGearSwitchState Current state of the landing gear switch
   * @param time Current system time in milliseconds
   * @return 32-bit RGB color value
   */
  uint32_t getColorForUpState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  
  /**
   * @brief Determines the appropriate color for a channel in the moving state
   * @param isRudder Flag indicating if the channel is for rudder control
   * @param rudderSwitchState Current state of the rudder switch
   * @param ldgGearSwitchState Current state of the landing gear switch
   * @param time Current system time in milliseconds
   * @return 32-bit RGB color value (possibly animated based on time)
   */
  uint32_t getColorForMovingState(bool isRudder, bool rudderSwitchState, bool ldgGearSwitchState, uint32_t time);
  
  /**
   * @brief Tests the relay functionality for all channels
   * @return true if all relays are functioning correctly
   * 
   * Cycles through all relays and verifies that they properly actuate.
   */
  bool relaysTest();
  
  /**
   * @brief Handles UART communication protocol
   * 
   * Processes incoming protocol messages and dispatches to appropriate handler methods.
   */
  void handleUartCommunication();
  
  /**
   * @brief Sets the LED brightness based on current user settings
   * 
   * Applies the brightness setting from UserSettings to the LED controller.
   */
  void setBrightness();

private:
  /** @brief Number of control channels in the system */
  static constexpr size_t NO_CHANNELS = 6;
  
  /**
   * @struct ChannelsSettings
   * @brief Structure containing settings for all control channels
   */
  struct ChannelsSettings
  {
    ControlChannelSettings channelSettings[NO_CHANNELS]; /**< Array of settings for each channel */
  };

  /** @brief Protocol handler for command processing */
  Protocol<InProtocolData, OutProtocolData, 10> mProtocol;
  
  /** @brief Reference to the Board Support Package for hardware interactions */
  Bsp &mBsp;
  
  /** @brief WS2812 LED controller for channel status indication */
  Ws2812<NO_CHANNELS> mLeds;
  
  /** @brief Array of control channel objects */
  ControlChannel mChannels[NO_CHANNELS];
  
  /** @brief Persistent storage for channel settings */
  Settings<ChannelsSettings> mChannelsSettings;
  
  /** @brief Persistent storage for user preferences */
  Settings<UserSettings> mUserSettings;
};

#endif // APPLICATION_H
