cmake_minimum_required(VERSION 3.20)
project(project_CubeModLauncher LANGUAGES CXX)

add_executable(CubeModLauncher 
    main.cpp
    Process.cpp
    ../CubeModLoader/Logger.cpp
)

target_include_directories(CubeModLauncher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../CubeModLoader
)