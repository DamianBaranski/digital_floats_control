#ifndef BASE64_H
#define BASE64_H

#include <gmock/gmock.h>

class Base64Mock {
public:
    static Base64Mock& getReference() {
        if (mPointer == nullptr) {
            mPointer = new Base64Mock();
        }
        return *mPointer;
    }

    static void clean() {
        if(mPointer == nullptr) {
            return;
        }
        
        delete mPointer;
        mPointer = nullptr;
    }

    MOCK_METHOD(size_t, decodedSize, (const char* input));
    MOCK_METHOD(bool, decode, (const char* input, uint8_t* output, size_t* decodedLen));
    MOCK_METHOD(bool, encode, (const uint8_t* input, size_t inputLen, char* output));
    MOCK_METHOD(size_t, encodedSize, (size_t inputLen));

private:
    static Base64Mock* mPointer;
};

// Initialize the static member
Base64Mock* Base64Mock::mPointer = nullptr;

class Base64 {
public:
    static size_t encodedSize(size_t input_len) {
        return Base64Mock::getReference().encodedSize(input_len);
    }

    static size_t decodedSize(const char* input) {
        return Base64Mock::getReference().decodedSize(input);
    }

    static bool encode(const uint8_t* input, size_t len, char* output) {
        return Base64Mock::getReference().encode(input, len, output);
    }

    static bool decode(const char* input, uint8_t* output, size_t* out_len) {
        return Base64Mock::getReference().decode(input, output, out_len);
    }
};

#endif // BASE64_H
