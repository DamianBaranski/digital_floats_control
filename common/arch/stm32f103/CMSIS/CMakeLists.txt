target_sources(${EXECUTABLE} PUBLIC
 startup_stm32f103xb.s
 syscalls.c
 sysmem.c
)

target_include_directories(${EXECUTABLE} PUBLIC 
Device/ST/STM32F1xx/Include
Include
)
