@echo off

:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
    IF "%PROCESSOR_ARCHITECTURE%" EQU "amd64" (
>nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) ELSE (
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params= %*
    echo UAC.ShellExecute "cmd.exe", "/c ""%~s0"" %params:"=""%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"
:--------------------------------------
taskkill /F /IM CPUAffinityGameBooster.exe
taskkill /F /IM ExaGameBooster.exe
rmdir "C:\Program Files\Exatek\CPUAffinityGameBooster" /s /q
rmdir "C:\Program Files\Exatek\ExaGameBooster" /s /q
mkdir "C:\Program Files\Exatek\ExaGameBooster"

echo F | xcopy /S /Q /Y /F "%~dp0\ExaGameBooster.exe" "C:\Program Files\Exatek\ExaGameBooster\ExaGameBooster.exe*"
del /f "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup\CPUAffinityGameBooster.exe"
del /f "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup\ExaGameBooster.exe"
mklink "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup\ExaGameBooster.exe" "C:\Program Files\Exatek\ExaGameBooster\ExaGameBooster.exe"
start /d "C:\Program Files\Exatek\ExaGameBooster" C:\Program Files\Exatek\ExaGameBooster\ExaGameBooster.exe
