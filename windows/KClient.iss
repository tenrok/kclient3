; Dependencies
#define use_msiproduct
#define use_vc2013
;#define use_vc2015
#define use_vc2017

#define GetEnvEx(str Name) Local[0] = GetEnv(Name), StringChange(Local[0], '"', '');

; Global variables
#define ARCH                         GetEnvEx('ARCH')
#define KCLIENT_BIN_DIR              GetEnvEx('KCLIENT_BIN_DIR')
#define KCLIENT_SRC_DIR              '..\'
#define OPENSSL_BIN_DIR              GetEnvEx('OPENSSL_BIN_DIR')
#define MSVC_REDIST_DIR              GetEnvEx('MSVC_REDIST_DIR')
#define ICU_BIN_DIR                  GetEnvEx('ICU_BIN_DIR')
#define QT_DIR                       GetEnvEx('QT_DIR')
#define QT_BIN_DIR                   GetEnvEx('QT_BIN_DIR')
#define QT_PLUGINS_DIR               GetEnvEx('QT_PLUGINS_DIR')
#define QTWEBENGINE_DICTIONARIES_DIR GetEnvEx('QTWEBENGINE_DICTIONARIES_DIR')
#define AppCopyright                 'Copyright (C) ' + GetDateTimeString('yyyy', '', '') + ' KORNET LLC'
#define AppName                      'KClient'
#define AppExeName                   'kclient.exe'
#define AppExePath                   KCLIENT_BIN_DIR + '\' + AppExeName

;#define AppVersion() ParseVersion(AppExePath, Local[0], Local[1], Local[2], Local[3]), Str(Local[0]) + '.' + Str(Local[1]) + '.' + Str(Local[2]) + '.' + Str(Local[3]);
#define AppVersion                   GetEnvEx('APPVEYOR_BUILD_VERSION')

#if ARCH == "x64"
	#define AppVerName AppName + ' (x64)'
#else
	#define AppVerName AppName
#endif

#define OutputBaseFilename StringChange(AppName, ' ', '') + '-' + AppVersion + '-' + ARCH;

[Setup]
AppCopyright={#AppCopyright}
AppId={#AppVerName}
AppName={#AppName}
AppPublisher=KORNET LLC
AppPublisherURL=http://kornet35.ru
AppVerName={#AppVerName}
AppVersion={#AppVersion}
;ChangesAssociations=yes
DefaultDirName={pf}\KORNET\{#AppName}
DefaultGroupName=KORNET\{#AppName}
MinVersion=6.1.7600
OutputBaseFilename={#OutputBaseFilename}
OutputDir=.
PrivilegesRequired=admin
SetupIconFile=setup-icon.ico
Uninstallable=yes
UninstallDisplayIcon={app}\{#AppVersion}\{#AppExeName}
UninstallFilesDir={app}\uninst
VersionInfoCopyright=KORNET LLC
VersionInfoVersion={#AppVersion}
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp
#if ARCH == "x64"
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
#endif
; Downloading and installing dependencies will only work if the memo/ready page is enabled (default and current behaviour)
DisableReadyPage=no
DisableReadyMemo=no

;[Languages]
;Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl";

; Supported languages
;#include "scripts\lang\english.iss"
;#include "scripts\lang\german.iss"
;#include "scripts\lang\french.iss"
;#include "scripts\lang\italian.iss"
;#include "scripts\lang\dutch.iss"
#ifdef UNICODE
;#include "scripts\lang\chinese.iss"
;#include "scripts\lang\polish.iss"
#include "scripts\lang\russian.iss"
;#include "scripts\lang\japanese.iss"
#endif

[Types]
Name: "full"; Description: "{code:GetFullInstallation}"
Name: "compact"; Description: "{code:GetCompactInstallation}"
Name: "custom"; Description: "{code:GetCustomInstallation}"; Flags: iscustom

[Components]
Name: "main"; Description: "Основные компоненты"; Types: full compact custom; Flags: fixed
Name: "themes"; Description: "Темы"; Types: full;
Name: "plugins"; Description: "Расширения"; Types: full;
Name: "translations"; Description: "Переводы"; Types: full;

[Files]
Source: "{#KCLIENT_BIN_DIR}\plugins\*.dll"; DestDir: "{app}\{#AppVersion}\plugins"; Components: "plugins"; Flags: ignoreversion
Source: "{#KCLIENT_BIN_DIR}\COPYRIGHT.txt"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#KCLIENT_BIN_DIR}\{#AppExeName}"; DestDir: "{app}\{#AppVersion}"; Components: "main"; Flags: ignoreversion
Source: "{#KCLIENT_BIN_DIR}\KClientPrivate.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"; Flags: ignoreversion
Source: "{#KCLIENT_BIN_DIR}\qt.conf"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#OPENSSL_BIN_DIR}\libeay32.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#OPENSSL_BIN_DIR}\ssleay32.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#OPENSSL_BIN_DIR}\msvcr*.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"; Flags: skipifsourcedoesntexist
Source: "{#MSVC_REDIST_DIR}\*"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#ICU_BIN_DIR}\icudt54.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#ICU_BIN_DIR}\icuin54.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#ICU_BIN_DIR}\icuuc54.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\libEGL.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\libGLESv2.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\opengl32sw.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\D3Dcompiler_47.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Core.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Gui.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Network.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Positioning.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5PrintSupport.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Qml.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Quick.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5QuickWidgets.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Sql.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Svg.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5WinExtras.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5WebEngine.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5WebEngineCore.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5WebEngineWidgets.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5WebChannel.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\Qt5Widgets.dll"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_BIN_DIR}\QtWebEngineProcess.exe"; DestDir: "{app}\{#AppVersion}"; Components: "main"
Source: "{#QT_PLUGINS_DIR}\iconengines\qsvgicon.dll"; DestDir: "{app}\{#AppVersion}\iconengines"; Components: "main"
Source: "{#QT_PLUGINS_DIR}\imageformats\*.dll"; DestDir: "{app}\{#AppVersion}\imageformats"; Components: "main"
Source: "{#QT_PLUGINS_DIR}\platforms\qwindows.dll"; DestDir: "{app}\{#AppVersion}\platforms"; Components: "main"
Source: "{#QT_PLUGINS_DIR}\printsupport\windowsprintersupport.dll"; DestDir: "{app}\{#AppVersion}\printsupport"; Components: "main"
Source: "{#QT_DIR}\qml\QtQuick.2\*"; DestDir: "{app}\{#AppVersion}\qml\QtQuick.2"; Components: "main"
Source: "{#QT_DIR}\qml\QtWebEngine\*"; DestDir: "{app}\{#AppVersion}\qml\QtWebEngine"; Components: "main"
Source: "{#QT_DIR}\resources\*"; DestDir: "{app}\{#AppVersion}\resources"; Components: "main"
Source: "{#QT_PLUGINS_DIR}\sqldrivers\qsqlite.dll"; DestDir: "{app}\{#AppVersion}\sqldrivers"; Components: "main"
Source: "{#QT_PLUGINS_DIR}\styles\*.dll"; DestDir: "{app}\{#AppVersion}\styles"; Components: "main"
Source: "{#KCLIENT_SRC_DIR}\themes\chrome\*"; DestDir: "{app}\{#AppVersion}\themes\chrome"; Components: "themes"; Flags: recursesubdirs
Source: "{#KCLIENT_SRC_DIR}\themes\mac\*"; DestDir: "{app}\{#AppVersion}\themes\mac"; Components: "themes"; Flags: recursesubdirs
Source: "{#KCLIENT_SRC_DIR}\themes\windows\*"; DestDir: "{app}\{#AppVersion}\themes\windows"; Components: "main"; Flags: recursesubdirs
Source: "{#QT_DIR}\translations\qtwebengine_locales\*"; DestDir: "{app}\{#AppVersion}\translations\qtwebengine_locales"; Components: "translations";
; In some packages underline '_' is used and in some other packages dash '-' is used so we use wildcard
Source: "{#QTWEBENGINE_DICTIONARIES_DIR}\doc\README*en*US.txt"; DestDir: "{app}\{#AppVersion}\qtwebengine_dictionaries\doc"; Components: "translations";
Source: "{#QTWEBENGINE_DICTIONARIES_DIR}\doc\README*ru*RU.txt"; DestDir: "{app}\{#AppVersion}\qtwebengine_dictionaries\doc"; Components: "translations";
; in some packages *.bdic files use dash '-' instead of underline '_' followed by a version number. e.g. en-US-3-0.bdic
Source: "{#QTWEBENGINE_DICTIONARIES_DIR}\en*US*.bdic"; DestDir: "{app}\{#AppVersion}\qtwebengine_dictionaries"; Components: "translations"
Source: "{#QTWEBENGINE_DICTIONARIES_DIR}\ru*RU*.bdic"; DestDir: "{app}\{#AppVersion}\qtwebengine_dictionaries"; Components: "translations"

[Icons]
Name: "{group}\{#AppVerName}"; Filename: "{app}\{#AppVersion}\{#AppExeName}"; WorkingDir: "{app}\{#AppVersion}"
Name: "{commondesktop}\{#AppVerName}"; Filename: "{app}\{#AppVersion}\{#AppExeName}"; WorkingDir: "{app}\{#AppVersion}"; Tasks: "desktopicon"

[Registry]
Root: HKCR; Subkey: "KClientHTML"; ValueType: string; ValueName: ""; ValueData: "KClient HTML Document"; Flags: uninsdeletekey
Root: HKCR; Subkey: "KClientHTML"; ValueType: string; ValueName: "FriendlyTypeName"; ValueData: "KClient HTML Document"
Root: HKCR; Subkey: "KClientHTML\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#AppVersion}\{#AppExeName},0"                 
Root: HKCR; Subkey: "KClientHTML\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#AppVersion}\{#AppExeName}"" ""%1"""
Root: HKCR; Subkey: "KClientURL"; ValueType: string; ValueName: ""; ValueData: "KClient URL"; Flags: uninsdeletekey
Root: HKCR; Subkey: "KClientURL"; ValueType: string; ValueName: "FriendlyTypeName"; ValueData: "KClient URL"
Root: HKCR; Subkey: "KClientURL"; ValueType: string; ValueName: "URL Protocol"; ValueData: ""
Root: HKCR; Subkey: "KClientURL\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#AppVersion}\{#AppExeName},0"                 
Root: HKCR; Subkey: "KClientURL\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#AppVersion}\{#AppExeName}"" ""%1"""
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities"; ValueType: string; ValueName: "ApplicationDescription"; ValueData: "KClient browser"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities"; ValueType: string; ValueName: "ApplicationIcon"; ValueData: "{app}\{#AppVersion}\{#AppExeName},0"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities"; ValueType: string; ValueName: "ApplicationName"; ValueData: "{#AppName}"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities\FileAssociations"; ValueType: string; ValueName: ".htm"; ValueData: "KClientHTML"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities\FileAssociations"; ValueType: string; ValueName: ".html"; ValueData: "KClientHTML"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities\URLAssociations"; ValueType: string; ValueName: "http"; ValueData: "KClientURL"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities\URLAssociations"; ValueType: string; ValueName: "https"; ValueData: "KClientURL"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities\URLAssociations"; ValueType: string; ValueName: "ftp"; ValueData: "KClientURL"
;Root: HKLM; Subkey: "Software\KORNET\{#AppName}\Capabilities\Startmenu"; ValueType: string; ValueName: "StartMenuInternet"; ValueData: "{app}\{#AppVersion}\{#AppExeName}"
;Root: HKLM; Subkey: "Software\RegisteredApplications"; ValueType: string; ValueName: "{#AppName}"; ValueData: "Software\KORNET\{#AppName}\Capabilities"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}"; ValueType: string; ValueName: ""; ValueData: "{#AppName}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities"; ValueType: string; ValueName: "ApplicationDescription"; ValueData: "KClient browser"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities"; ValueType: string; ValueName: "ApplicationIcon"; ValueData: "{app}\{#AppVersion}\{#AppExeName},0"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities"; ValueType: string; ValueName: "ApplicationName"; ValueData: "{#AppName}"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities\FileAssociations"; ValueType: string; ValueName: ".htm"; ValueData: "KClientHTML"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities\FileAssociations"; ValueType: string; ValueName: ".html"; ValueData: "KClientHTML"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities\URLAssociations"; ValueType: string; ValueName: "http"; ValueData: "KClientURL"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities\URLAssociations"; ValueType: string; ValueName: "https"; ValueData: "KClientURL"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities\URLAssociations"; ValueType: string; ValueName: "ftp"; ValueData: "KClientURL"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities\Startmenu"; ValueType: string; ValueName: "StartMenuInternet"; ValueData: "{#AppName}"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#AppVersion}\{#AppExeName},0"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\InstallInfo"; ValueType: string; ValueName: "HideIconsCommand"; ValueData: """{app}\{#AppVersion}\{#AppExeName}"" --hideicons"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\InstallInfo"; ValueType: dword; ValueName: "IconsVisible"; ValueData: "1"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\InstallInfo"; ValueType: string; ValueName: "ReinstallCommand"; ValueData: """{app}\{#AppVersion}\{#AppExeName}"" --makedefaultbrowser"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\InstallInfo"; ValueType: string; ValueName: "ShowIconsCommand"; ValueData: """{app}\{#AppVersion}\{#AppExeName}"" --showicons"
Root: HKLM; Subkey: "Software\Clients\StartMenuInternet\{#AppName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#AppVersion}\{#AppExeName}"""
Root: HKLM; Subkey: "Software\RegisteredApplications"; ValueType: string; ValueName: "{#AppName}"; ValueData: "Software\Clients\StartMenuInternet\{#AppName}\Capabilities"; Flags: uninsdeletevalue

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"

[CustomMessages]
DependenciesDir=MyProgramDependencies
WindowsServicePack=Windows %1 Service Pack %2

; Shared code for installing the products
#include "scripts\products.iss"

; Helper functions
#include "scripts\products\stringversion.iss"
#include "scripts\products\winversion.iss"
#include "scripts\products\fileversion.iss"
#include "scripts\products\dotnetfxversion.iss"

; Actual products
#ifdef use_msiproduct
#include "scripts\products\msiproduct.iss"
#endif
#ifdef use_vc2013
#include "scripts\products\vcredist2013.iss"
#endif
#ifdef use_vc2015
#include "scripts\products\vcredist2015.iss"
#endif
#ifdef use_vc2017
#include "scripts\products\vcredist2017.iss"
#endif

[Code]
function InitializeSetup(): Boolean;
begin
	// Initialize windows version
	initwinversion();

#ifdef use_vc2013
	//SetForceX86(true); // Force 32-bit install of next products
	vcredist2013('12'); // Min allowed version is 12.0
	//SetForceX86(false); // Disable forced 32-bit install again
#endif
#ifdef use_vc2015
	vcredist2015('14'); // Min allowed version is 14.0
#endif
#ifdef use_vc2017
	vcredist2017('14'); // Min allowed version is 14.0
#endif

	Result := True;
end;

function GetUninstallString(): String;
var
	sUnInstPath: String;
	sUnInstallString: String;
begin
	sUnInstPath := ExpandConstant('SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#emit SetupSetting("AppId")}_is1');
	sUnInstallString := '';
	if not RegQueryStringValue(HKLM, sUnInstPath, 'UninstallString', sUnInstallString) then
		RegQueryStringValue(HKCU, sUnInstPath, 'UninstallString', sUnInstallString);
	if IsWin64 then
	begin
		sUnInstPath := ExpandConstant('SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\{#emit SetupSetting("AppId")}_is1');
		RegQueryStringValue(HKLM, sUnInstPath, 'UninstallString', sUnInstallString);
	end;
	Result := sUnInstallString;
end;

function IsUpgrade(): Boolean;
begin
	Result := (GetUninstallString() <> '');
end;

function UnInstallOldVersion(): Integer;
var
	sUnInstallString: String;
	iResultCode: Integer;
begin
	// Return Values:
	// 1 - uninstall string is empty
	// 2 - error executing the UnInstallString
	// 3 - successfully executed the UnInstallString

	// Default return value
	Result := 0;

	// Get the uninstall string of the old app
	sUnInstallString := GetUninstallString();
	if sUnInstallString <> '' then
	begin
		sUnInstallString := RemoveQuotes(sUnInstallString);
		if Exec(sUnInstallString, '/SILENT /NORESTART /SUPPRESSMSGBOXES', '', SW_HIDE, ewWaitUntilTerminated, iResultCode) then
			Result := 3
		else
			Result := 2;
	end
	else
		Result := 1;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
	case CurStep of
		ssInstall:
			begin
				if (IsUpgrade()) then
					UnInstallOldVersion();
			end;
	end;
end;

(*procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
	case CurUninstallStep of
		usPostUninstall:
			begin
				DelTree(ExpandConstant('{%LOCALAPPDATA}\KORNET\{#AppName}'), True, True, True);
			end;
	end;
end;*)

function GetFullInstallation(Param: String): String;
begin
	Result := SetupMessage(msgFullInstallation);
end;

function GetCustomInstallation(Param: String): String;
begin
	Result := SetupMessage(msgCustomInstallation);
end;

function GetCompactInstallation(Param: String): String;
begin
	Result := SetupMessage(msgCompactInstallation);
end;