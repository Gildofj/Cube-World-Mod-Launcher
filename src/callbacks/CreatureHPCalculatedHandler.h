extern "C" int CreatureHPCalculatedHandler(cube::Creature* creature, float* hp) {
    if (!creature || !hp) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureHPCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureHPCalculated(creature, hp);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureHPCalculatedHandler();

void SetupCreatureHPCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x5FC11), (void*)&ASM_CreatureHPCalculatedHandler);
}
