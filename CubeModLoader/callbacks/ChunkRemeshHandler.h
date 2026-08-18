extern "C" int ChunkRemeshHandler(cube::Zone* zone) {
    if (!zone) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnChunkRemeshPriority == (GenericMod::Priority)priority) {
				dll->mod->OnChunkRemesh(zone);
			}
		}
	}
	return 0;
}

GETTER_VAR(void*, ASM_ChunkRemeshHandler_jmpback);
extern "C" void ASM_ChunkRemeshHandler();

void SetupChunkRemeshHandler() {
	WriteFarJMP(Offset(base, 0xE969C), (void*)&ASM_ChunkRemeshHandler);
	ASM_ChunkRemeshHandler_jmpback = Offset(base, 0xE96AA);
}
