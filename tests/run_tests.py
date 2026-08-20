#!/usr/bin/env python3
import os
import sys
import subprocess
import shutil

def main():
    workspace_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    build_dir = os.path.join(workspace_dir, "build_tests")

    print(f"[*] Workspace: {workspace_dir}")
    print(f"[*] Build Dir: {build_dir}")

    # Clang path
    clang_exe = r"C:\Program Files\LLVM\bin\clang++.exe"
    cmake_exe = "cmake"

    os.makedirs(build_dir, exist_ok=True)

    test_sources = [
        os.path.join(workspace_dir, "tests", "test_main.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_crc.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_priority_callbacks.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_version_compat.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_modwidget_storage.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_modwidget_pagination.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_mutex.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_trampoline.cpp"),
        os.path.join(workspace_dir, "tests", "unit", "test_launcher_process.cpp"),
        os.path.join(workspace_dir, "tests", "integration", "test_mod_lifecycle.cpp"),
        os.path.join(workspace_dir, "tests", "integration", "test_fault_injection.cpp"),
        os.path.join(workspace_dir, "src", "crc.cpp"),
        os.path.join(workspace_dir, "src", "mutex.cpp"),
    ]

    output_exe = os.path.join(build_dir, "test_runner.exe")

    compile_cmd = [
        clang_exe,
        "-std=c++17",
        "-O2",
        "-D_CRT_SECURE_NO_WARNINGS",
        f"-I{os.path.join(workspace_dir, 'src')}",
        f"-I{os.path.join(workspace_dir, 'tests')}",
        *test_sources,
        "-o", output_exe
    ]

    print("[*] Compiling test suite...")
    res = subprocess.run(compile_cmd)
    if res.returncode != 0:
        print("[!] Compilation failed!")
        return res.returncode

    print("[*] Running test suite...\n")
    run_res = subprocess.run([output_exe])
    return run_res.returncode

if __name__ == "__main__":
    sys.exit(main())
