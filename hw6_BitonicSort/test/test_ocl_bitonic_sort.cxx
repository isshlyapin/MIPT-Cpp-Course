#include "config.hxx"
#include "helpers.hxx"
#include "ocl_bitonic_sort.hxx"

#include <gtest/gtest.h>

#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>

template <typename T>
void test_sorting(size_t size, size_t lsz) {
    auto env = std::make_shared<OCLSimpleBitonicEnv>();
    OCLBitonicSorter<T> sorter(env, lsz);
    
    std::vector<T> data(size);
    rand_init(data.begin(), data.end(), -1000, 1000);
    
    std::vector<T> expected = data;
    std::sort(expected.begin(), expected.end());
    
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}

TEST(OCLBitonicSorterTest, InvalidLocalSizeThrowsException) {
    auto env = std::make_shared<OCLSimpleBitonicEnv>();
    
    // lsz = 0 is invalid
    EXPECT_THROW(OCLBitonicSorter<int>(env, 0), std::runtime_error);
    
    // lsz = 1 is invalid
    EXPECT_THROW(OCLBitonicSorter<int>(env, 1), std::runtime_error);
    
    // lsz that exceeds local memory size is invalid
    size_t max_lsz = env->get_local_mem_size() / sizeof(int);
    EXPECT_THROW(OCLBitonicSorter<int>(env, max_lsz + 1), std::runtime_error);
}

TEST(OCLBitonicSorterTest, InvalidInputSizeThrowsException) {
    auto env = std::make_shared<OCLSimpleBitonicEnv>();
    OCLBitonicSorter<int> sorter(env, 256);
    
    std::vector<int> data(3); // Not a power of 2
    EXPECT_THROW(sorter.sort(data.begin(), data.end()), std::runtime_error);
}

TEST(OCLBitonicSorterTest, SortIntSmall) {
    test_sorting<int>(16, 4);
}

TEST(OCLBitonicSorterTest, SortIntLarge) {
    test_sorting<int>(1024 * 1024, 256);
}

TEST(OCLBitonicSorterTest, SortFloat) {
    test_sorting<float>(1024, 128);
}

TEST(OCLBitonicSorterTest, SortAlreadySorted) {
    auto env = std::make_shared<OCLSimpleBitonicEnv>();
    OCLBitonicSorter<int> sorter(env, 256);
    
    std::vector<int> data(1024);
    std::iota(data.begin(), data.end(), 0);
    
    std::vector<int> expected = data;
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}

TEST(OCLBitonicSorterTest, SortReverseSorted) {
    auto env = std::make_shared<OCLSimpleBitonicEnv>();
    OCLBitonicSorter<int> sorter(env, 256);
    
    std::vector<int> data(1024);
    std::iota(data.rbegin(), data.rend(), 0);
    
    std::vector<int> expected = data;
    std::sort(expected.begin(), expected.end());
    
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}

TEST(OCLBitonicSorterTest, SortAllIdentical) {
    auto env = std::make_shared<OCLSimpleBitonicEnv>();
    OCLBitonicSorter<int> sorter(env, 256);
    
    std::vector<int> data(1024, 42);
    std::vector<int> expected = data;
    
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}
