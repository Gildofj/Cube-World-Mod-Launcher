extern "C" int ChunkRemeshedHandler(cube::Zone* zone) {
    if (!zone) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnChunkRemeshedPriority == (GenericMod::Priority)priority) {
				dll->mod->OnChunkRemeshed(zone);
			}
		}
	}
	return 0;
}

GETTER_VAR(void*, ASM_ChunkRemeshedHandler_jmpback);
extern "C" void ASM_ChunkRemeshedHandler();

void SetupChunkRemeshedHandler() {
	WriteFarJMP(Offset(base, 0xECB5F), (void*)&ASM_ChunkRemeshedHandler);
	ASM_ChunkRemeshedHandler_jmpback = Offset(base, 0xECB6D);
}
