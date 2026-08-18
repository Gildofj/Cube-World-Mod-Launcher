extern "C" int CreatureSpellPowerCalculatedHandler(cube::Creature* creature, float* power) {
    if (!creature || !power) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureSpellPowerCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureSpellPowerCalculated(creature, power);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureSpellPowerCalculatedHandler();

void SetupCreatureSpellPowerCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x65E84), (void*)&ASM_CreatureSpellPowerCalculatedHandler);
}
