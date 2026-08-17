# Contributing Guidelines

Thank you for your interest in contributing to **Cube-World-Mod-Launcher**!

## 1. Development Workflow

1. **Fork the Repository**: Create your own fork and clone with submodules:
   ```bash
   git clone --recurse-submodules https://github.com/<your-username>/Cube-World-Mod-Launcher.git
   ```
2. **Create a Feature Branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Implement Your Changes**: Ensure strict adherence to project standards (see below).
4. **Compile & Test**: Build with MinGW-w64 and test against Cube World version `1.0.0-1`.
5. **Submit a Pull Request**: Provide a descriptive PR explanation outlining the motivation, changes, and testing validation steps.

---

## 2. Engineering & Code Standards

* **C++ & Compiler Compatibility**: All runtime hooks and assembly trampolines target GCC x86_64 (`x86_64-w64-mingw32-g++`). Ensure code remains compatible with MinGW-w64.
* **Assembly Trampolines**: Naked functions (`__attribute__((naked))`) must properly preserve registers using `PUSH_ALL` and `POP_ALL` macros to prevent stack corruption.
* **Separation of Concerns**: Keep process injection logic in `CubeModLauncher`, runtime hooking and GUI widgets in `CubeModLoader`, and mod domain contracts in `GenericMod.h`.
* **Clean Code**: Code should be self-documenting. Use comments to explain *why* a reverse-engineered offset or patch exists, not *what* standard C++ syntax does.

---

## 3. Reporting Issues

When reporting bugs or crashes:
- Specify your Windows OS version (10/11 x64).
- Include the exact CRC32 checksum of your `cubeworld.exe` (reported in popup if mismatch occurs).
- List all mods present in your `Mods/` folder.
- Provide step-by-step reproduction instructions.
