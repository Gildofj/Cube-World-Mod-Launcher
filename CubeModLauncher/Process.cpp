#include "Process.h"
#include <iostream>
#include <vector>

Process::Process(std::string path)
    : path_(std::move(path))
{
    ZeroMemory(&si_, sizeof(si_));
    si_.cb = sizeof(si_);
    ZeroMemory(&pi_, sizeof(pi_));
}

Process::~Process()
{
    if (pi_.hThread != nullptr) {
        CloseHandle(pi_.hThread);
        pi_.hThread = nullptr;
    }
    if (pi_.hProcess != nullptr) {
        CloseHandle(pi_.hProcess);
        pi_.hProcess = nullptr;
    }
}

bool Process::Create()
{
    if (created_) {
        return true;
    }

    std::vector<char> cmdLine(path_.begin(), path_.end());
    cmdLine.push_back('\0');

    BOOL success = CreateProcessA(
        nullptr,
        cmdLine.data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_SUSPENDED,
        nullptr,
        nullptr,
        &si_,
        &pi_
    );

    if (!success) {
        return false;
    }

    created_ = true;
    return true;
}

bool Process::InjectDLL(std::string_view dllName)
{
    if (!created_ || pi_.hProcess == nullptr) {
        std::cerr << "[Launcher Error] Cannot inject DLL: Target process is not created.\n";
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        std::cerr << "[Launcher Error] Failed to get kernel32.dll handle. Code: " << GetLastError() << "\n";
        return false;
    }

    auto pfnLoadLibraryA = reinterpret_cast<LPTHREAD_START_ROUTINE>(
        GetProcAddress(hKernel32, "LoadLibraryA")
    );
    if (!pfnLoadLibraryA) {
        std::cerr << "[Launcher Error] Failed to resolve LoadLibraryA address.\n";
        return false;
    }

    const size_t stringBytes = dllName.size() + 1;
    LPVOID remoteString = VirtualAllocEx(
        pi_.hProcess,
        nullptr,
        stringBytes,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (!remoteString) {
        std::cerr << "[Launcher Error] VirtualAllocEx failed in target process. Code: " << GetLastError() << "\n";
        return false;
    }

    SIZE_T bytesWritten = 0;
    BOOL writeSuccess = WriteProcessMemory(
        pi_.hProcess,
        remoteString,
        dllName.data(),
        stringBytes,
        &bytesWritten
    );

    if (!writeSuccess || bytesWritten != stringBytes) {
        std::cerr << "[Launcher Error] WriteProcessMemory failed. Code: " << GetLastError() << "\n";
        VirtualFreeEx(pi_.hProcess, remoteString, 0, MEM_RELEASE);
        return false;
    }

    ScopedHandle hRemoteThread(CreateRemoteThread(
        pi_.hProcess,
        nullptr,
        0,
        pfnLoadLibraryA,
        remoteString,
        0, // Execute immediately
        nullptr
    ));

    if (!hRemoteThread.isValid()) {
        std::cerr << "[Launcher Error] CreateRemoteThread failed. Code: " << GetLastError() << "\n";
        VirtualFreeEx(pi_.hProcess, remoteString, 0, MEM_RELEASE);
        return false;
    }

    // Deterministic synchronization: Wait for remote LoadLibraryA to finish
    DWORD waitResult = WaitForSingleObject(hRemoteThread.get(), 10000);
    if (waitResult == WAIT_TIMEOUT) {
        std::cerr << "[Launcher Warning] Remote thread execution timed out.\n";
    }

    DWORD exitCode = 0;
    if (GetExitCodeThread(hRemoteThread.get(), &exitCode)) {
        if (exitCode == 0) {
            std::cerr << "[Launcher Warning] Remote LoadLibraryA returned NULL handle.\n";
        }
    }

    // Clean up allocated path buffer in remote address space
    VirtualFreeEx(pi_.hProcess, remoteString, 0, MEM_RELEASE);

    return true;
}

void Process::Run()
{
    if (created_ && !resumed_ && pi_.hThread != nullptr) {
        ResumeThread(pi_.hThread);
        resumed_ = true;
    }
}

bool Process::IsCreated() const noexcept
{
    return created_;
}

DWORD Process::GetProcessId() const noexcept
{
    return pi_.dwProcessId;
}
