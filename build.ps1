<#
.SYNOPSIS
    Cube World Mod Loader Build Script (MSVC x64) - Modernized via CMake Presets
.DESCRIPTION
    Script de automação para compilação unificada com MSVC x64 de CubeForgeLoader (.dll / .fip) e testes.
    Utiliza por debaixo dos panos o novo sistema de CMakePresets.json do CMake 3.25+.
.EXAMPLE
    .\build.ps1
    .\build.ps1 -Target loader
    .\build.ps1 -Target test
    .\build.ps1 -InstallPath "C:\Program Files (x86)\Steam\steamapps\common\Cube World"
#>

[CmdletBinding()]
param (
    [ValidateSet("all", "loader", "test", "clean")]
    [string]$Target = "all",

    [ValidateSet("Release", "Debug", "RelWithDebInfo")]
    [string]$BuildType = "Release",

    [string]$InstallPath = ""
)

$ErrorActionPreference = "Stop"

# Map to the unified CMake Preset names
$presetName = "windows-release"
if ($BuildType -eq "Debug") {
    $presetName = "windows-debug"
}

$buildDir = "build/$presetName"

if ($Target -eq "clean") {
    if (Test-Path "build") {
        Write-Host "Limpando diretórios de build..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force "build"
        Write-Host "Build limpo com sucesso." -ForegroundColor Green
    }
    exit 0
}

Write-Host "Configurando CMake usando o Preset: $presetName..." -ForegroundColor Cyan
cmake --preset $presetName

switch ($Target) {
    "all" {
        Write-Host "Compilando todo o projeto usando o Preset: $presetName..." -ForegroundColor Cyan
        cmake --build --preset $presetName --parallel
    }
    "loader" {
        Write-Host "Compilando target CubeForgeLoader..." -ForegroundColor Cyan
        cmake --build --preset $presetName --target CubeForgeLoader
    }
    "test" {
        Write-Host "Compilando e executando testes via CTest..." -ForegroundColor Cyan
        cmake --build --preset $presetName --target test_runner
        ctest --preset "windows-test"
    }
}

if ($InstallPath -ne "") {
    if (-not (Test-Path $InstallPath)) {
        Write-Error "Diretório de instalação não encontrado: $InstallPath"
    }

    Write-Host "Instalando binários em $InstallPath..." -ForegroundColor Magenta

    # Support both single-config generators (Ninja) and multi-config (Visual Studio) output structures
    $fipCandidates = @(
        (Join-Path $buildDir "src/CubeForgeLoader.fip"),
        (Join-Path $buildDir "src/$BuildType/CubeForgeLoader.fip")
    )
    $dllCandidates = @(
        (Join-Path $buildDir "src/CubeForgeLoader.dll"),
        (Join-Path $buildDir "src/$BuildType/CubeForgeLoader.dll")
    )

    $fipPath = $fipCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
    $dllPath = $dllCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

    if ($fipPath) {
        Copy-Item -Path $fipPath -Destination $InstallPath -Force
        Write-Host " -> Copiado CubeForgeLoader.fip ($fipPath)" -ForegroundColor Green
    } else {
        Write-Warning "CubeForgeLoader.fip não encontrado nos caminhos candidatos."
    }

    if ($dllPath) {
        Copy-Item -Path $dllPath -Destination $InstallPath -Force
        Write-Host " -> Copiado CubeForgeLoader.dll ($dllPath)" -ForegroundColor Green
    } else {
        Write-Warning "CubeForgeLoader.dll não encontrado nos caminhos candidatos."
    }
}

Write-Host "Processo concluído com sucesso!" -ForegroundColor Green
