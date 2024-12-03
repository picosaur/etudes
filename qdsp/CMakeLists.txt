cmake_minimum_required(VERSION 3.10.0)
project(qdsp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE QDSP_SRC *.cpp *.h)
add_library(${PROJECT_NAME} INTERFACE ${QDSP_SRC})
target_include_directories (${PROJECT_NAME} INTERFACE .)

