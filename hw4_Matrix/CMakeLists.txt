cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(Matrix LANGUAGES CXX)

set(CMAKE_CXX_MODULE_STD 1)

add_library(matrix STATIC)

target_sources(matrix
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    FILES 
      src/hello.cxxm)

target_compile_features(matrix
  PRIVATE   cxx_std_23
  INTERFACE cxx_std_20
)

add_executable(main)

target_compile_features(main PRIVATE cxx_std_20)

target_sources(main
  PRIVATE
    src/main.cxx
)

target_link_libraries(main PRIVATE matrix)
