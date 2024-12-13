cmake_minimum_required(VERSION 3.10.0)
project(migui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MIGUI_SRC *.cpp *.h)
add_library(${PROJECT_NAME} ${MIGUI_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC .)

set(MIGUI_LIBS
    imgui
    implot
    imclub
    imtexteditor
    emhttp
    snd
    dsp
    imtexinspect
    hello_imgui
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${MIGUI_LIBS})
