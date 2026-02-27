cmake_minimum_required(VERSION 3.20)

project(bitonic_sort LANGUAGES CXX)

#-----------------------------------------------------------------------------#
#                        Project configuration                                #
#-----------------------------------------------------------------------------#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-----------------------------------------------------------------------------#
#                            Build Options                                    #
#-----------------------------------------------------------------------------#
option(ANALYZE "Enable analysis output" OFF)
option(COMPARE_CPU "Compare with CPU implementation" OFF)
set(TYPE "int" CACHE STRING "Type to use for sorting")

#-----------------------------------------------------------------------------#
#                             Dependencies                                    #
#-----------------------------------------------------------------------------#
find_package(OpenCL REQUIRED)

#-----------------------------------------------------------------------------#
#                            Kernel Source Handling                           #
#-----------------------------------------------------------------------------#
set(KERNEL_CL ${CMAKE_CURRENT_SOURCE_DIR}/src/fast_bitonic_sort.cl)
file(READ "${KERNEL_CL}" KERNEL_TEXT)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/kernel_source.hxx.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/kernel_source.hxx
    @ONLY
)

#-----------------------------------------------------------------------------#
#                             Library target                                  #
#-----------------------------------------------------------------------------#
add_library(bitonic_sort
    src/config.cxx
    src/ocl_bitonic_sort.cxx
)
target_include_directories(bitonic_sort
    PUBLIC include
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated
    PRIVATE lib/CLI
)
target_compile_definitions(bitonic_sort
    PRIVATE ANALYZE=$<BOOL:${ANALYZE}>
)
target_link_libraries(bitonic_sort PRIVATE OpenCL::OpenCL)

#-----------------------------------------------------------------------------#
#                             Benchmark target (random input)                 #
#-----------------------------------------------------------------------------#
add_executable(bitonic_benchmark_random
    src/main.cxx
)
target_include_directories(bitonic_benchmark_random
    PRIVATE lib/CLI
)
target_compile_definitions(bitonic_benchmark_random
    PRIVATE ANALYZE=$<BOOL:${ANALYZE}>
    PRIVATE RANDOM_INPUT=1
    PRIVATE BENCHMARK=1
    PRIVATE COMPARE_CPU=$<BOOL:${COMPARE_CPU}>
    PRIVATE TYPE=${TYPE}
)
target_link_libraries(bitonic_benchmark_random PRIVATE OpenCL::OpenCL bitonic_sort)

#-----------------------------------------------------------------------------#
#                             Benchmark target (std::cin input)               #
#-----------------------------------------------------------------------------#
add_executable(bitonic_benchmark_stdin
    src/main.cxx
)
target_include_directories(bitonic_benchmark_stdin
    PRIVATE lib/CLI
)
target_compile_definitions(bitonic_benchmark_stdin
    PRIVATE ANALYZE=$<BOOL:${ANALYZE}>
    PRIVATE BENCHMARK=1
    PRIVATE COMPARE_CPU=$<BOOL:${COMPARE_CPU}>
    PRIVATE TYPE=${TYPE}
)
target_link_libraries(bitonic_benchmark_stdin PRIVATE OpenCL::OpenCL bitonic_sort)

#-----------------------------------------------------------------------------#
#                             Verify target (std::cin input)                  #
#-----------------------------------------------------------------------------#
add_executable(bitonic_verify
    src/main.cxx
)
target_include_directories(bitonic_verify
    PRIVATE lib/CLI
)
target_compile_definitions(bitonic_verify
    PRIVATE ANALYZE=$<BOOL:${ANALYZE}>
    PRIVATE VERIFY=1
    PRIVATE COMPARE_CPU=$<BOOL:${COMPARE_CPU}>
    PRIVATE TYPE=${TYPE}
)
target_link_libraries(bitonic_verify PRIVATE OpenCL::OpenCL bitonic_sort)

#-----------------------------------------------------------------------------#
#                             Testing                                         #
#-----------------------------------------------------------------------------#
option(BUILD_TEST "Build tests" OFF)
if (BUILD_TEST)
    add_subdirectory(test)
endif()