# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/tools/toolchain.cmake CACHE STRING "Path to toolchain file")

# Set the project name
project(DigitalFloatsControl)
cmake_policy(SET CMP0076 NEW)

# Global property for test targets
define_property(GLOBAL PROPERTY TEST_TARGETS_PROPERTY
    BRIEF_DOCS "Global list of test targets"
    FULL_DOCS "Global list of test targets to be run with 'make utest'"
)
set_property(GLOBAL PROPERTY TEST_TARGETS_PROPERTY "")

# Include the custom targets
include(${CMAKE_SOURCE_DIR}/tools/custom_targets.cmake)

# Add subdirectories
add_subdirectory(bootloader)
add_subdirectory(application)
add_subdirectory(common)

# Check if Doxygen is available
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN_FILE "${CMAKE_SOURCE_DIR}/doc/Doxyfile.in")
    set(DOXYGEN_OUT_FILE "${CMAKE_BINARY_DIR}/doc/Doxyfile")
    configure_file(${DOXYGEN_IN_FILE} ${DOXYGEN_OUT_FILE} @ONLY)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT_FILE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()
