@echo off
setlocal

:: ============================================================
:: build_installer.bat
:: Compiles the Inno Setup installer and places the output in
:: the 'dist' folder at the repository root.
:: ============================================================

:: Resolve the repository root (one level up from the bat folder)
set "REPO_ROOT=%~dp0.."
set "ISS_FILE=%REPO_ROOT%\build\build.iss"
set "DIST_DIR=%REPO_ROOT%\dist"

:: Create the dist folder if it does not exist
if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"

:: Locate ISCC.exe - check common install paths, then fall back to PATH
set "ISCC="

if exist "%PROGRAMFILES(X86)%\Inno Setup 6\ISCC.exe" (
    set "ISCC=%PROGRAMFILES(X86)%\Inno Setup 6\ISCC.exe"
) else if exist "%PROGRAMFILES%\Inno Setup 6\ISCC.exe" (
    set "ISCC=%PROGRAMFILES%\Inno Setup 6\ISCC.exe"
) else if exist "%PROGRAMFILES(X86)%\Inno Setup 5\ISCC.exe" (
    set "ISCC=%PROGRAMFILES(X86)%\Inno Setup 5\ISCC.exe"
) else (
    :: Try PATH
    where ISCC.exe >nul 2>&1
    if %errorlevel% equ 0 (
        set "ISCC=ISCC.exe"
    ) else (
        echo ERROR: Inno Setup compiler (ISCC.exe) not found.
        echo Please install Inno Setup from https://jrsoftware.org/isinfo.php
        echo or set the ISCC environment variable to the full path of ISCC.exe.
        exit /b 1
    )
)

echo Using Inno Setup compiler: %ISCC%
echo Building installer from: %ISS_FILE%
echo Output directory: %DIST_DIR%
echo.

:: Run the compiler, overriding the output directory to dist/
"%ISCC%" /O"%DIST_DIR%" "%ISS_FILE%"

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Installer build failed with exit code %errorlevel%.
    exit /b %errorlevel%
)

echo.
echo Installer built successfully: %DIST_DIR%\Installer.exe

endlocal
