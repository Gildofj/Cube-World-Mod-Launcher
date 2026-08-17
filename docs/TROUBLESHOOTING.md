# Troubleshooting & FAQ

## 1. Common Runtime Errors

### 1.1 "cubeworld.exe does not seem to be version 1.0.0-1. CRC XXXXXXXX"
* **Cause**: `CubeModLoader` verifies that the target executable matches known CRC32 checksums:
  - Steam Packed: `0xC7682619`
  - Steam Unpacked: `0xBA092543`
* **Resolution**: Ensure you are running the official Steam release version `1.0.0-1` of Cube World. If using a modified or altered executable, disable CRC checking during local compilation by defining `#undef USE_CHECKSUM` in `CubeModLoader/main.cpp`.

---

### 1.2 "cubeworld.exe not found" or "CubeModLoader.dll not found"
* **Cause**: `CubeModLauncher.exe` searches the current working directory for `cubeworld.exe` and `CubeModLoader.dll`.
* **Resolution**:
  1. Ensure both `CubeModLauncher.exe` and `CubeModLoader.dll` are placed in the exact same directory as `cubeworld.exe`.
  2. Launch `CubeModLauncher.exe` directly or pass the absolute path to `cubeworld.exe` as a CLI argument:
     ```cmd
     CubeModLauncher.exe "C:\Path\To\Cube World\cubeworld.exe"
     ```

---

### 1.3 Mod Version Incompatibility
* **Error**: `ModName.dll has major version X but requires Y.`
* **Cause**: The mod was compiled against an incompatible version of `CWSDK` or `CubeModLoader`.
* **Resolution**:
  - If the mod version is higher than loader version, update `CubeModLoader`.
  - If the mod version is lower, update the mod to target the latest `CWSDK` version (Major Version 7).

---

### 1.4 Antivirus False Positives
* **Cause**: `CubeModLauncher` utilizes `VirtualAllocEx`, `WriteProcessMemory`, and `CreateRemoteThread` to inject `CubeModLoader.dll` into `cubeworld.exe`. Heuristic anti-malware scanners frequently flag DLL injectors.
* **Resolution**: Add an exclusion in Windows Defender / Antivirus for the Cube World installation directory or build the launcher from source.

---

### 1.5 FreeImage.dll Crash on Windows 8 / 10 / 11
* **Cause**: Older builds of `FreeImage.dll` bundled with the game have incompatible memory management routines on modern Windows operating systems.
* **Resolution**: `CubeModLoader` automatically applies an in-memory NOP patch (`PatchFreeImage()`) at offsets `0x1E8C4E` and `0x1E8C62`. Ensure `CubeModLoader` is injected early before graphic rendering initializes.

---

## 2. Build & Compilation Issues

### 2.1 Missing `CWSDK/cwsdk.h`
* **Error**: `fatal error: CWSDK/cwsdk.h: No such file or directory`
* **Resolution**: Initialize git submodules:
  ```bash
  git submodule update --init --recursive
  ```

### 2.2 Inline Assembly Errors with MSVC
* **Error**: `__attribute__((naked))` or `asm(".intel_syntax")` syntax errors.
* **Resolution**: `CubeModLoader` requires **MinGW-w64 (GCC)** for x86_64 inline assembly support. MSVC x64 does not support inline `__asm` blocks. Use GCC 64-bit to build `CubeModLoader`.
