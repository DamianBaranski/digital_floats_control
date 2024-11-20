# Create test executables
add_unit_test(TARGET_NAME utest_base64 SOURCES test_base64.cpp ../base64.cpp)
add_unit_test(TARGET_NAME utest_bit_mask SOURCES test_bit_mask.cpp)
add_unit_test(TARGET_NAME utest_protocol SOURCES test_protocol.cpp MOCKS mocks/base64.h)


