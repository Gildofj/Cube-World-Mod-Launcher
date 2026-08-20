# Troubleshooting & FAQ

## 1. Common Runtime Errors

### 1.1 "cubeworld.exe does not seem to be version 1.0.0-1. CRC XXXXXXXX"
* **Cause**: `CubeForgeLoader` verifies that the target executable matches known CRC32 checksums:
  - Steam Packed: `0xC7682619`
  - Steam Unpacked: `0xBA092543`
* **Resolution**: Ensure you are running the official Steam release version `1.0.0-1` of Cube World. If using a modified executable, disable CRC checking during local compilation by defining `#undef USE_CHECKSUM` in `CubeForgeLoader/main.cpp`.

---

### 1.2 Mod Version Incompatibility
* **Error**: `ModName.dll has major version X but requires Y.`
* **Cause**: The mod was compiled against an incompatible major version of CubeForge SDK or CubeForgeLoader.
* **Resolution**: Update the mod to target the latest CubeForge SDK (Major Version 7) and rebuild.

---

### 1.3 FreeImage.dll Crash on Windows 8 / 10 / 11
* **Cause**: Older builds of `FreeImage.dll` bundled with the game have incompatible memory management routines on modern Windows operating systems.
* **Resolution**: `CubeForgeLoader` automatically applies an in-memory NOP patch (`PatchFreeImage()`) at offsets `0x1E8C4E` and `0x1E8C62` to prevent this crash.

---

## 2. Build & Compilation Issues

### 2.1 FetchContent Network Issues / Offline Development
* **Error**: CMake fails to download `cubeforge_sdk` or `imgui`.
* **Resolution**: Ensure internet access during initial CMake configuration, or point CMake to your local clone of the SDK:
  ```bash
  cmake -B build -S . -DFETCHCONTENT_SOURCE_DIR_CUBEFORGE_SDK="D:/Projects/cubeforge.sdk"
  ```

### 2.2 Assembly Hook Trampolines (`ml64.exe`)
* **Error**: `ml64.exe: command not found`
* **Resolution**: Ensure you are running CMake from a **Visual Studio Developer Command Prompt / PowerShell** (or have Visual Studio C++ build tools on your PATH). `ml64.exe` is the standard Microsoft Macro Assembler included with MSVC Desktop development tools.
