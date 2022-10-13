//____________________________________________________________________________________
openssl

OPENSSL_BUILDING_OPENSSL
OPENSSL_SYS_WIN32
WIN32_LEAN_AND_MEAN
UNICOD
_UNICODE
_CRT_SECURE_NO_DEPRECATE
_WINSOCK_DEPRECATED_NO_WARNINGS
NDEBUG


ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib


INCLUDE
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.33.31629\include;C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.33.31629\atlmfc\include;C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\VS\include;C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt;C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\VS\UnitTest\include;C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um;C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared;C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\winrt;C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\cppwinrt;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\Include\m;

LIB
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.33.31629\lib\x64;C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.33.31629\atlmfc\lib\x64;C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\VS\lib\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\ucrt\x64;C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\VS\UnitTest\lib;C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\um\x64;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\lib\um\x64;

C:\WinApp\strawberry\perl\bin
C:\WinApp\NASM
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64
C:\Program Files (x86)\Microsoft Visual Studio\Shared\NuGetPackages\microsoft.windows.sdk.buildtools\10.0.22621.1\bin\10.0.22621.0\x64

//____________________________________________________________________________________
source
來源:
https://github.com/google/boringssl
openssl_src

_CRT_SECURE_NO_WARNINGS      // 關閉 標準安全性 c funtion 警告
_CRT_NONSTDC_NO_DEPRECATE    // 關閉 c posix function _XXXXX 警告
NOCRYPT                      // 關閉 x509 重複
WIN32_LEAN_AND_MEAN          // 關閉 winsocket 重複



_CRT_SECURE_NO_WARNINGS
_CRT_NONSTDC_NO_DEPRECATE
NOCRYPT
WIN32_LEAN_AND_MEAN