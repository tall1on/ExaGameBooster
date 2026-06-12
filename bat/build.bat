@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: build.bat
:: Builds ExaGameBooster.exe (Release|x64) using MSBuild,
:: copies the output to the 'build' folder, and optionally
:: Authenticode-signs the binary to prevent AV false positives.
::
:: Code Signing (optional but strongly recommended):
::   Set one of the following environment variables before running:
::
::   SIGN_PFX      = full path to your .pfx certificate file
::   SIGN_PFX_PASS = password for the .pfx file (if needed)
::   SIGN_SUBJECT  = subject name of a cert already in the local
::                   Windows certificate store (My store)
::
::   Example (PFX):
::     set SIGN_PFX=C:\certs\MyCodeSign.pfx
::     set SIGN_PFX_PASS=mysecretpassword
::     bat\build.bat
::
::   Example (store):
::     set SIGN_SUBJECT=My Company Name
::     bat\build.bat
::
::   Without a certificate the build still succeeds but the exe
::   will be unsigned and may trigger AV heuristics on first run.
:: ============================================================

set "REPO_ROOT=%~dp0.."
set "SLN_FILE=%REPO_ROOT%\src\ExaGameBooster.sln"
set "BUILD_OUTPUT=%REPO_ROOT%\src\x64\Release\ExaGameBooster.exe"
set "DIST_DIR=%REPO_ROOT%\build"

:: ---- Locate MSBuild ----
set "MSBUILD="
set "PF=%ProgramFiles%"
set "PF86=%ProgramFiles(x86)%"

:: Visual Studio 2022 Community
if exist "%PF%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Visual Studio 2022 Professional
if exist "%PF%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Visual Studio 2022 Enterprise
if exist "%PF%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Visual Studio 2022 BuildTools
if exist "%PF%\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF%\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Visual Studio 2019 Community
if exist "%PF86%\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF86%\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Visual Studio 2019 Professional
if exist "%PF86%\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF86%\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Visual Studio 2019 Enterprise
if exist "%PF86%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF86%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Visual Studio 2019 BuildTools
if exist "%PF86%\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=%PF86%\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
    goto :msbuild_found
)
:: Try PATH
where MSBuild.exe >nul 2>&1
if %errorlevel% equ 0 (
    set "MSBUILD=MSBuild.exe"
    goto :msbuild_found
)
echo ERROR: MSBuild.exe not found.
echo Please install Visual Studio 2019/2022 (with C++ workload) or Build Tools.
exit /b 1

:msbuild_found
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

:: ---- Authenticode Signing (optional) ----
:: Locate signtool.exe from Windows SDK
set "SIGNTOOL="
set "WINSDK_BASE=%ProgramFiles(x86)%\Windows Kits\10\bin"
if not exist "%WINSDK_BASE%" set "WINSDK_BASE=%ProgramFiles%\Windows Kits\10\bin"

:: Search for signtool in known SDK versions (newest first via dir /b /ad /o-n)
if exist "%WINSDK_BASE%" (
    for /f "delims=" %%v in ('dir /b /ad /o-n "%WINSDK_BASE%" 2^>nul') do (
        if exist "%WINSDK_BASE%\%%v\x64\signtool.exe" (
            set "SIGNTOOL=%WINSDK_BASE%\%%v\x64\signtool.exe"
            goto :signtool_found
        )
    )
)

:: Fall back to PATH
where signtool.exe >nul 2>&1
if %errorlevel% equ 0 (
    set "SIGNTOOL=signtool.exe"
    goto :signtool_found
)

echo NOTE: signtool.exe not found. Skipping code signing.
echo       Install the Windows SDK or add signtool.exe to PATH.
goto :sign_done

:signtool_found
echo Using signtool: %SIGNTOOL%

:: Sign using a PFX file if SIGN_PFX is set
if defined SIGN_PFX (
    if not exist "%SIGN_PFX%" (
        echo WARNING: SIGN_PFX is set but file not found: %SIGN_PFX%
        goto :sign_done
    )
    echo Signing with PFX: %SIGN_PFX%
    if defined SIGN_PFX_PASS (
        "%SIGNTOOL%" sign /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 /f "%SIGN_PFX%" /p "%SIGN_PFX_PASS%" /d "ExaGameBooster" "%DIST_DIR%\ExaGameBooster.exe"
    ) else (
        "%SIGNTOOL%" sign /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 /f "%SIGN_PFX%" /d "ExaGameBooster" "%DIST_DIR%\ExaGameBooster.exe"
    )
    if %errorlevel% neq 0 (
        echo WARNING: Code signing failed. Unsigned exe is still in %DIST_DIR%.
        goto :sign_done
    )
    echo Signing successful.
    goto :sign_done
)

:: Sign using a certificate from the Windows certificate store if SIGN_SUBJECT is set
if defined SIGN_SUBJECT (
    echo Signing with store certificate subject: %SIGN_SUBJECT%
    "%SIGNTOOL%" sign /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 /n "%SIGN_SUBJECT%" /d "ExaGameBooster" "%DIST_DIR%\ExaGameBooster.exe"
    if %errorlevel% neq 0 (
        echo WARNING: Code signing failed. Unsigned exe is still in %DIST_DIR%.
        goto :sign_done
    )
    echo Signing successful.
    goto :sign_done
)

echo NOTE: No signing certificate configured.
echo       Set SIGN_PFX (path to .pfx) or SIGN_SUBJECT (cert store subject name)
echo       to enable Authenticode signing and eliminate AV false positives.

:sign_done

endlocal
