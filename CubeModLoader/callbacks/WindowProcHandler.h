#include <windows.h>

extern bool g_ImGuiInitialized;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern "C" int WindowProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (g_ImGuiInitialized) {
		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
			return 1;
		}
	}
	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnWindowProcPriority == (GenericMod::Priority)priority) {
				if (int result = dll->mod->OnWindowProc(hwnd, uMsg, wParam, lParam)) {
					return result;
				}
			}
		}
	}
    return 0;
}

GETTER_VAR(void*, ASM_WindowProcHandler_jmpback);
extern "C" void ASM_WindowProcHandler();

void SetupWindowProcHandler() {
    WriteFarJMP(Offset(base, 0x133C70), (void*)&ASM_WindowProcHandler);
	ASM_WindowProcHandler_jmpback = Offset(base, 0x133C80);
}
