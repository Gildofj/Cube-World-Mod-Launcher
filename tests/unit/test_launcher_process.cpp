#include "../framework/test_framework.h"
#include "../mocks/MockEnvironment.h"
#include <windows.h>
#include <string>

namespace LauncherHelper {
    inline bool CheckFileExists(const char* fileName) {
        DWORD dwAttrib = GetFileAttributesA(fileName);
        return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    inline bool CheckDirectoryExists(const char* dirName) {
        DWORD dwAttrib = GetFileAttributesA(dirName);
        return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    struct LaunchArgs {
        std::string executable_name = "cubeworld.exe";
        std::string dll_name = "CubeModLoader.dll";
    };

    inline LaunchArgs ParseArgs(int argc, const char* const* argv) {
        LaunchArgs args;
        if (argc >= 2) {
            args.executable_name = argv[1];
        }
        if (argc >= 3) {
            args.dll_name = argv[2];
        }
        return args;
    }
}

TEST_CASE(LauncherUtilities, FileExistsOnRegularFile) {
    Mocking::TempDirectoryScope temp_dir;
    auto test_file = temp_dir.create_file("cubeworld.exe", "fake_exe");

    ASSERT_TRUE(LauncherHelper::CheckFileExists(test_file.string().c_str()));
}

TEST_CASE(LauncherUtilities, FileExistsReturnsFalseForDirectories) {
    Mocking::TempDirectoryScope temp_dir;
    ASSERT_FALSE(LauncherHelper::CheckFileExists(temp_dir.path.string().c_str()));
    ASSERT_TRUE(LauncherHelper::CheckDirectoryExists(temp_dir.path.string().c_str()));
}

TEST_CASE(LauncherUtilities, FileExistsReturnsFalseForNonExistent) {
    Mocking::TempDirectoryScope temp_dir;
    auto non_existent = temp_dir.path / "missing_file.exe";
    ASSERT_FALSE(LauncherHelper::CheckFileExists(non_existent.string().c_str()));
}

TEST_CASE(LauncherUtilities, ArgParsingDefaults) {
    const char* argv[] = {"CubeModLauncher.exe"};
    auto args = LauncherHelper::ParseArgs(1, argv);
    ASSERT_STREQ(args.executable_name.c_str(), "cubeworld.exe");
    ASSERT_STREQ(args.dll_name.c_str(), "CubeModLoader.dll");
}

TEST_CASE(LauncherUtilities, ArgParsingCustomExecutable) {
    const char* argv[] = {"CubeModLauncher.exe", "custom_game.exe", "custom_loader.dll"};
    auto args = LauncherHelper::ParseArgs(3, argv);
    ASSERT_STREQ(args.executable_name.c_str(), "custom_game.exe");
    ASSERT_STREQ(args.dll_name.c_str(), "custom_loader.dll");
}
