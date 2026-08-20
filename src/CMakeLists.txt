cmake_minimum_required(VERSION 3.20)
project(project_CubeForgeLoader LANGUAGES CXX)

# Create an OBJECT library for the shared core files to avoid recompiling them in the tests
add_library(CubeForgeLoader_Common OBJECT
    crc.cpp
    mutex.cpp
    Logger.cpp
    CrashHandler.cpp
)

target_include_directories(CubeForgeLoader_Common PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/callbacks
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${cubeforge_sdk_SOURCE_DIR}
)

if(MSVC)
    set_property(TARGET CubeForgeLoader_Common PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_definitions(CubeForgeLoader_Common PRIVATE _ITERATOR_DEBUG_LEVEL=0 MODLOADER)
endif()

if(BUILD_LOADER)
    if(MSVC)
        # On MSVC, use native MASM compiler support for trampolines.asm
        add_library(CubeForgeLoader SHARED 
            $<TARGET_OBJECTS:CubeForgeLoader_Common>
            DLL.cpp
            main.cpp
            ModWidget.cpp
            trampolines.asm
        )
    else()
        # Non-MSVC platforms (like macOS during development/testing)
        add_library(CubeForgeLoader SHARED 
            $<TARGET_OBJECTS:CubeForgeLoader_Common>
            DLL.cpp
            main.cpp
            ModWidget.cpp
        )
    endif()

    target_include_directories(CubeForgeLoader PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/callbacks
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${cubeforge_sdk_SOURCE_DIR}
    )

    target_link_libraries(CubeForgeLoader LINK_PUBLIC CWSDK dbghelp imgui)
    target_compile_definitions(CubeForgeLoader PRIVATE MODLOADER _ITERATOR_DEBUG_LEVEL=0)
    target_compile_definitions(CWSDK PRIVATE MODLOADER)

    if(MSVC)
        set_property(TARGET CubeForgeLoader PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    add_custom_command(TARGET CubeForgeLoader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:CubeForgeLoader>
        $<TARGET_FILE_DIR:CubeForgeLoader>/CubeForgeLoader.fip
        COMMENT "Generating CubeForgeLoader.fip from dynamic library"
    )
endif()
