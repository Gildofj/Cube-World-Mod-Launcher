# CubeForge Loader & Launcher

[![C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-lightgrey.svg)](https://www.microsoft.com/windows)
[![Toolchain](https://img.shields.io/badge/Compiler-MSVC%20%2F%20MinGW%20(x64)-brightgreen.svg)](https://visualstudio.microsoft.com/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C.svg)](https://cmake.org/)
[![Target Game](https://img.shields.io/badge/Cube%20World-Release%201.0.0--1-orange.svg)](https://store.steampowered.com/app/1128000/Cube_World/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A robust DLL injection launcher, low-level runtime hook engine, and in-game mod management framework for **Cube World** (Steam release `1.0.0-1`, x86_64).

Powered by the **[CubeForge SDK](https://github.com/Gildofj/cubeforge.sdk)**.

---

## 🌟 Features

* **Early Initialization Hooking**: Intercepts `initterm_e` during early CRT startup to initialize mod subsystems before game loops run.
* **Low-Level ASM Trampolines**: 64-bit absolute indirect jumps (`FF 25`) with full register and stack preservation for low-overhead engine hooking.
* **Modern DirectX 11 & ImGui Integration**: Native overlay support alongside the Plasma engine for modern in-game mod interfaces.
* **In-Game Mod GUI (`ModWidget`)**: Embedded UI in the Start Menu powered by the Plasma engine to toggle mods on the fly with persistent config (`mods-settings.cwb`).
* **Priority Callback Engine**: Mod callbacks sorted by priority (`VeryHigh` to `VeryLow`) covering creature stats, combat events, inventory, input, rendering, world generation, and networking.
* **Binary Integrity Verification**: CRC32 validation against packed (`0xC7682619`) and unpacked (`0xBA092543`) Steam executables.
* **OS Compatibility Patching**: Built-in runtime patch for `FreeImage.dll` preventing crashes on Windows 8, 10, and 11.

---

## 📂 Repository Structure

```text
CubeForge-Loader/
├── CubeModLauncher/         # Standalone process injector executable (CubeModLauncher.exe)
├── CubeModLoader/           # Runtime hooking DLL and in-game mod loader (CubeModLoader.dll / .fip)
│   ├── callbacks/           # Engine hooks for GUI, Game, Creature, Items, World
│   ├── CWSDK/               # Submodule for CubeForge SDK types and structures
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

1. Download the latest release from the [Releases](https://github.com/Gildofj/cubeforge.loader/releases) section.
2. Place `CubeModLoader.fip` (or `CubeModLauncher.exe` and `CubeModLoader.dll`) into your Cube World root directory (same folder as `cubeworld.exe`).
3. Create a folder named `Mods` in the game directory.
4. Place your mod `.dll` files into the `Mods/` directory.

### Running with Mods

* **FIP Plugin (`CubeModLoader.fip`)**: Launch the game normally via Steam. A dialog will prompt whether to enable mods.
* **Standalone Launcher (`CubeModLauncher.exe`)**: Execute `CubeModLauncher.exe` directly.

---

## 🛠️ Quick Start for Developers

### Prerequisites

* **OS**: Windows 10/11 64-bit
* **Compiler**: Visual Studio 2022 (MSVC x64) or MinGW-w64 (`x86_64-w64-mingw32-g++`)
* **Build System**: CMake 3.20+
* **Dependencies**: [CubeForge SDK](https://github.com/Gildofj/cubeforge.sdk)

### Build Instructions (CMake / Visual Studio)

```bash
# 1. Clone with submodules
git clone --recurse-submodules https://github.com/Gildofj/cubeforge.loader.git
cd cubeforge.loader

# 2. Configure & Build via CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
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

## 🌟 Credits & Technical Lineage

* **[Picroma (Wollay & Pixwer)](https://picroma.com/)** — Creators of *Cube World*.
* **[ChrisMiuchiz](https://github.com/ChrisMiuchiz)** — Author of the original Cube-World-Mod-Launcher and ASM trampoline engine.
* **[Gijs Groenewegen](https://github.com/gijsgroenewegen)** — Author and maintainer of the previous upstream repository (2020–2022) with critical loader fixes and stability updates.
* **[Gildo FJ](https://github.com/Gildofj)** — Lead developer of CubeForge (C++20 modernization, MSVC/Clang pipelines, ImGui integration, tests & docs).
* **[Andoryuuta](https://github.com/Andoryuuta)** & **[matpow2](https://github.com/matpow2)** — Pioneering reverse-engineering work and struct mappings.
* **[ZaneYork](https://github.com/ZaneYork)** — Contributions to structures, commands, and early mod loader implementations.

See [`CREDITS.md`](CREDITS.md) for full technical lineage.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
