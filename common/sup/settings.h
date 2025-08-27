#ifndef SETTINGS_H
#define SETTINGS_H

#include "iflash.h"

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
    Settings(IFlash &flash, uint32_t address, T &data);

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
    IFlash &mFlash;         ///< Reference to the Flash object for flash memory operations.
    uint32_t mAddress;     ///< The address in flash memory where the settings are stored.
    T &mData;            ///< The settings data.
    /// @brief Structure to hold the settings data and its associated CRC.
    struct SettingsData {
        T data;            ///< The settings data.
        uint16_t crc;      ///< The CRC checksum for the settings data.
    };
};

template <typename T>
Settings<T>::Settings(IFlash &flash, uint32_t address, T &data): mFlash(flash), mAddress(address), mData(data) {
    // Attempt to load settings from flash on initialization
    load();
}

template <typename T>
bool Settings<T>::load() {
    SettingsData data = {};
    if(!mFlash.read(mAddress, reinterpret_cast<uint8_t*>(&data), sizeof(data))) {
        return false;
    }
    if(data.crc != 0) {
        return false;
    }

    memcpy(&mData, &data.data, sizeof(T));
    return true;
}

template <typename T>
bool Settings<T>::save() {
    SettingsData data = {};
    data.crc = 0;
    data.data = mData;
    // Erase necessary sectors before writing
    if(!mFlash.erase(mAddress, sizeof(data)/mFlash.getSectorSize()+1)) {
        return false;
    }
    return mFlash.write(mAddress, reinterpret_cast<uint8_t*>(&data), sizeof(data));
}

template <typename T>
T &Settings<T>::get() {
    return mData;
}

#endif // SETTINGS_H
