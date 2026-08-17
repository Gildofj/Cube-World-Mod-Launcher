cmake_minimum_required(VERSION 3.8)
project(CubeWorldModLauncherMaster)

option(BUILD_TESTS "Build automated unit and integration tests" ON)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
