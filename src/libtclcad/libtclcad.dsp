# Microsoft Developer Studio Project File - Name="libtclcad" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtclcad - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "libtclcad.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "libtclcad.mak" CFG="libtclcad - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "libtclcad - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libtclcad - Win32 Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "libtclcad - Win32 Release Multithreaded DLL" (basierend auf "Win32 (x86) Static Library")
!MESSAGE "libtclcad - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libtclcad - Win32 Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "libtclcad - Win32 Debug Multithreaded DLL" (bbased on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "libtclcad"
# PROP Scc_LocalPath "."
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "libtclcad - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\misc\win32-msvc\Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\misc\win32-msvc\ReleaseMt"
# PROP Intermediate_Dir "ReleaseMt"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Release Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\misc\win32-msvc\ReleaseMtDll"
# PROP Intermediate_Dir "ReleaseMtDll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\misc\win32-msvc\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\misc\win32-msvc\DebugMt"
# PROP Intermediate_Dir "DebugMt"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Debug Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\misc\win32-msvc\DebugMtDll"
# PROP Intermediate_Dir "DebugMtDll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF

# Begin Target

# Name "libtclcad - Win32 Release"
# Name "libtclcad - Win32 Release Multithreaded"
# Name "libtclcad - Win32 Release Multithreaded DLL"
# Name "libtclcad - Win32 Debug"
# Name "libtclcad - Win32 Debug Multithreaded"
# Name "libtclcad - Win32 Debug Multithreaded DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\tclcadAutoPath.c
# End Source File
# Begin Source File

SOURCE=.\tclcadTkSetup.c
# End Source File
# Begin Source File

SOURCE=.\tkCanvBezier.c
# End Source File
# Begin Source File

SOURCE=.\tkImgFmtPIX.c
# End Source File
# Begin Source File

SOURCE=.\vers.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Source File

SOURCE=..\..\configure.ac

!IF  "$(CFG)" == "libtclcad - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\..\configure.ac

"$(ProjDir)\vers.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cscript //nologo ..\..\misc\win32-msvc\vers.vbs tclcad_version BRL-CAD Routines for Tcl/Tk > $(ProjDir)\vers.c

# End Custom Build

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Release Multithreaded"

# Begin Custom Build
ProjDir=.
InputPath=..\..\configure.ac

"$(ProjDir)\vers.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cscript //nologo ..\..\misc\win32-msvc\vers.vbs tclcad_version BRL-CAD Routines for Tcl/Tk > $(ProjDir)\vers.c

# End Custom Build

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Release Multithreaded DLL"

# Begin Custom Build
ProjDir=.
InputPath=..\..\configure.ac

"$(ProjDir)\vers.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cscript //nologo ..\..\misc\win32-msvc\vers.vbs tclcad_version BRL-CAD Routines for Tcl/Tk > $(ProjDir)\vers.c

# End Custom Build

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\..\configure.ac

"$(ProjDir)\vers.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cscript //nologo ..\..\misc\win32-msvc\vers.vbs tclcad_version BRL-CAD Routines for Tcl/Tk > $(ProjDir)\vers.c

# End Custom Build

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Debug Multithreaded"

# Begin Custom Build
ProjDir=.
InputPath=..\..\configure.ac

"$(ProjDir)\vers.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cscript //nologo ..\..\misc\win32-msvc\vers.vbs tclcad_version BRL-CAD Routines for Tcl/Tk > $(ProjDir)\vers.c

# End Custom Build

!ELSEIF  "$(CFG)" == "libtclcad - Win32 Debug Multithreaded DLL"

# Begin Custom Build
ProjDir=.
InputPath=..\..\configure.ac

"$(ProjDir)\vers.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cscript //nologo ..\..\misc\win32-msvc\vers.vbs tclcad_version BRL-CAD Routines for Tcl/Tk > $(ProjDir)\vers.c

# End Custom Build

!ENDIF

# End Source File
# End Target
# End Project
