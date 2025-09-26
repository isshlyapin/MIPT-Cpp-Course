enable_testing()

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
