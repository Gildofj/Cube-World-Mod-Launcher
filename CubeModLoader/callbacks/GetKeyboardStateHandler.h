extern "C" int GetKeyboardStateHandler(BYTE* diKeys) {
    if (!diKeys) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnGetKeyboardStatePriority == (GenericMod::Priority)priority) {
				dll->mod->OnGetKeyboardState(diKeys);
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_GetKeyboardStateHandler_jmpback);
extern "C" void ASM_GetKeyboardStateHandler();

void SetupGetKeyboardStateHandler() {
    WriteFarJMP(Offset(base, 0x13664B), (void*)&ASM_GetKeyboardStateHandler);
	ASM_GetKeyboardStateHandler_jmpback = Offset(base, 0x13665F);
}
