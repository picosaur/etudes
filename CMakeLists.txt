cmake_minimum_required(VERSION 3.13)  
project(imhello VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/hello_imgui)
add_subdirectory(external/implot)
add_subdirectory(external/cpp-httplib)
add_subdirectory(external/imgui_club)
add_subdirectory(external/ImGuiColorTextEdit)

add_subdirectory(emhttp)
add_subdirectory(aude)
add_subdirectory(migui)

set(HELLOIMGUI_USE_SDL2 ON CACHE BOOL "" FORCE)
set(HELLOIMGUI_HAS_OPENGL3 ON CACHE BOOL "" FORCE)

hello_imgui_add_app(imhello app/main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC migui)
