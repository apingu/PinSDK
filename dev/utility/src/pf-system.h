//
//
//
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "pf-def.h"

enum CPU_INTRIN
{
    _CPU_INTRIN_FPU    = 1,
    _CPU_INTRIN_MMX    = 2,
    _CPU_INTRIN_SSE    = 3,
    _CPU_INTRIN_SSE2   = 4,
    _CPU_INTRIN_SSE3   = 5,
    _CPU_INTRIN_SSSE3  = 6,
    _CPU_INTRIN_IA64   = 7,
};

namespace pfkrl
{
    typedef struct _tagMemoryInfo
    {
       uint64_t        dwLength;
       uint64_t        dwMemoryLoad;
       uint64_t        dwTotalPhys;
       uint64_t        dwAvailPhys;
       uint64_t        dwTotalPageFile;
       uint64_t        dwAvailPageFile;
       uint64_t        dwTotalVirtual;
       uint64_t        dwAvailVirtual;
    } MemoryInfo;
    
    
    typedef struct _tagSystemInfo
    {
        unsigned short wProcessorArchitecture;
        unsigned short wReserved;
        unsigned int   dwPageSize;
        void*          lpMinimumApplicationAddress;
        void*          lpMaximumApplicationAddress;
        unsigned int   dwActiveProcessorMask;
        unsigned int   dwNumberOfProcessors;
        unsigned int   dwProcessorType;
        unsigned int   dwAllocationGranularity;
        unsigned short wProcessorLevel;
        unsigned short wProcessorRevision;
    } SystemInfo;
    
    
    //void  GetMemoryInfo(MemoryInfo& meminfo);
    //void  GetSystemInfo(SystemInfo& sysinfo);
    bool           CheckCPUInStruction( CPU_INTRIN d );
    void           killProcessByName( const char* filename );
    
    //  get the number of cores
    int            GetNumberOfProcessors();
    ulong64_t      GetPhysMemorySize();
    ulong64_t      GetAvailPhysMemorySize();
    ulong64_t      GetFreeMemorySize();

    //const tchar_t* GetCurrentProcessShortName( tchar_t* nbuf, int nlen );
    const char*    GetCurrentProcessFullName( char* nbuf, int nlen );
    //const wchar_t* GetCurrentProcessFullName( wchar_t* nbuf, int nlen );

    const char*    GetCommandLineA();
    const wchar_t* GetCommandLineW();
};