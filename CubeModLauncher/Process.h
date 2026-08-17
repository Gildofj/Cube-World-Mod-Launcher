#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <string_view>
#include <windows.h>
#include "ScopedHandle.h"

class Process
{
public:
    explicit Process(std::string path);
    virtual ~Process();

    // Disable copy
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    // Enable move
    Process(Process&&) noexcept = default;
    Process& operator=(Process&&) noexcept = default;

    [[nodiscard]] bool Create();
    [[nodiscard]] bool InjectDLL(std::string_view dllName);
    void Run();

    [[nodiscard]] bool IsCreated() const noexcept;
    [[nodiscard]] DWORD GetProcessId() const noexcept;

private:
    std::string path_;
    STARTUPINFOA si_{};
    PROCESS_INFORMATION pi_{};
    bool created_{false};
    bool resumed_{false};
};

#endif // PROCESS_H
