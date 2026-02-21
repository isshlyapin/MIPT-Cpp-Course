cmake_minimum_required(VERSION 3.20)
project(bitonic_sort LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)

set(KERNEL_CL ${CMAKE_CURRENT_SOURCE_DIR}/src/fast_bitonic_sort.cl)
file(READ "${KERNEL_CL}" KERNEL_TEXT)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/kernel_source.hxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/generated/kernel_source.hxx
  @ONLY
)

add_library(bitonic_sort
    src/config.cxx
    src/ocl_bitonic_sort.cxx
)
target_include_directories(bitonic_sort
    PUBLIC include
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(bitonic_sort PRIVATE OpenCL::OpenCL)

add_executable(bitonic 
    src/main.cxx
)
# target_include_directories(bitonic
#     PRIVATE include
# )
target_link_libraries(bitonic PRIVATE OpenCL::OpenCL bitonic_sort)
