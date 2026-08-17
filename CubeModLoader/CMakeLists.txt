cmake_minimum_required (VERSION 3.8)
project(project_CubeModLoader)
add_subdirectory(CWSDK)
add_library (CubeModLoader SHARED 
    crc.cpp
    DLL.cpp
    main.cpp
    mutex.cpp "macros.h" "callbacks/gui/cube__StartMenuWidget__Draw.h" "callbacks/game/cube__Game__MouseUp.h" "callbacks/gui/cube__GUI__Load.h" "ModWidget.h" "ModWidget.cpp" "callbacks/world/cube__World__SpawnCreaturesMaybe.h" "callbacks/creature/cube__Creature__OnPlayerCombatDeath.h" "callbacks/gui/cube__CharacterPreviewWidget__Draw.h" "callbacks/creature/cube__Creature__OnCreatureDeath.h" "callbacks/creature/cube__Creature__OnPlayerDrownDeath.h" "callbacks/creature/cube__Creature__OnPlayerFallDeath.h"  "callbacks/game/cube__Game__Update.h" "callbacks/item/cube__Item__OnGetSellingPrice.h" "callbacks/creature/cube__Creature__CanEquipItem.h" "callbacks/item/cube__Item__OnGetGoldBagValue.h")
target_link_libraries (CubeModLoader LINK_PUBLIC CWSDK)
target_compile_definitions(CubeModLoader PRIVATE MODLOADER)
target_compile_definitions(CWSDK PRIVATE MODLOADER)

if(MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(CubeModLoader PRIVATE -static -static-libgcc -static-libstdc++)
endif()

add_custom_command(TARGET CubeModLoader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:CubeModLoader>
    $<TARGET_FILE_DIR:CubeModLoader>/CubeModLoader.fip
    COMMENT "Generating CubeModLoader.fip from dynamic library"
)