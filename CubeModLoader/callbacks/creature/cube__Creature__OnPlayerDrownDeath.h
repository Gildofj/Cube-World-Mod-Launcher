#pragma once

extern "C" void cube__Creature__OnPlayerDrownDeath(cube::Creature * player)
{
    if (!player) return;
	if (player->entity_data.HP > 0)
		return;

	player->entity_data.HP = 0;
	
	cube::Game* game = cube::GetGame();

	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnPlayerDeathPriority == (GenericMod::Priority)priority) {
				dll->mod->OnPlayerDeath(game, player, GenericMod::DeathType::DROWN);
			}
		}
	}
}

GETTER_VAR(void*, ASM_cube__Creature__OnPlayerDrownDeath_JMPBACK);
extern "C" void ASM_cube__Creature__OnPlayerDrownDeath();

void setup_cube__Creature__OnPlayerDrownDeath() {
	WriteFarJMP(CWOffset(0x2E049A), (void*)&ASM_cube__Creature__OnPlayerDrownDeath);
	ASM_cube__Creature__OnPlayerDrownDeath_JMPBACK = CWOffset(0x2E04AB);
}