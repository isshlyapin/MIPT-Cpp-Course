enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(ocl_bitonic_sort_test test_ocl_bitonic_sort.cxx)
target_compile_features(ocl_bitonic_sort_test PRIVATE cxx_std_20)
target_link_libraries(ocl_bitonic_sort_test PRIVATE bitonic_sort GTest::GTest GTest::Main)

gtest_discover_tests(ocl_bitonic_sort_test)
