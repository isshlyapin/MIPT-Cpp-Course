cmake_minimum_required(VERSION 3.10)

project (LirsCache)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CLI11 REQUIRED)

set(SOURCES
    src/main.cpp
    src/utils.cpp
)

add_executable(cache ${SOURCES})

target_include_directories(cache
    PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(cache
    PRIVATE
    CLI11::CLI11
)

add_subdirectory(tests)