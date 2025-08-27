#include "bootloader.h"
#include "protocol_datatypes/firmware_chunk.h"
#include "protocol_datatypes/firmware_info.h"

void Bootloader::registerCommands(Protocol<1024, 10> &protocol) {
    // Register 'v' command to retrieve bootloader version
    protocol.registerCmd('v', [&](const uint8_t &in, uint8_t &out, size_t &outlen) {
        return this->sendBootloaderVersion(in, out, outlen);
    });
    
    // Register 'u' command to handle firmware updates
    protocol.registerCmd('u', [&](const uint8_t &in, uint8_t &out, size_t &outlen) {
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

bool Bootloader::sendBootloaderVersion(const uint8_t &in, uint8_t &out, size_t &outlen) {
    LOG << "Getting bootloader version";
    FirmwareInfo &firmwareInfo = reinterpret_cast<FirmwareInfo&>(out);
    strcpy(firmwareInfo.app_version, BOOTLOADER_VER);
    strcpy(firmwareInfo.build_date, __DATE__);
    strcpy(firmwareInfo.build_time, __TIME__);
    strcpy(firmwareInfo.git_commit, GIT_COMMIT);
    
    outlen = sizeof(firmwareInfo);
    return true;
}

bool Bootloader::updateFirmware(const uint8_t &in, uint8_t &out, size_t &outlen) {
    // Reset the wait timer each time we receive an update command
    mTime = getTime();

    // Cast input and output buffers to the appropriate request/response structures
    const UpdateFirmwareRequest &updateFirmware = reinterpret_cast<const UpdateFirmwareRequest&>(in);
    UpdateFirmwareResponse &outResponse = reinterpret_cast<UpdateFirmwareResponse&>(out);

    // Check if we're at the beginning of a sector to erase it before writing
    if ((updateFirmware.ptr % mFlash.getSectorSize()) == 0) {
        // Erase 1 sector at the calculated address
        mFlash.erase(ETX_APP_START_ADDRESS + updateFirmware.ptr, 1);
    }
    
    // Write the firmware chunk to flash at the calculated address
    // Store the result of the write operation in the response
    outResponse.result = mFlash.write(ETX_APP_START_ADDRESS + updateFirmware.ptr, 
                             updateFirmware.data, 
                             updateFirmware.len);
    
    // Set the output length to the size of a boolean (results)
    outlen = sizeof(outResponse.result);
    return true;
}

bool Bootloader::isWaiting() {
    // Return true if we're still within the waiting period
    // This checks if current time is less than (mTime + cWaitTime)
    return (mTime + cWaitTime > getTime());
}
