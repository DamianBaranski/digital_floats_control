#include "gtest/gtest.h"
#include "../base64.h"

// Test for encodedSize
TEST(Base64Test, EncodedSize) {
    EXPECT_EQ(Base64::encodedSize(0), 1);  // 1 for null terminator
    EXPECT_EQ(Base64::encodedSize(1), 5);  // 4 chars + null terminator
    EXPECT_EQ(Base64::encodedSize(2), 5);
    EXPECT_EQ(Base64::encodedSize(3), 5);
    EXPECT_EQ(Base64::encodedSize(4), 9);  // 8 chars + null terminator
}

// Test for decodedSize
TEST(Base64Test, DecodedSize) {
    EXPECT_EQ(Base64::decodedSize(""), 0);
    EXPECT_EQ(Base64::decodedSize("TQ=="), 1); // "M"
    EXPECT_EQ(Base64::decodedSize("TWE="), 2); // "Ma"
    EXPECT_EQ(Base64::decodedSize("TWFu"), 3); // "Man"
    EXPECT_EQ(Base64::decodedSize("TWFuYWdl"), 6); // "Manage"
}

// Test for encode
TEST(Base64Test, Encode) {
    unsigned char input[] = "Man";
    char output[Base64::encodedSize(sizeof(input) - 1)]; // Account for null terminator
    Base64::encode(input, sizeof(input) - 1, output);
    EXPECT_STREQ(output, "TWFu");
}

// Test for encode with padding
TEST(Base64Test, EncodeWithPadding) {
    unsigned char input1[] = "M";
    char output1[Base64::encodedSize(sizeof(input1) - 1)];
    Base64::encode(input1, sizeof(input1) - 1, output1);
    EXPECT_STREQ(output1, "TQ==");

    unsigned char input2[] = "Ma";
    char output2[Base64::encodedSize(sizeof(input2) - 1)];
    Base64::encode(input2, sizeof(input2) - 1, output2);
    EXPECT_STREQ(output2, "TWE=");
}

// Test for decode
TEST(Base64Test, Decode) {
    const char* input = "TWFu";
    unsigned char output[3];
    size_t out_len;
    EXPECT_TRUE(Base64::decode(input, output, &out_len));
    EXPECT_EQ(out_len, 3);
    EXPECT_EQ(std::memcmp(output, "Man", 3), 0);
}

// Test for decode with padding
TEST(Base64Test, DecodeWithPadding) {
    const char* input1 = "TQ==";
    unsigned char output1[1];
    size_t out_len1;
    EXPECT_TRUE(Base64::decode(input1, output1, &out_len1));
    EXPECT_EQ(out_len1, 1);
    EXPECT_EQ(output1[0], 'M');

    const char* input2 = "TWE=";
    unsigned char output2[2];
    size_t out_len2;
    EXPECT_TRUE(Base64::decode(input2, output2, &out_len2));
    EXPECT_EQ(out_len2, 2);
    EXPECT_EQ(std::memcmp(output2, "Ma", 2), 0);
}

// Test for invalid decode input
TEST(Base64Test, DecodeInvalidInput) {
    const char* invalid_input = "TWFu*";  // '*' is not a valid base64 character
    unsigned char output[3];
    size_t out_len;
    EXPECT_FALSE(Base64::decode(invalid_input, output, &out_len));
}