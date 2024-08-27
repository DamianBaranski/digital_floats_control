#ifndef BSP_H
#define BSP_H

#include "igpio.h"
#include "ii2c_master.h"
#include "iuart.h"
#include "ipwm_dma.h"
#include "ispi.h"
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

    /// @brief Unique pointer to an I2C master interface.
    ///
    /// This pointer is used to manage the I2C bus.
    std::unique_ptr<II2cMaster> i2cBus;

    /// @brief Unique pointer to a UART interface.
    ///
    /// This pointer is used to manage the UART bus.
    std::unique_ptr<IUart> uartBus;

    std::unique_ptr<IGpio> testSwitch;
    std::unique_ptr<IGpio> ldgSwitch;
    std::unique_ptr<IGpio> rudSwitch;
    std::unique_ptr<IPwmDma> leds;
    std::unique_ptr<IFlash> extFlash;

private:
    /// @brief Unique pointer to the GPIO pin used for SDA.
    ///
    /// This pointer is used to manage the SDA GPIO pin.
    std::unique_ptr<IGpio> mSdaPin;

    /// @brief Unique pointer to the GPIO pin used for SCL.
    ///
    /// This pointer is used to manage the SCL GPIO pin.
    std::unique_ptr<IGpio> mSclPin;

    /// @brief Unique pointer to the GPIO pin used for RX.
    ///
    /// This pointer is used to manage the RX GPIO pin.
    std::unique_ptr<IGpio> mRxPin;

    /// @brief Unique pointer to the GPIO pin used for TX.
    ///
    /// This pointer is used to manage the TX GPIO pin.
    std::unique_ptr<IGpio> mTxPin;

    std::unique_ptr<IGpio> mLedDataPin;

    std::unique_ptr<IGpio> mSpiClk;

    std::unique_ptr<IGpio> mSpiMosi;

    std::unique_ptr<IGpio> mSpiMiso;

    std::unique_ptr<ISpi> mSpi;

    std::unique_ptr<IGpio> mSpiCsPin;

    /// @brief Initializes the clock for the board.
    ///
    /// This method sets up the necessary clock configuration for the board.
    void initClock();
};

void sleep(uint32_t time_ms);
uint32_t getTime();
#endif // BSP_H