cmake_minimum_required(VERSION 3.30)

project (Triangles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_library(geometry)

target_sources(geometry
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES 
        modules/triangle.cppm
        modules/line.cppm
        modules/point.cppm
        modules/vector.cppm
        modules/plane.cppm
        modules/line_segment.cppm
        modules/config.cppm
)

add_library(intersection)

target_sources(intersection
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES 
        modules/tri_intersection.cppm
)

target_link_libraries(intersection
    PUBLIC geometry
)

add_executable(triangle_intersection)

target_sources(triangle_intersection
    PUBLIC
    src/main.cpp
)

target_link_libraries(triangle_intersection
    PUBLIC geometry intersection
)

add_subdirectory(test)
