target_sources(common_lib PRIVATE
gpio.cpp
i2c_master.cpp
uart.cpp
flash.cpp
pwm_dma.cpp
spi.cpp
pwm.cpp
)

target_include_directories(common_lib PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}
)



