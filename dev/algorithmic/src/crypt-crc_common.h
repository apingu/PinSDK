#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef _QWORD_DEFINED
    #define _QWORD_DEFINED
    typedef long long* LPQWORD;
#endif

#ifndef MAKEQWORD
#define MAKEQWORD(a, b) \
    ((QWORD)(((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))
#endif

#ifndef LODWORD
#define LODWORD(l) \
    ((DWORD)(l))
#endif

#ifndef HIDWORD
#define HIDWORD(l) \
    ((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))
#endif

// Read 4K of data at a time (used in the C++ streams, Win32 I/O, and assembly functions)
#define MAX_CRCBUFFER_SIZE  4096

// Map a "view" size of 10MB (used in the filemap function)
#define MAX_CRCVIEW_SIZE    10485760

#define CRCERROR_SUCCESS     0L
#define CRCNO_ERROR          0L             // dderror
#define CRCERROR_CRC         23L



#endif
