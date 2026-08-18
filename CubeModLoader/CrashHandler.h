#pragma once

#include <string>
#include <windows.h>

namespace cw {

class CrashHandler {
public:
    // Install the global unhandled exception filter and CRT handlers
    static bool Install(const std::string& logsDir = "cube-world-logs");

    // Uninstall handlers
    static void Uninstall();

    // Generate crash dump and log from an EXCEPTION_POINTERS structure
    static std::string WriteCrashReport(EXCEPTION_POINTERS* pExceptionPointers);

    // Get human-readable description of an exception code
    static const char* GetExceptionCodeDescription(DWORD code);

    // Format x64 CPU registers to string
    static std::string FormatRegisters(const CONTEXT* ctx);

    // Format stack trace to string
    static std::string FormatStackTrace(const CONTEXT* ctx, HANDLE hProcess = GetCurrentProcess(), HANDLE hThread = GetCurrentThread());

    // Generate crash timestamp string: "YYYY-MM-DD_HH-MM-SS"
    static std::string GetCrashTimestamp();

private:
    static LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionPointers);
    static LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionPointers);
    static void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);
    static void PureCallHandler();

    static std::string s_logsDir;
    static LPTOP_LEVEL_EXCEPTION_FILTER s_previousFilter;
    static PVOID s_vectoredHandle;
    static bool s_isHandlingCrash;
};

} // namespace cw
