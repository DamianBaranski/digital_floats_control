#ifndef EXPANDER_H
#define EXPANDER_H

#include "ii2c_master.h"
#include "pcf8574.h"

class Expander {
public:
    /// @brief Constructor for the Expander class.
    /// @param i2c Reference to an I2C master interface.
    Expander(II2cMaster &i2c);
    void setAddress(uint8_t address) {
        mAddr = address;
        mPcf.setAddress(mAddr);
    }
    bool update();
    uint8_t read();
    bool configure() {
        mPcf.setAddress(mAddr);
        return mPcf.write(cPcfCfg); // Configure PCF8574: lower 4 bits as inputs, upper 4 bits as outputs
    }

    void setMotor(uint8_t channel, bool enable, bool dir);

    bool write();
    bool connectionTest() const {
        return mPcf.connectionTest();
    }

    private:
    II2cMaster &mI2c; ///< Reference to the I2C master interface.
    uint8_t mAddr;    ///< I2C address of the expander.
    Pcf8574 mPcf; ///< Instance of the PCF8574 driver.
    uint8_t mData; ///< Data to be written to the expander.
    uint8_t mToWrite; ///< Data to be written to the expander.
    bool mOperationSuccessful; ///< Flag indicating if the last operation was successful.

    /** @name PCF8574 Configuration Constants */
    /**@{*/
    /** 
     * @brief Configuration value for the PCF8574
     * 
     * The PCF8574 configuration sets the lower 4 bits (0-3) as inputs for limit switches
     * and the upper 4 bits (4-7) as outputs for relay control.
     * Value 0x0F configures pins 0-3 as inputs (1) and pins 4-7 as outputs (0).
     */
    static constexpr uint8_t cPcfCfg = 0x0F; 

    /** @name Motor Direction Control Masks */
    /**@{*/
    static constexpr uint8_t cMotor1RightDirMask = 0x10; /**< Bit mask for Motor 1 right/up direction (pin 4) */
    static constexpr uint8_t cMotor1LeftDirMask = 0x20;  /**< Bit mask for Motor 1 left/down direction (pin 5) */
    static constexpr uint8_t cMotor2RightDirMask = 0x40; /**< Bit mask for Motor 2 right/up direction (pin 6) */
    static constexpr uint8_t cMotor2LeftDirMask = 0x80;  /**< Bit mask for Motor 2 left/down direction (pin 7) */
    /**@}*/

    /** @name Limit Switch Input Masks */
    /**@{*/
    static constexpr uint8_t cMotor1UpLimitSwitchMask = 0x01;   /**< Bit mask for Motor 1 up limit switch (pin 0) */
    static constexpr uint8_t cMotor1DownLimitSwitchMask = 0x02; /**< Bit mask for Motor 1 down limit switch (pin 1) */
    static constexpr uint8_t cMotor2UpLimitSwitchMask = 0x04;   /**< Bit mask for Motor 2 up limit switch (pin 2) */
    static constexpr uint8_t cMotor2DownLimitSwitchMask = 0x08; /**< Bit mask for Motor 2 down limit switch (pin 3) */
    /**@}*/
    /**@}*/
};

#endif // EXPANDER_H