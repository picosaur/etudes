cmake_minimum_required(VERSION 3.10.0)
project(etguiw)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ETGUIW_SRC *.cpp *.h)
add_library(${PROJECT_NAME} ${ETGUIW_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC .)

set(ETGUIW_LIBS
    hello_imgui
    etgui
    etosm
    etdsp
    etsnd
    ethttp
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${ETGUIW_LIBS})
