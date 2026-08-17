# Modding API & Development Specification

## 1. Overview

`CubeModLoader` dynamically loads mods packaged as Windows 64-bit DLLs placed in the `Mods/` directory. All mods must link against or include headers from [CWSDK](https://github.com/ChrisMiuchiz/CWSDK) and implement the standard `GenericMod` interface and exported C-ABI symbols.

---

## 2. Required DLL Exports

Every mod DLL must export the following functions with C linkage (`extern "C" __declspec(dllexport)`):

```cpp
#include "CWSDK/cwsdk.h"

// Define your mod class inheriting from GenericMod
class MyMod : public GenericMod {
public:
    virtual void Initialize() override {
        // Initialization logic
    }
};

// 1. Export Mod Major Version (Must match CubeModLoader MOD_MAJOR_VERSION: currently 7)
EXPORT int ModMajorVersion() {
    return 7;
}

// 2. Export Mod Minor Version (Must be <= CubeModLoader MOD_MINOR_VERSION: currently 3)
EXPORT int ModMinorVersion() {
    return 3;
}

// 3. Early pre-initialization routine (called before instantiation)
EXPORT void ModPreInitialize() {
    // Early memory setups, global state config
}

// 4. Factory function to instantiate the mod class instance
EXPORT GenericMod* MakeMod() {
    return new MyMod();
}
```

---

## 3. The `GenericMod` Interface

The base class `GenericMod` provides virtual hook methods and associated priority attributes.

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
    virtual int OnChat(void* msg) { return 0; }

    Priority OnP2PRequestPriority = NormalPriority;
    virtual int OnP2PRequest(uint64_t steamID) { return 0; }

    // Inventory & Items
    Priority OnCheckInventoryFullPriority = NormalPriority;
    virtual int OnCheckInventoryFull(void* player, void* item) { return 0; }

    // Game Loop & Input
    Priority OnGameTickPriority = NormalPriority;
    virtual void OnGameTick(void* game) {}

    Priority OnWindowProcPriority = NormalPriority;
    virtual int OnWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }

    Priority OnGetKeyboardStatePriority = NormalPriority;
    virtual void OnGetKeyboardState(BYTE* diKeys) {}

    Priority OnGetMouseStatePriority = NormalPriority;
    virtual void OnGetMouseState(void* diMouse) {}

    // Rendering
    Priority OnPresentPriority = NormalPriority;
    virtual void OnPresent(void* SwapChain, UINT SyncInterval, UINT Flags) {}

    // World & Zones
    Priority OnZoneGeneratedPriority = NormalPriority;
    virtual void OnZoneGenerated(void* zone) {}

    Priority OnZoneDestroyPriority = NormalPriority;
    virtual void OnZoneDestroy(void* zone) {}

    Priority OnChunkRemeshPriority = NormalPriority;
    virtual void OnChunkRemesh(void* zone) {}

    Priority OnChunkRemeshedPriority = NormalPriority;
    virtual void OnChunkRemeshed(void* zone) {}

    // Creature Stat Calculations
    Priority OnCreatureArmorCalculatedPriority = NormalPriority;
    virtual void OnCreatureArmorCalculated(void* creature, float* armor) {}

    Priority OnCreatureCriticalCalculatedPriority = NormalPriority;
    virtual void OnCreatureCriticalCalculated(void* creature, float* critical) {}

    Priority OnCreatureAttackPowerCalculatedPriority = NormalPriority;
    virtual void OnCreatureAttackPowerCalculated(void* creature, float* power) {}

    Priority OnCreatureSpellPowerCalculatedPriority = NormalPriority;
    virtual void OnCreatureSpellPowerCalculated(void* creature, float* power) {}

    Priority OnCreatureHasteCalculatedPriority = NormalPriority;
    virtual void OnCreatureHasteCalculated(void* creature, float* haste) {}

    Priority OnCreatureHPCalculatedPriority = NormalPriority;
    virtual void OnCreatureHPCalculated(void* creature, float* hp) {}

    Priority OnCreatureResistanceCalculatedPriority = NormalPriority;
    virtual void OnCreatureResistanceCalculated(void* creature, float* resistance) {}

    Priority OnCreatureRegenerationCalculatedPriority = NormalPriority;
    virtual void OnCreatureRegenerationCalculated(void* creature, float* regeneration) {}

    Priority OnCreatureManaGenerationCalculatedPriority = NormalPriority;
    virtual void OnCreatureManaGenerationCalculated(void* creature, float* manaGeneration) {}
};
```

---

## 4. Priority Execution Model

When multiple mods hook into the same callback, `CubeModLoader` invokes them in ascending priority order:

1. `VeryHighPriority (0)`
2. `HighPriority (1)`
3. `NormalPriority (2)`
4. `LowPriority (3)`
5. `VeryLowPriority (4)`

For callbacks returning an `int` status (such as `OnChat`, `OnCheckInventoryFull`, `OnWindowProc`):
* Returning non-zero typically indicates that the event was handled/intercepted, suppressing subsequent default processing or remaining mod hooks depending on the specific handler implementation.

---

## 5. Complete Example: Custom Health & Chat Mod

```cpp
#include "CWSDK/cwsdk.h"

class EnhancedPlayerMod : public GenericMod {
public:
    EnhancedPlayerMod() {
        // Assign hook priority
        OnCreatureHPCalculatedPriority = HighPriority;
        OnChatPriority = NormalPriority;
    }

    virtual void Initialize() override {
        // Mod initialization logic
    }

    virtual void OnCreatureHPCalculated(void* creature, float* hp) override {
        // Boost health calculation
        if (hp != nullptr) {
            *hp *= 1.25f; // +25% HP bonus
        }
    }

    virtual int OnChat(void* msg) override {
        // Intercept or log chat messages
        return 0; // 0 to allow message propagation
    }
};

EXPORT int ModMajorVersion() {
    return 7;
}

EXPORT int ModMinorVersion() {
    return 3;
}

EXPORT void ModPreInitialize() {
    // Early setup if needed
}

EXPORT GenericMod* MakeMod() {
    return new EnhancedPlayerMod();
}
```
