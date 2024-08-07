target_sources(${EXECUTABLE} PUBLIC
gpio.cpp
i2c_master.cpp
uart.cpp
)

target_include_directories(${EXECUTABLE} PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}
)



