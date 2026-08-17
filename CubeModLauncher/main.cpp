#include "main.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>
#include <windows.h>
#include "Process.h"

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
    std::string cubeExecutable(DEFAULT_CUBE_EXECUTABLE);
    if (argc >= 2) {
        cubeExecutable = argv[1];
    }

    std::cout << "========================================\n";
    std::cout << "  Cube World Mod Launcher (Modernized)  \n";
    std::cout << "========================================\n\n";

    // Validate target executable exists
    if (!FileExists(cubeExecutable)) {
        std::cerr << "[Error] Target executable not found: " << cubeExecutable << "\n";
        return Bail(1);
    }

    // Validate mod loader DLL exists
    if (!FileExists(MODLOADER_DLL)) {
        std::cerr << "[Error] ModLoader DLL not found: " << MODLOADER_DLL << "\n";
        return Bail(1);
    }

    Process process(cubeExecutable);

    std::cout << "[*] Starting " << cubeExecutable << " in suspended mode...\n";
    if (!process.Create()) {
        std::cerr << "[Error] Failed to create process. System error code: " << GetLastError() << "\n";
        return Bail(1);
    }
    std::cout << "[+] Process created successfully (PID: " << process.GetProcessId() << ").\n\n";

    std::cout << "[*] Injecting " << MODLOADER_DLL << " into remote process...\n";
    if (!process.InjectDLL(MODLOADER_DLL)) {
        std::cerr << "[Error] Injection failed.\n";
        return Bail(1);
    }
    std::cout << "[+] Injection completed successfully.\n\n";

    // Resume main game thread
    std::cout << "[*] Resuming target process execution...\n";
    process.Run();
    std::cout << "[+] Cube World is now running.\n";

    return 0;
}
