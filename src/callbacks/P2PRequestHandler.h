extern "C" int P2PRequestHandler(long long steamID) {
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnP2PRequestPriority == (GenericMod::Priority)priority) {
				if (int result = dll->mod->OnP2PRequest(steamID)) {
					return result;
				}
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASMP2PRequestHandler_jmpback);
GETTER_VAR(void*, ASMP2PRequestHandler_block);
GETTER_VAR(void*, ASMP2PRequestHandler_allow);
extern "C" void ASMP2PRequestHandler();

void SetupP2PRequestHandler() {
    WriteFarJMP(Offset(base, 0x9F6DF), (void*)&ASMP2PRequestHandler);
    ASMP2PRequestHandler_jmpback = Offset(base, 0x9F6ED);
    ASMP2PRequestHandler_block = Offset(base, 0x9F7A6);
    ASMP2PRequestHandler_allow = Offset(base, 0x9F783);
}
