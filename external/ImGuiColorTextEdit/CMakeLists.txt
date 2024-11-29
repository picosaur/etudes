cmake_minimum_required(VERSION 3.10.0)
project(imtexteditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE IMTEXTEDITOR_SRC ImGuiColorTextEdit/*.cpp ImGuiColorTextEdit/*.h)

add_library(${PROJECT_NAME} ${IMTEXTEDITOR_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC ImGuiColorTextEdit)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    imgui)
