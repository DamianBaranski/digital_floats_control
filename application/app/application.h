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
#include "expander.h"
#include "colors.h"
#include "uart_communication.h"
#include "state.h"



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
   * @brief Gets the current state of the landing gear switch
   * @return true if the landing gear switch is active, false otherwise
   */
  bool getLdgGearSwitch();

  /**
   * @brief Gets the current state of the rudder switch
   * @return true if the rudder switch is active, false otherwise
   */
  bool getRudderSwitch();

  void processChannels();

  void animateLeds();
  
  
  /**
   * @brief Sets the LED brightness based on current user settings
   * 
   * Applies the brightness setting from UserSettings to the LED controller.
   */
  void setBrightness();

  /**
   * @brief Loads saved settings from non-volatile storage
   * 
   * Retrieves both user settings and channel configuration from persistent storage
   * and applies them to the current system state.
   */
  void loadSettings();

  /**
   * @brief Processes any pending state requests
   * 
   * Handles actions requested by the user or system, such as saving settings
   * or performing a device reset.
   */
  void processStateRequests();

  void updateExpanderState();

  void updateMonitoringData();
public:
  /** @brief Number of control channels in the system */
  static constexpr size_t NO_CHANNELS = 6;
  
  static constexpr uint32_t cRudderInactiveColor = Colors::YELLOW;
  static constexpr uint32_t cRudderDownColor = Colors::BLUE;
  static constexpr uint32_t cRudderUpColor = Colors::GREEN;
  static constexpr uint32_t cLdgGearDownColor = Colors::GREEN;
  static constexpr uint32_t cLdgGearUpColor = Colors::BLUE;

  /**
   * @struct ChannelsSettings
   * @brief Structure containing settings for all control channels
   */
  struct ChannelsSettings
  {
    ControlChannelSettings channelSettings[NO_CHANNELS]; /**< Array of settings for each channel */
  };
  
  /** @brief Reference to the Board Support Package for hardware interactions */
  Bsp &mBsp;
  
  /** @brief WS2812 LED controller for channel status indication */
  Ws2812<NO_CHANNELS> mLeds;
  
  Expander mExpanders[NO_CHANNELS/2];

  /** @brief Array of control channel objects */
  ControlChannel mChannels[NO_CHANNELS];
  
  /** @brief Persistent storage for channel settings */
  Settings<State::ChannelSettings> mChannelsSettings;

  Adc121c mCurrentSensors[NO_CHANNELS];

  State mState;

  /** @brief UART communication */
  UARTCommunication mUartCommunication;

};

#endif // APPLICATION_H
