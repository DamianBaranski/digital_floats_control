# Function to add a test target with multiple source files and mock headers
function(add_unit_test)
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs SOURCES MOCKS)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(ABSOLUTE_SOURCES "")
    foreach(SOURCE ${TEST_SOURCES})
        get_filename_component(ABSOLUTE_PATH "${CMAKE_CURRENT_LIST_DIR}/${SOURCE}" ABSOLUTE)
        list(APPEND ABSOLUTE_SOURCES "${ABSOLUTE_PATH}")
    endforeach()

    foreach(MOCK ${TEST_MOCKS})
        get_filename_component(ABSOLUTE_MOCK_PATH "${CMAKE_CURRENT_LIST_DIR}/${MOCK}" ABSOLUTE)
        list(APPEND MOCK_INCLUDES "-include${ABSOLUTE_MOCK_PATH}")
    endforeach()

    set(UTEST_FLAGS -lpthread -lgtest -lgtest_main -lgmock -lgmock_main -fprofile-arcs -ftest-coverage)

    add_custom_target(${TEST_TARGET_NAME}_build
        COMMAND g++ ${ABSOLUTE_SOURCES} ${MOCK_INCLUDES} -o ${CMAKE_BINARY_DIR}/${TEST_TARGET_NAME} ${UTEST_FLAGS}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building ${TEST_TARGET_NAME}"
        VERBATIM
    )

    add_custom_target(${TEST_TARGET_NAME}
        COMMAND bash -c "${CMAKE_BINARY_DIR}/${TEST_TARGET_NAME}; echo $? >> result.log || true"
        DEPENDS ${TEST_TARGET_NAME}_build
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running ${TEST_TARGET_NAME}"
        VERBATIM
    )

    get_property(CURRENT_TESTS GLOBAL PROPERTY TEST_TARGETS_PROPERTY)
    list(APPEND CURRENT_TESTS ${TEST_TARGET_NAME})
    set_property(GLOBAL PROPERTY TEST_TARGETS_PROPERTY ${CURRENT_TESTS})

    add_dependencies(utest ${TEST_TARGET_NAME})
    add_dependencies(coverage ${TEST_TARGET_NAME})

endfunction()

# Create the 'utest' target after all subdirectories are processed
add_custom_target(utest
    COMMAND if grep -v 0 result.log > /dev/null\; then rm -rf result.log\; exit 1\; else rm -rf result.log\; exit 0\; fi
)

# Additional custom targets
add_custom_target(generate_version
    COMMAND ${CMAKE_COMMAND} -E echo "Generating version files..."
    COMMAND bash "${CMAKE_SOURCE_DIR}/tools/version.sh"
    COMMENT "Generating version files..."
    VERBATIM)

add_custom_target(pc_app_linux
    COMMAND ${CMAKE_COMMAND} -E echo "Running Python installer on Linux..."
    COMMAND pyinstaller "${CMAKE_SOURCE_DIR}/pc_app/main.spec"
    COMMENT "Installing Python application (Linux)"
    VERBATIM)

add_custom_target(pc_app_windows
    COMMAND ${CMAKE_COMMAND} -E echo "Running Python installer for Windows via Wine..."
    COMMAND wine pyinstaller "${CMAKE_SOURCE_DIR}/pc_app/main.spec"
    COMMENT "Installing Python application (Windows via Wine)"
    VERBATIM)

add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E echo "Running coverage..."
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND rm -rf result.log
    COMMAND /bin/bash -c "gcov *.gcno *.gcda"
    COMMAND /bin/bash -c "lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch --exclude '*utest/*' --exclude '/usr/*' --exclude '*mocks/*' --ignore-errors unused"
    COMMAND genhtml coverage.info --output-directory html
    VERBATIM)
