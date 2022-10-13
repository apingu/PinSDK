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

#include "PfSystem.h"

#ifdef WIN32
#include <windows.h>
#endif



namespace System
{
    void  GetMemoryInfo(MemoryInfo &meminfo)
    {
#ifdef WIN32

        MEMORYSTATUS mstat;
        GlobalMemoryStatus(&mstat);
        meminfo.dwLength        = mstat.dwLength;
        meminfo.dwMemoryLoad    = mstat.dwMemoryLoad;
        meminfo.dwTotalPhys     = mstat.dwTotalPhys;
        meminfo.dwAvailPhys     = mstat.dwAvailPhys;
        meminfo.dwTotalPageFile = mstat.dwTotalPageFile;
        meminfo.dwAvailPageFile = mstat.dwAvailPageFile;
        meminfo.dwTotalVirtual  = mstat.dwTotalVirtual;
        meminfo.dwAvailVirtual  = mstat.dwAvailVirtual;
#endif

        return;
    }


    void  GetSystemInfo(SystemInfo &sysinfo)
    {
#ifdef WIN32
        SYSTEM_INFO systemInfo;//
        GetSystemInfo(&systemInfo);
        sysinfo.wProcessorArchitecture = systemInfo.wProcessorArchitecture;
        sysinfo.wReserved = systemInfo.wReserved;
        sysinfo.dwPageSize = systemInfo.dwPageSize;
        sysinfo.lpMinimumApplicationAddress = systemInfo.lpMinimumApplicationAddress;
        sysinfo.lpMaximumApplicationAddress = systemInfo.lpMaximumApplicationAddress;
        sysinfo.dwActiveProcessorMask = systemInfo.dwActiveProcessorMask;
        sysinfo.dwNumberOfProcessors = systemInfo.dwNumberOfProcessors;
        sysinfo.dwProcessorType = systemInfo.dwProcessorType;
        sysinfo.dwAllocationGranularity = systemInfo.dwAllocationGranularity;
        sysinfo.wProcessorLevel = systemInfo.wProcessorLevel;
        sysinfo.wProcessorRevision = systemInfo.wProcessorRevision;
#endif

        return;

    }

#define    CPUID __asm _emit 0x0F _asm _emit 0xA2
    bool CheckCPUInStruction(CPU_INTRIN d)
    {
        bool isSupport;
        switch(d)
        {
        case _CPU_INTRIN_FPU:
            {
#if defined(_MSC_VER)
                __ASM_BEGIN__
                PUSHFD
                    POP       eax
                    XOR       eax, 00200000h
                    //XOR       eax, 0X00200000
                    PUSH      eax
                    POPFD
                    PUSHFD
                    POP       edx
                    CMP       eax, edx
                    JNE       NotSupportFPU
                    XOR       eax, eax
                    CPUID
                    OR        eax, eax
                    JZ        NotSupportFPU
                    MOV       eax, 1
                    CPUID
                    TEST      dl, 1
                    JZ        NotSupportFPU
                    MOV       isSupport,1
NotSupportFPU:
                __ASM_END__
#endif
            }
            break;
        case _CPU_INTRIN_MMX:
            {
#ifdef _MSC_VER
                __ASM_BEGIN__
                    PUSHFD
                    POP        eax
                    XOR        eax, 00200000h
                    PUSH       eax
                    POPFD
                    PUSHFD
                    POP        edx
                    CMP        eax, edx
                    JNE        NotSupportMMX
                    XOR        eax, eax
                    CPUID
                    OR         eax, eax
                    JZ         NotSupportMMX
                    MOV        eax, 1
                    CPUID
                    TEST       edx, 00800000h
                    JZ         NotSupportMMX
                    MOV        isSupport,1
NotSupportMMX:
                __ASM_END__
#endif
            }
            break;
        case _CPU_INTRIN_SSE:
            {
#ifdef _MSC_VER
                __ASM_BEGIN__
                    PUSHFD
                    POP        eax
                    XOR        eax, 00200000h
                    PUSH       eax
                    POPFD
                    PUSHFD
                    POP        edx
                    CMP        eax, edx
                    JNE        NotSupportSSE
                    XOR        eax, eax
                    CPUID
                    OR         eax, eax
                    JZ         NotSupportSSE
                    MOV        eax, 1
                    CPUID
                    TEST       edx, 02000000h
                    JZ         NotSupportSSE
                    MOV        isSupport, 1
NotSupportSSE:
                __ASM_END__
#endif
            }
            break;
        case _CPU_INTRIN_SSE2:
            {
#ifdef _MSC_VER
                __ASM_BEGIN__
                    PUSHFD
                    POP        eax
                    XOR        eax, 00200000h
                    PUSH       eax
                    POPFD
                    PUSHFD
                    POP        edx
                    CMP        eax, edx
                    JNE        NotSupportSSE2
                    XOR        eax, eax
                    CPUID
                    OR         eax, eax
                    JZ         NotSupportSSE2
                    MOV        eax, 1
                    CPUID
                    TEST       edx, 04000000h
                    JZ         NotSupportSSE2
                    MOV        isSupport, 1
NotSupportSSE2:
                __ASM_END__
#endif
            }
            break;
        case _CPU_INTRIN_SSE3:
            {
#ifdef _MSC_VER
                __ASM_BEGIN__
                    PUSHFD
                    POP        eax
                    XOR        eax, 00200000h
                    PUSH       eax
                    POPFD
                    PUSHFD
                    POP        edx
                    CMP        eax, edx
                    JNE        NotSupportSSE3
                    XOR        eax, eax
                    CPUID
                    OR         eax, eax
                    JZ         NotSupportSSE3
                    MOV        eax, 1
                    CPUID
                    TEST       cl, 1
                    JZ         NotSupportSSE3
                    MOV        isSupport, 1
NotSupportSSE3:
                __ASM_END__
#endif
            }
            break;
        case _CPU_INTRIN_SSSE3:
            {
#ifdef _MSC_VER
                __ASM_BEGIN__
                    PUSHFD
                    POP        eax
                    XOR        eax, 00200000h
                    PUSH       eax
                    POPFD
                    PUSHFD
                    POP        edx
                    CMP        eax, edx
                    JNE        NotSupportSSSE3
                    XOR        eax, eax
                    CPUID
                    OR         eax, eax
                    JZ         NotSupportSSSE3
                    MOV        eax, 1
                    CPUID
                    TEST       cx, 200h
                    JZ         NotSupportSSSE3
                    MOV        isSupport, 1
NotSupportSSSE3:
                __ASM_END__
#endif
            }
            break;
        case _CPU_INTRIN_IA64:
            {
#ifdef _MSC_VER
                __ASM_BEGIN__
                    PUSHFD
                    POP        eax
                    XOR        eax, 00200000h
                    PUSH       eax
                    POPFD
                    PUSHFD
                    POP        edx
                    CMP        eax, edx
                    JNE        NotSupportIA64
                    XOR        eax, eax
                    CPUID
                    OR         eax, eax
                    JZ         NotSupportIA64
                    MOV        eax, 1
                    CPUID
                    TEST       edx, 40000000h
                    JZ         NotSupportIA64
                    MOV        isSupport, 1
NotSupportIA64:
                __ASM_END__
#endif
            }
            break;
        }
        return isSupport;
    }
};
