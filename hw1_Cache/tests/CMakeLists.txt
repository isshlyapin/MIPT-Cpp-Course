enable_testing()

find_package(GTest REQUIRED)

add_executable(test_lirs_cache test_lirs_cache.cpp)

target_compile_definitions(test_lirs_cache PRIVATE TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/tests/data")

target_link_libraries(
    test_lirs_cache 
    PRIVATE 
    GTest::gtest
    GTest::gtest_main
    pthread
)

target_include_directories(
    test_lirs_cache
    PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(test_belady_cache test_belady_cache.cpp)

target_link_libraries(
    test_belady_cache 
    PRIVATE 
    GTest::gtest
    GTest::gtest_main
    pthread
)

target_include_directories(
    test_belady_cache
    PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)


add_test(
    NAME lirs_cache_tests 
    COMMAND test_lirs_cache
)

add_test(
    NAME belady_cache_tests 
    COMMAND test_belady_cache
)
