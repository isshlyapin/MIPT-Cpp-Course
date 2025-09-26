cmake_minimum_required(VERSION 3.30)

project (Triangles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_executable(triangle_intersection)
target_sources(triangle_intersection
    PUBLIC
    src/main.cpp
)

target_sources(triangle_intersection
    PUBLIC
    FILE_SET CXX_MODULES
    FILES
        modules/triangle_intersection.cppm
)

add_subdirectory(test)