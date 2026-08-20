extern "C" int CheckInventoryFullHandler(cube::Creature* player, cube::Item* item) {
    if (!player || !item) return 0;
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnCheckInventoryFullPriority == (GenericMod::Priority)priority) {
				if (int result = dll->mod->OnCheckInventoryFull(player, item)) {
					return result;
				}
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASMCheckInventoryFullHandler_jmpback);
GETTER_VAR(void*, ASMCheckInventoryFullHandler_retn);
extern "C" void ASMCheckInventoryFullHandler();

void SetupCheckInventoryFullHandler() {
    WriteFarJMP(Offset(base, 0x50670), (void*)&ASMCheckInventoryFullHandler);
    ASMCheckInventoryFullHandler_jmpback = Offset(base, 0x5067F);
    ASMCheckInventoryFullHandler_retn = Offset(base, 0x507A0);
}
