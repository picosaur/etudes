cmake_minimum_required(VERSION 3.11)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ethttp)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/etdsp)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/etsnd)

set(ETMODULES
    etdsp
    etsnd
    ethttp
)
