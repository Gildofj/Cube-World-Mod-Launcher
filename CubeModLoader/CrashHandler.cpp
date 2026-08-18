#include "CrashHandler.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <vector>
#include <psapi.h>
#include <dbghelp.h>
#include <tlhelp32.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

namespace fs = std::filesystem;

namespace cw {

std::string CrashHandler::s_logsDir = "cube-world-logs";
LPTOP_LEVEL_EXCEPTION_FILTER CrashHandler::s_previousFilter = nullptr;
PVOID CrashHandler::s_vectoredHandle = nullptr;
bool CrashHandler::s_isHandlingCrash = false;

std::string CrashHandler::GetCrashTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t timer = system_clock::to_time_t(now);
    std::tm bt{};
    localtime_s(&bt, &timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

const char* CrashHandler::GetExceptionCodeDescription(DWORD code) {
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION (0xC0000005)";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED (0xC000008C)";
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT (0x80000003)";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT (0x80000002)";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND (0xC000008D)";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO (0xC000008E)";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT (0xC000008F)";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION (0xC0000090)";
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW (0xC0000091)";
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK (0xC0000092)";
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW (0xC0000093)";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION (0xC000001D)";
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR (0xC0000006)";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO (0xC0000094)";
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW (0xC0000095)";
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION (0xC0000026)";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION (0xC0000025)";
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION (0xC0000096)";
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP (0x80000004)";
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW (0xC00000FD)";
        case 0xE06D7363:                         return "CPP_EXCEPTION (0xE06D7363)";
        default:                                 return "UNKNOWN_EXCEPTION";
    }
}

std::string CrashHandler::FormatRegisters(const CONTEXT* ctx) {
    if (!ctx) {
        return "<No context available>\n";
    }

    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');

#if defined(_M_X64) || defined(__x86_64__)
    oss << "RAX: 0x" << std::setw(16) << ctx->Rax << "  RBX: 0x" << std::setw(16) << ctx->Rbx << "\n";
    oss << "RCX: 0x" << std::setw(16) << ctx->Rcx << "  RDX: 0x" << std::setw(16) << ctx->Rdx << "\n";
    oss << "RSI: 0x" << std::setw(16) << ctx->Rsi << "  RDI: 0x" << std::setw(16) << ctx->Rdi << "\n";
    oss << "RBP: 0x" << std::setw(16) << ctx->Rbp << "  RSP: 0x" << std::setw(16) << ctx->Rsp << "\n";
    oss << "R8:  0x" << std::setw(16) << ctx->R8  << "  R9:  0x" << std::setw(16) << ctx->R9  << "\n";
    oss << "R10: 0x" << std::setw(16) << ctx->R10 << "  R11: 0x" << std::setw(16) << ctx->R11 << "\n";
    oss << "R12: 0x" << std::setw(16) << ctx->R12 << "  R13: 0x" << std::setw(16) << ctx->R13 << "\n";
    oss << "R14: 0x" << std::setw(16) << ctx->R14 << "  R15: 0x" << std::setw(16) << ctx->R15 << "\n";
    oss << "RIP: 0x" << std::setw(16) << ctx->Rip << "  EFLAGS: 0x" << std::setw(8) << ctx->EFlags << "\n";
#else
    oss << "EAX: 0x" << std::setw(8) << ctx->Eax << "  EBX: 0x" << std::setw(8) << ctx->Ebx << "\n";
    oss << "ECX: 0x" << std::setw(8) << ctx->Ecx << "  EDX: 0x" << std::setw(8) << ctx->Edx << "\n";
    oss << "ESI: 0x" << std::setw(8) << ctx->Esi << "  EDI: 0x" << std::setw(8) << ctx->Edi << "\n";
    oss << "EBP: 0x" << std::setw(8) << ctx->Ebp << "  ESP: 0x" << std::setw(8) << ctx->Esp << "\n";
    oss << "EIP: 0x" << std::setw(8) << ctx->Eip << "  EFLAGS: 0x" << std::setw(8) << ctx->EFlags << "\n";
#endif

    return oss.str();
}

std::string CrashHandler::FormatStackTrace(const CONTEXT* ctx, HANDLE hProcess, HANDLE hThread) {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');

    SymInitialize(hProcess, NULL, TRUE);

    CONTEXT ctxCopy;
    if (ctx) {
        ctxCopy = *ctx;
    } else {
        RtlCaptureContext(&ctxCopy);
    }

    STACKFRAME64 stackFrame{};
    DWORD machineType;

#if defined(_M_X64) || defined(__x86_64__)
    machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = ctxCopy.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = ctxCopy.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = ctxCopy.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
    machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = ctxCopy.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = ctxCopy.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = ctxCopy.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#endif

    int frameIndex = 0;
    while (StackWalk64(machineType, hProcess, hThread, &stackFrame, &ctxCopy,
                       NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
        if (stackFrame.AddrPC.Offset == 0) {
            break;
        }

        DWORD64 address = stackFrame.AddrPC.Offset;
        char moduleName[MAX_PATH] = "UnknownModule";
        DWORD64 moduleBase = SymGetModuleBase64(hProcess, address);
        if (moduleBase != 0) {
            GetModuleFileNameA((HMODULE)moduleBase, moduleName, sizeof(moduleName));
            // Keep only basename
            std::string modStr(moduleName);
            auto lastSlash = modStr.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                modStr = modStr.substr(lastSlash + 1);
            }
            strncpy_s(moduleName, modStr.c_str(), sizeof(moduleName) - 1);
        }

        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{};
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD64 displacement = 0;
        std::string symbolName;
        if (SymFromAddr(hProcess, address, &displacement, pSymbol)) {
            symbolName = pSymbol->Name;
        } else {
            symbolName = "<unknown symbol>";
        }

        IMAGEHLP_LINE64 lineInfo{};
        lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD lineDisplacement = 0;
        std::string sourceInfo;
        if (SymGetLineFromAddr64(hProcess, address, &lineDisplacement, &lineInfo)) {
            std::ostringstream lineOss;
            lineOss << " (" << lineInfo.FileName << ":" << std::dec << lineInfo.LineNumber << ")";
            sourceInfo = lineOss.str();
        }

        oss << "  #" << std::setw(2) << std::dec << frameIndex << "  0x" 
            << std::hex << std::setw(16) << address << " in " << moduleName;

        if (moduleBase != 0) {
            oss << "+0x" << std::hex << (address - moduleBase);
        }

        oss << " [" << symbolName << "]" << sourceInfo << "\n";

        frameIndex++;
        if (frameIndex >= 64) {
            oss << "  ... [truncated after 64 frames]\n";
            break;
        }
    }

    SymCleanup(hProcess);
    return oss.str();
}

std::string CrashHandler::WriteCrashReport(EXCEPTION_POINTERS* pExceptionPointers) {
    std::error_code ec;
    fs::path dirPath(s_logsDir);
    if (!fs::exists(dirPath, ec)) {
        fs::create_directories(dirPath, ec);
    }

    std::string timestamp = GetCrashTimestamp();
    std::string logFileName = "crash_" + timestamp + ".log";
    std::string dmpFileName = "crash_" + timestamp + ".dmp";
    fs::path logFilePath = dirPath / logFileName;
    fs::path dmpFilePath = dirPath / dmpFileName;

    // 1. Write MiniDump
    if (pExceptionPointers) {
        HANDLE hDumpFile = CreateFileA(dmpFilePath.string().c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                                       NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hDumpFile != INVALID_HANDLE_VALUE) {
            MINIDUMP_EXCEPTION_INFORMATION mei{};
            mei.ThreadId = GetCurrentThreadId();
            mei.ExceptionPointers = pExceptionPointers;
            mei.ClientPointers = FALSE;

            MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
                MiniDumpNormal |
                MiniDumpWithDataSegs |
                MiniDumpWithIndirectlyReferencedMemory |
                MiniDumpWithProcessThreadData |
                MiniDumpWithThreadInfo
            );

            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, dumpType, &mei, NULL, NULL);
            CloseHandle(hDumpFile);
        }
    }

    // 2. Write Text Crash Log
    std::ofstream file(logFilePath.string(), std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file << "======================================================================\n";
        file << "                     CUBE WORLD CRASH REPORT                          \n";
        file << "======================================================================\n\n";

        file << "Timestamp:    " << timestamp << "\n";
        file << "Process ID:   " << GetCurrentProcessId() << "\n";
        file << "Thread ID:    " << GetCurrentThreadId() << "\n";

        if (pExceptionPointers && pExceptionPointers->ExceptionRecord) {
            auto* er = pExceptionPointers->ExceptionRecord;
            DWORD code = er->ExceptionCode;
            file << "Exception:    0x" << std::hex << std::uppercase << code 
                 << " - " << GetExceptionCodeDescription(code) << "\n";
            file << "Fault Address: 0x" << std::hex << std::uppercase << (uintptr_t)er->ExceptionAddress << "\n";

            if (code == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2) {
                ULONG_PTR opType = er->ExceptionInformation[0];
                ULONG_PTR targetAddr = er->ExceptionInformation[1];
                file << "Access Type:  " << (opType == 0 ? "Read" : (opType == 1 ? "Write" : "Execute"))
                     << " attempt at memory address 0x" << std::hex << targetAddr << "\n";
            }
        }

        file << "\n----------------------------------------------------------------------\n";
        file << " CPU REGISTERS (x64)\n";
        file << "----------------------------------------------------------------------\n";
        if (pExceptionPointers && pExceptionPointers->ContextRecord) {
            file << FormatRegisters(pExceptionPointers->ContextRecord);
        } else {
            file << "<Context Record not provided>\n";
        }

        file << "\n----------------------------------------------------------------------\n";
        file << " CALL STACK BACKTRACE\n";
        file << "----------------------------------------------------------------------\n";
        if (pExceptionPointers && pExceptionPointers->ContextRecord) {
            file << FormatStackTrace(pExceptionPointers->ContextRecord);
        } else {
            file << "<Call stack unavailable>\n";
        }

        file << "\n----------------------------------------------------------------------\n";
        file << " LOADED MODULES SNAPSHOT\n";
        file << "----------------------------------------------------------------------\n";
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            MODULEENTRY32 me{};
            me.dwSize = sizeof(MODULEENTRY32);
            if (Module32First(hSnapshot, &me)) {
                do {
                    file << "  0x" << std::hex << std::uppercase << (uintptr_t)me.modBaseAddr
                         << " - 0x" << std::hex << (uintptr_t)(me.modBaseAddr + me.modBaseSize)
                         << "  " << me.szModule << " (" << me.szExePath << ")\n";
                } while (Module32Next(hSnapshot, &me));
            }
            CloseHandle(hSnapshot);
        }

        file << "\n======================================================================\n";
        file << " MiniDump saved to: " << dmpFilePath.string() << "\n";
        file << "======================================================================\n";
        file.flush();
        file.close();
    }

    return logFilePath.string();
}

LONG WINAPI CrashHandler::UnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionPointers) {
    if (s_isHandlingCrash) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    s_isHandlingCrash = true;

    // Log critical message in modloader.log
    CW_LOG_CRITICAL("CRITICAL UNHANDLED EXCEPTION INTERCEPTED! Generating crash report...");

    std::string reportPath = WriteCrashReport(pExceptionPointers);

    // Flush any ongoing log
    cw::Logger::Instance().Flush();

    // Display MessageBox alert
    std::string message = "Cube World encountered an unhandled exception and must close.\n\n"
                          "A detailed crash report and dump have been created:\n" +
                          reportPath + "\n\nPlease check the " + s_logsDir + "/ folder.";

    MessageBoxA(NULL, message.c_str(), "Cube World - Crash Reporter", MB_OK | MB_ICONERROR | MB_TOPMOST);

    if (s_previousFilter) {
        return s_previousFilter(pExceptionPointers);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI CrashHandler::VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionPointers) {
    if (!pExceptionPointers || !pExceptionPointers->ExceptionRecord) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    DWORD code = pExceptionPointers->ExceptionRecord->ExceptionCode;

    // Filter only fatal hardware/CRT exceptions
    if (code == EXCEPTION_ACCESS_VIOLATION ||
        code == EXCEPTION_ILLEGAL_INSTRUCTION ||
        code == EXCEPTION_INT_DIVIDE_BY_ZERO ||
        code == EXCEPTION_STACK_OVERFLOW ||
        code == EXCEPTION_PRIV_INSTRUCTION ||
        code == EXCEPTION_IN_PAGE_ERROR ||
        code == 0xE0000001 ||
        code == 0xE0000002) {

        if (s_isHandlingCrash) {
            return EXCEPTION_CONTINUE_SEARCH;
        }
        s_isHandlingCrash = true;

        CW_LOG_CRITICAL("CRITICAL HARDWARE EXCEPTION INTERCEPTED! Code: 0x%08X (%s)", 
                        code, GetExceptionCodeDescription(code));

        std::string reportPath = WriteCrashReport(pExceptionPointers);
        cw::Logger::Instance().Flush();

        std::string message = "Cube World encountered a critical exception and must close.\n\n"
                              "A detailed crash report has been generated at:\n" +
                              reportPath + "\n\nPlease check the " + s_logsDir + "/ folder.";

        MessageBoxA(NULL, message.c_str(), "Cube World - Crash Reporter", MB_OK | MB_ICONERROR | MB_TOPMOST);

        ExitProcess(code);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void CrashHandler::InvalidParameterHandler(const wchar_t* expression, const wchar_t* function,
                                          const wchar_t* file, unsigned int line, uintptr_t pReserved) {
    CW_LOG_CRITICAL("CRT Invalid Parameter detected in %ls (%ls:%u)", 
                    function ? function : L"unknown", 
                    file ? file : L"unknown", line);
    RaiseException(0xE0000001, EXCEPTION_NONCONTINUABLE, 0, NULL);
}

void CrashHandler::PureCallHandler() {
    CW_LOG_CRITICAL("CRT Pure Virtual Call detected!");
    RaiseException(0xE0000002, EXCEPTION_NONCONTINUABLE, 0, NULL);
}

bool CrashHandler::Install(const std::string& logsDir) {
    s_logsDir = logsDir;

    // Ensure logs directory exists
    std::error_code ec;
    fs::path dir(s_logsDir);
    if (!fs::exists(dir, ec)) {
        fs::create_directories(dir, ec);
    }

    s_previousFilter = SetUnhandledExceptionFilter(&CrashHandler::UnhandledExceptionFilter);
    s_vectoredHandle = AddVectoredExceptionHandler(1, &CrashHandler::VectoredExceptionHandler);
    _set_invalid_parameter_handler(&CrashHandler::InvalidParameterHandler);
    _set_purecall_handler(&CrashHandler::PureCallHandler);

    CW_LOG_INFO("CrashHandler initialized (SEH + VEH). Crash logs target: %s/", s_logsDir.c_str());
    return true;
}

void CrashHandler::Uninstall() {
    if (s_vectoredHandle) {
        RemoveVectoredExceptionHandler(s_vectoredHandle);
        s_vectoredHandle = nullptr;
    }
    if (s_previousFilter) {
        SetUnhandledExceptionFilter(s_previousFilter);
        s_previousFilter = nullptr;
    }
}

} // namespace cw
