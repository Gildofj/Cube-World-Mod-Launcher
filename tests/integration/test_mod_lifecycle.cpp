#include "../framework/test_framework.h"
#include "../mocks/MockMod.h"
#include "../mocks/MockEnvironment.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace LifecycleSimulation {

    struct SimulatedDLL {
        std::string fileName;
        int major_version = 7;
        int minor_version = 3;
        bool enabled = true;
        bool pre_initialized = false;
        std::unique_ptr<Mocking::MockMod> mod_instance;

        SimulatedDLL(std::string name, int maj = 7, int min = 3, GenericMod::Priority prio = GenericMod::NormalPriority)
            : fileName(std::move(name)), major_version(maj), minor_version(min) {
            mod_instance = std::make_unique<Mocking::MockMod>(fileName, prio);
        }

        void PreInitialize() {
            pre_initialized = true;
        }

        void Initialize() {
            if (mod_instance) {
                mod_instance->Initialize();
            }
        }
    };

    class SimulatedLoaderEngine {
    public:
        std::vector<std::shared_ptr<SimulatedDLL>> all_dlls;
        std::vector<std::shared_ptr<SimulatedDLL>> active_dlls;
        bool already_loaded = false;
        int target_major = 7;
        int target_minor = 3;

        bool RunStartupPipeline(std::vector<std::shared_ptr<SimulatedDLL>>& discovered_mods) {
            if (already_loaded) return false;
            already_loaded = true;

            // 1. Version validation
            for (auto& dll : discovered_mods) {
                if (dll->major_version != target_major || dll->minor_version > target_minor) {
                    return false; // Rejection on version mismatch
                }
            }

            all_dlls = discovered_mods;

            // 2. Filter enabled mods
            active_dlls.clear();
            for (auto& dll : all_dlls) {
                if (dll->enabled) {
                    active_dlls.push_back(dll);
                }
            }

            // 3. PreInitialize
            for (auto& dll : active_dlls) {
                dll->PreInitialize();
            }

            // 4. Initialize
            for (auto& dll : active_dlls) {
                dll->Initialize();
            }

            return true;
        }

        void DispatchGameTick(void* game) {
            for (uint8_t priority = 0; priority <= 4; ++priority) {
                for (auto& dll : active_dlls) {
                    if (dll->mod_instance->OnGameTickPriority == (GenericMod::Priority)priority) {
                        dll->mod_instance->OnGameTick(game);
                    }
                }
            }
        }

        void DispatchZoneLifecycle(void* zone) {
            for (uint8_t priority = 0; priority <= 4; ++priority) {
                for (auto& dll : active_dlls) {
                    if (dll->mod_instance->OnZoneGeneratedPriority == (GenericMod::Priority)priority) {
                        dll->mod_instance->OnZoneGenerated(zone);
                    }
                }
            }
            for (uint8_t priority = 0; priority <= 4; ++priority) {
                for (auto& dll : active_dlls) {
                    if (dll->mod_instance->OnZoneDestroyPriority == (GenericMod::Priority)priority) {
                        dll->mod_instance->OnZoneDestroy(zone);
                    }
                }
            }
        }
    };
}

TEST_CASE(IntegrationModLifecycle, FullPipelineExecution) {
    Mocking::ClearExecutionLog();

    LifecycleSimulation::SimulatedLoaderEngine engine;

    std::vector<std::shared_ptr<LifecycleSimulation::SimulatedDLL>> mods = {
        std::make_shared<LifecycleSimulation::SimulatedDLL>("Mods/ModA.dll", 7, 3, GenericMod::HighPriority),
        std::make_shared<LifecycleSimulation::SimulatedDLL>("Mods/ModB.dll", 7, 2, GenericMod::NormalPriority),
        std::make_shared<LifecycleSimulation::SimulatedDLL>("Mods/DisabledMod.dll", 7, 3, GenericMod::VeryHighPriority)
    };

    mods[2]->enabled = false; // Disable Mod 3

    bool success = engine.RunStartupPipeline(mods);
    ASSERT_TRUE(success);

    // Verify all_dlls vs active_dlls separation
    ASSERT_EQ(engine.all_dlls.size(), (size_t)3);
    ASSERT_EQ(engine.active_dlls.size(), (size_t)2);

    // Verify initialization hooks ran only on enabled mods
    ASSERT_TRUE(mods[0]->pre_initialized);
    ASSERT_TRUE(mods[0]->mod_instance->initialized);

    ASSERT_TRUE(mods[1]->pre_initialized);
    ASSERT_TRUE(mods[1]->mod_instance->initialized);

    ASSERT_FALSE(mods[2]->pre_initialized);
    ASSERT_FALSE(mods[2]->mod_instance->initialized);

    // Dispatch game events
    engine.DispatchGameTick(nullptr);
    ASSERT_EQ(mods[0]->mod_instance->on_game_tick_calls, 1);
    ASSERT_EQ(mods[1]->mod_instance->on_game_tick_calls, 1);
    ASSERT_EQ(mods[2]->mod_instance->on_game_tick_calls, 0);

    engine.DispatchZoneLifecycle(nullptr);
    ASSERT_EQ(mods[0]->mod_instance->on_zone_generated_calls, 1);
    ASSERT_EQ(mods[0]->mod_instance->on_zone_destroy_calls, 1);
    ASSERT_EQ(mods[2]->mod_instance->on_zone_generated_calls, 0);
}

TEST_CASE(IntegrationModLifecycle, IdempotentStartupPrevention) {
    LifecycleSimulation::SimulatedLoaderEngine engine;
    std::vector<std::shared_ptr<LifecycleSimulation::SimulatedDLL>> mods = {
        std::make_shared<LifecycleSimulation::SimulatedDLL>("Mods/Mod1.dll", 7, 3)
    };

    bool first_call = engine.RunStartupPipeline(mods);
    ASSERT_TRUE(first_call);

    bool second_call = engine.RunStartupPipeline(mods);
    ASSERT_FALSE(second_call); // Prevented by already_loaded guard
}
