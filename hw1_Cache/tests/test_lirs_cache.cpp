#include <cmath>
#include <gtest/gtest.h>

#include "lirs_cache.hpp"

namespace {
double get_page(int key) {
    return std::sin(key);
}
}

TEST(LirsCacheTest, BasicHitMiss) {
    LirsCache<double> cache(2);
    EXPECT_FALSE(cache.lookup_update(1, get_page));
    EXPECT_TRUE(cache.lookup_update(1, get_page));
}

TEST(LirsCacheTest, EvictionPolicy) {
    LirsCache<int> cache(2);
    EXPECT_FALSE(cache.lookup_update(1, get_page));
    EXPECT_FALSE(cache.lookup_update(2, get_page));
    EXPECT_FALSE(cache.lookup_update(3, get_page));
    EXPECT_TRUE( cache.lookup_update(1, get_page));
    EXPECT_TRUE( cache.lookup_update(3, get_page));
    EXPECT_TRUE( cache.lookup_update(3, get_page));
    EXPECT_FALSE(cache.lookup_update(2, get_page));
    EXPECT_FALSE(cache.lookup_update(1, get_page));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
