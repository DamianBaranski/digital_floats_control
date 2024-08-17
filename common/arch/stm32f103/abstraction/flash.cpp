#include "flash.h"
#include "stm32f1xx_hal.h"
#include <cstring>   // For memcpy
#include <cassert>   // For assert
#include <algorithm> // For std::min

bool Flash::erase(uint32_t address, size_t no_sectors)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;

    // Unlock the Flash to enable the flash control register access
    HAL_FLASH_Unlock();

    // Set the erase parameters
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = address;
    EraseInitStruct.NbPages = no_sectors;

    // Perform the erase operation
    HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);

    // Clear the Page Erase bit
    CLEAR_BIT(FLASH->CR, (FLASH_CR_PER));

    // Lock the Flash to disable the flash control register access
    HAL_FLASH_Lock();

    // Return the result of the erase operation
    return result == HAL_OK;
}

bool Flash::write(uint32_t address, const uint8_t *data, size_t size)
{
    HAL_StatusTypeDef status;
    uint64_t data64 = 0; // For 64-bit chunks

    // Ensure the provided address is aligned to 64-bit (8-byte) boundary
    assert((address % sizeof(uint64_t)) == 0);

    // Unlock the Flash to enable the flash control register access
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        return false;
    }

    // Write the data to flash
    for (size_t i = 0; i < size; i += sizeof(uint64_t))
    {
        data64 = 0;
        size_t to_copy = std::min(sizeof(uint64_t), size - i);
        memcpy(&data64, data + i, to_copy); // Copy data to 64-bit variable

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data64);
        if (status != HAL_OK)
        {
            break;
        }

        address += sizeof(uint64_t); // Move to the next address
    }

    // Lock the Flash to disable the flash control register access
    HAL_FLASH_Lock();

    return true;
}

bool Flash::read(uint32_t address, uint8_t *data, size_t size)
{
    memcpy(data, reinterpret_cast<uint8_t *>(address), size);
    return true;
}

size_t Flash::getSectorSize()
{
    return 1024;
}
