extern "C" int GetMouseStateHandler(DIMOUSESTATE* diMouse) {
    if (!diMouse) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnGetMouseStatePriority == (GenericMod::Priority)priority) {
				dll->mod->OnGetMouseState(diMouse);
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_GetMouseStateHandler_jmpback);
extern "C" void ASM_GetMouseStateHandler();

void SetupGetMouseStateHandler() {
    WriteFarJMP(Offset(base, 0x13665F), (void*)&ASM_GetMouseStateHandler);
	ASM_GetMouseStateHandler_jmpback = Offset(base, 0x136675);
}
