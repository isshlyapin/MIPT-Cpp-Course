#include "config.hxx"
#include "helpers.hxx"
#include "bitonic_sort.hxx"

#include <gtest/gtest.h>

#include <vector>
#include <numeric>
#include <algorithm>
#include <stdexcept>

using namespace iss::ocl;

namespace {

template <typename T>
void test_sorting(size_t size, size_t lsz) {
    auto env = GpuEnvironment();
    BitonicSorter<T, GpuEnvironment> sorter(env, lsz);
    
    std::vector<T> data(size);
    rand_init(data.begin(), data.end(), -1000, 1000);
    
    std::vector<T> expected = data;
    std::sort(expected.begin(), expected.end());
    
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}

} // namespace

TEST(BitonicSorterTest, InvalidLocalSizeThrowsException) {
    auto env = GpuEnvironment();
    
    // lsz = 0 is invalid
    EXPECT_THROW((BitonicSorter<int, GpuEnvironment>(env, 0)), std::runtime_error);
    
    // lsz = 1 is invalid
    EXPECT_THROW((BitonicSorter<int, GpuEnvironment>(env, 1)), std::runtime_error);
    
    // lsz that exceeds local memory size is invalid
    auto max_lsz = env.get_local_mem_size() / sizeof(int);
    EXPECT_THROW((BitonicSorter<int, GpuEnvironment>(env, max_lsz + 1)), std::runtime_error);
}

TEST(BitonicSortTest, ValidRangeDoesNotThrow) {
    auto env = GpuEnvironment();
    BitonicSorter<int, GpuEnvironment> sorter(env, 256);
    
    std::vector<int> data(10);
    
    // Empty range is valid (no sorting needed)
    EXPECT_NO_THROW(sorter.sort(data.begin(), data.begin()));
    
    // Single element range is valid (already sorted)
    EXPECT_NO_THROW(sorter.sort(data.begin(), data.begin() + 1));
    
    // Valid range
    EXPECT_NO_THROW(sorter.sort(data.begin(), data.end()));
}

TEST(BitonicSortTest, InvalidRangeThrowsException) {
    auto env = GpuEnvironment();
    BitonicSorter<int, GpuEnvironment> sorter(env, 256);
    
    std::vector<int> data(10);
    
    // end < begin is invalid
    EXPECT_THROW(sorter.sort(data.end(), data.begin()), std::invalid_argument);
}

TEST(BitonicSorterTest, SortIntSmall) {
    test_sorting<int>(16, 4);
}

TEST(BitonicSorterTest, SortIntLarge) {
    test_sorting<int>(1024 * 1024, 256);
}

TEST(BitonicSorterTest, SortFloat) {
    test_sorting<float>(1024, 128);
}

TEST(BitonicSorterTest, SortAlreadySorted) {
    auto env = GpuEnvironment();
    BitonicSorter<int, GpuEnvironment> sorter(env, 256);
    
    std::vector<int> data(1024);
    std::iota(data.begin(), data.end(), 0);
    
    std::vector<int> expected = data;
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}

TEST(BitonicSorterTest, SortReverseSorted) {
    auto env = GpuEnvironment();
    BitonicSorter<int, GpuEnvironment> sorter(env, 256);
    
    std::vector<int> data(1024);
    std::iota(data.rbegin(), data.rend(), 0);
    
    std::vector<int> expected = data;
    std::sort(expected.begin(), expected.end());
    
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}

TEST(BitonicSorterTest, SortAllIdentical) {
    auto env = GpuEnvironment();
    BitonicSorter<int, GpuEnvironment> sorter(env, 256);
    
    std::vector<int> data(1024, 42);
    std::vector<int> expected = data;
    
    sorter.sort(data.begin(), data.end());
    
    EXPECT_EQ(data, expected);
}

TEST(BitonicSorterTest, SortRandomSize) {
    test_sorting<int>(12345, 256);
    test_sorting<int>(67890, 256);

    test_sorting<float>(12345, 128);
    test_sorting<float>(67890, 128);
}