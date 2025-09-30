cmake_minimum_required(VERSION 3.30)

project (Triangles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_library(triangle_intersection_lib)

target_sources(triangle_intersection_lib
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES 
        modules/triangle_intersection.cppm
)

add_executable(triangle_intersection)

target_sources(triangle_intersection
    PUBLIC
    src/main.cpp
)

target_link_libraries(triangle_intersection
    PUBLIC triangle_intersection_lib
)

add_subdirectory(test)
