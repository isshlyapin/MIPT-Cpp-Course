enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(test_tree test_tree.cpp)
target_link_libraries(test_tree PRIVATE tree GTest::GTest GTest::Main)

add_executable(test_tree_iterator test_tree_iterator.cpp)
target_link_libraries(test_tree_iterator PRIVATE tree GTest::GTest GTest::Main)

gtest_discover_tests(test_tree)
gtest_discover_tests(test_tree_iterator)