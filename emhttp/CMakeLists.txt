cmake_minimum_required(VERSION 3.10.0)
project(emhttp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE EMHTTP_SRC *.cpp *.h)

add_library(${PROJECT_NAME} ${EMHTTP_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC .)

if(EMSCRIPTEN)
    target_link_options(${PROJECT_NAME} PUBLIC -sFETCH)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE httplib)
endif()

# if (EMSCRIPTEN)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -sUSE_GLFW=3 -sUSE_PTHREADS=1 -sALLOW_MEMORY_GROWTH=1)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -sTOTAL_MEMORY=134217728)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -sMAX_WEBGL_VERSION=2)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -sEXPORTED_RUNTIME_METHODS=ccall)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -sFETCH)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -sWASM_BIGINT)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -O1)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -sLEGACY_GL_EMULATION)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -fsanitize=null)
#     target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -fsanitize-minimal-runtime)
#     target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE idbfs.js)
# endif ()

