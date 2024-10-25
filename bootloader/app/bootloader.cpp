#include "bootloader.h"

void Bootloader::registerCommands(Protocol<InProtocolData, OutProtocolData, 10> &protocol) {
    protocol.registerCmd('v', [&](const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
        return this->sendBootloaderVersion(in, out, outlen);
    });
    protocol.registerCmd('u', [&](const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
        return this->updateFirmware(in, out, outlen);
    });
    mTime = getTime();
}

void Bootloader::gotoApplication() {
    LOG << "Gonna Jump to Application...";
    sleep(100);
    void (*app_reset_handler)(void) = (void (*)())(*((volatile uint32_t *)(ETX_APP_START_ADDRESS + 4U)));

    if (app_reset_handler == (void (*)())0xFFFFFFFF) {
        LOG << "Invalid Application... HALT!!!";
        while (1);
    }
    app_reset_handler();
}

bool Bootloader::sendBootloaderVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    strcat(out.appVersion.string, BOOTLOADER_VER);
    outlen = strlen(BOOTLOADER_VER);
    return true;
}

bool Bootloader::updateFirmware(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    mTime = getTime();
    if ((in.updateFirmware.ptr % mFlash.getSectorSize()) == 0) {
        mFlash.erase(ETX_APP_START_ADDRESS + in.updateFirmware.ptr, 1);
    }
    out.result = mFlash.write(ETX_APP_START_ADDRESS + in.updateFirmware.ptr, in.updateFirmware.data, in.updateFirmware.len);
    outlen = sizeof(bool);
    return true;
}

bool Bootloader::isWaiting() {
    return (mTime + cWaitTime > getTime());
}
