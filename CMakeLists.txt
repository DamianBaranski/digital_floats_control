cmake_minimum_required(VERSION 3.10)

# Set the project name
project(DigitalFloatsControl)

# Add subdirectories
#add_subdirectory(bootloader)
add_subdirectory(application)

# Check if Doxygen is available
find_package(Doxygen)

if (DOXYGEN_FOUND)
    # Doxygen configuration file
    set(DOXYGEN_IN_FILE "${CMAKE_SOURCE_DIR}/doc/Doxyfile.in")
    set(DOXYGEN_OUT_FILE "${CMAKE_BINARY_DIR}/doc/Doxyfile")

    # Configure Doxygen
    configure_file(${DOXYGEN_IN_FILE} ${DOXYGEN_OUT_FILE} @ONLY)

    # Add a target for generating documentation
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT_FILE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()