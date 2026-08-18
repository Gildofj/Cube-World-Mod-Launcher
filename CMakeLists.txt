cmake_minimum_required(VERSION 3.15)
project(CubeWorldModLauncherMaster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0 _CRT_SECURE_NO_WARNINGS DIRECTINPUT_VERSION=0x0800)
    add_compile_options(/W3 /EHsc /utf-8 /wd4828)
endif()

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
