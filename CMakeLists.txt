cmake_minimum_required(VERSION 3.11)
project(etudes VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# configure
# -----------------------------------------------------------------------------
set(ETUDES_SRC_TEST OFF CACHE INTERNAL "" FORCE)
set(ETSND_BACKEND "" CACHE INTERNAL "" FORCE)
set(ETHTTP_BACKEND "" CACHE INTERNAL "" FORCE)

# externals
# -----------------------------------------------------------------------------
set(HELLOIMGUI_USE_SDL2 ON CACHE INTERNAL "" FORCE)
set(HELLOIMGUI_HAS_OPENGL3 ON CACHE INTERNAL "" FORCE)

include(external/imguiextras.cmake)
add_subdirectory(external/hello_imgui)

include(external/clay.cmake)

if(ETSND_BACKEND MATCHES "MINIAUDIO")
    add_subdirectory(external/miniaudio)
endif()

if(ETHTTP_BACKEND MATCHES "CPPHTTPLIB")
    add_subdirectory(external/cpp-httplib)
endif()

# internals
# -----------------------------------------------------------------------------
add_subdirectory(modules/ethttp)
add_subdirectory(modules/etdsp)
add_subdirectory(modules/etsnd)
add_subdirectory(modules/etfire)
add_subdirectory(modules/etlab)

set(ETMODULES
    etdsp
    etsnd
    ethttp
    etfire
    etlab
)

# application
# -----------------------------------------------------------------------------
if(ETUDES_SRC_TEST)
    file(GLOB_RECURSE ETUDES_SRC src_test/*.cpp src_test/*.h)
    hello_imgui_add_app(${PROJECT_NAME} ${ETUDES_SRC} ${IMGUIEXTRAS_SRC})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${ETMODULES})
else()
    file(GLOB_RECURSE ETUDES_SRC src/*.cpp src/*.h)
    hello_imgui_add_app(${PROJECT_NAME} ${ETUDES_SRC} ${IMGUIEXTRAS_SRC})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${ETMODULES})
    target_include_directories(${PROJECT_NAME} PUBLIC src/etgui)
endif()



