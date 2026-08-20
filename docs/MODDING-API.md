# Modding API & Development Specification

## 1. Overview

`CubeForgeLoader` dynamically discovers and loads mods packaged as 64-bit native DLLs placed inside the `Mods/` directory. All mods interact with the game engine by including the [CubeForge SDK](https://github.com/Gildofj/cubeforge.sdk) and inheriting from the `GenericMod` interface.

For a ready-to-use project template, see:
👉 **[CubeForge Mod Template (`cubeforge.mod-template`)](https://github.com/Gildofj/cubeforge.mod-template)**

---

## 2. Required DLL Exports

Every mod DLL must export `MakeMod()` with C linkage (`extern "C" __declspec(dllexport)`):

```cpp
#include "cwsdk.h"

// Define your mod class inheriting from GenericMod
class MyMod : public GenericMod {
public:
    virtual void Initialize() override {
        // Initialization logic
    }
};

// Main mod instantiation factory function
EXPORT GenericMod* MakeMod() {
    return new MyMod();
}
```

> [!NOTE]
> Default implementations of `ModMajorVersion()`, `ModMinorVersion()`, and `ModPreInitialize()` are automatically provided and exported by `CWSDK.lib`. You only need to export `MakeMod()`.

---

## 3. The `GenericMod` Interface

The base class `GenericMod` provides virtual callback methods and priority attributes for game events:

```cpp
class GenericMod {
public:
    enum Priority : uint8_t {
        VeryHighPriority = 0,
        HighPriority     = 1,
        NormalPriority   = 2, // Default
        LowPriority      = 3,
        VeryLowPriority  = 4
    };

    // Lifecycle
    virtual void Initialize() {}

    // Network & Chat
    Priority OnChatPriority = NormalPriority;
    virtual int OnChat(std::wstring* message) { return 0; }

    Priority OnP2PRequestPriority = NormalPriority;
    virtual int OnP2PRequest(uint64_t steamID) { return 0; }

    // Inventory & Items
    Priority OnCheckInventoryFullPriority = NormalPriority;
    virtual int OnCheckInventoryFull(cube::Creature* player, cube::Item* item) { return 0; }

    Priority OnGetItemBuyingPricePriority = NormalPriority;
    virtual void OnGetItemBuyingPrice(cube::Item* item, int* price) {}

    Priority OnGetItemSellingPricePriority = NormalPriority;
    virtual void OnGetItemSellingPrice(cube::Item* item, int* price) {}

    Priority OnCreatureCanEquipItemPriority = NormalPriority;
    virtual void OnCreatureCanEquipItem(cube::Creature* creature, cube::Item* item, bool* equipable) {}

    Priority OnClassCanWearItemPriority = NormalPriority;
    virtual void OnClassCanWearItem(cube::Item* item, int classType, bool* wearable) {}

    // Game Loop & Input
    Priority OnGameTickPriority = NormalPriority;
    virtual void OnGameTick(cube::Game* game) {}

    Priority OnGameUpdatePriority = NormalPriority;
    virtual void OnGameUpdate(cube::Game* game) {}

    Priority OnWindowProcPriority = NormalPriority;
    virtual int OnWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }

    Priority OnGetKeyboardStatePriority = NormalPriority;
    virtual void OnGetKeyboardState(BYTE* diKeys) {}

    Priority OnGetMouseStatePriority = NormalPriority;
    virtual void OnGetMouseState(DIMOUSESTATE* diMouse) {}

    // Rendering
    Priority OnPresentPriority = NormalPriority;
    virtual void OnPresent(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags) {}

    Priority OnDrawImGuiPriority = NormalPriority;
    virtual void OnDrawImGui() {}

    // World & Zones
    Priority OnZoneGeneratedPriority = NormalPriority;
    virtual void OnZoneGenerated(cube::Zone* zone) {}

    Priority OnZoneDestroyPriority = NormalPriority;
    virtual void OnZoneDestroy(cube::Zone* zone) {}

    Priority OnChunkRemeshPriority = NormalPriority;
    virtual void OnChunkRemesh(cube::Zone* zone) {}

    Priority OnChunkRemeshedPriority = NormalPriority;
    virtual void OnChunkRemeshed(cube::Zone* zone) {}

    // Creature & Player Combat / Death
    enum DeathType : __int32 { COMBAT = 0, DROWN, FALL };

    Priority OnPlayerDeathPriority = NormalPriority;
    virtual void OnPlayerDeath(cube::Game* game, cube::Creature* player, DeathType type) {}

    Priority OnCreatureDeathPriority = NormalPriority;
    virtual void OnCreatureDeath(cube::Game* game, cube::Creature* creature, cube::Creature* attacker) {}

    // Creature Stat Calculations
    Priority OnCreatureArmorCalculatedPriority = NormalPriority;
    virtual void OnCreatureArmorCalculated(cube::Creature* creature, float* armor) {}

    Priority OnCreatureCriticalCalculatedPriority = NormalPriority;
    virtual void OnCreatureCriticalCalculated(cube::Creature* creature, float* critical) {}

    Priority OnCreatureAttackPowerCalculatedPriority = NormalPriority;
    virtual void OnCreatureAttackPowerCalculated(cube::Creature* creature, float* power) {}

    Priority OnCreatureSpellPowerCalculatedPriority = NormalPriority;
    virtual void OnCreatureSpellPowerCalculated(cube::Creature* creature, float* power) {}

    Priority OnCreatureHasteCalculatedPriority = NormalPriority;
    virtual void OnCreatureHasteCalculated(cube::Creature* creature, float* haste) {}

    Priority OnCreatureHPCalculatedPriority = NormalPriority;
    virtual void OnCreatureHPCalculated(cube::Creature* creature, float* hp) {}

    Priority OnCreatureResistanceCalculatedPriority = NormalPriority;
    virtual void OnCreatureResistanceCalculated(cube::Creature* creature, float* resistance) {}

    Priority OnCreatureRegenerationCalculatedPriority = NormalPriority;
    virtual void OnCreatureRegenerationCalculated(cube::Creature* creature, float* regeneration) {}

    Priority OnCreatureManaGenerationCalculatedPriority = NormalPriority;
    virtual void OnCreatureManaGenerationCalculated(cube::Creature* creature, float* manaGeneration) {}
};
```

---

## 4. Priority Execution Model

When multiple mods hook into the same callback, `CubeForgeLoader` invokes them in ascending priority order:

1. `VeryHighPriority (0)`
2. `HighPriority (1)`
3. `NormalPriority (2)`
4. `LowPriority (3)`
5. `VeryLowPriority (4)`

For callbacks returning an `int` status (such as `OnChat`, `OnCheckInventoryFull`, `OnWindowProc`):
* Returning `1` (or non-zero) suppresses the event or cancels original game behavior.
* Returning `0` allows normal event propagation to continue.

---

## 5. Complete Example

```cpp
#include "cwsdk.h"

class EnhancedPlayerMod : public GenericMod {
public:
    EnhancedPlayerMod() {
        // Set event priorities
        OnCreatureHPCalculatedPriority = HighPriority;
        OnChatPriority = NormalPriority;
    }

    virtual void OnCreatureHPCalculated(cube::Creature* creature, float* hp) override {
        if (hp != nullptr) {
            *hp *= 1.25f; // +25% HP bonus
        }
    }

    virtual int OnChat(std::wstring* message) override {
        if (message && *message == L"/ping") {
            cube::Game* game = (cube::Game*)CWOffset(0x0);
            if (game) {
                FloatRGBA green(0.2f, 1.0f, 0.2f, 1.0f);
                game->PrintMessage(L"Pong from CubeForge!", &green);
            }
            return 1; // Suppress original chat message
        }
        return 0; // Normal chat
    }
};

EXPORT GenericMod* MakeMod() {
    return new EnhancedPlayerMod();
}
```
