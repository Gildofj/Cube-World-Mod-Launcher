# Development & Build Guide

## 1. Prerequisites & Toolchain

Building `Cube-World-Mod-Launcher` requires an x86_64 Windows compilation environment supporting GCC inline assembly (`.intel_syntax` / GNU C extensions):

* **Operating System**: Windows 10/11 64-bit
* **C++ Compiler**: MinGW-w64 (x86_64-w64-mingw32-gcc / g++)
* **Build System**: [CMake](https://cmake.org/) (Version 3.8 or higher)
* **Scripting**: Python 3.x (used for dynamic CMakeLists generation scripts)
* **SDK Dependency**: [CWSDK](https://github.com/ChrisMiuchiz/CWSDK) (located inside `CubeModLoader/CWSDK`)

---

## 2. Project Layout

```text
Cube-World-Mod-Launcher/
├── CubeModLauncher/                # Executable Process Injector
│   ├── CMakeLists.txt              # CMake configuration
│   ├── CMakeSettings.json          # Visual Studio / IDE CMake profile
│   ├── GenerateProjectCMake.py     # Python script to regenerate CMakeLists.txt
│   ├── Process.h / Process.cpp     # Process creation and DLL injection logic
│   └── main.h / main.cpp           # Launcher CLI entry point
│
├── CubeModLoader/                  # Core Injected DLL & Mod Runtime
│   ├── CMakeLists.txt              # CMake configuration
│   ├── CMakeSettings.json          # Visual Studio / IDE CMake profile
│   ├── GenerateProjectCMake.py     # Python script to regenerate CMakeLists.txt
│   ├── CWSDK/                      # Submodule / SDK headers (cwsdk.h)
│   ├── callbacks/                  # Hook trampolines and domain handlers
│   │   ├── creature/               # Creature death, armor, equip hooks
│   │   ├── game/                   # Game loop, mouse hooks
│   │   ├── gui/                    # Plasma UI, start menu, character preview
│   │   ├── item/                   # Item pricing, gold bag, wearability
│   │   └── world/                  # World generation and chunk remeshing
│   ├── GenericMod.h                # Base class interface for all mods
│   ├── ModWidget.h / .cpp          # In-game mod management widget
│   ├── DLL.h / DLL.cpp             # Dynamic library loader wrapper
│   ├── crc.h / crc.cpp             # Executable CRC32 validator
│   ├── macros.h                    # ASM trampoline macros & stack helpers
│   └── main.h / main.cpp           # DllMain, initterm_e hook, mod discovery
│
├── docs/                           # Technical documentation & guides
├── README.md                       # Repository overview & quickstart
└── steam_appid.txt                 # Steam AppID configuration (1128000)
```

---

## 3. Cloning with Submodules

To clone the repository and initialize the nested `CWSDK` submodule:

```bash
git clone --recurse-submodules https://github.com/Gildofj/cubeforge.loader.git
cd cubeforge.loader
```

If already cloned without submodules:
```bash
git submodule update --init --recursive
```

---

## 4. Building with CMake

### 4.1 Building `CubeModLauncher`

```bash
cd CubeModLauncher
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
This produces `CubeModLauncher.exe`.

### 4.2 Building `CubeModLoader`

```bash
cd CubeModLoader
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
This produces `CubeModLoader.dll`.

---

## 5. CMake Project Generator Scripts

Both `CubeModLauncher` and `CubeModLoader` include a `GenerateProjectCMake.py` script. When new `.cpp` files are added to the directories, execute:

```bash
# Inside CubeModLauncher or CubeModLoader
python GenerateProjectCMake.py
```
This automatically enumerates `.cpp` source files and writes a clean `CMakeLists.txt`.

---

## 6. Testing & Deployment

1. Copy `CubeModLauncher.exe` and `CubeModLoader.dll` into your root *Cube World* directory (where `cubeworld.exe` is located).
2. Create a `Mods/` folder in the same directory:
   ```text
   <Cube World Directory>/
   ├── cubeworld.exe
   ├── CubeModLauncher.exe
   ├── CubeModLoader.dll (or CubeModLoader.fip)
   ├── Mods/
   │   ├── CustomMod1.dll
   │   └── CustomMod2.dll
   ```
3. Run `CubeModLauncher.exe` to launch the game with the injector active.
