cmake_minimum_required(VERSION 4.0)

project(ThreadedBinaryTree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rq 
    src/main.cpp
)

target_compile_features(rq PRIVATE cxx_std_20)

target_include_directories(rq PRIVATE include)

option(BUILD_TEST "Build tests" OFF)
if (BUILD_TEST)
    add_subdirectory(test)
endif()