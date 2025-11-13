cmake_minimum_required(VERSION 3.30)

project(ThreadedBinaryTree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tree INTERFACE)
target_include_directories(tree INTERFACE include)
target_compile_features(tree INTERFACE cxx_std_20)

add_library(range_query INTERFACE)
target_include_directories(range_query INTERFACE include)
target_compile_features(range_query INTERFACE cxx_std_20)

add_executable(rq
    src/main.cpp
)

target_compile_features(rq PRIVATE cxx_std_20)
target_link_libraries(rq PRIVATE tree range_query)

option(BUILD_TEST "Build tests" OFF)
if (BUILD_TEST)
    add_subdirectory(test)
endif()

option(BUILD_BENCHMARK "Build benchmark" OFF)
if (BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()