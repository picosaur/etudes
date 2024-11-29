cmake_minimum_required(VERSION 3.10.0)
project(imclub)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE IMCLUB_SRC imgui_club/imgui_memory_editor/*.cpp imgui_club/imgui_memory_editor/*.h)

add_library(${PROJECT_NAME} ${IMCLUB_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC imgui_club/imgui_memory_editor/)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    imgui)
