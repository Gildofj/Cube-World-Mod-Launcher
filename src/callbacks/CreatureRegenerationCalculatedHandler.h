extern "C" int CreatureRegenerationCalculatedHandler(cube::Creature* creature, float* regeneration) {
    if (!creature || !regeneration) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureRegenerationCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureRegenerationCalculated(creature, regeneration);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureRegenerationCalculatedHandler();

void SetupCreatureRegenerationCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x648A1), (void*)&ASM_CreatureRegenerationCalculatedHandler);
}
