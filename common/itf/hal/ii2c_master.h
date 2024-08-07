#ifndef II2CMASTER_H
#define II2CMASTER_H

/// @class II2cMaster
/// @brief Interface class for I2C Master operations.
///
/// This class provides an abstract interface for I2C master communication.
class II2cMaster {
public:
    /// @brief Transmits data via I2C.
    ///
    /// This pure virtual method must be implemented by derived classes to handle
    /// the transmission of data over the I2C bus.
    ///
    /// @return True if the transmission was successful, false otherwise.
    virtual bool transmit() = 0;
};

#endif // II2CMASTER_H
