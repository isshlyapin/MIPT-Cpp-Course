#include <cmath>
#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <gtest/gtest.h>

#include "utils.hpp"
#include "lirs_cache.hpp"

using namespace utils;
using namespace caches;

namespace fs = std::filesystem;

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

// чтение входных данных по ссылке
void read_input_cache_data(const std::string& filename, InputCacheData& data) {
    std::ifstream fin(filename);
    if (!fin) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    fin >> data.size_cache >> data.n_requests;
    data.requests.resize(data.n_requests);

    for (size_t i = 0; i < data.n_requests; i++) {
        fin >> data.requests[i];
    }
}

// параметризованный тест
class LirsCacheFileTest : public ::testing::TestWithParam<std::string> {};

TEST_P(LirsCacheFileTest, RunWithoutCrashes) {
    const std::string filename = GetParam();
    InputCacheData data;

    ASSERT_NO_THROW({
        read_input_cache_data(filename, data);
    }) << "Ошибка чтения файла: " << filename;

    LirsCache<double> cache(data.size_cache);

    int n_hits = 0;
    EXPECT_NO_THROW({
        for (auto key : data.requests) {
            if (cache.lookup_update(key, get_page)) {
                n_hits++;
            }
        }
    }) << "Сбой на файле: " << filename;

    EXPECT_GE(n_hits, 0) << "Некорректный результат в файле: " << filename;
}

// генерация списка файлов
std::vector<std::string> get_all_dat_files(const std::string& dir) {
    std::vector<std::string> files;
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dat") {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

// инстанцирование набора тестов
INSTANTIATE_TEST_SUITE_P(
    AllDataFiles,
    LirsCacheFileTest,
    ::testing::ValuesIn(get_all_dat_files(TEST_DATA_DIR))
);


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
