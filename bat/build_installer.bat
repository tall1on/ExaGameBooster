@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: build_installer.bat
:: Compiles the Inno Setup installer.
:: Installer output goes to build\Output\Installer.exe
:: ============================================================

:: Resolve the repository root (one level up from the bat folder)
set "REPO_ROOT=%~dp0.."
set "ISS_FILE=%REPO_ROOT%\build\build.iss"
set "PF=%ProgramFiles%"
set "PF86=%ProgramFiles(x86)%"

:: Locate ISCC.exe
set "ISCC="

if exist "!PF86!\Inno Setup 6\ISCC.exe" (
    set "ISCC=!PF86!\Inno Setup 6\ISCC.exe"
    goto :iscc_found
)
if exist "!PF!\Inno Setup 6\ISCC.exe" (
    set "ISCC=!PF!\Inno Setup 6\ISCC.exe"
    goto :iscc_found
)
if exist "!PF86!\Inno Setup 5\ISCC.exe" (
    set "ISCC=!PF86!\Inno Setup 5\ISCC.exe"
    goto :iscc_found
)
if exist "!PF!\Inno Setup 5\ISCC.exe" (
    set "ISCC=!PF!\Inno Setup 5\ISCC.exe"
    goto :iscc_found
)
:: Try PATH
where ISCC.exe >nul 2>&1
if %errorlevel% equ 0 (
    set "ISCC=ISCC.exe"
    goto :iscc_found
)
echo ERROR: Inno Setup compiler (ISCC.exe) not found.
echo Please install Inno Setup from https://jrsoftware.org/isinfo.php
echo or add ISCC.exe to the PATH.
exit /b 1

:iscc_found
echo Using Inno Setup compiler: %ISCC%
echo Building installer from: %ISS_FILE%
echo Output: %REPO_ROOT%\build\Output\Installer.exe
echo.

:: Run the compiler - output directory is controlled by build.iss (OutputDir=Output)
"%ISCC%" "%ISS_FILE%"

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Installer build failed with exit code %errorlevel%.
    exit /b %errorlevel%
)

echo.
echo Installer built successfully: %REPO_ROOT%\build\Output\Installer.exe

endlocal
