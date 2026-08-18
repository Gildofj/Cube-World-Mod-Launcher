extern "C" int ChatHandler(std::wstring* msg) {
    if (!msg) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnChatPriority == (GenericMod::Priority)priority) {
				if (dll->mod->OnChat(msg)) {
					return 1;
				}
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASMChatHandler_jmpback);
GETTER_VAR(void*, ASMChatHandler_bail);
extern "C" void ASMChatHandler();

void SetupChatHandler() {
	WriteFarJMP(Offset(base, 0x97175), (void*)&ASMChatHandler);
	ASMChatHandler_jmpback = Offset(base, 0x97188);
	ASMChatHandler_bail = Offset(base, 0x9777A);
}
