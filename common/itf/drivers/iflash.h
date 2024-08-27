#ifndef IFLASH_H
#define IFLASH_H

#include <cstdint>
#include <cstddef>

class IFlash {
    public:
    /// @brief Erases the specified number of sectors starting from the given address.
    /// @param address The starting address of the flash memory to erase.
    /// @param no_sectors The number of sectors to erase.
    /// @return `true` if the sectors were successfully erased, `false` otherwise.
    virtual bool erase(uint32_t address, size_t no_sectors) = 0;

    /// @brief Writes data to flash memory starting at the given address.
    /// @param address The starting address in flash memory where data will be written.
    /// @param data Pointer to the data to be written to flash memory.
    /// @param size The size of the data to be written in bytes.
    /// @return `true` if the data was successfully written, `false` otherwise.
    virtual bool write(uint32_t address, const uint8_t* data, size_t size) = 0;

    /// @brief Reads data from flash memory starting at the given address.
    /// @param address The starting address in flash memory from where data will be read.
    /// @param data Pointer to the buffer where the read data will be stored.
    /// @param size The size of the data to be read in bytes.
    /// @return `true` if the data was successfully read, `false` otherwise.
    virtual bool read(uint32_t address, uint8_t* data, size_t size) = 0;

    /// @brief Returns the size of a flash memory sector.
    /// @return The size of a flash memory sector in bytes.
    virtual size_t getSectorSize() = 0;

};

#endif