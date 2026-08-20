#ifndef MOCK_MOD_H
#define MOCK_MOD_H

#include <vector>
#include <string>
#include <cstdint>
#include "../../src/GenericMod.h"

namespace Mocking {

    struct ExecutionLogEntry {
        std::string mod_name;
        GenericMod::Priority priority;
        std::string event_name;
    };

    inline std::vector<ExecutionLogEntry>& GetExecutionLog() {
        static std::vector<ExecutionLogEntry> log;
        return log;
    }

    inline void ClearExecutionLog() {
        GetExecutionLog().clear();
    }

    class MockMod : public GenericMod {
    public:
        std::string name;
        bool initialized = false;
        int on_chat_calls = 0;
        int on_game_tick_calls = 0;
        int on_zone_generated_calls = 0;
        int on_zone_destroy_calls = 0;
        int on_p2p_calls = 0;
        int on_inventory_full_calls = 0;
        int on_remesh_calls = 0;
        int on_remeshed_calls = 0;

        int chat_return_value = 0;
        int p2p_return_value = 0;
        int inventory_return_value = 0;

        float hp_multiplier = 1.0f;
        float armor_multiplier = 1.0f;
        float attack_power_multiplier = 1.0f;
        float critical_bonus = 0.0f;
        float spell_power_bonus = 0.0f;
        float haste_bonus = 0.0f;
        float resistance_bonus = 0.0f;
        float regeneration_bonus = 0.0f;
        float mana_generation_bonus = 0.0f;

        MockMod(std::string mod_name, Priority priority = NormalPriority) : name(std::move(mod_name)) {
            OnChatPriority = priority;
            OnCheckInventoryFullPriority = priority;
            OnP2PRequestPriority = priority;
            OnGameTickPriority = priority;
            OnZoneGeneratedPriority = priority;
            OnZoneDestroyPriority = priority;
            OnWindowProcPriority = priority;
            OnGetKeyboardStatePriority = priority;
            OnGetMouseStatePriority = priority;
            OnPresentPriority = priority;
            OnCreatureArmorCalculatedPriority = priority;
            OnCreatureCriticalCalculatedPriority = priority;
            OnCreatureAttackPowerCalculatedPriority = priority;
            OnCreatureSpellPowerCalculatedPriority = priority;
            OnCreatureHasteCalculatedPriority = priority;
            OnCreatureHPCalculatedPriority = priority;
            OnCreatureResistanceCalculatedPriority = priority;
            OnCreatureRegenerationCalculatedPriority = priority;
            OnCreatureManaGenerationCalculatedPriority = priority;
            OnChunkRemeshPriority = priority;
            OnChunkRemeshedPriority = priority;
        }

        virtual void Initialize() override {
            initialized = true;
        }

        virtual int OnChat(void* msg) override {
            on_chat_calls++;
            GetExecutionLog().push_back({name, OnChatPriority, "OnChat"});
            return chat_return_value;
        }

        virtual int OnCheckInventoryFull(void* player, void* item) override {
            on_inventory_full_calls++;
            GetExecutionLog().push_back({name, OnCheckInventoryFullPriority, "OnCheckInventoryFull"});
            return inventory_return_value;
        }

        virtual int OnP2PRequest(uint64_t steamID) override {
            on_p2p_calls++;
            GetExecutionLog().push_back({name, OnP2PRequestPriority, "OnP2PRequest"});
            return p2p_return_value;
        }

        virtual void OnGameTick(void* game) override {
            on_game_tick_calls++;
            GetExecutionLog().push_back({name, OnGameTickPriority, "OnGameTick"});
        }

        virtual void OnZoneGenerated(void* zone) override {
            on_zone_generated_calls++;
            GetExecutionLog().push_back({name, OnZoneGeneratedPriority, "OnZoneGenerated"});
        }

        virtual void OnZoneDestroy(void* zone) override {
            on_zone_destroy_calls++;
            GetExecutionLog().push_back({name, OnZoneDestroyPriority, "OnZoneDestroy"});
        }

        virtual void OnCreatureHPCalculated(void* creature, float* hp) override {
            if (hp) *hp *= hp_multiplier;
            GetExecutionLog().push_back({name, OnCreatureHPCalculatedPriority, "OnCreatureHPCalculated"});
        }

        virtual void OnCreatureArmorCalculated(void* creature, float* armor) override {
            if (armor) *armor *= armor_multiplier;
            GetExecutionLog().push_back({name, OnCreatureArmorCalculatedPriority, "OnCreatureArmorCalculated"});
        }

        virtual void OnCreatureAttackPowerCalculated(void* creature, float* power) override {
            if (power) *power *= attack_power_multiplier;
            GetExecutionLog().push_back({name, OnCreatureAttackPowerCalculatedPriority, "OnCreatureAttackPowerCalculated"});
        }

        virtual void OnCreatureCriticalCalculated(void* creature, float* critical) override {
            if (critical) *critical += critical_bonus;
            GetExecutionLog().push_back({name, OnCreatureCriticalCalculatedPriority, "OnCreatureCriticalCalculated"});
        }

        virtual void OnCreatureSpellPowerCalculated(void* creature, float* power) override {
            if (power) *power += spell_power_bonus;
            GetExecutionLog().push_back({name, OnCreatureSpellPowerCalculatedPriority, "OnCreatureSpellPowerCalculated"});
        }

        virtual void OnCreatureHasteCalculated(void* creature, float* haste) override {
            if (haste) *haste += haste_bonus;
            GetExecutionLog().push_back({name, OnCreatureHasteCalculatedPriority, "OnCreatureHasteCalculated"});
        }

        virtual void OnCreatureResistanceCalculated(void* creature, float* resistance) override {
            if (resistance) *resistance += resistance_bonus;
            GetExecutionLog().push_back({name, OnCreatureResistanceCalculatedPriority, "OnCreatureResistanceCalculated"});
        }

        virtual void OnCreatureRegenerationCalculated(void* creature, float* regeneration) override {
            if (regeneration) *regeneration += regeneration_bonus;
            GetExecutionLog().push_back({name, OnCreatureRegenerationCalculatedPriority, "OnCreatureRegenerationCalculated"});
        }

        virtual void OnCreatureManaGenerationCalculated(void* creature, float* manaGeneration) override {
            if (manaGeneration) *manaGeneration += mana_generation_bonus;
            GetExecutionLog().push_back({name, OnCreatureManaGenerationCalculatedPriority, "OnCreatureManaGenerationCalculated"});
        }

        virtual void OnChunkRemesh(void* zone) override {
            on_remesh_calls++;
            GetExecutionLog().push_back({name, OnChunkRemeshPriority, "OnChunkRemesh"});
        }

        virtual void OnChunkRemeshed(void* zone) override {
            on_remeshed_calls++;
            GetExecutionLog().push_back({name, OnChunkRemeshedPriority, "OnChunkRemeshed"});
        }
    };

} // namespace Mocking

#endif // MOCK_MOD_H
