# Cube-World-Mod-Launcher

A robust DLL injection launcher, runtime hook engine, and in-game mod management framework for **Cube World** (Steam release `1.0.0-1`, x86_64).

---

## 🌟 Features

* **Early Initialization Hooking**: Intercepts `initterm_e` during early CRT startup to initialize mod subsystems before game loops run.
* **Low-Level ASM Trampolines**: 64-bit absolute indirect jumps (`FF 25`) with full register and stack preservation for low-overhead engine hooking.
* **In-Game Mod GUI (`ModWidget`)**: Embedded UI in the Start Menu powered by the Plasma engine to toggle mods on the fly with persistent config (`mods-settings.cwb`).
* **Priority Callback Engine**: Mod callbacks sorted by priority (`VeryHigh` to `VeryLow`) covering creature stats, combat events, inventory, input, rendering, world generation, and networking.
* **Binary Integrity Verification**: CRC32 validation against packed (`0xC7682619`) and unpacked (`0xBA092543`) Steam executables.
* **OS Compatibility Patching**: Built-in runtime patch for `FreeImage.dll` preventing crashes on Windows 8, 10, and 11.

---

## 📂 Repository Structure

```text
Cube-World-Mod-Launcher/
├── CubeModLauncher/         # Standalone process injector executable (CubeModLauncher.exe)
├── CubeModLoader/           # Runtime hooking DLL and in-game mod loader (CubeModLoader.dll / .fip)
│   ├── callbacks/           # Engine hooks for GUI, Game, Creature, Items, World
│   ├── CWSDK/               # Submodule for Cube World SDK types and structures
│   ├── GenericMod.h         # Base interface for custom mods
│   └── ModWidget.cpp        # In-game Start Menu mod selector UI
├── docs/                    # Technical documentation and guides
│   ├── ARCHITECTURE.md      # Detailed system design, memory layout & execution pipeline
│   ├── MODDING-API.md       # Guide and reference for writing custom mods
│   ├── DEVELOPMENT-GUIDE.md # Build instructions, CMake setup, toolchain requirements
│   ├── TROUBLESHOOTING.md   # Solutions for common errors, CRC mismatch & OS patches
│   └── CONTRIBUTING.md      # Contributing guidelines and code standards
└── steam_appid.txt          # Steam App ID (1128000)
```

---

## 🚀 Quick Start for Players

### Installation

1. Download the latest release from the [Releases](https://github.com/ChrisMiuchiz/Cube-World-Mod-Launcher/releases) section.
2. Place `CubeModLoader.fip` (or `CubeModLauncher.exe` and `CubeModLoader.dll`) into the same folder as `cubeworld.exe`.
3. Create a folder named `Mods` in the game directory.
4. Place your mod `.dll` files into the `Mods/` directory.

### Running with Mods

* If using **FIP plugin** (`CubeModLoader.fip`): Launch the game normally via Steam. A dialog will prompt whether to enable mods.
* If using **Standalone Launcher** (`CubeModLauncher.exe`): Execute `CubeModLauncher.exe` directly.

---

## 🛠️ Quick Start for Developers

### Prerequisites

* **OS**: Windows 10/11 64-bit
* **Compiler**: MinGW-w64 (`x86_64-w64-mingw32-g++` with inline assembly support)
* **Build System**: CMake 3.8+
* **Dependencies**: [CWSDK](https://github.com/ChrisMiuchiz/CWSDK)

### Build Instructions

```bash
# 1. Clone with submodules
git clone --recurse-submodules https://github.com/ChrisMiuchiz/Cube-World-Mod-Launcher.git
cd Cube-World-Mod-Launcher

# 2. Build everything from root (Makefile / MinGW)
make
# or via PowerShell:
# .\build.ps1

# Options:
make loader     # Builds only CubeModLoader (.dll and .fip)
make launcher   # Builds only CubeModLauncher (.exe)
make test       # Builds and runs test suite
make clean      # Cleans build artifacts
```

For comprehensive build details, see the [Development & Build Guide](docs/DEVELOPMENT-GUIDE.md).

---

## 🧩 Creating a Mod

All mods inherit from `GenericMod` and export required version and factory functions:

```cpp
#include "CWSDK/cwsdk.h"

class SampleMod : public GenericMod {
public:
    SampleMod() {
        OnCreatureHPCalculatedPriority = NormalPriority;
    }

    virtual void OnCreatureHPCalculated(void* creature, float* hp) override {
        if (hp) *hp *= 1.10f; // +10% HP boost
    }
};

EXPORT int ModMajorVersion() { return 7; }
EXPORT int ModMinorVersion() { return 3; }
EXPORT void ModPreInitialize() {}
EXPORT GenericMod* MakeMod() { return new SampleMod(); }
```

Refer to the [Modding API Documentation](docs/MODDING-API.md) for available hooks, callbacks, and structures.

---

## 📚 Documentation Index

* 📐 [Architecture & Technical Design](docs/ARCHITECTURE.md)
* 🔌 [Modding API Specification](docs/MODDING-API.md)
* 🔨 [Development & Build Guide](docs/DEVELOPMENT-GUIDE.md)
* ❓ [Troubleshooting & FAQ](docs/TROUBLESHOOTING.md)
* 🤝 [Contributing Guidelines](docs/CONTRIBUTING.md)

---

## 📄 License & Credits

* Created by **ChrisMiuchiz**.
* Target Game: *Cube World* by Picroma.
