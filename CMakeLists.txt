cmake_minimum_required(VERSION 3.16)
project("ai-agent"
    DESCRIPTION "An AI-Agent simulation with genetic algorithms"
    VERSION 0.0.1
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${CMAKE_PROJECT_NAME})

set(GLAD_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glad"
    CACHE STRING "Path to glad"
)

set(GLFW_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw"
    CACHE STRING "Path to GLFW"
)
set(DEAR_IMGUI_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui"
    CACHE STRING "Path to Dear ImGui"
)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

set(sources 
    src/main.cpp
    src/util/util.cpp
    )

set(resource_files
    ${CMAKE_SOURCE_DIR}/assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf
)

# required packages
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

# glad
add_library(glad "${GLAD_PREFIX}/src/glad.c")
include_directories(
    "${GLAD_PREFIX}/include"
)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_PREFIX})
include_directories(
    "${GLFW_PREFIX}/include"
)

#imgui
list(APPEND sources
    ${DEAR_IMGUI_PREFIX}/imgui.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_draw.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_tables.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_widgets.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_impl_glfw.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_impl_opengl3.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_demo.cpp
)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${sources}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
    ${OPENGL_gl_LIBRARY}
    glad
    glfw
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    ${X11_LIBRARIES}
    ${CMAKE_DL_LIBS}
)
