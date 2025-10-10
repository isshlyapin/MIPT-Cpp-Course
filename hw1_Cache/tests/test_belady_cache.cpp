#include <vector>
#include <gtest/gtest.h>

#include "belady_cache.hpp"

using namespace caches;

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

    // Запрос 3 проходит без вытеснения
    EXPECT_FALSE(cache.lookup_update(3, slow_get_page));

    // Запрос 4 проходит без вытеснения
    EXPECT_FALSE(cache.lookup_update(4, slow_get_page));

    // Сейчас в кэше {1, 2}
    EXPECT_TRUE(cache.lookup_update(1, slow_get_page));

    // Сейчас в кэше {1, 2} 
    EXPECT_TRUE(cache.lookup_update(2, slow_get_page));

    // Запрос 3 - вытеснит 1
    EXPECT_FALSE(cache.lookup_update(3, slow_get_page));

    // Теперь в кэше {3, 2}
    EXPECT_FALSE(cache.lookup_update(4, slow_get_page));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
