#ifndef BSP_H
#define BSP_H

#include "igpio.h"
#include "iuart.h"
#include "iflash.h"
#include <memory>

/// @class Bsp
/// @brief Board Support Package (BSP) class.
///
/// This class provides the initialization and management of hardware resources 
class Bsp {
public:
    /// @brief Constructor for the Bsp class.
    ///
    /// Initializes the Bsp instance.
    Bsp();

    /// @brief Unique pointer to a UART interface.
    ///
    /// This pointer is used to manage the UART bus.
    std::unique_ptr<IUart> uartBus;

    std::unique_ptr<IGpio> ledPin;

    std::unique_ptr<IFlash> intFlash;
private:
    /// @brief Unique pointer to the GPIO pin used for RX.
    ///
    /// This pointer is used to manage the RX GPIO pin.
    std::unique_ptr<IGpio> mRxPin;

    /// @brief Unique pointer to the GPIO pin used for TX.
    ///
    /// This pointer is used to manage the TX GPIO pin.
    std::unique_ptr<IGpio> mTxPin;

    /// @brief Initializes the clock for the board.
    ///
    /// This method sets up the necessary clock configuration for the board.
    void initClock();
};

#endif // BSP_H