@echo off
setlocal

:: ============================================================
:: build.bat
:: Builds ExaGameBooster.exe (Release|x64) using MSBuild and
:: copies the output to the 'build' folder so the Inno Setup
:: installer script can pick it up.
:: ============================================================

set "REPO_ROOT=%~dp0.."
set "SLN_FILE=%REPO_ROOT%\src\ExaGameBooster.sln"
set "BUILD_OUTPUT=%REPO_ROOT%\src\ExaGameBooster\x64\Release\ExaGameBooster.exe"
set "DIST_DIR=%REPO_ROOT%\dist"

:: ---- Locate MSBuild ----
set "MSBUILD="

:: Visual Studio 2022
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
:: Visual Studio 2019
) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
) else (
    :: Try PATH
    where MSBuild.exe >nul 2>&1
    if %errorlevel% equ 0 (
        set "MSBUILD=MSBuild.exe"
    ) else (
        echo ERROR: MSBuild.exe not found.
        echo Please install Visual Studio 2019/2022 (with C++ workload) or Build Tools.
        exit /b 1
    )
)

echo Using MSBuild: %MSBUILD%
echo Building solution: %SLN_FILE%
echo Configuration: Release^|x64
echo.

:: Build Release|x64
"%MSBUILD%" "%SLN_FILE%" /p:Configuration=Release /p:Platform=x64 /m /nologo /verbosity:minimal

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Build failed with exit code %errorlevel%.
    exit /b %errorlevel%
)

:: Copy the built executable to the dist folder
if not exist "%BUILD_OUTPUT%" (
    echo ERROR: Build succeeded but output not found at:
    echo   %BUILD_OUTPUT%
    exit /b 1
)

if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"

echo.
echo Copying %BUILD_OUTPUT% to %DIST_DIR%\ExaGameBooster.exe
copy /Y "%BUILD_OUTPUT%" "%DIST_DIR%\ExaGameBooster.exe" >nul

if %errorlevel% neq 0 (
    echo ERROR: Failed to copy executable.
    exit /b %errorlevel%
)

echo Build complete: %DIST_DIR%\ExaGameBooster.exe

endlocal
