cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(Matrix LANGUAGES CXX)

set(CMAKE_CXX_MODULE_STD 1)

add_library(linal STATIC)

target_sources(linal
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    FILES 
      src/vector.cxxm
      src/matrix.cxxm
      src/linal.cxxm
)

target_compile_features(linal
  PRIVATE   cxx_std_23
  INTERFACE cxx_std_20
)

add_executable(det)

target_compile_features(det PRIVATE cxx_std_23)

target_sources(det
  PRIVATE
    src/main.cxx
)

target_link_libraries(det PRIVATE linal)

option(BUILD_TEST "Build tests" OFF)
if (BUILD_TEST)
    add_subdirectory(test)
endif()