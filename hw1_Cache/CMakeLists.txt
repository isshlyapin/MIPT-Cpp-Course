cmake_minimum_required(VERSION 4.0)

project (LirsCache)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    src/main.cpp
    src/utils.cpp
)

add_executable(cache ${SOURCES})

target_include_directories(
    cache
    PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(tests)