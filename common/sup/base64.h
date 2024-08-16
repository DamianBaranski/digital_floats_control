#ifndef BASE64_H
#define BASE64_H

#include <cstring>
#include <cstdint>

/// @class Base64
/// @brief A utility class for encoding and decoding data in Base64 format.
class Base64 {
private:
    static const char base64_table[]; ///< Base64 encoding table.
    static const char base64_pad = '='; ///< Padding character used in Base64 encoding.

    /// @brief Gets the index of a Base64 character in the encoding table.
    /// @param c The Base64 character.
    /// @return The index of the character in the Base64 table, or -1 if the character is invalid.
    static int base64_char_index(char c);

public:
    /// @brief Calculates the size of the Base64 encoded output given an input length.
    /// @param input_len The length of the input data in bytes.
    /// @return The size of the encoded data in bytes, including the null terminator.
    static size_t encodedSize(size_t input_len);

    /// @brief Calculates the size of the decoded output given a Base64 encoded input.
    /// @param input The Base64 encoded input string.
    /// @return The size of the decoded data in bytes.
    static size_t decodedSize(const char *input);

    /// @brief Encodes a binary input into a Base64 encoded string.
    /// @param input The binary data to encode.
    /// @param len The length of the binary data.
    /// @param output The output buffer to store the Base64 encoded string. This buffer should be large enough to hold the encoded data plus a null terminator.
    static void encode(const unsigned char *input, size_t len, char *output);

    /// @brief Decodes a Base64 encoded string into binary data.
    /// @param input The Base64 encoded input string.
    /// @param output The output buffer to store the decoded binary data.
    /// @param out_len The length of the decoded data.
    /// @return True if the decoding was successful, false if the input is invalid.
    static bool decode(const char *input, unsigned char *output, size_t *out_len);
};

#endif
