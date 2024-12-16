cmake_minimum_required(VERSION 3.10.0)
project(etfire)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ETFIRE_SRC *.cpp *.h)
add_library(${PROJECT_NAME} ${ETFIRE_SRC})
target_include_directories (${PROJECT_NAME} PUBLIC .)

find_package(ArrayFire REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${ArrayFire_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ArrayFire_LIBRARIES})
