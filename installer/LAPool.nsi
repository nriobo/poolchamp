;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  !define MUI_ICON "LAPC.ico"

;--------------------------------
;General

  ;Name and file
  Name "LA (Los Altares) Pool Champ"
  OutFile "PoolChamp.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\PoolChamp"
  ;InstallDir "$PROGRAMFILES\PoolChamp"
    
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\LA PoolChamp" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "license.txt"
  ; !insertmacro MUI_PAGE_COMPONENTS
 

  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\PoolChamp" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\PoolChamp" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

 
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
 ; !insertmacro MUI_PAGE_FINISH

       # These indented statements modify settings for MUI_PAGE_FINISH
    !define MUI_FINISHPAGE_NOAUTOCLOSE
    !define MUI_FINISHPAGE_RUN
    !define MUI_FINISHPAGE_RUN_NOTCHECKED
    !define MUI_FINISHPAGE_RUN_TEXT "Play now"
	!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
	!define MUI_FINISHPAGE_SHOWREADME ""
	!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
	!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create desktop link"
	!define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction


  !insertmacro MUI_PAGE_FINISH


;Languages

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Spanish"
!define MUI_LANGDLL_ALWAYSSHOW

;--------------------------------
;Installer Sections

Section "Files (required)" SecDummy

  SetOutPath "$INSTDIR"

  SectionIn RO
  
  ;ADD YOUR OWN FILES HERE...
  ; File ..\AUTHORS.txt
  File irrKlang.dll
  File Irrlicht.dll
  File Newton.dll
  File Pool.exe
    
  File /r textures
  File /r data
	
  ;Store installation folder
  WriteRegStr HKCU "Software\PoolChamp" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\LA Pool Champ.lnk" "$INSTDIR\Pool.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Function LaunchLink
  ExecShell "" "$INSTDIR\Pool.exe"
FunctionEnd

Function finishpageaction
	CreateShortCut "$DESKTOP\LA Pool Champ.lnk" "$INSTDIR\Pool.exe"
FunctionEnd

	
;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "Game files."

  ;Assign language strings to sections
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  ;  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"
  
  Delete "$INSTDIR\irrKlang.dll"
  Delete "$INSTDIR\Irrlicht.dll"
  Delete "$INSTDIR\Newton.dll"
  Delete "$INSTDIR\Pool.exe"
  Delete "$INSTDIR\textures\*.*"
  Delete "$INSTDIR\data\*.*"
    
  RMDir "$INSTDIR\textures"
  RMDir "$INSTDIR\data"
  RMDir "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\LA Pool Champ.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKCU "Software\PoolChamp"

SectionEnd