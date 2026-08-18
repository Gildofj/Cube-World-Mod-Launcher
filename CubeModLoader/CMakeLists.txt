cmake_minimum_required(VERSION 3.20)
project(project_CubeModLoader LANGUAGES CXX)

add_subdirectory(CWSDK)

set(TRAMPOLINES_OBJ "${CMAKE_CURRENT_BINARY_DIR}/trampolines.obj")

add_custom_command(
    OUTPUT "${TRAMPOLINES_OBJ}"
    COMMAND ml64.exe /c /Fo "${TRAMPOLINES_OBJ}" "${CMAKE_CURRENT_SOURCE_DIR}/trampolines.asm"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/trampolines.asm"
    COMMENT "Assembling trampolines.asm with ml64"
)

add_library(CubeModLoader SHARED 
    crc.cpp
    DLL.cpp
    main.cpp
    mutex.cpp
    ModWidget.cpp
    Logger.cpp
    CrashHandler.cpp
    "${TRAMPOLINES_OBJ}"
)

target_include_directories(CubeModLoader PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/callbacks
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(CubeModLoader LINK_PUBLIC CWSDK dbghelp imgui)
target_compile_definitions(CubeModLoader PRIVATE MODLOADER _ITERATOR_DEBUG_LEVEL=0)
target_compile_definitions(CWSDK PRIVATE MODLOADER)

if(MSVC)
    set_property(TARGET CubeModLoader PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_custom_command(TARGET CubeModLoader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:CubeModLoader>
    $<TARGET_FILE_DIR:CubeModLoader>/CubeModLoader.fip
    COMMENT "Generating CubeModLoader.fip from dynamic library"
)