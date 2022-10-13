@ECHO %%G

RMDIR/s/q .\include
RMDIR/s/q .\lib
RMDIR/s/q .\lib\win32
RMDIR/s/q .\lib\win32\Debug
RMDIR/s/q .\lib\win32\Release
RMDIR/s/q .\lib\win64
RMDIR/s/q .\lib\win64\Debug
RMDIR/s/q .\lib\win64\Release

MKDIR .\include
MKDIR .\lib
MKDIR .\lib\win32
MKDIR .\lib\win32\Debug
MKDIR .\lib\win32\Release
MKDIR .\lib\win64
MKDIR .\lib\win64\Debug
MKDIR .\lib\win64\Release

XCOPY %PinSDK%c\3th_party\home\libcurl\include .\include /E

COPY /y "%PinSDK%c\3th_party\home\libssh2\bin\src\Debug\libssh2.lib"   .\lib\win32\Debug\libssh2.lib
COPY /y "%PinSDK%c\3th_party\home\libssh2\bin\src\Release\libssh2.lib" .\lib\win32\Release\libssh2.lib
COPY /y "%PinSDK%c\3th_party\home\libssh2\bin\src\Debug\libssh2.lib"   .\lib\win64\Debug\libssh2.lib
COPY /y "%PinSDK%c\3th_party\home\libssh2\bin\src\Release\libssh2.lib" .\lib\win64\Release\libssh2.lib

COPY /y "%PinSDK%c\3th_party\home\libcurl\build\Win32\VC14.30\LIB Debug - LIB OpenSSL - LIB LibSSH2\libcurld.lib"   .\lib\win32\Debug\libcurl.lib
COPY /y "%PinSDK%c\3th_party\home\libcurl\build\Win32\VC14.30\LIB Release - LIB OpenSSL - LIB LibSSH2\libcurl.lib" .\lib\win32\Release\libcurl.lib
COPY /y "%PinSDK%c\3th_party\home\libcurl\build\Win64\VC14.30\LIB Debug - LIB OpenSSL - LIB LibSSH2\libcurld.lib"   .\lib\win64\Debug\libcurl.lib
COPY /y "%PinSDK%c\3th_party\home\libcurl\build\Win64\VC14.30\LIB Release - LIB OpenSSL - LIB LibSSH2\libcurl.lib" .\lib\win64\Release\libcurl.lib

PAUSE
