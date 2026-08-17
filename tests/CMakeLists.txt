cmake_minimum_required(VERSION 3.8)
project(project_CubeWorldTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(TEST_SOURCES
    test_main.cpp
    unit/test_crc.cpp
    unit/test_priority_callbacks.cpp
    unit/test_version_compat.cpp
    unit/test_modwidget_storage.cpp
    unit/test_modwidget_pagination.cpp
    unit/test_mutex.cpp
    unit/test_trampoline.cpp
    unit/test_launcher_process.cpp
    integration/test_mod_lifecycle.cpp
    integration/test_fault_injection.cpp
    ../CubeModLoader/crc.cpp
    ../CubeModLoader/mutex.cpp
)

add_executable(test_runner ${TEST_SOURCES})

include(CTest)
enable_testing()
add_test(NAME AllTests COMMAND test_runner)
