cmake_minimum_required(VERSION 3.10.0)
project(emwrapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE EMWRAPPER_SRC *.cpp *.h)

add_library(${PROJECT_NAME} ${EMWRAPPER_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC .)

target_link_options(${PROJECT_NAME} PUBLIC -sFETCH)

