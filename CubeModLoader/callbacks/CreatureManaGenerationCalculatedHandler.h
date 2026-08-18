extern "C" int CreatureManaGenerationCalculatedHandler(cube::Creature* creature, float* manaGeneration) {
    if (!creature || !manaGeneration) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureManaGenerationCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureManaGenerationCalculated(creature, manaGeneration);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureManaGenerationCalculatedHandler();

void SetupCreatureManaGenerationCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x5F8D0), (void*)&ASM_CreatureManaGenerationCalculatedHandler);
}
