@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by PUMPKIN.HPJ. >"help\pumpkin.hm"
echo. >>"help\pumpkin.hm"
echo // Commands (ID_* and IDM_*) >>"help\pumpkin.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"help\pumpkin.hm"
echo. >>"help\pumpkin.hm"
echo // Prompts (IDP_*) >>"help\pumpkin.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"help\pumpkin.hm"
echo. >>"help\pumpkin.hm"
echo // Resources (IDR_*) >>"help\pumpkin.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"help\pumpkin.hm"
echo. >>"help\pumpkin.hm"
echo // Dialogs (IDD_*) >>"help\pumpkin.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"help\pumpkin.hm"
echo. >>"help\pumpkin.hm"
echo // Frame Controls (IDW_*) >>"help\pumpkin.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"help\pumpkin.hm"
REM -- Make help for Project PUMPKIN

echo Building RTF file
xsltproc -o help/pumpkin.rtf shared-code/kinhelp.xsl help/pumpkin.xml
echo Building Win32 Help files
start /wait hcrtf -x "help\pumpkin.hpj"
echo.
if exist Debug\nul copy "help\pumpkin.hlp" Debug
if exist Debug\nul copy "help\pumpkin.cnt" Debug
if exist Release\nul copy "help\pumpkin.hlp" Release
if exist Release\nul copy "help\pumpkin.cnt" Release
if exist Releast\nul copy "help\pumpkin.hlp" Releast
if exist Releast\nul copy "help\pumpkin.cnt" Releast
echo.
