#include <gtest/gtest.h>

#include "helpers.hxx"

using namespace iss::ocl;

TEST(HelpersTest, IsPowerOf2) {
    EXPECT_TRUE(is_power_of_2(1));
    EXPECT_TRUE(is_power_of_2(2));
    EXPECT_TRUE(is_power_of_2(4));
    EXPECT_TRUE(is_power_of_2(8));
    EXPECT_TRUE(is_power_of_2(16));
    EXPECT_TRUE(is_power_of_2(32));

    EXPECT_FALSE(is_power_of_2(0));
    EXPECT_FALSE(is_power_of_2(3));
    EXPECT_FALSE(is_power_of_2(5));
    EXPECT_FALSE(is_power_of_2(7));
    EXPECT_FALSE(is_power_of_2(11));
    EXPECT_FALSE(is_power_of_2(17));    
}

TEST(HelpersTest, NextPowerOf2) {
    EXPECT_EQ(next_power_of_2(0), 2);
    EXPECT_EQ(next_power_of_2(1), 2);
    EXPECT_EQ(next_power_of_2(2), 2);
    EXPECT_EQ(next_power_of_2(3), 4);
    EXPECT_EQ(next_power_of_2(4), 4);
    EXPECT_EQ(next_power_of_2(5), 8);
    EXPECT_EQ(next_power_of_2(6), 8);
    EXPECT_EQ(next_power_of_2(7), 8);
    EXPECT_EQ(next_power_of_2(8), 8);
    EXPECT_EQ(next_power_of_2(9), 16);
    EXPECT_EQ(next_power_of_2(15), 16);
    EXPECT_EQ(next_power_of_2(16), 16);
    EXPECT_EQ(next_power_of_2(17), 32);    
}

TEST(HelpersTest, LargestDivisorLeqLimit) {
    EXPECT_EQ(largest_divisor_leq_limit(10, 3), 2);
    EXPECT_EQ(largest_divisor_leq_limit(10, 4), 2);
    EXPECT_EQ(largest_divisor_leq_limit(10, 5), 5);
    EXPECT_EQ(largest_divisor_leq_limit(10, 6), 5);
    EXPECT_EQ(largest_divisor_leq_limit(10, 7), 5);
    EXPECT_EQ(largest_divisor_leq_limit(10, 8), 5);
    EXPECT_EQ(largest_divisor_leq_limit(10, 9), 5);
    EXPECT_EQ(largest_divisor_leq_limit(10, 10), 10);
}