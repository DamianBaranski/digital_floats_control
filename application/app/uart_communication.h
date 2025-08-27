#include "state.h"
#include "iuart.h"
#include "protocol.h"


class UARTCommunication
{

public:
    /** @brief Constructor for the UARTCommunication class
     *  @param state Reference to the application state
     *  @param uart Reference to the UART interface for communication
     * 
     * Initializes the UART communication handler with the provided state and UART interface.
     * Sets up protocol command handlers for processing incoming messages.
     */
    UARTCommunication(State &state, IUart &uart);

    /**
     * @brief Main loop function to process incoming UART commands
     * 
     * This function should be called repeatedly in the main program loop.
     * It handles incoming UART data, processes protocol commands, and sends responses.
     * 
     * @return true if a command was processed successfully, false otherwise
     */
    bool spin();
private:
  /**
   * @brief Handles the 'v' command to send the application version
   * @param in Input protocol data (unused)
   * @param out Output protocol data containing the application version string
   * @param outlen Output length of the data being sent
   * @return true Always returns true
   */
  bool sendFirmwareInfo(const uint8_t &in, uint8_t &out, size_t &outlen);

  /**
   * @brief Handles the 'S' command to send the device status
   * @param in Input protocol data (unused)
   * @param out Output protocol data containing the status information
   * @param outlen Output length of the data being sent
   * @return true if status was successfully sent
   */
  bool sendStatus(const uint8_t &in, uint8_t &out, size_t &outlen);

  /**
   * @brief Handles the 'r' command to reset the device
   * @param in Input protocol data (unused)
   * @param out Output protocol data (unused)
   * @param outlen Output length of the data being sent (unused)
   * @return true Always returns true
   */
  bool resetDevice(const uint8_t &in, uint8_t &out, size_t &outlen);

  /**
   * @brief Sends control channel settings to the client
   * @param in Input protocol data containing channel ID
   * @param out Output protocol data containing channel settings
   * @param outlen Output length of the data being sent
   * @return true if settings were successfully sent
   */
  bool sendChannelSettings(const uint8_t &in, uint8_t &out, size_t &outlen);

  /**
   * @brief Updates control channel settings with values from client
   * @param in Input protocol data containing new channel settings
   * @param out Output protocol data with result
   * @param outlen Output length of the data being sent
   * @return true if settings were successfully updated
   */
  bool updateChannelSettings(const uint8_t &in, uint8_t &out, size_t &outlen);

  /**
   * @brief Sends monitoring data for a specific channel to the client
   * @param in Input protocol data containing channel ID
   * @param out Output protocol data containing monitoring information
   * @param outlen Output length of the data being sent
   * @return true if monitoring data was successfully sent
   */
  bool sendMonitoringData(const uint8_t &in, uint8_t &out, size_t &outlen);

    /**
     * @brief Handles the 'l' command to simulate remote control input
     * @param in Input protocol data containing remote control states
     * @param out Output protocol data (unused)
     * @param outlen Output length of the data being sent (unused)
     * @return true Always returns true
     */
  bool remoteControl(const uint8_t &in, uint8_t &out, size_t &outlen);

  /**
   * @brief Sends current error states to the client
   * @param in Input protocol data (unused)
   * @param out Output protocol data containing error information
   * @param outlen Output length of the data being sent
   * @return true if errors were successfully sent
   */
  bool sendErrors(const uint8_t &in, uint8_t &out, size_t &outlen);

    /** @brief Reference to the application state */
    State &mState;
    /** @brief Reference to the UART interface for communication */
    IUart &mUart;
      /** @brief Protocol handler for command processing */
    Protocol<160, 10> mProtocol;
};


