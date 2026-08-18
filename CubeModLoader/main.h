#ifndef MAIN_H
#define MAIN_H

#include "DLL.h"
#include <vector>

void WriteFarJMP(void* source, void* destination);
void Popup(const char* title, const char* msg);
void PrintLoadedMods();

#define MUST_IMPORT(dllname, name)\
dllname->name = GetProcAddress(dllname->handle, #name);\
            if (!dllname->name) {\
                char ERROR_MESSAGE_POPUP[512] = {0};\
                sprintf(ERROR_MESSAGE_POPUP, "%s does not export " #name ".\n", dllname->fileName.c_str());\
                Popup("Error", ERROR_MESSAGE_POPUP);\
                exit(1);\
            }

#define IMPORT(dllname, name)\
dllname->name = GetProcAddress(dllname->handle, #name);

#define GETTER_VAR(vartype, varname)\
	extern "C" vartype varname = 0;\
	extern "C" vartype Get_##varname(){ return varname; }

#define GLOBAL static

void* Offset(void* x1, uint64_t x2);

#endif // MAIN_H

