cmake_minimum_required(VERSION 3.13)  

project(imhello VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor/hello_imgui)

hello_imgui_add_app(imhello src/main.cpp)
