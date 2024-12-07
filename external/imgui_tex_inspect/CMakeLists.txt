cmake_minimum_required(VERSION 3.10.0)
project(imtexinspect)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMTEXINSPECT_SRC
    imgui_tex_inspect/imgui_tex_inspect.cpp
    imgui_tex_inspect/backends/tex_inspect_opengl.h
    imgui_tex_inspect/backends/tex_inspect_opengl.cpp)

add_library(${PROJECT_NAME} ${IMTEXINSPECT_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC imgui_tex_inspect)
target_link_libraries(${PROJECT_NAME} PUBLIC  hello_imgui)
