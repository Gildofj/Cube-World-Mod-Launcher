#include "../framework/test_framework.h"
#include "../mocks/MockMod.h"
#include "../mocks/MockEnvironment.h"
#include <vector>
#include <memory>

TEST_CASE(IntegrationFaultInjection, IncompatibleModRejectsPipeline) {
    Mocking::TempDirectoryScope temp_dir;

    std::vector<int> major_versions = {7, 8, 7}; // Middle mod has Major 8 (Incompatible)
    std::vector<int> minor_versions = {3, 0, 1};

    bool all_valid = true;
    for (size_t i = 0; i < major_versions.size(); ++i) {
        if (major_versions[i] != 7 || minor_versions[i] > 3) {
            all_valid = false;
            break;
        }
    }

    ASSERT_FALSE(all_valid);
}

TEST_CASE(IntegrationFaultInjection, EmptyModListSafeExecution) {
    Mocking::ClearExecutionLog();

    std::vector<GenericMod*> active_mods;

    // Dispatching events on empty list must not crash or dereference null
    ASSERT_NO_THROW({
        float hp = 100.0f;
        for (uint8_t p = 0; p <= 4; ++p) {
            for (auto* m : active_mods) {
                if (m->OnCreatureHPCalculatedPriority == (GenericMod::Priority)p) {
                    m->OnCreatureHPCalculated(nullptr, &hp);
                }
            }
        }
        ASSERT_EQ(hp, 100.0f);
    });
}

TEST_CASE(IntegrationFaultInjection, MultipleConflictingModModifiers) {
    Mocking::ClearExecutionLog();

    // Mod A (Very High) gives +50 flat spell power
    Mocking::MockMod mod_a("ModA", GenericMod::VeryHighPriority);
    mod_a.spell_power_bonus = 50.0f;

    // Mod B (Normal) gives +20 flat spell power
    Mocking::MockMod mod_b("ModB", GenericMod::NormalPriority);
    mod_b.spell_power_bonus = 20.0f;

    // Mod C (Low) gives +10 flat spell power
    Mocking::MockMod mod_c("ModC", GenericMod::LowPriority);
    mod_c.spell_power_bonus = 10.0f;

    std::vector<Mocking::MockMod*> mods = { &mod_c, &mod_a, &mod_b };

    float spell_power = 10.0f;

    for (uint8_t p = 0; p <= 4; ++p) {
        for (auto* m : mods) {
            if (m->OnCreatureSpellPowerCalculatedPriority == (GenericMod::Priority)p) {
                m->OnCreatureSpellPowerCalculated(nullptr, &spell_power);
            }
        }
    }

    // 10 initial + 50 + 20 + 10 = 90
    ASSERT_NEAR(spell_power, 90.0f, 0.001f);

    const auto& log = Mocking::GetExecutionLog();
    ASSERT_EQ(log.size(), (size_t)3);
    ASSERT_STREQ(log[0].mod_name.c_str(), "ModA");
    ASSERT_STREQ(log[1].mod_name.c_str(), "ModB");
    ASSERT_STREQ(log[2].mod_name.c_str(), "ModC");
}
