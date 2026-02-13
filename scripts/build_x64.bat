@echo off
setlocal EnableDelayedExpansion

set "ROOT=%~dp0.."
set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "!VSWHERE!" set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "!VSWHERE!" (
    echo [ERROR] vswhere.exe not found: !VSWHERE!
    exit /b 1
)

for /f "usebackq delims=" %%I in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%I"

if not defined VS_PATH if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" set "VS_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools"

if not defined VS_PATH (
    echo [ERROR] No Visual Studio C++ Build Tools installation found.
    exit /b 1
)

call "!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    exit /b 1
)

pushd "%ROOT%"
if not exist build\Release mkdir build\Release

cl /nologo /LD /EHsc /std:c++17 /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN /DNOMINMAX src\plugin.cpp /link /NOLOGO user32.lib /DEF:S2TConverter.def /OUT:build\Release\S2TConverter.dll
if errorlevel 1 (
    popd
    echo [ERROR] Build failed.
    exit /b 1
)

popd
echo [OK] build\Release\S2TConverter.dll
exit /b 0
