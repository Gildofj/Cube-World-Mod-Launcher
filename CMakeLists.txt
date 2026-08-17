cmake_minimum_required(VERSION 3.15)
project(CubeWorldModLauncherMaster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MINGW OR (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND WIN32))
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

option(BUILD_LAUNCHER "Build CubeModLauncher executable" ON)
option(BUILD_LOADER "Build CubeModLoader dynamic library and .fip" ON)
option(BUILD_TESTS "Build automated unit and integration tests" ON)

if(BUILD_LOADER)
    add_subdirectory(CubeModLoader)
endif()

if(BUILD_LAUNCHER)
    add_subdirectory(CubeModLauncher)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

