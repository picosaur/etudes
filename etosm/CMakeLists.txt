cmake_minimum_required(VERSION 3.10.0)
project(etosm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ETOSM_SRC *.cpp *.h)
add_library(${PROJECT_NAME} ${ETOSM_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC .)

set(ETOSM_LIBS
    hello_imgui
    ethttp
    etgui
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${ETOSM_LIBS})
