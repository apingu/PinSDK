# Microsoft Developer Studio Project File - Name="Chinese" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Chinese - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Chinese.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Chinese.mak" CFG="Chinese - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Chinese - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Chinese - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Chinese - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Obj\Release"
# PROP Intermediate_Dir ".\Obj\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Lib\Chinese.lib"
# Begin Custom Build
WkspDir=.
ProjDir=.
InputPath=.\Lib\Chinese.lib
SOURCE="$(InputPath)"

".\force" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xcopy /y /r /i "$(ProjDir)\Source\*.h" "$(ProjDir)\Include" 
	xcopy /y /r /i "$(ProjDir)\Lib\*.lib" "$(WkspDir)\Lib" 
	xcopy /y /r /i "$(ProjDir)\Include\*.*" "$(WkspDir)\Include" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Chinese - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Obj\Debug"
# PROP Intermediate_Dir ".\Obj\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Lib\ChineseD.lib"
# Begin Custom Build
WkspDir=.
ProjDir=.
InputPath=.\Lib\ChineseD.lib
SOURCE="$(InputPath)"

".\force" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xcopy /y /r /i "$(ProjDir)\Source\*.h" "$(ProjDir)\Include" 
	xcopy /y /r /i "$(ProjDir)\Lib\*.lib" "$(WkspDir)\Lib" 
	xcopy /y /r /i "$(ProjDir)\Include\*.*" "$(WkspDir)\Include" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Chinese - Win32 Release"
# Name "Chinese - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\Font.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Text.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\Font.h
# End Source File
# Begin Source File

SOURCE=.\Source\Text.h
# End Source File
# End Group
# End Target
# End Project
