#pragma once

extern "C" void cube__Creature__OnPlayerCombatDeath(cube::Game* game)
{
    if (!game || !game->world || !game->world->local_creature) return;
	cube::Creature* player = game->world->local_creature;
	player->entity_data.HP = 0;

	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnPlayerDeathPriority == (GenericMod::Priority)priority) {
				dll->mod->OnPlayerDeath(game, player, GenericMod::DeathType::COMBAT);
			}
		}
	}
}

extern "C" void SoundPacket__ctor(void* a1)
{
	((void (*)(void*))CWOffset(0x80270))(a1);
}

GETTER_VAR(void*, ASM_cube__Creature__OnPlayerCombatDeath_JMPBACK);
extern "C" void ASM_cube__Creature__OnPlayerCombatDeath();

void setup_cube__Creature__OnPlayerCombatDeath() {
		WriteFarJMP(CWOffset(0xA8EE7), (void*)&ASM_cube__Creature__OnPlayerCombatDeath);
		ASM_cube__Creature__OnPlayerCombatDeath_JMPBACK = CWOffset(0xA8EFA);
}