cmake_minimum_required(VERSION 3.10.0)
project(etgui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ETGUI_SRC *.cpp *.h)
add_library(${PROJECT_NAME} ${ETGUI_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC .)

set(ETGUI_LIBS
    hello_imgui
    imgui
    implot
    imclub
    imtexteditor
    imtexinspect
    ethttp
    etsnd
    etdsp
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${ETGUI_LIBS})
