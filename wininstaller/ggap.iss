[Setup]
AppName=GGAP
AppVerName=GGAP 0.5.2
AppPublisher=Yevgen Muntyan
AppPublisherURL=http://ggap.sourceforge.net/
AppSupportURL=http://ggap.sourceforge.net/
AppUpdatesURL=http://ggap.sourceforge.net/
DefaultDirName={pf}\GGAP
DefaultGroupName=GGAP
AllowNoIcons=yes
; LicenseFile=C:\ggap\license.txt
Compression=lzma
SolidCompression=yes

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: C:\ggap\ggap.exe; DestDir: {app}; Flags: ignoreversion
Source: C:\ggap\cygwin1.dll; DestDir: {app}; Flags: ignoreversion
Source: C:\ggap\termhelper.exe; DestDir: {app}; Flags: ignoreversion
Source: C:\ggap\pkg\*; DestDir: {app}\pkg; Flags: ignoreversion recursesubdirs
Source: C:\ggap\language-specs\*; DestDir: {app}\language-specs; Flags: ignoreversion recursesubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: {group}\GGAP; Filename: {app}\ggap.exe
Name: {group}\{cm:UninstallProgram,GGAP}; Filename: {uninstallexe}
Name: {userdesktop}\GGAP; Filename: {app}\ggap.exe; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\GGAP; Filename: {app}\ggap.exe; Tasks: quicklaunchicon

[Run]
Filename: {app}\ggap.exe; Description: {cm:LaunchProgram,GGAP}; Flags: nowait postinstall skipifsilent
