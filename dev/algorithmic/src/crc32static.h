#ifndef _CRC32STATIC_H_
#define _CRC32STATIC_H_

#include <basedef.h>
#include <windows.h>
#include "CrcCommon.h"

class CCrc32Static
{
public:
	CCrc32Static();
	virtual ~CCrc32Static();

	static DWORD StringCrc32( const char* szString, DWORD &dwCrc32);
	static DWORD FileCrc32Streams( const char* szFilename, DWORD &dwCrc32);
	static DWORD FileCrc32Win32( const char* szFilename, DWORD &dwCrc32);
	static DWORD FileCrc32Filemap( const char* szFilename, DWORD &dwCrc32);
	static DWORD FileCrc32Assembly( const char* szFilename, DWORD &dwCrc32);
protected:
	static bool GetFileSizeQW(const HANDLE hFile, QWORD &qwSize);
	static inline void CalcCrc32(const BYTE byte, DWORD &dwCrc32);

	static DWORD s_arrdwCrc32Table[256];
};

#endif
