#include <vector>
#include <gtest/gtest.h>

#include "belady_cache.hpp"

// Заглушка "медленной" функции
int slow_get_page(const int& key) {
    return key * 10;
}

TEST(BeladyCacheTest, SimpleHitMiss) {
    const std::vector<int> requests = {1, 2, 3, 1, 2, 3};
    BeladyCache<int> cache(3, requests);

    // Первый проход — всегда промахи
    EXPECT_FALSE(cache.lookup_update(1, slow_get_page));
    EXPECT_FALSE(cache.lookup_update(2, slow_get_page));
    EXPECT_FALSE(cache.lookup_update(3, slow_get_page));

    // Второй проход — должны быть хиты
    EXPECT_TRUE(cache.lookup_update(1, slow_get_page));
    EXPECT_TRUE(cache.lookup_update(2, slow_get_page));
    EXPECT_TRUE(cache.lookup_update(3, slow_get_page));
}

TEST(BeladyCacheTest, EvictionPolicy) {
    const std::vector<int> requests = {1, 2, 3, 4, 1, 2, 3, 4};
    BeladyCache<int> cache(2, requests);

    // Загружаем первые два
    EXPECT_FALSE(cache.lookup_update(1, slow_get_page));
    EXPECT_FALSE(cache.lookup_update(2, slow_get_page));

    // Запрос 3 — вытеснит 2
    EXPECT_FALSE(cache.lookup_update(3, slow_get_page));

    // Запрос 4 — вытеснит 3
    EXPECT_FALSE(cache.lookup_update(4, slow_get_page));

    // Теперь в кэше {1, 4}
    EXPECT_TRUE(cache.lookup_update(1, slow_get_page));

    // Запрос 2 - вытеснит 1 
    EXPECT_FALSE(cache.lookup_update(2, slow_get_page));

    // Запрос 3 - вытеснит 2 
    EXPECT_FALSE(cache.lookup_update(3, slow_get_page));

    // Теперь в кэше {4, 3}
    EXPECT_TRUE(cache.lookup_update(4, slow_get_page));
}

TEST(BeladyCacheTest, HandlesDeadKeys) {
    std::vector<int> requests = {10, 20, 30, 10};
    BeladyCache<int> cache(2, requests);

    EXPECT_FALSE(cache.lookup_update(10, slow_get_page));
    EXPECT_FALSE(cache.lookup_update(20, slow_get_page));

    // 30 вытеснит 20 (т.к. 10 ещё нужен, а 20 больше не встречается)
    EXPECT_FALSE(cache.lookup_update(30, slow_get_page));

    EXPECT_TRUE(cache.lookup_update(10, slow_get_page));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
