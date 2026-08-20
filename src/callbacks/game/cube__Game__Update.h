#pragma once
#include "cwsdk.h"

extern "C" void cube__Game__Update(cube::Game * game)
{
    cube::Game* real_game = cube::GetGame();
    if (!real_game) real_game = game;
    if (!real_game) return;

	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnGameUpdatePriority == (GenericMod::Priority)priority) {
				dll->mod->OnGameUpdate(real_game);
			}
		}
	}
}

GETTER_VAR(void*, ASM_cube__Game__Update_jmpback);
GETTER_VAR(void*, ASM_cube__Game__Update_bail);
extern "C" void ASM_cube__Game__Update();

void setup_cube__Game__Update() {
	WriteFarJMP(CWOffset(0xAD724), (void*)&ASM_cube__Game__Update);
	ASM_cube__Game__Update_jmpback = CWOffset(0xAD737);
	ASM_cube__Game__Update_bail = CWOffset(0xAD74E);
}