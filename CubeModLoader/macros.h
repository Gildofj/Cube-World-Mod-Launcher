#pragma once

#define overwrite_function(offset, function)\
__attribute__((naked)) void ASM_##function() {\
	asm(".intel_syntax noprefix \n" \
		"push rbp \n" \
		"mov rbp, rsp \n" \
		"call "#function" \n"\
		"mov rsp, rbp \n" \
		"pop rbp \n" \
		"ret \n"\
		".att_syntax prefix \n" \
	);\
}\
void setup_##function() {\
	WriteFarJMP(Offset(base, offset), (void*)&ASM_##function);\
}

#define overwrite_function_w_jmp(offset, function, ret_addr)\
GETTER_VAR(void*, ASM_##function_jmpback); \
__attribute__((naked)) void ASM_##function() {\
	asm(".intel_syntax noprefix \n" \
		PUSH_ALL \
		"push rbp \n" \
		"mov rbp, rsp \n" \
		PREPARE_STACK \
		"call "#function" \n"\
		RESTORE_STACK \
		"mov rsp, rbp \n" \
		"pop rbp \n" \
		POP_ALL \
		DEREF_JMP(ASM_##function_jmpback) \
		".att_syntax prefix \n" \
	);\
}\
void setup_##function() {\
	WriteFarJMP(Offset(base, offset), (void*)&ASM_##function);\
	ASM_##function_jmpback = Offset(base, ret_addr); \
}

#define setup_function(function) setup_##function()