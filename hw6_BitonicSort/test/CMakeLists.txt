enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(bitonic_sort_test test_bitonic_sort.cxx)
target_compile_features(bitonic_sort_test PRIVATE cxx_std_20)
target_link_libraries(bitonic_sort_test PRIVATE bitonic_sort GTest::GTest GTest::Main)

add_executable(helpers_test test_helpers.cxx)
target_compile_features(helpers_test PRIVATE cxx_std_20)
target_link_libraries(helpers_test PRIVATE bitonic_sort GTest::GTest GTest::Main)

gtest_discover_tests(bitonic_sort_test)
gtest_discover_tests(helpers_test)
