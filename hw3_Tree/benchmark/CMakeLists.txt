find_package(benchmark REQUIRED)

add_executable(benchmark benchmark.cpp)

target_link_libraries(benchmark PRIVATE tree benchmark::benchmark)