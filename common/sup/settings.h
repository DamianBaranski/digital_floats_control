#ifndef SETTINGS_H
#define SETTINGS_H
#include "flash.h"

template <typename T>
class Settings
{
public:
    Settings(Flash &flash, uint32_t address);
    bool load();
    bool save();
    T &get();

private:
    Flash &mFlash;
    uint32_t mAddress;
    struct {
        T data;
        uint16_t crc;
    } mData;
};

template <typename T>
Settings<T>::Settings(Flash &flash, uint32_t address): mFlash(flash), mAddress(address) {
    load();
}

template <typename T>
bool Settings<T>::load() {
    return false;
}

template <typename T>
bool Settings<T>::save() {
    uint16_t tmp;
    uint32_t address = mAddress;
    for(size_t i=0; i<sizeof(mData); i++) {
        tmp |= reinterpret_cast<uint8_t*>(&mData)[i] << (i%sizeof(uint16_t)*8);
        if(i%2 == 1) {
            mFlash.write(address, tmp);
            tmp = 0;
            address++;
        }
    }
    return true;
}

#endif