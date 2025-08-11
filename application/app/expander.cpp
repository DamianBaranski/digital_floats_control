#include "expander.h"
#pragma GCC push_options
#pragma GCC optimize ("O0")

Expander::Expander(II2cMaster &i2c)
    : mI2c(i2c), mPcf(i2c) {
}

void Expander::update() {
    mData = mPcf.read();
    mToWrite = cPcfCfg;
}

uint8_t Expander::read() {
    return mData;
}

bool Expander::write() {
    return mPcf.write(mToWrite);
}

    void Expander::setMotor(uint8_t channel, bool enable, bool dir) {
        uint8_t mask = 0;
        if(channel == 0) {
            mask = dir?cMotor1RightDirMask:cMotor1LeftDirMask;
        } else if(channel == 1) {
            mask = dir?cMotor2RightDirMask:cMotor2LeftDirMask;
        }

        if(enable) {
            mToWrite |= mask;
        } else {
            mToWrite &= ~mask;
        }
        mToWrite |= cPcfCfg; // Ensure configuration bits are preserved
    }

    #pragma GCC pop_options