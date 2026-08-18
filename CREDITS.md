# 🌟 Credits & Technical Lineage

**CubeForge Loader & Launcher** (formerly Cube-World-Mod-Launcher) is the runtime modding framework, binary injector, and hook engine for Cube World (Steam release 1.0.0-1).

---

## 🏛️ Technical Lineage

```
[Andoryuuta / matpow2]  ── (Reverse engineering, memory offsets & early injection tools)
         │
         ▼
  [ChrisMiuchiz]        ── (Cube-World-Mod-Launcher, ASM Trampoline engine & FIP hook)
         │
         ▼
[Gijs Groenewegen]      ── (Substantial loader refinements, memory patches & maintenance 2020-2022)
         │
         ▼
[CubeForge / Gildo FJ]  ── (C++20 modernization, MSVC/Clang toolchain integration, ImGui runtime,
                            Robust ASM hooking, CRC32 checks & Complete Documentation)
```

---

## 👥 Contributors & Key Acknowledgments

* **[Picroma (Wollay & Pixwer)](https://picroma.com/)**
  * Creators of *Cube World*.

* **[ChrisMiuchiz](https://github.com/ChrisMiuchiz)**
  * Original creator and author of **Cube-World-Mod-Launcher** and **CubeModLoader**. Designed the low-level assembly trampoline hooking engine and early FreeImage runtime patches.

* **[Gijs Groenewegen](https://github.com/gijsgroenewegen)**
  * Maintained and developed the project throughout 2020–2022 ([gijsgroenewegen/Cube-World-Mod-Launcher](https://github.com/gijsgroenewegen/Cube-World-Mod-Launcher)). Introduced essential hook refinements, offset updates, and stability fixes.

* **[Gildo FJ](https://github.com/Gildofj)**
  * Lead maintainer of **CubeForge**. Modernized the entire build pipeline to C++20 with MSVC / Clang-cl, integrated Dear ImGui directly into the DirectX 11 rendering pipeline, refined low-level 64-bit trampolines, and added automated test suites and architectural documentation.

* **[Andoryuuta](https://github.com/Andoryuuta)**
  * Disassembly analysis, reverse engineering of the game engine, and early injection mechanics.

* **[matpow2](https://github.com/matpow2)**
  * Alpha version offsets, memory scanning techniques, and reverse-engineering contributions.

* **[ZaneYork](https://github.com/ZaneYork)**
  * Contributions to early mod loading mechanics, structures, and `CommandsMod`.

* **[Omar Cornut & ImGui Contributors](https://github.com/ocornut/imgui)**
  * Authors of Dear ImGui, used for modern in-game overlays and UI.

---

## 📄 License

CubeForge Loader is released under the **MIT License**. See [`LICENSE`](LICENSE) for complete copyright terms.
