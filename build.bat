@echo off
REM build.bat — Build script for NetStorm-Like (Windows)
REM Requires: Visual Studio 2022 with MSVC, vcpkg with raylib installed
setlocal

set "VCPKG_ROOT=S:\programming\my\vcpkg"
set "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "MSVC_BIN=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.40.33807\bin\Hostx64\x64"
set "BUILD_DIR=%~dp0build"

echo === NetStorm-Like Build Script ===
echo vcpkg:   %VCPKG_ROOT%
echo Build:   %BUILD_DIR%
echo.

REM Check vcpkg
if not exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
    echo ERROR: vcpkg not found at %VCPKG_ROOT%
    echo Set VCPKG_ROOT env var or install vcpkg
    exit /b 1
)

REM Check compiler
if not exist "%MSVC_BIN%\cl.exe" (
    echo ERROR: MSVC compiler not found at %MSVC_BIN%
    echo Make sure Visual Studio 2022 is installed
    exit /b 1
)

REM Clean build dir
if exist "%BUILD_DIR%" (
    echo --- Cleaning build dir ---
    rmdir /s /q "%BUILD_DIR%"
)

REM Configure (skip manifest install since raylib is already installed)
echo --- Configuring CMake ---
set VCPKG_MANIFEST_MODE=OFF
"%CMAKE_EXE%" -B "%BUILD_DIR%" -S "%~dp0." ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
    -DCMAKE_C_COMPILER="%MSVC_BIN%\cl.exe" ^
    -DCMAKE_CXX_COMPILER="%MSVC_BIN%\cl.exe" ^
    -DBUILD_GAME=OFF
if errorlevel 1 exit /b 1

REM Build tests
echo.
echo --- Building tests ---
"%CMAKE_EXE%" --build "%BUILD_DIR%" --config Release
if errorlevel 1 exit /b 1

REM Run tests
echo.
echo --- Running tests ---
if exist "%BUILD_DIR%\Release\netstorm_tests.exe" (
    "%BUILD_DIR%\Release\netstorm_tests.exe" --gtest_brief=1
) else (
    echo WARNING: Tests executable not found at %BUILD_DIR%\Release\netstorm_tests.exe
)

echo.
echo === Build complete ===
