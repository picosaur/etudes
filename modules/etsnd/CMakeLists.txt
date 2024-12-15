cmake_minimum_required(VERSION 3.10.0)
project(etsnd)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ETSND_SRC *.cpp *.h)
add_library(${PROJECT_NAME} ${ETSND_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC .)

if(ETSND_BACKEND MATCHES "MINIAUDIO")
    target_compile_definitions(${PROJECT_NAME} PUBLIC ETSND_USE_MINIAUDIO)
    target_link_libraries(${PROJECT_NAME} PUBLIC miniaudio)
elseif(ETSND_BACKEND MATCHES "SDL")
    target_compile_definitions(${PROJECT_NAME} PUBLIC ETSND_USE_SDL)
    target_link_libraries(${PROJECT_NAME} PUBLIC hello_imgui)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC ETSND_USE_DUMMY)
endif()


