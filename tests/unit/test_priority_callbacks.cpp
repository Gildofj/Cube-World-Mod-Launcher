#include "../framework/test_framework.h"
#include "../mocks/MockMod.h"
#include <vector>
#include <string>

// Helper dispatcher simulating CubeModLoader's callback priority dispatcher
namespace DispatcherHelper {

    struct FakeDLL {
        std::string fileName;
        GenericMod* mod;
        bool enabled;

        FakeDLL(std::string name, GenericMod* m, bool en = true)
            : fileName(std::move(name)), mod(m), enabled(en) {}
    };

    int DispatchChat(const std::vector<FakeDLL*>& dlls, void* msg) {
        for (uint8_t priority = 0; priority <= 4; priority += 1) {
            for (FakeDLL* dll : dlls) {
                if (!dll->enabled) continue;
                if (dll->mod->OnChatPriority == (GenericMod::Priority)priority) {
                    if (dll->mod->OnChat(msg)) {
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    int DispatchP2PRequest(const std::vector<FakeDLL*>& dlls, uint64_t steamID) {
        for (uint8_t priority = 0; priority <= 4; priority += 1) {
            for (FakeDLL* dll : dlls) {
                if (!dll->enabled) continue;
                if (dll->mod->OnP2PRequestPriority == (GenericMod::Priority)priority) {
                    if (dll->mod->OnP2PRequest(steamID)) {
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    void DispatchCreatureHPCalculated(const std::vector<FakeDLL*>& dlls, void* creature, float* hp) {
        for (uint8_t priority = 0; priority <= 4; priority += 1) {
            for (FakeDLL* dll : dlls) {
                if (!dll->enabled) continue;
                if (dll->mod->OnCreatureHPCalculatedPriority == (GenericMod::Priority)priority) {
                    dll->mod->OnCreatureHPCalculated(creature, hp);
                }
            }
        }
    }

    void DispatchCreatureCriticalCalculated(const std::vector<FakeDLL*>& dlls, void* creature, float* crit) {
        for (uint8_t priority = 0; priority <= 4; priority += 1) {
            for (FakeDLL* dll : dlls) {
                if (!dll->enabled) continue;
                if (dll->mod->OnCreatureCriticalCalculatedPriority == (GenericMod::Priority)priority) {
                    dll->mod->OnCreatureCriticalCalculated(creature, crit);
                }
            }
        }
    }
}

TEST_CASE(PriorityDispatcher, StrictExecutionOrder) {
    Mocking::ClearExecutionLog();

    Mocking::MockMod mod_low("LowMod", GenericMod::LowPriority);
    Mocking::MockMod mod_very_high("VeryHighMod", GenericMod::VeryHighPriority);
    Mocking::MockMod mod_normal("NormalMod", GenericMod::NormalPriority);
    Mocking::MockMod mod_high("HighMod", GenericMod::HighPriority);
    Mocking::MockMod mod_very_low("VeryLowMod", GenericMod::VeryLowPriority);

    // Added deliberately in mixed order
    std::vector<DispatcherHelper::FakeDLL> dll_storage = {
        DispatcherHelper::FakeDLL("Low.dll", &mod_low),
        DispatcherHelper::FakeDLL("VeryHigh.dll", &mod_very_high),
        DispatcherHelper::FakeDLL("Normal.dll", &mod_normal),
        DispatcherHelper::FakeDLL("High.dll", &mod_high),
        DispatcherHelper::FakeDLL("VeryLow.dll", &mod_very_low)
    };

    std::vector<DispatcherHelper::FakeDLL*> dlls;
    for (auto& d : dll_storage) dlls.push_back(&d);

    float hp = 100.0f;
    DispatcherHelper::DispatchCreatureHPCalculated(dlls, nullptr, &hp);

    const auto& log = Mocking::GetExecutionLog();
    ASSERT_EQ(log.size(), (size_t)5);
    ASSERT_STREQ(log[0].mod_name.c_str(), "VeryHighMod");
    ASSERT_STREQ(log[1].mod_name.c_str(), "HighMod");
    ASSERT_STREQ(log[2].mod_name.c_str(), "NormalMod");
    ASSERT_STREQ(log[3].mod_name.c_str(), "LowMod");
    ASSERT_STREQ(log[4].mod_name.c_str(), "VeryLowMod");
}

TEST_CASE(PriorityDispatcher, EarlyReturnShortCircuitChat) {
    Mocking::ClearExecutionLog();

    Mocking::MockMod mod1("Mod1_High", GenericMod::HighPriority);
    mod1.chat_return_value = 1; // Intercepts and consumes the message

    Mocking::MockMod mod2("Mod2_Normal", GenericMod::NormalPriority);
    mod2.chat_return_value = 0;

    Mocking::MockMod mod3("Mod3_Low", GenericMod::LowPriority);
    mod3.chat_return_value = 1;

    std::vector<DispatcherHelper::FakeDLL> storage = {
        DispatcherHelper::FakeDLL("Mod1.dll", &mod1),
        DispatcherHelper::FakeDLL("Mod2.dll", &mod2),
        DispatcherHelper::FakeDLL("Mod3.dll", &mod3)
    };

    std::vector<DispatcherHelper::FakeDLL*> dlls = { &storage[0], &storage[1], &storage[2] };

    int result = DispatcherHelper::DispatchChat(dlls, nullptr);
    ASSERT_EQ(result, 1);

    const auto& log = Mocking::GetExecutionLog();
    // Only HighPriority mod should have been called because it returned 1
    ASSERT_EQ(log.size(), (size_t)1);
    ASSERT_STREQ(log[0].mod_name.c_str(), "Mod1_High");
    ASSERT_EQ(mod1.on_chat_calls, 1);
    ASSERT_EQ(mod2.on_chat_calls, 0);
    ASSERT_EQ(mod3.on_chat_calls, 0);
}

TEST_CASE(PriorityDispatcher, FallthroughWhenNoHandlerConsumes) {
    Mocking::ClearExecutionLog();

    Mocking::MockMod mod1("Mod1", GenericMod::HighPriority);
    mod1.chat_return_value = 0;
    Mocking::MockMod mod2("Mod2", GenericMod::NormalPriority);
    mod2.chat_return_value = 0;
    Mocking::MockMod mod3("Mod3", GenericMod::LowPriority);
    mod3.chat_return_value = 0;

    std::vector<DispatcherHelper::FakeDLL> storage = {
        DispatcherHelper::FakeDLL("Mod1.dll", &mod1),
        DispatcherHelper::FakeDLL("Mod2.dll", &mod2),
        DispatcherHelper::FakeDLL("Mod3.dll", &mod3)
    };
    std::vector<DispatcherHelper::FakeDLL*> dlls = { &storage[0], &storage[1], &storage[2] };

    int result = DispatcherHelper::DispatchChat(dlls, nullptr);
    ASSERT_EQ(result, 0);

    const auto& log = Mocking::GetExecutionLog();
    ASSERT_EQ(log.size(), (size_t)3);
    ASSERT_EQ(mod1.on_chat_calls, 1);
    ASSERT_EQ(mod2.on_chat_calls, 1);
    ASSERT_EQ(mod3.on_chat_calls, 1);
}

TEST_CASE(PriorityDispatcher, CumulativeStatCalculation) {
    Mocking::ClearExecutionLog();

    Mocking::MockMod mod_first("First", GenericMod::VeryHighPriority);
    mod_first.hp_multiplier = 2.0f; // 100 * 2 = 200

    Mocking::MockMod mod_second("Second", GenericMod::NormalPriority);
    mod_second.hp_multiplier = 1.5f; // 200 * 1.5 = 300

    Mocking::MockMod mod_third("Third", GenericMod::LowPriority);
    mod_third.hp_multiplier = 0.5f; // 300 * 0.5 = 150

    std::vector<DispatcherHelper::FakeDLL> storage = {
        DispatcherHelper::FakeDLL("Third.dll", &mod_third),
        DispatcherHelper::FakeDLL("First.dll", &mod_first),
        DispatcherHelper::FakeDLL("Second.dll", &mod_second)
    };
    std::vector<DispatcherHelper::FakeDLL*> dlls = { &storage[0], &storage[1], &storage[2] };

    float hp = 100.0f;
    DispatcherHelper::DispatchCreatureHPCalculated(dlls, nullptr, &hp);

    ASSERT_NEAR(hp, 150.0f, 0.001f);
}

TEST_CASE(PriorityDispatcher, DisabledModsAreIgnored) {
    Mocking::ClearExecutionLog();

    Mocking::MockMod active_mod("Active", GenericMod::HighPriority);
    Mocking::MockMod disabled_mod("Disabled", GenericMod::VeryHighPriority);

    std::vector<DispatcherHelper::FakeDLL> storage = {
        DispatcherHelper::FakeDLL("Active.dll", &active_mod, true),
        DispatcherHelper::FakeDLL("Disabled.dll", &disabled_mod, false)
    };
    std::vector<DispatcherHelper::FakeDLL*> dlls = { &storage[0], &storage[1] };

    float crit = 0.05f;
    DispatcherHelper::DispatchCreatureCriticalCalculated(dlls, nullptr, &crit);

    const auto& log = Mocking::GetExecutionLog();
    ASSERT_EQ(log.size(), (size_t)1);
    ASSERT_STREQ(log[0].mod_name.c_str(), "Active");
}
