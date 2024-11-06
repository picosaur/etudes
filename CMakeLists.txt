cmake_minimum_required(VERSION 3.13)  
project(imhello VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/hello_imgui)
add_subdirectory(emwrapper)
add_subdirectory(gui)

hello_imgui_add_app(imhello app/main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC gui)
