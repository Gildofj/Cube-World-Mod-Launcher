extern "C" int CreatureCriticalCalculatedHandler(cube::Creature* creature, float* critical) {
    if (!creature || !critical) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureCriticalCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureCriticalCalculated(creature, critical);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureCriticalCalculatedHandler();

void SetupCreatureCriticalCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x50E7A), (void*)&ASM_CreatureCriticalCalculatedHandler);
}
