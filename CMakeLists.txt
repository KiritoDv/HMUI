cmake_minimum_required(VERSION 3.23)
project(hmui)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LOCAL_DEPS "Enable to retrieve deps from remote repositories using fetch" OFF)

include_directories("src")
include_directories("lib")
file(GLOB_RECURSE SOURCES src/*.cpp)

if(NOT ${LOCAL_DEPS})
## raylib ##

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG ae50bfa2cc569c0f8d5bc4315d39db64005b1b08
)
FetchContent_MakeAvailable(raylib)

include_directories(${raylib_SOURCE_DIR}/include)
link_directories(${raylib_SOURCE_DIR}/src)

## ImGui ##

FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.6-docking
)
FetchContent_MakeAvailable(ImGui)

list(APPEND SOURCES 
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
)

include_directories(${imgui_SOURCE_DIR})

## rlImGui ##

FetchContent_Declare(
    rlImGui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG 3c6986358c001851d9bc20e55acf9de803c73b5d
)
FetchContent_MakeAvailable(rlImGui)

list(APPEND SOURCES ${rlimgui_SOURCE_DIR}/rlImGui.cpp)
include_directories(${rlimgui_SOURCE_DIR})

else()
## raylib ##

include_directories(lib/raylib/include)
link_directories(lib/raylib/src)

## ImGui ##

list(APPEND SOURCES 
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui.cpp
)

include_directories(lib/imgui)

## rlImGui ##

list(APPEND SOURCES lib/rlImGui/rlImGui.cpp)
include_directories(lib/rlImGui)

endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} raylib)
