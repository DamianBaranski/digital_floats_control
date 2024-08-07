#ifndef FLASH_H
#define FLASH_H

#include "stm32f1xx_hal.h"

#include "logger.h"

class Flash
{
public:
    bool erase(uint32_t address, size_t no_sectors)
    {
        //Logger() << "Erasing " << no_sectors << " from address:" << address;
        FLASH_EraseInitTypeDef EraseInitStruct;
        uint32_t PAGEError;
        HAL_FLASH_Unlock();
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.PageAddress = address;
        EraseInitStruct.NbPages = no_sectors;
        HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
        CLEAR_BIT(FLASH->CR, (FLASH_CR_PER));
        HAL_FLASH_Lock();
        return result == HAL_OK;
    }

    bool write(uint32_t address, const uint16_t data)
    {
        // Simulate flash write
        //Logger() << "Writing data to address " << address;
        HAL_FLASH_Unlock();
        HAL_StatusTypeDef result = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data);
        HAL_FLASH_Lock();
        return result == HAL_OK;
    }

    static size_t getSectorSize()
    {
        return 1024;
    }
};

#endif