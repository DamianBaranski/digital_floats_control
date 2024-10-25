#include "bootloader.h"
#include "bsp.h"
#include "logger.h"

UartStream *UartStream::mInstance = nullptr;

int main() {
    Bsp bsp;
    UartStream logStream(*bsp.uartBus);
    LOG << BOOTLOADER_VER;

    Bootloader bootloader;
    Protocol<Bootloader::InProtocolData, Bootloader::OutProtocolData, 10> protocol;
    bootloader.registerCommands(protocol);

    char inBuff[1024] = {};
    char outBuff[128] = {};

    while (bootloader.isWaiting()) {
        if (UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0)) {
            if (protocol.process(inBuff, outBuff, sizeof(outBuff))) {
                Logger() << outBuff;
            }
        }
        sleep(1);
    }

    bootloader.gotoApplication();
    while (true);

    return 0;
}
