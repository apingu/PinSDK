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

//01. == C 系統文件/C++ 系統文件
#ifdef _WINDOWS
#include <windows.h>
#endif
#ifdef _MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#endif
#ifdef _LINUX
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#endif

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pf-system.h"


namespace pfkrl
{
    void GetMemoryInfo( MemoryInfo& meminfo )
    {
#ifdef _WINDOWS
        MEMORYSTATUS mstat;
        GlobalMemoryStatus( &mstat );
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
    
    
    void  GetSystemInfo( SystemInfo& sysinfo )
    {
#ifdef _WINDOWS
        SYSTEM_INFO systemInfo;
        GetSystemInfo( &systemInfo );
        sysinfo.wProcessorArchitecture          = systemInfo.wProcessorArchitecture;
        sysinfo.wReserved                       = systemInfo.wReserved;
        sysinfo.dwPageSize                      = systemInfo.dwPageSize;
        sysinfo.lpMinimumApplicationAddress     = systemInfo.lpMinimumApplicationAddress;
        sysinfo.lpMaximumApplicationAddress     = systemInfo.lpMaximumApplicationAddress;
        sysinfo.dwActiveProcessorMask           = systemInfo.dwActiveProcessorMask;
        sysinfo.dwNumberOfProcessors            = systemInfo.dwNumberOfProcessors;
        sysinfo.dwProcessorType                 = systemInfo.dwProcessorType;
        sysinfo.dwAllocationGranularity         = systemInfo.dwAllocationGranularity;
        sysinfo.wProcessorLevel                 = systemInfo.wProcessorLevel;
        sysinfo.wProcessorRevision              = systemInfo.wProcessorRevision;
#endif
        return;
        
    }
    
    
    
#define CPUID __asm _emit 0x0F _asm _emit 0xA2
    
    bool CheckCPUInStruction( CPU_INTRIN d )
    {
        bool isSupport = false;
#if defined(_X86_)
#if defined(__MSVC__)
        switch( d )
        {
        case _CPU_INTRIN_FPU:
            {
            __ASM_BEGIN__
                PUSHFD
                POP     eax
                XOR     eax, 00200000h
                //XOR    eax, 0X00200000
                PUSH    eax
                POPFD
                PUSHFD
                POP     edx
                CMP     eax, edx
                JNE     NotSupportFPU
                XOR     eax, eax
            CPUID
                OR      eax, eax
                JZ      NotSupportFPU
                MOV     eax, 1
            CPUID
                TEST    dl, 1
                JZ      NotSupportFPU
                MOV     isSupport, 1
NotSupportFPU:
            __ASM_END__
            }
            break;
        case _CPU_INTRIN_MMX:
            {
            __ASM_BEGIN__
                PUSHFD
                POP     eax
                XOR     eax, 00200000h
                PUSH    eax
                POPFD
                PUSHFD
                POP     edx
                CMP     eax, edx
                JNE     NotSupportMMX
                XOR     eax, eax
            CPUID
                OR      eax, eax
                JZ      NotSupportMMX
                MOV     eax, 1
            CPUID
                TEST    edx, 00800000h
                JZ      NotSupportMMX
                MOV     isSupport, 1
NotSupportMMX:
            __ASM_END__
            }
            break;
        case _CPU_INTRIN_SSE:
            {
            __ASM_BEGIN__
                PUSHFD
                POP  eax
                XOR  eax, 00200000h
                PUSH    eax
                POPFD
                PUSHFD
                POP  edx
                CMP  eax, edx
                JNE  NotSupportSSE
                XOR  eax, eax
            CPUID
                OR   eax, eax
                JZ   NotSupportSSE
                MOV  eax, 1
            CPUID
                TEST    edx, 02000000h
                JZ   NotSupportSSE
                MOV  isSupport, 1
NotSupportSSE:
            __ASM_END__
            }
            break;
        case _CPU_INTRIN_SSE2:
            {
            __ASM_BEGIN__
                PUSHFD
                POP  eax
                XOR  eax, 00200000h
                PUSH    eax
                POPFD
                PUSHFD
                POP  edx
                CMP  eax, edx
                JNE  NotSupportSSE2
                XOR  eax, eax
            CPUID
                OR   eax, eax
                JZ   NotSupportSSE2
                MOV  eax, 1
            CPUID
                TEST    edx, 04000000h
                JZ   NotSupportSSE2
                MOV  isSupport, 1
NotSupportSSE2:
            __ASM_END__
            }
            break;
        case _CPU_INTRIN_SSE3:
            {
            __ASM_BEGIN__
                PUSHFD
                POP  eax
                XOR  eax, 00200000h
                PUSH    eax
                POPFD
                PUSHFD
                POP  edx
                CMP  eax, edx
                JNE  NotSupportSSE3
                XOR  eax, eax
            CPUID
                OR   eax, eax
                JZ   NotSupportSSE3
                MOV  eax, 1
            CPUID
                TEST    cl, 1
                JZ   NotSupportSSE3
                MOV  isSupport, 1
NotSupportSSE3:
            __ASM_END__
            }
            break;
        case _CPU_INTRIN_SSSE3:
            {
            __ASM_BEGIN__
                PUSHFD
                POP  eax
                XOR  eax, 00200000h
                PUSH    eax
                POPFD
                PUSHFD
                POP  edx
                CMP  eax, edx
                JNE  NotSupportSSSE3
                XOR  eax, eax
            CPUID
                OR   eax, eax
                JZ   NotSupportSSSE3
                MOV  eax, 1
            CPUID
                TEST    cx, 200h
                JZ   NotSupportSSSE3
                MOV  isSupport, 1
NotSupportSSSE3:
            __ASM_END__
            }
            break;
        case _CPU_INTRIN_IA64:
            {
            __ASM_BEGIN__
                PUSHFD
                POP  eax
                XOR  eax, 00200000h
                PUSH    eax
                POPFD
                PUSHFD
                POP  edx
                CMP  eax, edx
                JNE  NotSupportIA64
                XOR  eax, eax
            CPUID
                OR   eax, eax
                JZ   NotSupportIA64
                MOV  eax, 1
            CPUID
                TEST    edx, 40000000h
                JZ   NotSupportIA64
                MOV  isSupport, 1
NotSupportIA64:
            __ASM_END__
            }
            break;
        }
#endif
        
        /*
        #if defined(__GNUC__)
        switch(d)
        {
        case _CPU_INTRIN_FPU:
        {
        __ASM_BEGIN__
        "  PUSHFD     "
        "  POP    eax    "
        "  XOR    eax, 00200000h  "
        "  PUSH   eax    "
        "  POPFD      "
        "  PUSHFD     "
        "  POP    edx    "
        "  CMP    eax, edx  "
        "  JNE    NotSupportFPU   "
        "  XOR    eax, eax  "
        "  CPUID      "
        "  OR  eax, eax  "
        "  JZ  NotSupportFPU   "
        "  MOV    eax, 1    "
        "  CPUID      "
        "  TEST   dl, 1     "
        "  JZ  NotSupportFPU   "
        "  MOV    isSupport, 1 "
        " NotSupportFPU:    "
        __ASM_END__
        }
        break;
        case _CPU_INTRIN_MMX:
        {
        __ASM_BEGIN__
        "  PUSHFD       "
        "  POP  eax     "
        "  XOR  eax, 00200000h   "
        "  PUSH    eax     "
        "  POPFD        "
        "  PUSHFD       "
        "  POP  edx     "
        "  CMP  eax, edx   "
        "  JNE  NotSupportMMX "
        "  XOR  eax, eax   "
        "  CPUID        "
        "  OR   eax, eax   "
        "  JZ   NotSupportMMX "
        "  MOV  eax, 1     "
        "  CPUID        "
        "  TEST    edx, 00800000h   "
        "  JZ   NotSupportMMX "
        "  MOV  isSupport, 1  "
        " NotSupportMMX:      "
        __ASM_END__
        }
        break;
        case _CPU_INTRIN_SSE:
        {
        __ASM_BEGIN__
        "  PUSHFD       "
        "  POP  eax     "
        "  XOR  eax, 00200000h   "
        "  PUSH    eax     "
        "  POPFD        "
        "  PUSHFD       "
        "  POP  edx     "
        "  CMP  eax, edx   "
        "  JNE  NotSupportSSE "
        "  XOR  eax, eax   "
        "  CPUID        "
        "  OR   eax, eax   "
        "  JZ   NotSupportSSE "
        "  MOV  eax, 1     "
        "  CPUID        "
        "  TEST    edx, 02000000h   "
        "  JZ   NotSupportSSE "
        "  MOV  isSupport, 1  "
        " NotSupportSSE:      "
        __ASM_END__
        }
        break;
        case _CPU_INTRIN_SSE2:
        {
        __ASM_BEGIN__
          "  PUSHFD        "
          "  POP  eax      "
          "  XOR  eax, 00200000h "
          "  PUSH    eax      "
          "  POPFD      "
          "  PUSHFD        "
          "  POP  edx      "
          "  CMP  eax, edx    "
          "  JNE  NotSupportSSE2 "
          "  XOR  eax, eax    "
          "  CPUID      "
          "  OR   eax, eax    "
          "  JZ   NotSupportSSE2 "
          "  MOV  eax, 1   "
          "  CPUID      "
          "  TEST    edx, 04000000h "
          "  JZ   NotSupportSSE2 "
          "  MOV  isSupport, 1   "
          " NotSupportSSE2:      "
          __ASM_END__
        }
        break;
        case _CPU_INTRIN_SSE3:
        {
        __ASM_BEGIN__
        " PUSHFD        "
        " POP  eax      "
        " XOR  eax, 00200000h "
        " PUSH    eax      "
        " POPFD      "
        " PUSHFD        "
        " POP  edx      "
        " CMP  eax, edx    "
        " JNE  NotSupportSSE3 "
        " XOR  eax, eax    "
        " CPUID      "
        " OR   eax, eax    "
        " JZ   NotSupportSSE3 "
        " MOV  eax, 1   "
        " CPUID      "
        " TEST    cl, 1    "
        " JZ   NotSupportSSE3 "
        " MOV  isSupport, 1   "
        " NotSupportSSE3:     "
        __ASM_END__
        }
        break;
        case _CPU_INTRIN_SSSE3:
        {
        __ASM_BEGIN__
        "  PUSHFD       "
        "  POP  eax     "
        "  XOR  eax, 00200000h   "
        "  PUSH    eax     "
        "  POPFD        "
        "  PUSHFD       "
        "  POP  edx     "
        "  CMP  eax, edx   "
        "  JNE  NotSupportSSSE3  "
        "  XOR  eax, eax   "
        "  CPUID        "
        "  OR   eax, eax   "
        "  JZ   NotSupportSSSE3  "
        "  MOV  eax, 1     "
        "  CPUID        "
        "  TEST    cx, 200h   "
        "  JZ   NotSupportSSSE3  "
        "  MOV  isSupport, 1  "
        " NotSupportSSSE3:    "
        __ASM_END__
        }
        break;
        case _CPU_INTRIN_IA64:
        {
        __ASM_BEGIN__
        " PUSHFD        "
        " POP  eax      "
        " XOR  eax, 00200000h "
        " PUSH    eax      "
        " POPFD      "
        " PUSHFD        "
        " POP  edx      "
        " CMP  eax, edx    "
        " JNE  NotSupportIA64 "
        " XOR  eax, eax    "
        " CPUID      "
        " OR   eax, eax    "
        " JZ   NotSupportIA64 "
        " MOV  eax, 1   "
        " CPUID      "
        " TEST    edx, 40000000h "
        " JZ   NotSupportIA64 "
        " MOV  isSupport, 1   "
        " NotSupportIA64:     "
        __ASM_END__
        }
        break;
        }
        #endif
        */
#endif
        return isSupport;
    }
    
    
#ifdef _WINDOWS
    #include <process.h>
    #include <Tlhelp32.h>
    #include <winbase.h>
    #include <string.h>
#endif
    
    void killProcessByName( const char* filename )
    {
#ifdef _WINDOWS
        HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, NULL );
        PROCESSENTRY32 pEntry;
        pEntry.dwSize = sizeof ( pEntry );
        BOOL hRes = Process32First( hSnapShot, &pEntry );
        while ( hRes )
        {
            if ( strcmp( pEntry.szExeFile, filename ) == 0 )
            {
                HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, 0, ( dword_t ) pEntry.th32ProcessID );
                if ( hProcess != NULL )
                {
                    TerminateProcess( hProcess, 9 );
                    CloseHandle( hProcess );
                }
            }
            hRes = Process32Next( hSnapShot, &pEntry );
        }
        CloseHandle( hSnapShot );
#endif
    }
        
    int GetNumberOfProcessors()
    {
#ifdef _WINDOWS
        SYSTEM_INFO sysinfo;
        GetSystemInfo( &sysinfo );
        return sysinfo.dwNumberOfProcessors;
#endif
#ifdef _MACOS
        int nm[2];
        size_t len = 4;
        uint32_t count;
        
        nm[0] = CTL_HW;
        nm[1] = HW_AVAILCPU;
        sysctl( nm, 2, &count, &len, NULL, 0 );
        
        if( count < 1 ) {
            nm[1] = HW_NCPU;
            sysctl( nm, 2, &count, &len, NULL, 0 );
            if( count < 1 ) { count = 1; }
        }
        return count;
#endif
#ifdef _LINUX
        return (int)sysconf( _SC_NPROCESSORS_ONLN );
#endif
        
    }
        
    ulong64_t GetPhysMemorySize()
    {
#if defined(_WINDOWS)
        /* Cygwin under Windows. ------------------------------------ */
        /* New 64-bit MEMORYSTATUSEX isn't available.  Use old 32.bit */
        MEMORYSTATUS status;
        status.dwLength = sizeof( status );
        GlobalMemoryStatus( &status );
        return ( size_t )status.dwTotalPhys;
        
#elif defined(_WINDOWS)
        /* Windows. ------------------------------------------------- */
        /* Use new 64-bit MEMORYSTATUSEX, not old 32-bit MEMORYSTATUS */
        MEMORYSTATUSEX status;
        status.dwLength = sizeof( status );
        GlobalMemoryStatusEx( &status );
        return ( size_t )status.ullTotalPhys;
        
#elif defined(_LINUX) || defined(_MACOS)
        /* UNIX variants. ------------------------------------------- */
        /* Prefer sysctl() over sysconf() except sysctl() HW_REALMEM and HW_PHYSMEM */
        
#if defined(CTL_HW) && (defined(HW_MEMSIZE) || defined(HW_PHYSMEM64))
        int mib[2];
        mib[0] = CTL_HW;
#if defined(HW_MEMSIZE)
        mib[1] = HW_MEMSIZE;   /* OSX. --------------------- */
#elif defined(HW_PHYSMEM64)
        mib[1] = HW_PHYSMEM64;    /* NetBSD, OpenBSD. --------- */
#endif
        int64_t size = 0;      /* 64-bit */
        size_t len = sizeof( size );
        if ( sysctl( mib, 2, &size, &len, NULL, 0 ) == 0 )
        { return ( size_t )size; }
        return 0L;            /* Failed? */
        
#elif defined(_SC_AIX_REALMEM)
        /* AIX. ----------------------------------------------------- */
        return ( size_t )sysconf( _SC_AIX_REALMEM ) * ( size_t )1024L;
        
#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
        /* FreeBSD, Linux, OpenBSD, and Solaris. -------------------- */
        return ( size_t )sysconf( _SC_PHYS_PAGES ) * ( size_t )sysconf( _SC_PAGESIZE );
        
#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGE_SIZE)
        /* Legacy. -------------------------------------------------- */
        return ( size_t )sysconf( _SC_PHYS_PAGES ) * ( size_t )sysconf( _SC_PAGE_SIZE );
        
#elif defined(CTL_HW) && (defined(HW_PHYSMEM) || defined(HW_REALMEM))
        /* DragonFly BSD, FreeBSD, NetBSD, OpenBSD, and OSX. -------- */
        int mib[2];
        mib[0] = CTL_HW;
#if defined(HW_REALMEM)
        mib[1] = HW_REALMEM;      /* FreeBSD. ----------------- */
#elif defined(HW_PYSMEM)
        mib[1] = HW_PHYSMEM;      /* Others. ------------------ */
#endif
        unsigned int size = 0;        /* 32-bit */
        size_t len = sizeof( size );
        if ( sysctl( mib, 2, &size, &len, NULL, 0 ) == 0 )
        { return ( size_t )size; }
        return 0L;            /* Failed? */
#endif /* sysctl and sysconf variants */
        
#else
        return 0L;            /* Unknown OS. */
#endif
        
    }
    
    ulong64_t GetAvailPhysMemorySize()
    {
#if defined(_WINDOWS)
        /* Windows. ------------------------------------------------- */
        /* Use new 64-bit MEMORYSTATUSEX, not old 32-bit MEMORYSTATUS */
        MEMORYSTATUS status;
        status.dwLength = sizeof( status );
        GlobalMemoryStatus( &status );
        return ( size_t )status.dwAvailPhys;
#endif
        
#if defined(_LINUX)
        return sysconf( _SC_AVPHYS_PAGES ) * sysconf( _SC_PAGESIZE );
#endif
    }
    
    ulong64_t GetFreeMemorySize()
    {
        /*
        uint64_t vm_size = 0;
        FILE *statm = fopen("/proc/self/statm", "r");
        if (!statm)
         return 0;
        if (fscanf("%ld", &vm_size) != 1)
        {
         flcose(statm);
         return 0;
        }
        vm_size = (vm_size + 1) * 1024;
        
        rlimit lim;
        if (getrlimit(RLIMIT_AS, &lim) != 0)
         return 0;
        if (lim.rlim_cur <= vm_size)
         return 0;
        if (lim.rlim_cur >= 0xC000000000000000ull) // most systems cannot address more than 48 bits
         lim.rlim_cur  = 0xBFFFFFFFFFFFFFFFull;
        return lim.rlim_cur - vm_size;
        */
        return 0;
    }

    const char* GetCurrentProcessFullName( char* nbuf, int nlen )
    {
#ifdef _WINDOWS
        GetModuleFileNameA( NULL, nbuf, nlen );
#endif
#ifdef _LINUX
        sprintf( nbuf, _T( "%s" ), program_invocation_name );
#endif
        return nbuf;
    }

    /*
    const wchar_t* GetCurrentProcessFullName( wchar_t* nbuf, int nlen )
    {
#ifdef _WINDOWS
        GetModuleFileNameW( NULL, nbuf, nlen );
#endif
#ifdef _LINUX
       //int len = ( strlen( program_invocation_name ) + 1 ) * 2;
       ////wchar_t* strW = new wchar_t[len];
       //std::vector<wchar_t> unic( len );
       //mtow( program_invocation_name, &unic[ 0 ], len );
       //program_invocation_name;
       //wsprintf( nbuf, L"%s", &unic[ 0 ] );
#endif
        return nbuf;
    }
    */

    //  const tchar_t* GetCurrentProcessShortName( tchar_t* nbuf, int nlen )
    //  {
    //#ifdef _WINDOWS
    //      GetModuleFileName( NULL, nbuf, nlen );
    //      getpathfname( nbuf, nbuf );
    //#endif
    //#ifdef _LINUX
    //          _stprintf( nbuf, _T( "%s" ), program_invocation_short_name );
    //#endif
    //      return nbuf;
    //  }
   
    const char* GetCommandLineA()
    {
#ifdef _WINDOWS
        return ::GetCommandLineA();
#endif
#ifdef _LINUX

#endif
        return NULL;
    }

    const wchar_t* GetCommandLineW()
    {
#ifdef _WINDOWS
        return ::GetCommandLineW();
#endif
#ifdef _LINUX

#endif
        return NULL;
    }
};
