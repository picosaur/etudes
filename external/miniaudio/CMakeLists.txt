cmake_minimum_required(VERSION 3.10.0)
project(miniaudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MINIAUDIO_SRC miniaudio.cpp)

add_library(${PROJECT_NAME} ${MINIAUDIO_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC miniaudio)
