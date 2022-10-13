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
//
#ifndef PFSYSTEM_H
#define PFSYSTEM_H

#include "basedef.h"

enum CPU_INTRIN
{
    _CPU_INTRIN_FPU     = 1,
    _CPU_INTRIN_MMX     = 2,
    _CPU_INTRIN_SSE     = 3,
    _CPU_INTRIN_SSE2    = 4,
    _CPU_INTRIN_SSE3    = 5,
    _CPU_INTRIN_SSSE3   = 6,
    _CPU_INTRIN_IA64    = 7,
};

namespace System
{
    typedef struct _tagMemoryInfo
    {
        unsigned int  dwLength;
        unsigned int  dwMemoryLoad;
        unsigned long dwTotalPhys;
        unsigned long dwAvailPhys;
        unsigned long dwTotalPageFile;
        unsigned long dwAvailPageFile;
        unsigned long dwTotalVirtual;
        unsigned long dwAvailVirtual;
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


    void  GetMemoryInfo(MemoryInfo &meminfo);
    void  GetSystemInfo(SystemInfo &sysinfo);

    bool  CheckCPUInStruction(CPU_INTRIN d);
};


#endif
