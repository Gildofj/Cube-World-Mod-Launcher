extern "C" int PresentHandler(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags) { // Note that this hooks a METHOD, so swapchain is the first argument
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnPresentPriority == (GenericMod::Priority)priority) {
				dll->mod->OnPresent(SwapChain, SyncInterval, Flags);
			}
		}
	}
	return 0;
}

GETTER_VAR(void*, ASM_PresentHandler_jmpback);
extern "C" void ASM_PresentHandler();

void SetupPresentHandler() {
	WriteFarJMP(Offset(base, 0x134743), (void*)&ASM_PresentHandler);
	ASM_PresentHandler_jmpback = Offset(base, 0x134751);
}
