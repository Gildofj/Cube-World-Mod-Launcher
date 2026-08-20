cmake_minimum_required(VERSION 3.20)
project(CubeForgeLoader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    # Enable MASM support natively on MSVC
    enable_language(ASM_MASM)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0 _CRT_SECURE_NO_WARNINGS DIRECTINPUT_VERSION=0x0800)
    add_compile_options(/W3 /EHsc /utf-8 /wd4828)
endif()

# Enable Link-Time Optimization (LTO) globally if supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported LANGUAGES CXX)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
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

FetchContent_Declare(
    cubeforge_sdk
    GIT_REPOSITORY https://github.com/Gildofj/cubeforge.sdk.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cubeforge_sdk)

option(BUILD_LOADER "Build CubeForgeLoader dynamic library and .fip" ON)
option(BUILD_TESTS "Build automated unit and integration tests" OFF)

# Always add src subdirectory because both loader and test runner depend on its objects
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
