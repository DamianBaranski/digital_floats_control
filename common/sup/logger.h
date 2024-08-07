#ifndef LOGGER_H
#define LOGGER_H

#include "iuart.h"
#include "ring_buffer.h"
#include <cassert>
#include <cstring>
#include <cstdio>



class UartStream {
    public:
        UartStream(IUart &uart):mUart(uart), mLines(0), mSending(0) {
            if(mInstance) {
                assert("Cannot create second instance");
            }
            mInstance = this;
            mUart.registerTxCallback([this](){this->txCompleted();});
            mUart.registerRxCallback([this](uint8_t data){this->rxCompleted(data);});
        }

        void print(const char* str) {
            std::size_t len = strlen(str);
            mTxBuffer.push(reinterpret_cast<const uint8_t*>(str), len);
            send();
        }

        static UartStream *getInstance() {
            if(!mInstance) {
                assert("UartStream is not created");
            }
            return mInstance;
        }

        bool readLine(char *buff, size_t buff_size, size_t readed) {
            if(mLines == 0) return false;
            for(size_t i=0; i<buff_size; i++){
                buff[i] = mRxBuffer.pop();
                if(buff[i] == '\r') {
                    mLines--;
                    buff[i] = 0;
                    return true;
                }
            }
            return true;
        }

    private:
    void txCompleted(){
        mTxBuffer.remove(mSending);
        if(!mTxBuffer.empty()) {
            send();
        }
    }

    void rxCompleted(uint8_t data){
        mRxBuffer.push(data);
        if(data=='\r') {
            mLines++;
        }
    }

    void send() {
        if(mUart.isSending()) {
            return;
        }
        mSending = mTxBuffer.chunkSize();
        mUart.send(mTxBuffer.front(), mSending);
    }

    IUart &mUart;
    size_t mLines;
    size_t mSending;
    static UartStream *mInstance;
    RingBuffer<uint8_t, 1024> mTxBuffer;
    RingBuffer<char, 1024> mRxBuffer;



};


class Logger {
    public:
    Logger(){};
    ~Logger(){operator<<("\r\n");}
    Logger& operator<<(const char* str) {
        UartStream::getInstance()->print(str);
        return *this;
    }
    Logger& operator<<(char c) {
        char tmp[2] = {};
        tmp[0] = c;
        UartStream::getInstance()->print(tmp);
        return *this;
    }

    Logger& operator<<(int value) {
        char buffer[12]; // Enough to hold all int values including negative sign
        snprintf(buffer, sizeof(buffer), "%d", value);
        UartStream::getInstance()->print(buffer);
        return *this;
    }

    Logger& operator<<(uint32_t value) {
        operator<<(static_cast<int>(value));
        return *this;
    }

    Logger& operator<<(size_t value) {
        operator<<(static_cast<int>(value));
        return *this;
    }
};

#endif