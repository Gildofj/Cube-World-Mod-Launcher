cmake_minimum_required(VERSION 3.20)
project(project_CubeWorldTests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TEST_SOURCES
    test_main.cpp
    unit/test_crc.cpp
    unit/test_priority_callbacks.cpp
    unit/test_version_compat.cpp
    unit/test_modwidget_storage.cpp
    unit/test_modwidget_pagination.cpp
    unit/test_mutex.cpp
    unit/test_trampoline.cpp
    unit/test_logger.cpp
    unit/test_crash_handler.cpp
    integration/test_mod_lifecycle.cpp
    integration/test_fault_injection.cpp
)

add_executable(test_runner 
    ${TEST_SOURCES}
    $<TARGET_OBJECTS:CubeForgeLoader_Common>
)

target_include_directories(test_runner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/callbacks
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${cubeforge_sdk_SOURCE_DIR}
)

target_compile_definitions(test_runner PRIVATE _CRT_SECURE_NO_WARNINGS)

if(MSVC)
    target_link_libraries(test_runner PRIVATE dbghelp)
    target_compile_definitions(test_runner PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    set_property(TARGET test_runner PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(CTest)
enable_testing()
add_test(NAME AllTests COMMAND test_runner)
