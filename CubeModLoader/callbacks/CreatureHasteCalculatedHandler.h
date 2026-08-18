extern "C" int CreatureHasteCalculatedHandler(cube::Creature* creature, float* haste) {
    if (!creature || !haste) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureHasteCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureHasteCalculated(creature, haste);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureHasteCalculatedHandler();

void SetupCreatureHasteCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x66652), (void*)&ASM_CreatureHasteCalculatedHandler);
}
