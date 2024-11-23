cmake_minimum_required(VERSION 3.10.0)
project(gui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE GUI_SRC *.cpp *.h)

add_library(${PROJECT_NAME} ${GUI_SRC} ${IMPLOT_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} PUBLIC mi)
