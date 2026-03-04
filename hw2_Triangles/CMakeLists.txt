cmake_minimum_required(VERSION 3.30)

project (Triangles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# find_package(glm REQUIRED)
# find_package(GLEW REQUIRED)
# find_package(glfw3 REQUIRED)
# find_package(OpenGL REQUIRED)

# #-----------------------------------------------------------------------------#
# #                            Kernel Source Handling                           #
# #-----------------------------------------------------------------------------#
# set(SHADER_OGL ${CMAKE_CURRENT_SOURCE_DIR}/modules/render/vertex_shader.glsl)
# file(READ "${SHADER_OGL}" VERTEX_TEXT)

# set(SHADER_OGL ${CMAKE_CURRENT_SOURCE_DIR}/modules/render/fragment_shader.glsl)
# file(READ "${SHADER_OGL}" FRAGMENT_TEXT)

# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/modules/render/shader_source.cppm.in
#     ${CMAKE_CURRENT_BINARY_DIR}/generated/shader_source.cppm
#     @ONLY
# )

add_library(geometry)
target_sources(geometry
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES 
        modules/triangle.cppm
        modules/line.cppm
        modules/point.cppm
        modules/vector.cppm
        modules/plane.cppm
        modules/line_segment.cppm
        modules/config.cppm
)

add_library(intersection)
target_sources(intersection
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES 
        modules/tri_intersection.cppm
)
target_link_libraries(intersection
    PUBLIC geometry
)

# add_library(trivis)
# target_sources(trivis
#     PUBLIC
#         FILE_SET cxx_modules TYPE CXX_MODULES
#         FILES 
#         modules/render/mesh.cppm        
#         modules/render/scene.cppm        
#         modules/render/window.cppm        
#         modules/render/trivis.cppm
#         modules/render/config.cppm
#         modules/render/render.cppm
#         modules/render/camera.cppm
#         modules/render/helpers.cppm
#         modules/render/exceptions.cppm
#         modules/render/shader_program.cppm
#         modules/render/triangle_intersection_view.cppm
#         ${CMAKE_CURRENT_BINARY_DIR}/generated/shader_source.cppm
# )
# target_link_libraries(trivis
#     PRIVATE
#     OpenGL::GL
#     GLEW::GLEW
#     glfw
#     glm
# )

add_executable(triangle_intersection)
target_sources(triangle_intersection
    PUBLIC
    src/main.cpp
)
target_link_libraries(triangle_intersection
    PUBLIC geometry intersection 
)

option(BUILD_TEST "Build tests" OFF)
if (BUILD_TEST)
    add_subdirectory(test)
endif()
