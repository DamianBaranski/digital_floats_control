target_sources(common_lib PUBLIC
 startup_stm32f103xb.s
 syscalls.c
 sysmem.c
)

target_include_directories(common_lib PUBLIC 
Device/ST/STM32F1xx/Include
Include
)
