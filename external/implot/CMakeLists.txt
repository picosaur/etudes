cmake_minimum_required(VERSION 3.10.0)
project(implot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE IMPLOT_SRC implot/*.cpp implot/*.h)

add_library(${PROJECT_NAME} ${IMPLOT_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC implot/)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    imgui)
