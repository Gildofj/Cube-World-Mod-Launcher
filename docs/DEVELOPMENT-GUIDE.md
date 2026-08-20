# Development & Build Guide

This guide covers building, testing, and contributing to the **CubeForge Loader & Launcher** suite.

---

## 1. Prerequisites & Toolchain

Building `cubeforge.loader` requires a 64-bit Windows C++20 toolchain:

* **Operating System**: Windows 10 or 11 (64-bit)
* **Compiler**: Microsoft Visual Studio 2022 (MSVC x64 v143) with `ml64.exe` (MASM), or Clang-cl / MinGW-w64
* **Build System**: [CMake](https://cmake.org/) (Version 3.20 or higher)
* **SDK & Dependencies**: Handled automatically by CMake via `FetchContent`:
  - **[CubeForge SDK](https://github.com/Gildofj/cubeforge.sdk)**
  - **[Dear ImGui](https://github.com/ocornut/imgui)** (v1.91.8)

---

## 2. Project Layout

```text
cubeforge.loader/
├── CubeForgeLoader/                  # Core Injected DLL & In-Game Mod Runtime
│   ├── CMakeLists.txt              # Subdirectory CMake configuration
│   ├── callbacks/                  # Hook trampolines and domain handlers
│   │   ├── creature/               # Creature death, armor, equip hooks
│   │   ├── game/                   # Game loop, mouse hooks
│   │   ├── gui/                    # Plasma UI, start menu, character preview
│   │   ├── item/                   # Item pricing, gold bag, wearability
│   │   └── world/                  # World generation and creature spawn hooks
│   ├── GenericMod.h                # Base class interface for all mods
│   ├── ModWidget.h / .cpp          # In-game mod management widget (Plasma UI)
│   ├── DLL.h / DLL.cpp             # Dynamic library loader wrapper
│   ├── crc.h / crc.cpp             # Executable CRC32 validator
│   ├── macros.h                    # ASM trampoline macros & stack helpers
│   ├── trampolines.asm             # 64-bit MASM assembly hook trampolines
│   └── main.h / main.cpp           # DllMain, initterm_e hook, mod discovery
│
├── tests/                          # Automated test suite
├── docs/                           # Technical documentation & guides
├── README.md                       # Repository overview & quickstart
└── steam_appid.txt                 # Steam AppID configuration (1128000)
```

---

## 3. Cloning the Repository

Because dependencies are managed via CMake `FetchContent`, a standard clone is all that is required:

```bash
git clone https://github.com/Gildofj/cubeforge.loader.git
cd cubeforge.loader
```

---

## 4. Building with CMake

### 4.1 Building from Root (All Targets)

You can build the entire project (Loader and Tests) from the repository root:

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build all targets
cmake --build build --config Release
```

### 4.2 Building Specific Targets

```bash
# Build only the injected DLL & FIP plugin
cmake --build build --config Release --target CubeForgeLoader

# Build and run the test suite
cmake --build build --config Release --target test_runner
ctest --test-dir build -C Release --output-on-failure
```

### 4.3 Local SDK Development Override

If you are developing features in `cubeforge.sdk` locally and want `cubeforge.loader` to use your local SDK clone instead of fetching from Git:

```bash
cmake -B build -S . -DFETCHCONTENT_SOURCE_DIR_CUBEFORGE_SDK="D:/Projects/cubeforge.sdk"
```

---

## 5. Visual Studio 2022 Integration

1. Open Visual Studio 2022.
2. Select **File -> Open -> Folder...** and choose the `cubeforge.loader` directory.
3. Select **`x64-Release`** from the configuration dropdown.
4. Press **`Ctrl + Shift + B`** to build all components.

---

## 6. Testing & Deployment

1. Copy `CubeForgeLoader.dll` (or `CubeForgeLoader.fip`) into your root *Cube World* directory (where `cubeworld.exe` is located).
2. Create a `Mods/` folder in the same directory:
   ```text
   <Cube World Directory>/
   ├── cubeworld.exe
   ├── CubeForgeLoader.dll (or CubeForgeLoader.fip)
   ├── Mods/
   │   ├── CustomMod1.dll
   │   └── CustomMod2.dll
   ```
3. Launch Cube World via Steam or directly via `cubeworld.exe` with `CubeForgeLoader.fip` (Steam FIP hook) in place.

---

## 7. Writing Mods

To develop custom mods for Cube World, use the official starter template:
👉 **[CubeForge Mod Template](https://github.com/Gildofj/cubeforge.mod-template)**
