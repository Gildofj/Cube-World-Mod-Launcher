#include "../framework/test_framework.h"
#include <cstdint>
#include <cstring>
#include <windows.h>

namespace TrampolineHelper {
    inline void* CalculateOffset(void* base_addr, uint64_t offset) {
        return (void*)((uintptr_t)base_addr + offset);
    }

    inline void GenerateFarJmpBytes(char* location, void* destination) {
        // Far jump: jmp [rip+0]
        *((uint16_t*)&location[0]) = 0x25FF;

        // 32-bit RIP relative displacement
        *((uint32_t*)&location[2]) = 0x00000000;

        // 64-bit absolute target address
        *((uint64_t*)&location[6]) = (uint64_t)destination;
    }
}

TEST_CASE(TrampolineHooking, OffsetCalculation) {
    void* base = (void*)0x140000000;
    void* target = TrampolineHelper::CalculateOffset(base, 0x50640);
    ASSERT_EQ((uintptr_t)target, (uintptr_t)0x140050640);

    void* zero_base = (void*)0x0;
    void* offset_val = TrampolineHelper::CalculateOffset(zero_base, 0x1234);
    ASSERT_EQ((uintptr_t)offset_val, (uintptr_t)0x1234);
}

TEST_CASE(TrampolineHooking, FarJmpOpcodeLayout) {
    char buffer[14];
    std::memset(buffer, 0xCC, sizeof(buffer));

    void* target_dest = (void*)0x00007FF7BADC0FFE;
    TrampolineHelper::GenerateFarJmpBytes(buffer, target_dest);

    // Byte 0: 0xFF, Byte 1: 0x25 (FF 25 = jmp qword ptr [rip+offset])
    ASSERT_EQ((uint8_t)buffer[0], (uint8_t)0xFF);
    ASSERT_EQ((uint8_t)buffer[1], (uint8_t)0x25);

    // Bytes 2..5: 0x00 0x00 0x00 0x00
    uint32_t disp = *((uint32_t*)&buffer[2]);
    ASSERT_EQ(disp, 0x00000000u);

    // Bytes 6..13: 64-bit pointer
    uint64_t read_dest = *((uint64_t*)&buffer[6]);
    ASSERT_EQ(read_dest, (uint64_t)0x00007FF7BADC0FFE);
}

TEST_CASE(TrampolineHooking, VirtualMemoryProtectionPatchSimulation) {
    // Allocate 14 executable bytes in memory
    LPVOID mem = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    ASSERT_TRUE(mem != NULL);

    void* dummy_dest = (void*)0x140001000;
    char* byte_ptr = (char*)mem;

    DWORD oldProtect;
    VirtualProtect(mem, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    TrampolineHelper::GenerateFarJmpBytes(byte_ptr, dummy_dest);
    VirtualProtect(mem, 14, oldProtect, &oldProtect);

    ASSERT_EQ(*((uint16_t*)&byte_ptr[0]), (uint16_t)0x25FF);
    ASSERT_EQ(*((uint64_t*)&byte_ptr[6]), (uint64_t)dummy_dest);

    VirtualFree(mem, 0, MEM_RELEASE);
}
