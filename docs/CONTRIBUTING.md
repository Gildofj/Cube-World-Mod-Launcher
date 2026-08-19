# Contributing Guidelines

Thank you for your interest in contributing to **CubeForge Loader & Launcher**!

---

## 1. Development Workflow

1. **Fork & Clone**: Fork the repository on GitHub and clone locally:
   ```bash
   git clone https://github.com/<your-username>/cubeforge.loader.git
   cd cubeforge.loader
   ```
2. **Create a Feature Branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Implement Changes**: Adhere to the architectural standards below.
4. **Compile & Test**: Build using MSVC x64 (Visual Studio 2022) or Clang-cl:
   ```bash
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```
5. **Submit a Pull Request**: Provide a clear description of your changes, the motivation, and testing validation steps.

---

## 2. Engineering & Code Standards

* **C++ Standard**: C++20 standard (`CMAKE_CXX_STANDARD 20`). Avoid non-standard compiler-specific extensions unless guarded.
* **Toolchains**: Primary targets are Microsoft Visual C++ (`cl.exe` v143) and Clang-cl on 64-bit Windows.
* **Assembly Trampolines**: Low-level 64-bit hooks are maintained in `trampolines.asm` and assembled via Microsoft Macro Assembler (`ml64.exe`). Always preserve register states (`PUSH_ALL_REGS` / `POP_ALL_REGS`) and align the stack to 16-byte boundaries before calling C++ handlers.
* **Separation of Concerns**:
  - `CubeModLauncher`: Standalone process injector.
  - `CubeModLoader`: In-game runtime engine, assembly trampolines, and UI widget (`ModWidget`).
  - `cubeforge.sdk`: Reconstructed game structures, math primitives, and API interfaces.
* **Clean Code**: Code must be self-documenting. Use comments only to explain *why* an offset, calling convention, or patch exists.

---

## 3. Reporting Issues

When reporting bugs or crashes:
- Specify your Windows OS version (10/11 x64).
- Include the exact CRC32 checksum of your `cubeworld.exe`.
- List all mods loaded in your `Mods/` directory.
- Provide step-by-step reproduction instructions.
