#ifndef IUART_H
#define IUART_H

#include <cstdint>
#include <functional>

/// @class IUart
/// @brief Interface class for UART (Universal Asynchronous Receiver/Transmitter) operations.
///
/// This class provides an abstract interface for UART communication, including
/// methods to send data, check if data is being sent, and register a callback
/// for transmission completion.
class IUart {
public:
    /// @brief Sends data via UART.
    ///
    /// This pure virtual method must be implemented by derived classes to send
    /// the specified data through UART.
    ///
    /// @param data Pointer to the data to be sent.
    /// @param len The length of the data to be sent.
    /// @return True if the data was sent successfully, false otherwise.
    virtual bool send(const uint8_t *data, std::size_t len) = 0;

    /// @brief Checks if data is currently being sent via UART.
    ///
    /// This pure virtual method must be implemented by derived classes to
    /// determine if there is an ongoing data transmission.
    ///
    /// @return True if data is currently being sent, false otherwise.
    virtual bool isSending() const = 0;

    /// @brief Registers a callback function to be called when transmission is complete.
    ///
    /// This pure virtual method must be implemented by derived classes to
    /// register a callback function that will be called when the data transmission
    /// is complete.
    ///
    /// @param callback A std::function object representing the callback function.
    virtual void registerTxCallback(std::function<void()> callback) = 0;

    virtual void registerRxCallback(std::function<void(uint8_t)> callback) = 0;
};

#endif // IUART_H
