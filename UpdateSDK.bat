CD /D "%~dp0"

RMDIR/s/q dev\include
RMDIR/s/q dev\lib
RMDIR/s/q include
RMDIR/s/q lib

MKDIR dev\include
MKDIR dev\include\net
MKDIR dev\include\game
MKDIR dev\include\helper
MKDIR dev\include\graphic

MKDIR dev\lib
MKDIR dev\lib\windows_x86
MKDIR dev\lib\windows_x86\Debug
MKDIR dev\lib\windows_x86\Release

MKDIR dev\lib\windows_x64
MKDIR dev\lib\windows_x64\Debug
MKDIR dev\lib\windows_x64\Release

MKDIR dev\lib\android
MKDIR dev\lib\android\Debug
MKDIR dev\lib\android\Release

MKDIR dev\lib\ios
MKDIR dev\lib\ios\Debug
MKDIR dev\lib\ios\Release


MKDIR include
MKDIR include\net
MKDIR include\game
MKDIR include\game\ui
MKDIR include\template
MKDIR include\helper
MKDIR include\parser
MKDIR include\gadget
MKDIR include\database
MKDIR include\algorithmic
MKDIR include\sdk
MKDIR include\sdk\gl
MKDIR include\sdk\d3d
MKDIR include\sdk\sky
MKDIR include\sdk\rw

MKDIR lib
MKDIR lib\windows_x86
MKDIR lib\windows_x86\Debug
MKDIR lib\windows_x86\Release

MKDIR lib\windows_x64
MKDIR lib\windows_x64\Debug
MKDIR lib\windows_x64\Release

MKDIR lib\android
MKDIR lib\android\Debug
MKDIR lib\android\Release

MKDIR lib\ios
MKDIR lib\ios\Debug
MKDIR lib\ios\Release

MKDIR lib\gc
MKDIR lib\gc\Debug
MKDIR lib\gc\Release

MKDIR lib\xbox
MKDIR lib\xbox\Debug
MKDIR lib\xbox\Release

MKDIR lib\ps2
MKDIR lib\ps2\Debug
MKDIR lib\ps2\Release

MKDIR lib\ps4
MKDIR lib\ps4\Debug
MKDIR lib\ps4\Release

pause
