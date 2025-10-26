enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(test1 test.cpp)
target_link_libraries(test1 PRIVATE GTest::GTest GTest::Main)

gtest_discover_tests(test1)
