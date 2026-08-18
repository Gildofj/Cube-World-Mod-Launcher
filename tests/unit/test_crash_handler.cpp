#include "../framework/test_framework.h"
#include "../mocks/MockEnvironment.h"
#include "../../CubeModLoader/CrashHandler.h"
#include "../../CubeModLoader/Logger.h"
#include <fstream>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

TEST_CASE(CrashReporter, ExceptionCodeDescriptions) {
    ASSERT_TRUE(std::string(cw::CrashHandler::GetExceptionCodeDescription(EXCEPTION_ACCESS_VIOLATION)).find("EXCEPTION_ACCESS_VIOLATION") != std::string::npos);
    ASSERT_TRUE(std::string(cw::CrashHandler::GetExceptionCodeDescription(EXCEPTION_INT_DIVIDE_BY_ZERO)).find("EXCEPTION_INT_DIVIDE_BY_ZERO") != std::string::npos);
    ASSERT_TRUE(std::string(cw::CrashHandler::GetExceptionCodeDescription(EXCEPTION_ILLEGAL_INSTRUCTION)).find("EXCEPTION_ILLEGAL_INSTRUCTION") != std::string::npos);
    ASSERT_TRUE(std::string(cw::CrashHandler::GetExceptionCodeDescription(EXCEPTION_STACK_OVERFLOW)).find("EXCEPTION_STACK_OVERFLOW") != std::string::npos);
    ASSERT_TRUE(std::string(cw::CrashHandler::GetExceptionCodeDescription(0xE06D7363)).find("CPP_EXCEPTION") != std::string::npos);
    ASSERT_STREQ(cw::CrashHandler::GetExceptionCodeDescription(0x12345678), "UNKNOWN_EXCEPTION");
}

TEST_CASE(CrashReporter, FormatRegistersNullSafe) {
    std::string nullRegs = cw::CrashHandler::FormatRegisters(nullptr);
    ASSERT_TRUE(nullRegs.find("<No context available>") != std::string::npos);
}

TEST_CASE(CrashReporter, FormatRegistersValidContext) {
    CONTEXT ctx{};
#if defined(_M_X64) || defined(__x86_64__)
    ctx.Rax = 0x1122334455667788ULL;
    ctx.Rbx = 0xAABBCCDDEEFF0011ULL;
    ctx.Rip = 0x00007FF712345678ULL;
    ctx.EFlags = 0x00000246;
#else
    ctx.Eax = 0x11223344;
    ctx.Ebx = 0xAABBCCDD;
    ctx.Eip = 0x12345678;
    ctx.EFlags = 0x00000246;
#endif

    std::string formatted = cw::CrashHandler::FormatRegisters(&ctx);
#if defined(_M_X64) || defined(__x86_64__)
    ASSERT_TRUE(formatted.find("RAX: 0x1122334455667788") != std::string::npos);
    ASSERT_TRUE(formatted.find("RBX: 0xAABBCCDDEEFF0011") != std::string::npos);
    ASSERT_TRUE(formatted.find("RIP: 0x00007FF712345678") != std::string::npos);
#else
    ASSERT_TRUE(formatted.find("EAX: 0x11223344") != std::string::npos);
    ASSERT_TRUE(formatted.find("EBX: 0xAABBCCDD") != std::string::npos);
    ASSERT_TRUE(formatted.find("EIP: 0x12345678") != std::string::npos);
#endif
}

TEST_CASE(CrashReporter, TimestampFormatValidation) {
    std::string ts = cw::CrashHandler::GetCrashTimestamp();
    // Expected format: YYYY-MM-DD_HH-MM-SS (length 19)
    ASSERT_EQ(ts.length(), (size_t)19);
    ASSERT_EQ(ts[4], '-');
    ASSERT_EQ(ts[7], '-');
    ASSERT_EQ(ts[10], '_');
    ASSERT_EQ(ts[13], '-');
    ASSERT_EQ(ts[16], '-');
}

TEST_CASE(CrashReporter, WriteCrashReportSyntheticException) {
    Mocking::TempDirectoryScope temp_dir;
    std::string logsDir = (temp_dir.path / "cube-world-logs-crash").string();

    cw::CrashHandler::Install(logsDir);

    EXCEPTION_RECORD er{};
    er.ExceptionCode = EXCEPTION_ACCESS_VIOLATION;
    er.ExceptionAddress = (PVOID)0x00007FF7DEADBEEFULL;
    er.NumberParameters = 2;
    er.ExceptionInformation[0] = 1; // Write violation
    er.ExceptionInformation[1] = 0x0000000000000042ULL; // Target memory

    CONTEXT ctx{};
#if defined(_M_X64) || defined(__x86_64__)
    ctx.Rip = (DWORD64)er.ExceptionAddress;
    ctx.Rax = 0x42;
#else
    ctx.Eip = (DWORD)er.ExceptionAddress;
    ctx.Eax = 0x42;
#endif

    EXCEPTION_POINTERS ep{};
    ep.ExceptionRecord = &er;
    ep.ContextRecord = &ctx;

    std::string logFilePath = cw::CrashHandler::WriteCrashReport(&ep);
    ASSERT_TRUE(fs::exists(logFilePath));

    // Verify log file content
    std::ifstream file(logFilePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    ASSERT_TRUE(content.find("CUBE WORLD CRASH REPORT") != std::string::npos);
    ASSERT_TRUE(content.find("EXCEPTION_ACCESS_VIOLATION (0xC0000005)") != std::string::npos);
    ASSERT_TRUE(content.find("Write attempt at memory address 0x42") != std::string::npos ||
                content.find("0x0000000000000042") != std::string::npos ||
                content.find("0x42") != std::string::npos);
    ASSERT_TRUE(content.find("CPU REGISTERS") != std::string::npos);
    ASSERT_TRUE(content.find("LOADED MODULES SNAPSHOT") != std::string::npos);

    cw::CrashHandler::Uninstall();
}
