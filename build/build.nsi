OutFile "Installer.exe"

InstallDir "C:\Program Files\Exatek\ExaGameBooster"

RequestExecutionLevel admin

Section
    ExecWait 'taskkill /F /IM "ExaGameBooster.exe"'

    SetOutPath $INSTDIR
    File "ExaGameBooster.exe"

    # autostart

    SetRegView 64
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "ExaGameBooster" '"$InstDir\ExaGameBooster.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "DisplayIcon" "$INSTDIR\ExaGameBooster.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "DisplayName" "ExaGameBooster"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "Publisher" "Exatek Deutschland"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "DisplayVersion" "3.0.0.0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "Major Version" "3"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "Minor Version" "0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"

    # create the uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"

    Exec '"$INSTDIR\ExaGameBooster.exe"'

    # old cleanup, keep
    Delete "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup\ExaGameBooster.exe"
    Delete "$SMPROGRAMS\Startup\ExaGameBooster.exe"

    MessageBox MB_OK "ExaGameBooster was successfully installed."
SectionEnd

Section "uninstall"
    ExecWait 'taskkill /F /IM "ExaGameBooster.exe"'

    SetRegView 64
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ExaGameBooster"
    DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "ExaGameBooster"

    Delete "$INSTDIR\*.*"

    Delete $INSTDIR\ExaGameBooster.exe

    # uninstaller
    Delete $INSTDIR\uninstaller.exe

    RMDir /r "$INSTDIR"
SectionEnd