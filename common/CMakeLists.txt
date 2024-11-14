add_library(common_lib STATIC)

target_compile_definitions(common_lib PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F103xB
)

set_property(TARGET common_lib PROPERTY CXX_STANDARD 11)

add_subdirectory(arch/stm32f103)
add_subdirectory(itf/hal)
add_subdirectory(itf/drivers)
add_subdirectory(sup)
add_subdirectory(drivers)
enable_language(C CXX ASM)
