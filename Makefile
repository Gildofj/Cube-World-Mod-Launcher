# ==============================================================================
# Cube World Mod Launcher & Loader Makefile
# Automation for MinGW-w64 / CMake build workflow
# ==============================================================================

BUILD_DIR ?= build
BUILD_TYPE ?= Release
GENERATOR ?= "MinGW Makefiles"

.PHONY: all loader launcher test clean install help

# Default target: builds everything (Launcher, Loader .dll + .fip, Tests)
all: configure
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) --parallel

# Configures the build directory with CMake
configure:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cmake -B $(BUILD_DIR) -S . -G $(GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

# Builds only CubeModLoader (generates CubeModLoader.dll and CubeModLoader.fip)
loader: configure
	@cmake --build $(BUILD_DIR) --target CubeModLoader --config $(BUILD_TYPE)

# Builds only CubeModLauncher (generates CubeModLauncher.exe)
launcher: configure
	@cmake --build $(BUILD_DIR) --target CubeModLauncher --config $(BUILD_TYPE)

# Builds and runs all unit & integration tests
test: configure
	@cmake --build $(BUILD_DIR) --target test_runner --config $(BUILD_TYPE)
	@ctest --test-dir $(BUILD_DIR) --output-on-failure

# Cleans build artifacts
clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo [OK] Build artifacts cleaned.

# Installs binaries to target Cube World directory
# Usage: make install DEST="C:/Program Files (x86)/Steam/steamapps/common/Cube World"
install:
	@if "$(DEST)"=="" ( \
		echo [ERROR] DEST directory not specified. Usage: make install DEST="C:/path/to/Cube World" \
		&& exit /b 1 \
	)
	@if exist $(BUILD_DIR)\CubeModLoader\CubeModLoader.fip ( \
		copy /y $(BUILD_DIR)\CubeModLoader\CubeModLoader.fip "$(DEST)\" && \
		echo [OK] Copied CubeModLoader.fip to "$(DEST)" \
	)
	@if exist $(BUILD_DIR)\CubeModLoader\CubeModLoader.dll ( \
		copy /y $(BUILD_DIR)\CubeModLoader\CubeModLoader.dll "$(DEST)\" && \
		echo [OK] Copied CubeModLoader.dll to "$(DEST)" \
	)
	@if exist $(BUILD_DIR)\CubeModLauncher\CubeModLauncher.exe ( \
		copy /y $(BUILD_DIR)\CubeModLauncher\CubeModLauncher.exe "$(DEST)\" && \
		echo [OK] Copied CubeModLauncher.exe to "$(DEST)" \
	)

help:
	@echo Available targets:
	@echo   make            - Build entire project (Launcher, Loader DLL & FIP, Tests)
	@echo   make loader     - Build only CubeModLoader (creates .dll and .fip)
	@echo   make launcher   - Build only CubeModLauncher (.exe)
	@echo   make test       - Build and run unit and integration tests
	@echo   make clean      - Remove build directory
	@echo   make install DEST="<path>" - Copy binaries to Cube World folder
