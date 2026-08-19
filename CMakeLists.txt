cmake_minimum_required(VERSION 3.20)
project(CubeWorldModLauncherMaster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0 _CRT_SECURE_NO_WARNINGS DIRECTINPUT_VERSION=0x0800)
    add_compile_options(/W3 /EHsc /utf-8 /wd4828)
endif()

# Fetch and configure Dear ImGui
include(FetchContent)
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    if(MSVC)
        target_compile_definitions(imgui PUBLIC _ITERATOR_DEBUG_LEVEL=0)
        set_property(TARGET imgui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
    target_link_libraries(imgui PUBLIC d3d11 dxgi)
endif()

# Fetch and configure CubeForge SDK
FetchContent_Declare(
    cubeforge_sdk
    GIT_REPOSITORY https://github.com/Gildofj/cubeforge.sdk.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cubeforge_sdk)

option(BUILD_LAUNCHER "Build CubeModLauncher executable" ON)
option(BUILD_LOADER "Build CubeModLoader dynamic library and .fip" ON)
option(BUILD_TESTS "Build automated unit and integration tests" ON)

enable_testing()

if(BUILD_LOADER)
    add_subdirectory(CubeModLoader)
endif()

if(BUILD_LAUNCHER)
    add_subdirectory(CubeModLauncher)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
