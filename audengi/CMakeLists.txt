cmake_minimum_required(VERSION 3.10.0)
project(audengi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE AUDENGI_SRC *.cpp *.h)

add_library(${PROJECT_NAME} ${AUDENGI_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC .)

if(IOS)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2-static SDL2main)
elseif(EMSCRIPTEN)
    target_compile_options(${PROJECT_NAME} PUBLIC -s USE_SDL=2)
    target_link_options(${PROJECT_NAME} INTERFACE -s USE_SDL=2)
elseif(ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2main SDL2)
elseif(TARGET SDL2-static)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2-static)
else()
    find_package(SDL2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2 SDL2::SDL2main)
endif()

