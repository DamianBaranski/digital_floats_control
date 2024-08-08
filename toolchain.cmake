set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# specify the cross compiler
set(CMAKE_C_COMPILER /usr/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER /usr/bin/arm-none-eabi-gcc)

# specify additional tools
set(CMAKE_AR /usr/bin/arm-none-eabi-ar)
set(CMAKE_OBJCOPY /usr/bin/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP /usr/bin/arm-none-eabi-objdump)
set(CMAKE_NM /usr/bin/arm-none-eabi-nm)
set(CMAKE_STRIP /usr/bin/arm-none-eabi-strip)
set(CMAKE_RANLIB /usr/bin/arm-none-eabi-ranlib)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -DUSE_HAL_DRIVER -Og -F dwarf -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -flto")
set(CMAKE_CXX_FLAGS "-mcpu=cortex-m3 -mthumb -DUSE_HAL_DRIVER -Og -F dwarf -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -flto -fno-exceptions -fno-rtti")
