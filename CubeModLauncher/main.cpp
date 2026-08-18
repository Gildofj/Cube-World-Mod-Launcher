#include "main.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>
#include <windows.h>
#include "Process.h"
#include "Logger.h"

namespace fs = std::filesystem;

constexpr std::string_view DEFAULT_CUBE_EXECUTABLE = "cubeworld.exe";
constexpr std::string_view MODLOADER_DLL = "CubeModLoader.dll";

bool FileExists(std::string_view fileName) {
    std::error_code ec;
    fs::path p(fileName);
    return fs::exists(p, ec) && fs::is_regular_file(p, ec);
}

int Bail(int result) {
    std::cout << "\nPress Enter to exit.\n";
    std::cin.get();
    return result;
}

int main(int argc, char** argv) {
    cw::Logger::Instance().Init("cube-world-logs", "launcher.log", true);

    std::string cubeExecutable(DEFAULT_CUBE_EXECUTABLE);
    if (argc >= 2) {
        cubeExecutable = argv[1];
    }

    CW_LOG_INFO("========================================");
    CW_LOG_INFO("  Cube World Mod Launcher (Modernized)  ");
    CW_LOG_INFO("========================================");

    // Validate target executable exists
    if (!FileExists(cubeExecutable)) {
        CW_LOG_ERROR("Target executable not found: %s", cubeExecutable.c_str());
        return Bail(1);
    }

    // Validate mod loader DLL exists
    if (!FileExists(MODLOADER_DLL)) {
        CW_LOG_ERROR("ModLoader DLL not found: %s", std::string(MODLOADER_DLL).c_str());
        return Bail(1);
    }

    Process process(cubeExecutable);

    CW_LOG_INFO("Starting %s in suspended mode...", cubeExecutable.c_str());
    if (!process.Create()) {
        CW_LOG_ERROR("Failed to create process. System error code: %lu", GetLastError());
        return Bail(1);
    }
    CW_LOG_INFO("Process created successfully (PID: %lu).", process.GetProcessId());

    CW_LOG_INFO("Injecting %s into remote process...", std::string(MODLOADER_DLL).c_str());
    if (!process.InjectDLL(MODLOADER_DLL)) {
        CW_LOG_ERROR("Injection failed.");
        return Bail(1);
    }
    CW_LOG_INFO("Injection completed successfully.");

    // Resume main game thread
    CW_LOG_INFO("Resuming target process execution...");
    process.Run();
    CW_LOG_INFO("Cube World is now running.");

    cw::Logger::Instance().Shutdown();
    return 0;
}

