extern "C" int CreatureResistanceCalculatedHandler(cube::Creature* creature, float* resistance) {
    if (!creature || !resistance) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureResistanceCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureResistanceCalculated(creature, resistance);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureResistanceCalculatedHandler();

void SetupCreatureResistanceCalculatedHandler() {
	// cube::Creature::GetResistance has two returns
	WriteFarJMP(Offset(base, 0x64EF1), (void*)&ASM_CreatureResistanceCalculatedHandler);
	WriteFarJMP(Offset(base, 0x64F0F), (void*)&ASM_CreatureResistanceCalculatedHandler);
}
