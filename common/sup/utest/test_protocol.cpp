#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../protocol.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::DoAll;

// Test structures for input and output
struct TestInData {
    uint32_t value;
};

struct TestOutData {
    uint32_t result;
};

// Protocol frame structure for testing
struct __attribute__((__packed__)) TestFrame {
    char cmd;
    uint8_t len;
    TestInData data;
    uint16_t crc;
};

class ProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
        base64Mock = &Base64Mock::getReference();
    }

    void TearDown() override {
        base64Mock->clean();
        ::testing::Mock::VerifyAndClearExpectations(base64Mock);
    }

    Base64Mock* base64Mock;
};

// Command handler function for testing
bool testCommandHandler(const TestInData& inData, TestOutData& outData, size_t& outDataLen) {
    outData.result = inData.value * 2;  // Simple multiplication
    outDataLen = sizeof(TestOutData);
    return true;
}

TEST_F(ProtocolTest, RegisterCommandSuccessfully) {
    Protocol<TestInData, TestOutData, 5> protocol;
    EXPECT_TRUE(protocol.registerCmd('A', testCommandHandler));
}

TEST_F(ProtocolTest, RegisterSameCommandTwiceFails) {
    Protocol<TestInData, TestOutData, 5> protocol;
    EXPECT_TRUE(protocol.registerCmd('A', testCommandHandler));
    EXPECT_FALSE(protocol.registerCmd('A', testCommandHandler));
}

TEST_F(ProtocolTest, RegisterMoreCommandsThanCapacityFails) {
    Protocol<TestInData, TestOutData, 2> protocol;
    EXPECT_TRUE(protocol.registerCmd('A', testCommandHandler));
    EXPECT_TRUE(protocol.registerCmd('B', testCommandHandler));
    EXPECT_FALSE(protocol.registerCmd('C', testCommandHandler));
}

/*TEST_F(ProtocolTest, ProcessValidCommand) {
    Protocol<TestInData, TestOutData, 5> protocol;
    protocol.registerCmd('A', testCommandHandler);

    // Mock input data
    const char inputStr[] = "TestInput";
    char outputStr[100];
    
    // Setup expected behavior for Base64 mock
    EXPECT_CALL(*base64Mock, decodedSize(inputStr))
        .WillOnce(Return(sizeof(TestFrame)));

    EXPECT_CALL(*base64Mock, decode(_, _, _))
        .WillOnce(DoAll(
            [](const char*, uint8_t* output, size_t*) {
                auto* decoded = reinterpret_cast<TestFrame*>(output);
                decoded->cmd = 'A';
                decoded->len = sizeof(TestInData);
                decoded->data.value = 42;
                decoded->crc = 0;
                return true;
            },
            Return(true)
        ));

    EXPECT_CALL(*base64Mock, encodedSize(_))
        .WillOnce(Return(50));  // Return a size smaller than output buffer

    EXPECT_CALL(*base64Mock, encode(_, _, _))
        .WillOnce(Return(true));

    EXPECT_TRUE(protocol.process(inputStr, outputStr, sizeof(outputStr)));
}*/

TEST_F(ProtocolTest, ProcessFailsWithInvalidBase64Input) {
    Protocol<TestInData, TestOutData, 5> protocol;
    const char inputStr[] = "InvalidInput";
    char outputStr[100];
    
    EXPECT_CALL(*base64Mock, decodedSize(inputStr))
        .WillOnce(Return(1000));  // Return size larger than possible

    EXPECT_FALSE(protocol.process(inputStr, outputStr, sizeof(outputStr)));
}

TEST_F(ProtocolTest, ProcessFailsWithUnregisteredCommand) {
    Protocol<TestInData, TestOutData, 5> protocol;
    const char inputStr[] = "TestInput";
    char outputStr[100];
    
    EXPECT_CALL(*base64Mock, decodedSize(inputStr))
        .WillOnce(Return(sizeof(TestFrame)));

    EXPECT_CALL(*base64Mock, decode(_, _, _))
        .WillOnce(DoAll(
            [](const char*, uint8_t* output, size_t*) {
                auto* decoded = reinterpret_cast<TestFrame*>(output);
                decoded->cmd = 'X';  // Unregistered command
                decoded->len = sizeof(TestInData);
                decoded->data.value = 42;
                decoded->crc = 0;
                return true;
            },
            Return(true)
        ));

    EXPECT_FALSE(protocol.process(inputStr, outputStr, sizeof(outputStr)));
}

/*TEST_F(ProtocolTest, ProcessFailsWithInsufficientOutputBuffer) {
    Protocol<TestInData, TestOutData, 5> protocol;
    protocol.registerCmd('A', testCommandHandler);
    const char inputStr[] = "TestInput";
    char outputStr[10];  // Small buffer
    
    EXPECT_CALL(*base64Mock, decodedSize(inputStr))
        .WillOnce(Return(sizeof(TestFrame)));

    EXPECT_CALL(*base64Mock, decode(_, _, _))
        .WillOnce(DoAll(
            [](const char*, uint8_t* output, size_t*) {
                auto* decoded = reinterpret_cast<TestFrame*>(output);
                decoded->cmd = 'A';
                decoded->len = sizeof(TestInData);
                decoded->data.value = 42;
                decoded->crc = 0;
                return true;
            },
            Return(true)
        ));

    EXPECT_CALL(*base64Mock, encodedSize(_))
        .WillOnce(Return(100));  // Return size larger than output buffer

    EXPECT_FALSE(protocol.process(inputStr, outputStr, sizeof(outputStr)));
}
*/
// Add test for command handler failure
TEST_F(ProtocolTest, ProcessFailsWhenCommandHandlerFails) {
    Protocol<TestInData, TestOutData, 5> protocol;
    
    // Register a command handler that always fails
    protocol.registerCmd('A', [](const TestInData&, TestOutData&, size_t&) { return false; });
    
    const char inputStr[] = "TestInput";
    char outputStr[100];
    
    EXPECT_CALL(*base64Mock, decodedSize(inputStr))
        .WillOnce(Return(sizeof(TestFrame)));

    EXPECT_CALL(*base64Mock, decode(_, _, _))
        .WillOnce(DoAll(
            [](const char*, uint8_t* output, size_t*) {
                auto* decoded = reinterpret_cast<TestFrame*>(output);
                decoded->cmd = 'A';
                decoded->len = sizeof(TestInData);
                decoded->data.value = 42;
                decoded->crc = 0;
                return true;
            },
            Return(true)
        ));

    EXPECT_FALSE(protocol.process(inputStr, outputStr, sizeof(outputStr)));
}