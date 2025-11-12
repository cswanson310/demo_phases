#include "lib.h"
#include <gtest/gtest.h>

TEST(AddTest, Positive) {
    EXPECT_EQ(add(2, 3), 5);
}

TEST(AddTest, NegativeAndZero) {
    EXPECT_EQ(add(-1, 1), 0);
    EXPECT_EQ(add(0, 0), 0);
}
