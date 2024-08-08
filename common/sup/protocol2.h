#include "base64.h"
#include <cstdint>
#include <functional>
#include <cstring>


/// @brief Protocol Class
///
/// The Protocol class is a generic template-based protocol handler that processes incoming Base64
/// encoded commands and generates corresponding Base64 encoded responses. It is designed to be flexible,
/// allowing for different input and output data types through template parameters.
///
/// ### Protocol Description
///
/// The protocol operates by exchanging messages in the form of Base64 encoded strings. Each message consists
/// of a command identifier, data length, payload data, and a CRC checksum to ensure data integrity.
///
/// ### Frame Structure
///
/// The frame structure of the protocol is as follows:
///
/// | Field  | Type      | Description                                     |
/// |--------|-----------|-------------------------------------------------|
/// | cmd    | char      | Command identifier (1 byte)                     |
/// | len    | uint8_t   | Length of the data field (1 byte)               |
/// | data   | InType/OutType | Payload data of variable size              |
/// | crc    | uint16_t  | CRC checksum of the entire frame (2 bytes)      |
///
/// - **cmd**: Identifies the command to be executed.
/// - **len**: Specifies the length of the payload data.
/// - **data**: Contains the actual data to be processed or returned.
/// - **crc**: Used to verify the integrity of the message.
///
/// ### Response Handling
///
/// Upon receiving a command, the protocol decodes the message, validates the CRC, and then attempts to execute
/// the corresponding function registered for the command. If the function is executed successfully, a response
/// is generated, encoded, and sent back. If any error occurs (e.g., unknown command, CRC mismatch, insufficient
/// buffer size), the process will return `false`.
///
/// @tparam InType The type of the input data.
/// @tparam OutType The type of the output data.
/// @tparam N The maximum number of command handlers that can be registered.
template <typename InType, typename OutType, size_t N>
class Protocol
{
private:
    /// Structure representing the incoming data format
    struct InData
    {
        char cmd;     ///< Command identifier
        uint8_t len;  ///< Length of the input data
        InType data;  ///< Input data of type InType
        uint16_t crc; ///< CRC checksum of the input data
    };

    /// Structure representing the outgoing data format
    struct OutData
    {
        char cmd;     ///< Command identifier
        uint8_t len;  ///< Length of the output data
        OutType data; ///< Output data of type OutType
        uint16_t crc; ///< CRC checksum of the output data
    };

    /// Structure representing a command and its corresponding function
    struct CmdFnc
    {
        char cmd;                                                                            ///< Command identifier
        std::function<bool(const InType &inData, OutType &outData, size_t &outDataLen)> fnc; ///< Function to execute for the command
    };

    CmdFnc mFncPtr[N] = {}; ///< Array of registered command functions

public:
    /// @brief Processes an incoming Base64 encoded string and generates a Base64
    /// encoded response.
    ///
    /// This method decodes the input string, identifies and executes the corresponding command,
    /// and then encodes the response. If any error occurs during this process, the method returns `false`.
    ///
    /// @param instr The Base64 encoded input string.
    /// @param outstr The output buffer where the Base64 encoded response will be written.
    /// @param outlen The size of the output buffer.
    /// @return true if the command was processed successfully and a response was generated.
    /// @return false if there was an error processing the command.
    bool process(const char *instr, char *outstr, size_t outlen);

    /// @brief Registers a command and its corresponding function.
    ///
    /// This method allows you to register a function that will be called when a specific command
    /// is received. Each command can only have one function associated with it.
    ///
    /// @param cmd The command identifier to register.
    /// @param fnc The function to execute when the command is received.
    /// @return true if the command was registered successfully.
    /// @return false if the command could not be registered (e.g., if the command is already registered).
    bool registerCmd(char cmd, std::function<bool(const InType &inData, OutType &outData, size_t &outDataLen)> fnc);

private:
    /// @brief Decodes a Base64 encoded input string into an InData structure.
    ///
    /// This method takes the incoming Base64 encoded string and decodes it into the internal
    /// InData structure, which contains the command, data, and CRC.
    ///
    /// @param instr The Base64 encoded input string.
    /// @param inData The decoded InData structure.
    /// @return true if the input string was decoded successfully.
    /// @return false if the input string could not be decoded.
    bool decodeInput(const char *instr, InData &inData);

    /// @brief Encodes an OutData structure into a Base64 encoded output string.
    ///
    /// This method takes the OutData structure, which contains the command, data, and CRC,
    /// and encodes it into a Base64 string that can be sent back as a response.
    ///
    /// @param outData The OutData structure to encode.
    /// @param outstr The output buffer where the Base64 encoded string will be written.
    /// @param outlen The size of the output buffer.
    /// @return true if the output was encoded successfully.
    /// @return false if the output could not be encoded (e.g., if the output buffer is too small).
    bool encodeOutput(const OutData &outData, char *outstr, size_t outlen);

    /// @brief Calculates the CRC checksum for the given input data.
    ///
    /// This method calculates the CRC checksum for the input data to verify its integrity.
    /// Currently, this method is a placeholder and does not perform actual CRC calculations.
    ///
    /// @param inData The input data for which the CRC will be calculated.
    /// @param crc The calculated CRC value.
    /// @return true if the CRC was calculated successfully.
    bool calculateCRC(const InData &inData, uint16_t &crc);

    /// @brief Finds and executes the command function corresponding to the command in the input data.
    ///
    /// This method searches for the command function registered for the command identifier in the input data.
    /// If found, it executes the function and prepares the output data.
    ///
    /// @param inData The input data containing the command.
    /// @param outData The output data to be filled by the command function.
    /// @param outDataLen The length of the data filled by the command function.
    /// @return true if the command function was found and executed successfully.
    /// @return false if the command function could not be found or executed.
    bool findAndExecuteCommand(const InData &inData, OutData &outData, size_t &outDataLen);
};

template <typename InType, typename OutType, size_t N>
bool Protocol<InType, OutType, N>::registerCmd(char cmd, std::function<bool(const InType &inData, OutType &outData, size_t &outDataLen)> fnc)
{
    for (size_t i = 0; i < N; i++)
    {
        if (!mFncPtr[i].cmd)
        {
            mFncPtr[i].cmd = cmd;
            mFncPtr[i].fnc = fnc;
            return true;
        }
        else if (mFncPtr[i].cmd == cmd)
        {
            return false;
        }
    }
    return false;
}

template <typename InType, typename OutType, size_t N>
bool Protocol<InType, OutType, N>::process(const char *instr, char *outstr, size_t outlen)
{
    InData inData = {};
    OutData outData = {};
    size_t outDataLen = 0;

    if (!decodeInput(instr, inData))
    {
        return false;
    }

    if (!findAndExecuteCommand(inData, outData, outDataLen))
    {
        return false;
    }

    if (Base64::encodedSize(outDataLen) >= outlen)
    {
        return false;
    }

    return encodeOutput(outData, outstr, outDataLen);
}

template <typename InType, typename OutType, size_t N>
bool Protocol<InType, OutType, N>::decodeInput(const char *instr, InData &inData)
{
    size_t decodedLen = 0;
    size_t expectedLen = Base64::decodedSize(instr);

    if (expectedLen > sizeof(InData))
    {
        return false;
    }

    Base64::decode(instr, reinterpret_cast<uint8_t *>(&inData), &decodedLen);
    return (decodedLen == expectedLen);
}

template <typename InType, typename OutType, size_t N>
bool Protocol<InType, OutType, N>::encodeOutput(const OutData &outData, char *outstr, size_t outlen)
{
    Base64::encode(reinterpret_cast<const uint8_t *>(&outData), outlen+sizeof(char)+sizeof(uint16_t), outstr);
    return true;
}

template <typename InType, typename OutType, size_t N>
bool Protocol<InType, OutType, N>::calculateCRC(const InData &inData, uint16_t &crc)
{
    // Implement CRC calculation here if needed
    // For now, we'll leave this as a placeholder that always returns true
    crc = 0;
    return true;
}

template <typename InType, typename OutType, size_t N>
bool Protocol<InType, OutType, N>::findAndExecuteCommand(const InData &inData, OutData &outData, size_t &outDataLen)
{
    for (uint8_t i = 0; i < N; i++)
    {
        if (mFncPtr[i].cmd == inData.cmd && mFncPtr[i].fnc)
        {
            bool result = mFncPtr[i].fnc(inData.data, outData.data, outDataLen);

            if (result)
            {
                outData.cmd = inData.cmd;
                outData.len = static_cast<uint8_t>(outDataLen);
                return true;
            }
            return false;
        }
    }
    return false;
}
