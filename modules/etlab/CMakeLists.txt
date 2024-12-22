cmake_minimum_required(VERSION 3.10.0)
project(etlab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ETLAB_SRC *.cpp *.h)
add_library(${PROJECT_NAME} ${ETLAB_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC .)

