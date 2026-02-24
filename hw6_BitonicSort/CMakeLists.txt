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
#                             Executable target                               #
#-----------------------------------------------------------------------------#
add_executable(bitonic 
    src/main.cxx
)
target_include_directories(bitonic
    PRIVATE lib/CLI
)
target_compile_definitions(bitonic 
    PRIVATE ANALYZE=$<BOOL:${ANALYZE}>
    PRIVATE COMPARE_CPU=$<BOOL:${COMPARE_CPU}>
    PRIVATE TYPE=${TYPE}
)
target_link_libraries(bitonic PRIVATE OpenCL::OpenCL bitonic_sort)

#-----------------------------------------------------------------------------#
#                             Testing                                         #
#-----------------------------------------------------------------------------#
option(BUILD_TEST "Build tests" OFF)
if (BUILD_TEST)
    add_subdirectory(test)
endif()