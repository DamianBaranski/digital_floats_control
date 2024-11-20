#include <gtest/gtest.h>
#include "../bit_mask.h"

// Example flags enumeration for testing
enum class TestFlags : uint32_t {
    Flag1 = 1 << 0,  // 0x01
    Flag2 = 1 << 1,  // 0x02
    Flag3 = 1 << 2,  // 0x04
    Flag4 = 1 << 3   // 0x08
};

// Test fixture for the BitMask class with TestFlags
class BitMaskTest : public ::testing::Test {
protected:
    BitMask<TestFlags> bitmask;  // Initialize a BitMask instance for each test
};

// Test setting a single bit
TEST_F(BitMaskTest, SetSingleBit) {
    bitmask.set(TestFlags::Flag1);
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag1));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag2));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag3));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag4));
}

// Test setting multiple bits
TEST_F(BitMaskTest, SetMultipleBits) {
    bitmask.set(TestFlags::Flag1);
    bitmask.set(TestFlags::Flag3);
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag1));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag2));
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag3));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag4));
}

// Test clearing a bit
TEST_F(BitMaskTest, ClearBit) {
    bitmask.set(TestFlags::Flag1);
    bitmask.set(TestFlags::Flag2);
    bitmask.clr(TestFlags::Flag1);
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag1));
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag2));
}

// Test clearing all bits
TEST_F(BitMaskTest, ClearAllBits) {
    bitmask.set(TestFlags::Flag1);
    bitmask.set(TestFlags::Flag2);
    bitmask.set(TestFlags::Flag3);
    bitmask.clr(TestFlags::Flag1);
    bitmask.clr(TestFlags::Flag2);
    bitmask.clr(TestFlags::Flag3);
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag1));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag2));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag3));
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag4));
}

// Test setting and clearing the same bit
TEST_F(BitMaskTest, SetAndClearSameBit) {
    bitmask.set(TestFlags::Flag1);
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag1));
    bitmask.clr(TestFlags::Flag1);
    EXPECT_FALSE(bitmask.isSet(TestFlags::Flag1));
}

// Test setting all bits and checking them
TEST_F(BitMaskTest, SetAllBits) {
    bitmask.set(TestFlags::Flag1);
    bitmask.set(TestFlags::Flag2);
    bitmask.set(TestFlags::Flag3);
    bitmask.set(TestFlags::Flag4);
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag1));
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag2));
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag3));
    EXPECT_TRUE(bitmask.isSet(TestFlags::Flag4));
}
