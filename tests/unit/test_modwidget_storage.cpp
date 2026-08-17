#include "../framework/test_framework.h"
#include "../mocks/MockEnvironment.h"
#include <vector>
#include <string>
#include <fstream>

namespace ModWidgetStorage {

    struct FakeDLLItem {
        std::string fileName;
        bool enabled;

        FakeDLLItem(std::string name, bool en = true) : fileName(std::move(name)), enabled(en) {}
    };

    void LoadSaveFromFile(const std::string& file_path, std::vector<FakeDLLItem*>* mods) {
        std::ifstream in(file_path.c_str());
        if (!in.is_open()) return;

        std::string line;
        while (getline(in, line)) {
            auto pos = line.find(":");
            if (pos != std::string::npos) {
                std::string name(line.substr(0, pos));
                std::string val_str = line.substr(pos + 1);
                if (val_str.empty()) continue;
                try {
                    bool enabled = (std::stoi(val_str) != 0);
                    for (FakeDLLItem* dll : *mods) {
                        if (dll->fileName == name) {
                            dll->enabled = enabled;
                        }
                    }
                } catch (...) {
                    // Ignore corrupted integer parse
                }
            }
        }
    }

    void StoreSaveToFile(const std::string& file_path, const std::vector<FakeDLLItem*>& mods) {
        std::ofstream out(file_path.c_str());
        for (const FakeDLLItem* dll : mods) {
            out << dll->fileName << ":" << (dll->enabled ? 1 : 0) << "\n";
        }
        out.close();
    }
}

TEST_CASE(ModWidgetStorage, RoundTripSaveAndLoad) {
    Mocking::TempDirectoryScope temp_dir;
    auto save_path = temp_dir.path / "mods-settings.cwb";

    ModWidgetStorage::FakeDLLItem mod1("Mods/BetterInventory.dll", true);
    ModWidgetStorage::FakeDLLItem mod2("Mods/ChatCommands.dll", false);
    ModWidgetStorage::FakeDLLItem mod3("Mods/CameraMod.dll", true);

    std::vector<ModWidgetStorage::FakeDLLItem*> mods = { &mod1, &mod2, &mod3 };

    ModWidgetStorage::StoreSaveToFile(save_path.string(), mods);

    // Reset states
    mod1.enabled = false;
    mod2.enabled = true;
    mod3.enabled = false;

    // Load from disk
    ModWidgetStorage::LoadSaveFromFile(save_path.string(), &mods);

    ASSERT_TRUE(mod1.enabled);
    ASSERT_FALSE(mod2.enabled);
    ASSERT_TRUE(mod3.enabled);
}

TEST_CASE(ModWidgetStorage, NonExistentFileGracefulHandling) {
    Mocking::TempDirectoryScope temp_dir;
    auto non_existent = temp_dir.path / "does_not_exist.cwb";

    ModWidgetStorage::FakeDLLItem mod1("Mods/Test.dll", true);
    std::vector<ModWidgetStorage::FakeDLLItem*> mods = { &mod1 };

    ASSERT_NO_THROW({
        ModWidgetStorage::LoadSaveFromFile(non_existent.string(), &mods);
    });

    // Default value should remain unaltered
    ASSERT_TRUE(mod1.enabled);
}

TEST_CASE(ModWidgetStorage, CorruptedLinesResilience) {
    Mocking::TempDirectoryScope temp_dir;
    std::string malformed_data = 
        "Mods/Valid1.dll:1\n"
        "\n"
        "corrupted_line_without_colon\n"
        "Mods/InvalidNumber.dll:abc\n"
        "Mods/Valid2.dll:0\n"
        "::::\n"
        "Mods/Valid3.dll:1\n";

    auto save_path = temp_dir.create_file("mods-settings.cwb", malformed_data);

    ModWidgetStorage::FakeDLLItem mod1("Mods/Valid1.dll", false);
    ModWidgetStorage::FakeDLLItem mod2("Mods/Valid2.dll", true);
    ModWidgetStorage::FakeDLLItem mod3("Mods/Valid3.dll", false);
    ModWidgetStorage::FakeDLLItem mod_inv("Mods/InvalidNumber.dll", true);

    std::vector<ModWidgetStorage::FakeDLLItem*> mods = { &mod1, &mod2, &mod3, &mod_inv };

    ModWidgetStorage::LoadSaveFromFile(save_path.string(), &mods);

    ASSERT_TRUE(mod1.enabled);
    ASSERT_FALSE(mod2.enabled);
    ASSERT_TRUE(mod3.enabled);
    ASSERT_TRUE(mod_inv.enabled); // Unmodified due to bad parse
}

TEST_CASE(ModWidgetStorage, ExtraModsInConfigIgnored) {
    Mocking::TempDirectoryScope temp_dir;
    std::string data = 
        "Mods/OldRemovedMod.dll:0\n"
        "Mods/ActiveMod.dll:1\n";

    auto save_path = temp_dir.create_file("mods-settings.cwb", data);

    ModWidgetStorage::FakeDLLItem mod_active("Mods/ActiveMod.dll", false);
    std::vector<ModWidgetStorage::FakeDLLItem*> mods = { &mod_active };

    ModWidgetStorage::LoadSaveFromFile(save_path.string(), &mods);

    ASSERT_TRUE(mod_active.enabled);
}
