cmake_minimum_required(VERSION 3.10.0)
project(etdsp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ETDSP_SRC *.cpp *.h)
add_library(${PROJECT_NAME} INTERFACE ${ETDSP_SRC})
target_include_directories (${PROJECT_NAME} INTERFACE .)

