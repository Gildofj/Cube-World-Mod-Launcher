#ifndef MOCK_ENVIRONMENT_H
#define MOCK_ENVIRONMENT_H

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <cstdio>
#include <windows.h>

namespace Mocking {

    class TempDirectoryScope {
    public:
        std::filesystem::path path;

        TempDirectoryScope(const std::string& prefix = "cw_test_") {
            auto temp_base = std::filesystem::temp_directory_path();
            path = temp_base / (prefix + std::to_string(GetTickCount64()) + "_" + std::to_string(rand()));
            std::filesystem::create_directories(path);
        }

        ~TempDirectoryScope() {
            try {
                if (std::filesystem::exists(path)) {
                    std::filesystem::remove_all(path);
                }
            } catch (...) {}
        }

        std::filesystem::path create_file(const std::string& relative_path, const std::string& content) {
            auto full_path = path / relative_path;
            if (full_path.has_parent_path()) {
                std::filesystem::create_directories(full_path.parent_path());
            }
            std::ofstream out(full_path.string(), std::ios::binary);
            out.write(content.data(), content.size());
            out.close();
            return full_path;
        }

        std::string read_file(const std::string& relative_path) {
            auto full_path = path / relative_path;
            std::ifstream in(full_path.string(), std::ios::binary);
            std::string content((std::istreambuf_iterator<char>(in)),
                                std::istreambuf_iterator<char>());
            return content;
        }
    };

} // namespace Mocking

#endif // MOCK_ENVIRONMENT_H
