enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(vector_test test_vector.cxx)
target_link_libraries(vector_test PRIVATE linal GTest::GTest GTest::Main)

# add_executable(matrix_test test_matrix.cxx)
# target_link_libraries(matrix_test PRIVATE linal GTest::GTest GTest::Main)

gtest_discover_tests(vector_test)
# gtest_discover_tests(matrix_test)