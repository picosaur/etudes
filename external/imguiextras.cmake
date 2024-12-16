cmake_minimum_required(VERSION 3.11)

# implot
set(IMPLOT_DIR ${CMAKE_CURRENT_LIST_DIR}/implot)
set(IMPLOT_SRC
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp)
include_directories(${IMPLOT_DIR})

# memory_editor
set(IMGUICLUB_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui_club)
include_directories(${IMGUICLUB_DIR}/imgui_memory_editor)

# ImGuiColorTextEdit
set(IMTEXTEDITOR_DIR ${CMAKE_CURRENT_LIST_DIR}/ImGuiColorTextEdit)
set(IMTEXTEDITOR_SRC 
    ${IMTEXTEDITOR_DIR}/TextEditor.cpp
    ${IMTEXTEDITOR_DIR}/LanguageDefinitions.cpp)

# imgui_tex_inspect
set(IMTEXINSPECT_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui_tex_inspect)
set(IMTEXINSPECT_SRC
    ${IMTEXINSPECT_DIR}/imgui_tex_inspect.cpp
    ${IMTEXINSPECT_DIR}/backends/tex_inspect_opengl.cpp)
include_directories(${IMTEXINSPECT_DIR})

# all
set(IMGUIEXTRAS_SRC
    ${IMPLOT_SRC}
    ${IMTEXTEDITOR_SRC}
    ${IMTEXINSPECT_SRC}
)

include_directories(${CMAKE_CURRENT_LIST_DIR})


