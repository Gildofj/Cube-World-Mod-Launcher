<#
.SYNOPSIS
    Cube World Mod Launcher & Loader Build Script
.DESCRIPTION
    Script de automação para compilação unificada de CubeModLauncher, CubeModLoader (.dll / .fip) e testes.
.EXAMPLE
    .\build.ps1
    .\build.ps1 -Target loader
    .\build.ps1 -Target test
    .\build.ps1 -InstallPath "C:\Program Files (x86)\Steam\steamapps\common\Cube World"
#>

[CmdletBinding()]
param (
    [ValidateSet("all", "loader", "launcher", "test", "clean")]
    [string]$Target = "all",

    [ValidateSet("Release", "Debug", "RelWithDebInfo")]
    [string]$BuildType = "Release",

    [string]$BuildDir = "build",

    [string]$InstallPath = ""
)

$ErrorActionPreference = "Stop"

if ($Target -eq "clean") {
    if (Test-Path $BuildDir) {
        Write-Host "Limpando diretório $BuildDir..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $BuildDir
        Write-Host "Build limpo com sucesso." -ForegroundColor Green
    }
    exit 0
}

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

Write-Host "Configurando CMake ($BuildType)..." -ForegroundColor Cyan
cmake -B $BuildDir -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=$BuildType

switch ($Target) {
    "all" {
        Write-Host "Compilando todo o projeto..." -ForegroundColor Cyan
        cmake --build $BuildDir --config $BuildType --parallel
    }
    "loader" {
        Write-Host "Compilando CubeModLoader (.dll e .fip)..." -ForegroundColor Cyan
        cmake --build $BuildDir --target CubeModLoader --config $BuildType
    }
    "launcher" {
        Write-Host "Compilando CubeModLauncher..." -ForegroundColor Cyan
        cmake --build $BuildDir --target CubeModLauncher --config $BuildType
    }
    "test" {
        Write-Host "Compilando e executando testes..." -ForegroundColor Cyan
        cmake --build $BuildDir --target test_runner --config $BuildType
        ctest --test-dir $BuildDir --output-on-failure
    }
}

if ($InstallPath -ne "") {
    if (-not (Test-Path $InstallPath)) {
        Write-Error "Diretório de instalação não encontrado: $InstallPath"
    }

    Write-Host "Instalando binários em $InstallPath..." -ForegroundColor Magenta

    $fipPath = Join-Path $BuildDir "CubeModLoader\CubeModLoader.fip"
    $dllPath = Join-Path $BuildDir "CubeModLoader\CubeModLoader.dll"
    $exePath = Join-Path $BuildDir "CubeModLauncher\CubeModLauncher.exe"

    if (Test-Path $fipPath) {
        Copy-Item -Path $fipPath -Destination $InstallPath -Force
        Write-Host " -> Copiado CubeModLoader.fip" -ForegroundColor Green
    }
    if (Test-Path $dllPath) {
        Copy-Item -Path $dllPath -Destination $InstallPath -Force
        Write-Host " -> Copiado CubeModLoader.dll" -ForegroundColor Green
    }
    if (Test-Path $exePath) {
        Copy-Item -Path $exePath -Destination $InstallPath -Force
        Write-Host " -> Copiado CubeModLauncher.exe" -ForegroundColor Green
    }
}

Write-Host "Processo concluído com sucesso!" -ForegroundColor Green
