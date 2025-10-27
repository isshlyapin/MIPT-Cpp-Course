enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(test_tree test_tree.cpp)
target_link_libraries(test_tree PRIVATE tree GTest::GTest GTest::Main)

gtest_discover_tests(test_tree)