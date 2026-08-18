extern "C" int ZoneGeneratedHandler(cube::Zone* zone) {
    if (!zone) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnZoneGeneratedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnZoneGenerated(zone);
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_ZoneGeneratedHandler_jmpback);
extern "C" void ASM_ZoneGeneratedHandler();

void SetupZoneGeneratedHandler() {
    WriteFarJMP(Offset(base, 0x2AE34F), (void*)&ASM_ZoneGeneratedHandler);
	ASM_ZoneGeneratedHandler_jmpback = Offset(base, 0x2AE35F);
}
