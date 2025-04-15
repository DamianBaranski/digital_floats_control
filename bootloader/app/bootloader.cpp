#include "bootloader.h"

void Bootloader::registerCommands(Protocol<InProtocolData, OutProtocolData, 10> &protocol) {
    // Register 'v' command to retrieve bootloader version
    protocol.registerCmd('v', [&](const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
        return this->sendBootloaderVersion(in, out, outlen);
    });
    
    // Register 'u' command to handle firmware updates
    protocol.registerCmd('u', [&](const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
        return this->updateFirmware(in, out, outlen);
    });
    
    // Initialize wait timer for bootloader-to-application transition
    mTime = getTime();
}

void Bootloader::gotoApplication() {
    LOG << "Gonna Jump to Application...";
    sleep(100);  // Brief delay before jump
    
    // Get the application reset handler from the vector table
    // The reset handler is at offset 4 in the vector table
    void (*app_reset_handler)(void) = (void (*)())(*((volatile uint32_t *)(ETX_APP_START_ADDRESS + 4U)));

    // Check if the application reset handler is valid
    if (app_reset_handler == (void (*)())0xFFFFFFFF) {
        LOG << "Invalid Application... HALT!!!";
        while (1);  // Infinite loop if application is invalid
    }
    
    // Jump to the application reset handler
    app_reset_handler();
}

bool Bootloader::sendBootloaderVersion(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    // Copy the bootloader version to the output buffer
    strcat(out.appVersion.string, BOOTLOADER_VER);
    outlen = strlen(BOOTLOADER_VER);
    return true;
}

bool Bootloader::updateFirmware(const InProtocolData &in, OutProtocolData &out, size_t &outlen) {
    // Reset the wait timer each time we receive an update command
    mTime = getTime();
    
    // Check if we're at the beginning of a sector to erase it before writing
    if ((in.updateFirmware.ptr % mFlash.getSectorSize()) == 0) {
        // Erase 1 sector at the calculated address
        mFlash.erase(ETX_APP_START_ADDRESS + in.updateFirmware.ptr, 1);
    }
    
    // Write the firmware chunk to flash at the calculated address
    // Store the result of the write operation in the response
    out.result = mFlash.write(ETX_APP_START_ADDRESS + in.updateFirmware.ptr, 
                             in.updateFirmware.data, 
                             in.updateFirmware.len);
    
    // Set the output length to the size of a boolean (results)
    outlen = sizeof(bool);
    return true;
}

bool Bootloader::isWaiting() {
    // Return true if we're still within the waiting period
    // This checks if current time is less than (mTime + cWaitTime)
    return (mTime + cWaitTime > getTime());
}
