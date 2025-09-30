enable_testing()

find_package(Catch2 REQUIRED)

set(TESTS
    1
    2
    3
    4
    5
    6
    7
    8
    9
    10
    11
)

foreach(t ${TESTS})
    add_test(
        NAME test_${t}
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/compare_outputs.py
            ${t}
            $<TARGET_FILE:triangle_intersection>
            ${CMAKE_CURRENT_SOURCE_DIR}/data/${t}.in
            ${CMAKE_CURRENT_SOURCE_DIR}/data/${t}.out
    )
endforeach()

add_executable(test_triangle_intersection 
    ${CMAKE_CURRENT_SOURCE_DIR}/test_line_segment_intersection_2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_line_segment_intersection_3d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_triangle_intersection_2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_line_intersection_3d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_triangle_intersection_3d.cpp
)

target_link_libraries(test_triangle_intersection 
    PRIVATE Catch2::Catch2WithMain
    PRIVATE triangle_intersection_lib
)

include(CTest)
include(Catch)
catch_discover_tests(test_triangle_intersection)
