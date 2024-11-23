cmake_minimum_required(VERSION 3.10.0)
project(mi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MI_SRC *.cpp *.h)

add_library(${PROJECT_NAME} ${MI_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    imgui
    implot
    em
    hello_imgui)
