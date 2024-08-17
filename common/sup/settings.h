#ifndef SETTINGS_H
#define SETTINGS_H

#include "flash.h"

/// @brief A template class for managing settings stored in flash memory.
/// @tparam T The type of the settings data to be managed.
/// @todo Add calculation and checking CRC
template <typename T>
class Settings
{
public:
    /// @brief Constructor that initializes the Settings class.
    /// @param flash Reference to the Flash object used for reading and writing to flash memory.
    /// @param address The address in flash memory where the settings are stored.
    Settings(Flash &flash, uint32_t address);

    /// @brief Loads the settings from flash memory.
    /// @return `true` if the settings were successfully loaded, `false` otherwise.
    bool load();

    /// @brief Saves the current settings to flash memory.
    /// @return `true` if the settings were successfully saved, `false` otherwise.
    bool save();

    /// @brief Returns a reference to the settings data.
    /// @return A reference to the settings data.
    T &get();

private:
    Flash &mFlash;         ///< Reference to the Flash object for flash memory operations.
    uint32_t mAddress;     ///< The address in flash memory where the settings are stored.
    
    /// @brief Structure to hold the settings data and its associated CRC.
    struct {
        T data;            ///< The settings data.
        uint16_t crc;      ///< The CRC checksum for the settings data.
    } mData;
};

template <typename T>
Settings<T>::Settings(Flash &flash, uint32_t address): mFlash(flash), mAddress(address) {
    load();
}

template <typename T>
bool Settings<T>::load() {
    if(!mFlash.read(mAddress, reinterpret_cast<uint8_t*>(&mData), sizeof(mData))) {
        return false;
    }
    if(mData.crc != 0) {
        return false;
    }

    return true;
}

template <typename T>
bool Settings<T>::save() {
    mData.crc = 0;
    if(!mFlash.erase(mAddress, sizeof(mData)/mFlash.getSectorSize()+1)) {
        return false;
    }
    return mFlash.write(mAddress, reinterpret_cast<uint8_t*>(&mData), sizeof(mData));
}

template <typename T>
T &Settings<T>::get() {
    return mData.data;
}

#endif // SETTINGS_H
