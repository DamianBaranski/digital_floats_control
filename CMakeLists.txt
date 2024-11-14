cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/tools/toolchain.cmake CACHE STRING "Path to toolchain file")

# Set the project name
project(DigitalFloatsControl)
cmake_policy(SET CMP0076 NEW)

# Create a global property to store test targets
define_property(GLOBAL PROPERTY TEST_TARGETS_PROPERTY
    BRIEF_DOCS "Global list of test targets"
    FULL_DOCS "Global list of test targets to be run with 'make utest'"
)
set_property(GLOBAL PROPERTY TEST_TARGETS_PROPERTY "")

# Function to add a test target with multiple source files and mock headers
function(add_unit_test)
    # Define options for parsing
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs SOURCES MOCKS)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Initialize an empty list to hold the absolute paths
    set(ABSOLUTE_SOURCES "")

    # Resolve each source file path relative to where add_unit_test is called
    foreach(SOURCE ${TEST_SOURCES})
        get_filename_component(ABSOLUTE_PATH "${CMAKE_CURRENT_LIST_DIR}/${SOURCE}" ABSOLUTE)
        list(APPEND ABSOLUTE_SOURCES "${ABSOLUTE_PATH}")
    endforeach()

    # Add -include options for each mock file
    foreach(MOCK ${TEST_MOCKS})
        get_filename_component(ABSOLUTE_MOCK_PATH "${CMAKE_CURRENT_LIST_DIR}/${MOCK}" ABSOLUTE)
        list(APPEND MOCK_INCLUDES "-include${ABSOLUTE_MOCK_PATH}")
    endforeach()

    set(UTEST_FLAGS -lpthread -lgtest -lgtest_main -lgmock -lgmock_main)

    # Add a custom target for building the test
    add_custom_target(${TEST_TARGET_NAME}_build
        COMMAND g++ ${ABSOLUTE_SOURCES} ${MOCK_INCLUDES} -o ${CMAKE_BINARY_DIR}/${TEST_TARGET_NAME} ${UTEST_FLAGS}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building ${TEST_TARGET_NAME}"
        VERBATIM
    )

    # Add a custom target for running the test
    add_custom_target(${TEST_TARGET_NAME}
        COMMAND ${CMAKE_BINARY_DIR}/${TEST_TARGET_NAME}
        DEPENDS ${TEST_TARGET_NAME}_build
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running ${TEST_TARGET_NAME}"
        VERBATIM
    )

    # Add the target to our global property
    get_property(CURRENT_TESTS GLOBAL PROPERTY TEST_TARGETS_PROPERTY)
    list(APPEND CURRENT_TESTS ${TEST_TARGET_NAME})
    set_property(GLOBAL PROPERTY TEST_TARGETS_PROPERTY ${CURRENT_TESTS})
endfunction()

# Add subdirectories first so that the unit tests can be added later
add_subdirectory(bootloader)
add_subdirectory(application)
add_subdirectory(common)

# Create the 'utest' target after all subdirectories are processed
add_custom_target(utest
    COMMAND ${CMAKE_COMMAND} -E echo "Running all unit tests..."
)

# Add dependencies for the utest target
get_property(ALL_TESTS GLOBAL PROPERTY TEST_TARGETS_PROPERTY)
foreach(TEST_TARGET ${ALL_TESTS})
    add_dependencies(utest ${TEST_TARGET})
endforeach()

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

add_custom_target(generate_version
    COMMAND ${CMAKE_COMMAND} -E echo "Generating version files..."
    COMMAND bash "${CMAKE_SOURCE_DIR}/tools/version.sh"
    COMMENT "Generating version files..."
    VERBATIM)

# Target for Linux Python installer
add_custom_target(pc_app_linux
    COMMAND ${CMAKE_COMMAND} -E echo "Running Python installer on Linux..."
    COMMAND pyinstaller "${CMAKE_SOURCE_DIR}/pc_app/main.spec"
    COMMENT "Installing Python application (Linux)"
    VERBATIM)

# Target for Windows Python installer via Wine
add_custom_target(pc_app_windows
    COMMAND ${CMAKE_COMMAND} -E echo "Running Python installer for Windows via Wine..."
    COMMAND wine pyinstaller "${CMAKE_SOURCE_DIR}/pc_app/main.spec"
    COMMENT "Installing Python application (Windows via Wine)"
    VERBATIM)