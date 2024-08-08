#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include "logger.h"
#include "base64.h"
#include "flash.h"

// Struct to represent protocol data
struct __attribute__((__packed__)) ProtocolData 
{
  char cmd;
  union __attribute__((__packed__))
  {
    struct __attribute__((__packed__))
    {
      size_t size;
    } load;
    struct __attribute__((__packed__))
    {
      uint8_t data_chunk_size;
      uint8_t data_chunk[255];
    } dataChunk;
  } data;
};

ProtocolData data;
// Class to manage the protocol
class Protocol
{
public:
  Protocol() : mFileSize(-1), mFlashPtr(0), mSectors(0) {}

  bool handleData(const char *encodedData)
  {
    if (strlen(encodedData) > Base64::encodedSize(sizeof(ProtocolData)) - 1)
    {
      Logger() << "Response: Too long data!!!";
      return false;
    }


    size_t out_len;
    if (!Base64::decode(encodedData, reinterpret_cast<uint8_t *>(&data), &out_len))
    {
      Logger() << "Response: Invalid Base64 data!";
      return false;
    }

    switch (data.cmd)
    {
    case 'L':
      return handleLoadCommand(data.data.load.size);
    case 'C':
      return handleChunkCommand(data.data.dataChunk.data_chunk, data.data.dataChunk.data_chunk_size);
    default:
      Logger() << "Response: Unknown command.";
      return false;
    }
  }

private:
  bool handleLoadCommand(size_t size)
  {
    //Logger() << "Command L received.";
    mFileSize = size;
    mSectors = (mFileSize + Flash::getSectorSize() - 1) / Flash::getSectorSize();
    mFlashPtr = 0;

    if (!mFlash.erase(0x08004000, mSectors))
    {
      sendResponse("Erase failed");
      return false;
    }

    sendResponse("Erase successful");
    return true;
  }

  bool handleChunkCommand(const uint8_t *data_chunk, size_t data_chunk_size)
  {
    if (mFileSize <= 0)
    {
      sendResponse("Load command required");
      return false;
    }

    for (size_t idx = 0; idx < data_chunk_size; idx+=2)
    {
      if (!mFlash.write(0x08004000+mFlashPtr + idx, (static_cast<uint16_t>(data_chunk[idx+1])<<8)+data_chunk[idx]))
      {
        sendResponse("Write failed");
        return false;
      }
    }

    mFlashPtr += data_chunk_size;
    sendResponse("Write successful");
    return true;
  }

  void sendResponse(const char *response)
  {
    Logger() << "Response: " << response;
  }

  int mFileSize;
  size_t mFlashPtr;
  size_t mSectors;
  Flash mFlash;
};

#endif