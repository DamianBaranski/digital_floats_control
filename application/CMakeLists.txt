cmake_minimum_required(VERSION 3.15.3)

project(floats_bs)

enable_language(C CXX ASM)

set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE})
target_sources(${EXECUTABLE} PUBLIC
app/main.cpp
app/application.cpp
app/system_stm32f1xx.c
app/control_channel.cpp
bsp/stm32f103/bsp.cpp
app/expander.cpp
app/uart_communication.cpp
)

target_include_directories(${EXECUTABLE} PUBLIC 
app
bsp/stm32f103
)
target_link_libraries(${EXECUTABLE} PRIVATE common_lib)

set_property(TARGET ${EXECUTABLE} PROPERTY CXX_STANDARD 11)

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
        )


target_link_options(${EXECUTABLE} PRIVATE
	-T${CMAKE_CURRENT_LIST_DIR}/STM32F103XB_FLASH.ld
        -Wl,-Map=${PROJECT_NAME}.map,--cref
)

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)

add_dependencies(${EXECUTABLE} generate_version)