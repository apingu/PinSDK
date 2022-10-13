@ECHO %%G

RMDIR/s/q .\include
RMDIR/s/q .\lib

MKDIR .\include

MKDIR .\lib\windows_x64\Debug
MKDIR .\lib\windows_x64\Release
MKDIR .\lib\windows_x86\Debug
MKDIR .\lib\windows_x86\Release

XCOPY /E %PinSDK%c\3th_party\home\openssl\include         .\include
XCOPY /E %PinSDK%c\3th_party\home\openssl\lib\x64\Debug   .\lib\windows_x64\Debug
XCOPY /E %PinSDK%c\3th_party\home\openssl\lib\x64\Release .\lib\windows_x64\Release
XCOPY /E %PinSDK%c\3th_party\home\openssl\lib\x86\Debug   .\lib\windows_x86\Debug
XCOPY /E %PinSDK%c\3th_party\home\openssl\lib\x86\Release .\lib\windows_x86\Release


PAUSE
