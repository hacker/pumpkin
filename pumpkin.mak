# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Install - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Install - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PumpKIN - Win32 Release" && "$(CFG)" !=\
 "PumpKIN - Win32 Debug" && "$(CFG)" != "PumpKIN - Win32 Static" && "$(CFG)" !=\
 "Install - Win32 Debug" && "$(CFG)" != "Install - Win32 Pure" && "$(CFG)" !=\
 "Install - Win32 Static" && "$(CFG)" != "Install - Win32 Canned"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "pumpkin.mak" CFG="Install - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PumpKIN - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PumpKIN - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "PumpKIN - Win32 Static" (based on "Win32 (x86) Application")
!MESSAGE "Install - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Install - Win32 Pure" (based on "Win32 (x86) Application")
!MESSAGE "Install - Win32 Static" (based on "Win32 (x86) Application")
!MESSAGE "Install - Win32 Canned" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Install - Win32 Canned"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
TargetName=pumpkin
# End Custom Macros

ALL : "$(OUTDIR)\pumpkin.exe" "$(OUTDIR)\pumpkin.ex_" "$(OUTDIR)\pumpkin.hlp"\
 "$(OUTDIR)\pumpkin.cnt" "$(OUTDIR)\pumpkin.hl_" "$(OUTDIR)\pumpkin.cn_"

CLEAN : 
	-@erase "$(INTDIR)\ACLTargetCombo.obj"
	-@erase "$(INTDIR)\ConfirmRRQDlg.obj"
	-@erase "$(INTDIR)\ConfirmWRQDlg.obj"
	-@erase "$(INTDIR)\PropsACL.obj"
	-@erase "$(INTDIR)\PropsNetwork.obj"
	-@erase "$(INTDIR)\PropsServer.obj"
	-@erase "$(INTDIR)\PropsSounds.obj"
	-@erase "$(INTDIR)\pumpkin.cn_"
	-@erase "$(INTDIR)\pumpkin.cnt"
	-@erase "$(INTDIR)\pumpkin.hl_"
	-@erase "$(INTDIR)\pumpkin.hlp"
	-@erase "$(INTDIR)\PumpKIN.obj"
	-@erase "$(INTDIR)\pumpkin.pch"
	-@erase "$(INTDIR)\pumpkin.res"
	-@erase "$(INTDIR)\PumpKINDlg.obj"
	-@erase "$(INTDIR)\RequestDlg.obj"
	-@erase "$(INTDIR)\Resolver.obj"
	-@erase "$(INTDIR)\Retrier.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Trayer.obj"
	-@erase "$(OUTDIR)\pumpkin.ex_"
	-@erase "$(OUTDIR)\pumpkin.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/pumpkin.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/pumpkin.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pumpkin.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/pumpkin.pdb" /machine:I386 /out:"$(OUTDIR)/pumpkin.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ACLTargetCombo.obj" \
	"$(INTDIR)\ConfirmRRQDlg.obj" \
	"$(INTDIR)\ConfirmWRQDlg.obj" \
	"$(INTDIR)\PropsACL.obj" \
	"$(INTDIR)\PropsNetwork.obj" \
	"$(INTDIR)\PropsServer.obj" \
	"$(INTDIR)\PropsSounds.obj" \
	"$(INTDIR)\PumpKIN.obj" \
	"$(INTDIR)\pumpkin.res" \
	"$(INTDIR)\PumpKINDlg.obj" \
	"$(INTDIR)\RequestDlg.obj" \
	"$(INTDIR)\Resolver.obj" \
	"$(INTDIR)\Retrier.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Trayer.obj"

"$(OUTDIR)\pumpkin.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Release
TargetName=pumpkin
InputPath=.\Release\pumpkin.exe
SOURCE=$(InputPath)

"$(OutDir)\$(TargetName).ex_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   compress $(OutDir)\$(TargetName).exe $(OutDir)\$(TargetName).ex_

# End Custom Build

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
TargetName=pumpkin
# End Custom Macros

ALL : "$(OUTDIR)\pumpkin.exe" "$(OUTDIR)\pumpkin.bsc" "$(OUTDIR)\pumpkin.ex_"\
 "$(OUTDIR)\pumpkin.hlp" "$(OUTDIR)\pumpkin.cnt" "$(OUTDIR)\pumpkin.hl_"\
 "$(OUTDIR)\pumpkin.cn_"

CLEAN : 
	-@erase "$(INTDIR)\ACLTargetCombo.obj"
	-@erase "$(INTDIR)\ACLTargetCombo.sbr"
	-@erase "$(INTDIR)\ConfirmRRQDlg.obj"
	-@erase "$(INTDIR)\ConfirmRRQDlg.sbr"
	-@erase "$(INTDIR)\ConfirmWRQDlg.obj"
	-@erase "$(INTDIR)\ConfirmWRQDlg.sbr"
	-@erase "$(INTDIR)\PropsACL.obj"
	-@erase "$(INTDIR)\PropsACL.sbr"
	-@erase "$(INTDIR)\PropsNetwork.obj"
	-@erase "$(INTDIR)\PropsNetwork.sbr"
	-@erase "$(INTDIR)\PropsServer.obj"
	-@erase "$(INTDIR)\PropsServer.sbr"
	-@erase "$(INTDIR)\PropsSounds.obj"
	-@erase "$(INTDIR)\PropsSounds.sbr"
	-@erase "$(INTDIR)\pumpkin.cn_"
	-@erase "$(INTDIR)\pumpkin.cnt"
	-@erase "$(INTDIR)\pumpkin.hl_"
	-@erase "$(INTDIR)\pumpkin.hlp"
	-@erase "$(INTDIR)\PumpKIN.obj"
	-@erase "$(INTDIR)\pumpkin.pch"
	-@erase "$(INTDIR)\pumpkin.res"
	-@erase "$(INTDIR)\PumpKIN.sbr"
	-@erase "$(INTDIR)\PumpKINDlg.obj"
	-@erase "$(INTDIR)\PumpKINDlg.sbr"
	-@erase "$(INTDIR)\RequestDlg.obj"
	-@erase "$(INTDIR)\RequestDlg.sbr"
	-@erase "$(INTDIR)\Resolver.obj"
	-@erase "$(INTDIR)\Resolver.sbr"
	-@erase "$(INTDIR)\Retrier.obj"
	-@erase "$(INTDIR)\Retrier.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Trayer.obj"
	-@erase "$(INTDIR)\Trayer.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\pumpkin.bsc"
	-@erase "$(OUTDIR)\pumpkin.ex_"
	-@erase "$(OUTDIR)\pumpkin.exe"
	-@erase "$(OUTDIR)\pumpkin.ilk"
	-@erase "$(OUTDIR)\pumpkin.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/pumpkin.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/pumpkin.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pumpkin.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ACLTargetCombo.sbr" \
	"$(INTDIR)\ConfirmRRQDlg.sbr" \
	"$(INTDIR)\ConfirmWRQDlg.sbr" \
	"$(INTDIR)\PropsACL.sbr" \
	"$(INTDIR)\PropsNetwork.sbr" \
	"$(INTDIR)\PropsServer.sbr" \
	"$(INTDIR)\PropsSounds.sbr" \
	"$(INTDIR)\PumpKIN.sbr" \
	"$(INTDIR)\PumpKINDlg.sbr" \
	"$(INTDIR)\RequestDlg.sbr" \
	"$(INTDIR)\Resolver.sbr" \
	"$(INTDIR)\Retrier.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\Trayer.sbr"

"$(OUTDIR)\pumpkin.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/pumpkin.pdb" /debug /machine:I386 /out:"$(OUTDIR)/pumpkin.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ACLTargetCombo.obj" \
	"$(INTDIR)\ConfirmRRQDlg.obj" \
	"$(INTDIR)\ConfirmWRQDlg.obj" \
	"$(INTDIR)\PropsACL.obj" \
	"$(INTDIR)\PropsNetwork.obj" \
	"$(INTDIR)\PropsServer.obj" \
	"$(INTDIR)\PropsSounds.obj" \
	"$(INTDIR)\PumpKIN.obj" \
	"$(INTDIR)\pumpkin.res" \
	"$(INTDIR)\PumpKINDlg.obj" \
	"$(INTDIR)\RequestDlg.obj" \
	"$(INTDIR)\Resolver.obj" \
	"$(INTDIR)\Retrier.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Trayer.obj"

"$(OUTDIR)\pumpkin.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Debug
TargetName=pumpkin
InputPath=.\Debug\pumpkin.exe
SOURCE=$(InputPath)

"$(OutDir)\$(TargetName).ex_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   compress $(OutDir)\$(TargetName).exe $(OutDir)\$(TargetName).ex_

# End Custom Build

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PumpKIN_"
# PROP BASE Intermediate_Dir "PumpKIN_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Releast"
# PROP Intermediate_Dir "Releast"
# PROP Target_Dir ""
OUTDIR=.\Releast
INTDIR=.\Releast
# Begin Custom Macros
OutDir=.\Releast
TargetName=pumpkin
# End Custom Macros

ALL : "$(OUTDIR)\pumpkin.exe" "$(OUTDIR)\pumpkin.ex_" "$(OUTDIR)\pumpkin.hlp"\
 "$(OUTDIR)\pumpkin.cnt" "$(OUTDIR)\pumpkin.hl_" "$(OUTDIR)\pumpkin.cn_"

CLEAN : 
	-@erase "$(INTDIR)\ACLTargetCombo.obj"
	-@erase "$(INTDIR)\ConfirmRRQDlg.obj"
	-@erase "$(INTDIR)\ConfirmWRQDlg.obj"
	-@erase "$(INTDIR)\PropsACL.obj"
	-@erase "$(INTDIR)\PropsNetwork.obj"
	-@erase "$(INTDIR)\PropsServer.obj"
	-@erase "$(INTDIR)\PropsSounds.obj"
	-@erase "$(INTDIR)\pumpkin.cn_"
	-@erase "$(INTDIR)\pumpkin.cnt"
	-@erase "$(INTDIR)\pumpkin.hl_"
	-@erase "$(INTDIR)\pumpkin.hlp"
	-@erase "$(INTDIR)\PumpKIN.obj"
	-@erase "$(INTDIR)\pumpkin.pch"
	-@erase "$(INTDIR)\pumpkin.res"
	-@erase "$(INTDIR)\PumpKINDlg.obj"
	-@erase "$(INTDIR)\RequestDlg.obj"
	-@erase "$(INTDIR)\Resolver.obj"
	-@erase "$(INTDIR)\Retrier.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Trayer.obj"
	-@erase "$(OUTDIR)\pumpkin.ex_"
	-@erase "$(OUTDIR)\pumpkin.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /Zp1 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/pumpkin.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Releast/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/pumpkin.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pumpkin.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/pumpkin.pdb" /machine:I386 /out:"$(OUTDIR)/pumpkin.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ACLTargetCombo.obj" \
	"$(INTDIR)\ConfirmRRQDlg.obj" \
	"$(INTDIR)\ConfirmWRQDlg.obj" \
	"$(INTDIR)\PropsACL.obj" \
	"$(INTDIR)\PropsNetwork.obj" \
	"$(INTDIR)\PropsServer.obj" \
	"$(INTDIR)\PropsSounds.obj" \
	"$(INTDIR)\PumpKIN.obj" \
	"$(INTDIR)\pumpkin.res" \
	"$(INTDIR)\PumpKINDlg.obj" \
	"$(INTDIR)\RequestDlg.obj" \
	"$(INTDIR)\Resolver.obj" \
	"$(INTDIR)\Retrier.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Trayer.obj"

"$(OUTDIR)\pumpkin.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Releast
TargetName=pumpkin
InputPath=.\Releast\pumpkin.exe
SOURCE=$(InputPath)

"$(OutDir)\$(TargetName).ex_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   compress $(OutDir)\$(TargetName).exe $(OutDir)\$(TargetName).ex_

# End Custom Build

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Install\Debug"
# PROP BASE Intermediate_Dir "Install\Debug"
# PROP BASE Target_Dir "Install"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Install\Debug"
# PROP Intermediate_Dir "Install\Debug"
# PROP Target_Dir "Install"
OUTDIR=.\Install\Debug
INTDIR=.\Install\Debug

ALL : "$(OUTDIR)\Install.exe"

CLEAN : 
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\Install.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Install.exe"
	-@erase "$(OUTDIR)\Install.ilk"
	-@erase "$(OUTDIR)\Install.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/Install.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Install\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Install.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Install.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Install.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Install.exe" 
LINK32_OBJS= \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\Install.res"

"$(OUTDIR)\Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Install - Win32 Pure"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Install\Pure"
# PROP BASE Intermediate_Dir "Install\Pure"
# PROP BASE Target_Dir "Install"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Install\Pure"
# PROP Intermediate_Dir "Install\Pure"
# PROP Target_Dir "Install"
OUTDIR=.\Install\Pure
INTDIR=.\Install\Pure

ALL : "$(OUTDIR)\Install.exe" "$(OUTDIR)\Install.bsc"

CLEAN : 
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\Install.res"
	-@erase "$(INTDIR)\install.sbr"
	-@erase "$(OUTDIR)\Install.bsc"
	-@erase "$(OUTDIR)\Install.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Install.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Install\Pure/
CPP_SBRS=.\Install\Pure/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Install.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Install.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\install.sbr"

"$(OUTDIR)\Install.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Install.pdb" /machine:I386 /out:"$(OUTDIR)/Install.exe" 
LINK32_OBJS= \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\Install.res"

"$(OUTDIR)\Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Install - Win32 Static"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Install\Static"
# PROP BASE Intermediate_Dir "Install\Static"
# PROP BASE Target_Dir "Install"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Install\Static"
# PROP Intermediate_Dir "Install\Static"
# PROP Target_Dir "Install"
OUTDIR=.\Install\Static
INTDIR=.\Install\Static

ALL : "$(OUTDIR)\Install.exe"

CLEAN : 
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\Install.res"
	-@erase "$(OUTDIR)\Install.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "STATI_K" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "STATI_K" /Fp"$(INTDIR)/Install.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Install\Static/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "STATI_K"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Install.res" /d "NDEBUG" /d "STATI_K" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Install.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Install.pdb" /machine:I386 /out:"$(OUTDIR)/Install.exe" 
LINK32_OBJS= \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\Install.res"

"$(OUTDIR)\Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Install - Win32 Canned"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Install\Canned"
# PROP BASE Intermediate_Dir "Install\Canned"
# PROP BASE Target_Dir "Install"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Install\Canned"
# PROP Intermediate_Dir "Install\Canned"
# PROP Target_Dir "Install"
OUTDIR=.\Install\Canned
INTDIR=.\Install\Canned

ALL : "$(OUTDIR)\Install.exe"

CLEAN : 
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\Install.res"
	-@erase "$(OUTDIR)\Install.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "K_ANNED" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /D "K_ANNED" /Fp"$(INTDIR)/Install.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Install\Canned/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "K_ANNED"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Install.res" /d "NDEBUG" /d "K_ANNED" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Install.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Install.pdb" /machine:I386 /out:"$(OUTDIR)/Install.exe" 
LINK32_OBJS= \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\Install.res"

"$(OUTDIR)\Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "PumpKIN - Win32 Release"
# Name "PumpKIN - Win32 Debug"
# Name "PumpKIN - Win32 Static"

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\PumpKIN.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_PUMPK=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PumpKIN.obj" : $(SOURCE) $(DEP_CPP_PUMPK) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_PUMPK=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PumpKIN.obj" : $(SOURCE) $(DEP_CPP_PUMPK) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\PumpKIN.sbr" : $(SOURCE) $(DEP_CPP_PUMPK) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_PUMPK=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PumpKIN.obj" : $(SOURCE) $(DEP_CPP_PUMPK) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PumpKINDlg.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_PUMPKI=\
	".\ACLTargetCombo.h"\
	".\ConfirmRRQDlg.h"\
	".\ConfirmWRQDlg.h"\
	".\PropsACL.h"\
	".\PropsNetwork.h"\
	".\PropsServer.h"\
	".\PropsSounds.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\RequestDlg.h"\
	".\Resolver.h"\
	".\Retrier.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	".\Trayer.h"\
	

"$(INTDIR)\PumpKINDlg.obj" : $(SOURCE) $(DEP_CPP_PUMPKI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_PUMPKI=\
	".\ACLTargetCombo.h"\
	".\ConfirmRRQDlg.h"\
	".\ConfirmWRQDlg.h"\
	".\PropsACL.h"\
	".\PropsNetwork.h"\
	".\PropsServer.h"\
	".\PropsSounds.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\RequestDlg.h"\
	".\Resolver.h"\
	".\Retrier.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	".\Trayer.h"\
	

"$(INTDIR)\PumpKINDlg.obj" : $(SOURCE) $(DEP_CPP_PUMPKI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\PumpKINDlg.sbr" : $(SOURCE) $(DEP_CPP_PUMPKI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_PUMPKI=\
	".\ACLTargetCombo.h"\
	".\ConfirmRRQDlg.h"\
	".\ConfirmWRQDlg.h"\
	".\PropsACL.h"\
	".\PropsNetwork.h"\
	".\PropsServer.h"\
	".\PropsSounds.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\RequestDlg.h"\
	".\Resolver.h"\
	".\Retrier.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	".\Trayer.h"\
	

"$(INTDIR)\PumpKINDlg.obj" : $(SOURCE) $(DEP_CPP_PUMPKI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/pumpkin.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pumpkin.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/pumpkin.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pumpkin.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /Zp1 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/pumpkin.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pumpkin.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pumpkin.rc
DEP_RSC_PUMPKIN=\
	".\res\down.ico"\
	".\res\failed.wav"\
	".\res\finished.wav"\
	".\res\PumpKIN.ico"\
	".\res\pumpkin.rc2"\
	".\res\remove.ico"\
	".\res\ring.wav"\
	".\res\rrq.ico"\
	".\res\up.ico"\
	".\res\wrq.ico"\
	".\shared-data\browse-icon.ico"\
	".\shared-data\klever-background.bmp"\
	".\shared-data\play-icon.ico"\
	

!IF  "$(CFG)" == "PumpKIN - Win32 Release"


"$(INTDIR)\pumpkin.res" : $(SOURCE) $(DEP_RSC_PUMPKIN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"


"$(INTDIR)\pumpkin.res" : $(SOURCE) $(DEP_RSC_PUMPKIN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"


"$(INTDIR)\pumpkin.res" : $(SOURCE) $(DEP_RSC_PUMPKIN) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\help\pumpkin.hpj

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

# Begin Custom Build - Making help files...
OutDir=.\Release
ProjDir=.
TargetName=pumpkin
InputPath=.\help\pumpkin.hpj

BuildCmds= \
	"$(ProjDir)\makehelp.bat" \
	compress $(OutDir)\$(TargetName).hlp $(OutDir)\$(TargetName).hl_ \
	compress $(OutDir)\$(TargetName).cnt $(OutDir)\$(TargetName).cn_ \
	

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).hl_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cn_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

# Begin Custom Build - Making help files...
OutDir=.\Debug
ProjDir=.
TargetName=pumpkin
InputPath=.\help\pumpkin.hpj

BuildCmds= \
	"$(ProjDir)\makehelp.bat" \
	compress $(OutDir)\$(TargetName).hlp $(OutDir)\$(TargetName).hl_ \
	compress $(OutDir)\$(TargetName).cnt $(OutDir)\$(TargetName).cn_ \
	

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).hl_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cn_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

# Begin Custom Build - Making help files...
OutDir=.\Releast
ProjDir=.
TargetName=pumpkin
InputPath=.\help\pumpkin.hpj

BuildCmds= \
	"$(ProjDir)\makehelp.bat" \
	compress $(OutDir)\$(TargetName).hlp $(OutDir)\$(TargetName).hl_ \
	compress $(OutDir)\$(TargetName).cnt $(OutDir)\$(TargetName).cn_ \
	

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).hl_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cn_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PropsServer.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_PROPS=\
	".\PropsServer.h"\
	".\pumpkin.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsServer.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_PROPS=\
	".\PropsServer.h"\
	".\pumpkin.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsServer.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\PropsServer.sbr" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_PROPS=\
	".\PropsServer.h"\
	".\pumpkin.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsServer.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PropsNetwork.cpp
DEP_CPP_PROPSN=\
	".\PropsNetwork.h"\
	".\pumpkin.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "PumpKIN - Win32 Release"


"$(INTDIR)\PropsNetwork.obj" : $(SOURCE) $(DEP_CPP_PROPSN) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"


"$(INTDIR)\PropsNetwork.obj" : $(SOURCE) $(DEP_CPP_PROPSN) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\PropsNetwork.sbr" : $(SOURCE) $(DEP_CPP_PROPSN) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"


"$(INTDIR)\PropsNetwork.obj" : $(SOURCE) $(DEP_CPP_PROPSN) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ConfirmRRQDlg.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_CONFI=\
	".\ConfirmRRQDlg.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ConfirmRRQDlg.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_CONFI=\
	".\ConfirmRRQDlg.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ConfirmRRQDlg.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\ConfirmRRQDlg.sbr" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_CONFI=\
	".\ConfirmRRQDlg.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ConfirmRRQDlg.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ConfirmWRQDlg.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_CONFIR=\
	".\ConfirmWRQDlg.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ConfirmWRQDlg.obj" : $(SOURCE) $(DEP_CPP_CONFIR) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_CONFIR=\
	".\ConfirmWRQDlg.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ConfirmWRQDlg.obj" : $(SOURCE) $(DEP_CPP_CONFIR) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\ConfirmWRQDlg.sbr" : $(SOURCE) $(DEP_CPP_CONFIR) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_CONFIR=\
	".\ConfirmWRQDlg.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ConfirmWRQDlg.obj" : $(SOURCE) $(DEP_CPP_CONFIR) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RequestDlg.cpp
DEP_CPP_REQUE=\
	".\pumpkin.h"\
	".\RequestDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "PumpKIN - Win32 Release"


"$(INTDIR)\RequestDlg.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"


"$(INTDIR)\RequestDlg.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\RequestDlg.sbr" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"


"$(INTDIR)\RequestDlg.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Resolver.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_RESOL=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\Resolver.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Resolver.obj" : $(SOURCE) $(DEP_CPP_RESOL) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_RESOL=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\Resolver.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Resolver.obj" : $(SOURCE) $(DEP_CPP_RESOL) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\Resolver.sbr" : $(SOURCE) $(DEP_CPP_RESOL) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_RESOL=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\Resolver.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Resolver.obj" : $(SOURCE) $(DEP_CPP_RESOL) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Retrier.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_RETRI=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\Retrier.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Retrier.obj" : $(SOURCE) $(DEP_CPP_RETRI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_RETRI=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\Retrier.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Retrier.obj" : $(SOURCE) $(DEP_CPP_RETRI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\Retrier.sbr" : $(SOURCE) $(DEP_CPP_RETRI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_RETRI=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\Retrier.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Retrier.obj" : $(SOURCE) $(DEP_CPP_RETRI) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Trayer.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_TRAYE=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	".\Trayer.h"\
	

"$(INTDIR)\Trayer.obj" : $(SOURCE) $(DEP_CPP_TRAYE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_TRAYE=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	".\Trayer.h"\
	

"$(INTDIR)\Trayer.obj" : $(SOURCE) $(DEP_CPP_TRAYE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\Trayer.sbr" : $(SOURCE) $(DEP_CPP_TRAYE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_TRAYE=\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	".\Trayer.h"\
	

"$(INTDIR)\Trayer.obj" : $(SOURCE) $(DEP_CPP_TRAYE) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\help\pumpkin.cnt

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PropsSounds.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_PROPSS=\
	".\PropsSounds.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsSounds.obj" : $(SOURCE) $(DEP_CPP_PROPSS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_PROPSS=\
	".\PropsSounds.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsSounds.obj" : $(SOURCE) $(DEP_CPP_PROPSS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\PropsSounds.sbr" : $(SOURCE) $(DEP_CPP_PROPSS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_PROPSS=\
	".\PropsSounds.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsSounds.obj" : $(SOURCE) $(DEP_CPP_PROPSS) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PropsACL.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_PROPSA=\
	".\ACLTargetCombo.h"\
	".\PropsACL.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsACL.obj" : $(SOURCE) $(DEP_CPP_PROPSA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_PROPSA=\
	".\ACLTargetCombo.h"\
	".\PropsACL.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsACL.obj" : $(SOURCE) $(DEP_CPP_PROPSA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\PropsACL.sbr" : $(SOURCE) $(DEP_CPP_PROPSA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_PROPSA=\
	".\ACLTargetCombo.h"\
	".\PropsACL.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\PropsACL.obj" : $(SOURCE) $(DEP_CPP_PROPSA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ACLTargetCombo.cpp

!IF  "$(CFG)" == "PumpKIN - Win32 Release"

DEP_CPP_ACLTA=\
	".\ACLTargetCombo.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ACLTargetCombo.obj" : $(SOURCE) $(DEP_CPP_ACLTA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Debug"

DEP_CPP_ACLTA=\
	".\ACLTargetCombo.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ACLTargetCombo.obj" : $(SOURCE) $(DEP_CPP_ACLTA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"

"$(INTDIR)\ACLTargetCombo.sbr" : $(SOURCE) $(DEP_CPP_ACLTA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ELSEIF  "$(CFG)" == "PumpKIN - Win32 Static"

DEP_CPP_ACLTA=\
	".\ACLTargetCombo.h"\
	".\pumpkin.h"\
	".\PumpKINDlg.h"\
	".\shared-code\BellsNWhistles.h"\
	".\shared-code\kHelpers.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ACLTargetCombo.obj" : $(SOURCE) $(DEP_CPP_ACLTA) "$(INTDIR)"\
 "$(INTDIR)\pumpkin.pch"


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Install - Win32 Debug"
# Name "Install - Win32 Pure"
# Name "Install - Win32 Static"
# Name "Install - Win32 Canned"

!IF  "$(CFG)" == "Install - Win32 Debug"

!ELSEIF  "$(CFG)" == "Install - Win32 Pure"

!ELSEIF  "$(CFG)" == "Install - Win32 Static"

!ELSEIF  "$(CFG)" == "Install - Win32 Canned"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Install\Install.rc
DEP_RSC_INSTA=\
	".\Install\Custom.rch"\
	".\shared-data\install-icon.ico"\
	

!IF  "$(CFG)" == "Install - Win32 Debug"


"$(INTDIR)\Install.res" : $(SOURCE) $(DEP_RSC_INSTA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Install.res" /i "Install" /d "_DEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Install - Win32 Pure"


"$(INTDIR)\Install.res" : $(SOURCE) $(DEP_RSC_INSTA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Install.res" /i "Install" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Install - Win32 Static"


"$(INTDIR)\Install.res" : $(SOURCE) $(DEP_RSC_INSTA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Install.res" /i "Install" /d "NDEBUG" /d\
 "STATI_K" $(SOURCE)


!ELSEIF  "$(CFG)" == "Install - Win32 Canned"


"$(INTDIR)\Install.res" : $(SOURCE) $(DEP_RSC_INSTA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Install.res" /i "Install" /d "NDEBUG" /d\
 "K_ANNED" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Install\install.cpp
DEP_CPP_INSTAL=\
	".\shared-code\install.h"\
	

!IF  "$(CFG)" == "Install - Win32 Debug"


"$(INTDIR)\install.obj" : $(SOURCE) $(DEP_CPP_INSTAL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Install - Win32 Pure"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\install.obj" : $(SOURCE) $(DEP_CPP_INSTAL) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\install.sbr" : $(SOURCE) $(DEP_CPP_INSTAL) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Install - Win32 Static"


"$(INTDIR)\install.obj" : $(SOURCE) $(DEP_CPP_INSTAL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Install - Win32 Canned"


"$(INTDIR)\install.obj" : $(SOURCE) $(DEP_CPP_INSTAL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section PumpKIN : {4FD78B34-1FD7-11D0-A9FB-444553540000}
# 	2:11:CPumpKINDlg:1
# 	2:11:CRequestDlg:1
# End Section
################################################################################
