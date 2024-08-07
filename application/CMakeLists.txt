cmake_minimum_required(VERSION 3.15.3)

project(floats_bs)

enable_language(C CXX ASM)


set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE})
add_subdirectory(${CMAKE_SOURCE_DIR}/../common common)
target_sources(${EXECUTABLE} PUBLIC
app/main.cpp
app/system_stm32f1xx.c
)

set_property(TARGET ${EXECUTABLE} PROPERTY CXX_STANDARD 11)

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
        )


target_link_options(${EXECUTABLE} PRIVATE
	-T${CMAKE_SOURCE_DIR}/STM32F103XB_FLASH.ld
        -mcpu=cortex-m3
        -mthumb
        #-lc
        #-lm
        -lnosys
        --specs=nano.specs
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -g 
        -F dwarf
        -Wl,--print-memory-usage
        -flto
)

set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -DUSE_HAL_DRIVER -Og -F dwarf -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -flto")
set(CMAKE_CXX_FLAGS "-mcpu=cortex-m3 -mthumb -DUSE_HAL_DRIVER -Og -F dwarf -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -flto -fno-exceptions -fno-rtti")

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)