extern "C" int ZoneDestroyHandler(cube::Zone* zone) {
    if (!zone) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnZoneDestroyPriority == (GenericMod::Priority)priority) {
				dll->mod->OnZoneDestroy(zone);
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_ZoneDestroyHandler_jmpback);
extern "C" void ASM_ZoneDestroyHandler();

void SetupZoneDestroyHandler() {
    WriteFarJMP(Offset(base, 0x2F9940), (void*)&ASM_ZoneDestroyHandler);
	ASM_ZoneDestroyHandler_jmpback = Offset(base, 0x2F9952);
}
