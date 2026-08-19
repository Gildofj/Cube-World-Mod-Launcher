extern "C" int CreatureAttackPowerCalculatedHandler(cube::Creature* creature, float* power) {
    if (!creature || !power) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCreatureAttackPowerCalculatedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnCreatureAttackPowerCalculated(creature, power);
			}
		}
	}
	return 0;
}

extern "C" void ASM_CreatureAttackPowerCalculatedHandler();

void SetupCreatureAttackPowerCalculatedHandler() {
	WriteFarJMP(Offset(base, 0x4FD2B), (void*)&ASM_CreatureAttackPowerCalculatedHandler);
}
