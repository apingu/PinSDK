//memio.inl memory control
//
//
//    1. use MMX to do memcpy is in vain, it will be more solw than REP
//    2. 32 bits mode int VGA card it A R G B
//    3. gcc + -masm=intel
//
//
//
//
//
//
//
//                                               Copyright    (C)    1999    Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <string.h>
#ifdef _WINDOWS
#include <io.h>
//extern "C" int x64MEMCPY();
#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "pf-def.h"


#define __pfmemcpy   IOMEMCPY  // 2018 實測 下來 memcpy 已比組合語言跑得快
#define __pfmemset   IOMEMSET 

//=============================================================================
// NAME:
// DESC:1 byte pixel move
//=============================================================================
inline void IOMEMCPY( void* dest, const void* src, int n )
{
#if defined( ENV32BIT )
    #if defined(__MSVC__)
        __ASM_BEGIN__
            MOV    esi, src
            MOV    edi, dest
            MOV    eax, n
            MOV    ecx, eax
            SHR    ecx, 2
            REP    MOVSD
            MOV    ecx, eax
            AND    ecx, 3
            REP    MOVSB
            __ASM_END__
    #elif defined(__GNUC__)
        __ASM_BEGIN__
            //"    MOV    %1,        %%esi  \n\t"
            //"    MOV    %0,        %%edi  \n\t"
            "    MOV    %2,        %%eax  \n\t"
            "    MOV    %%eax,     %%ecx  \n\t"
            "    SHR    $2,        %%ecx  \n\t"
            "    REP    MOVSD             \n\t"
            "    MOV    %%eax,     %%ecx  \n\t"
            "    AND    $3,        %%ecx  \n\t"
            "    REP    MOVSB             \n\t"
            : "=D"
            ( dest )
            : "S"( src ), "a"( n )
            :
            __ASM_END__
    #endif
#else
    memcpy( dest, src, n );
#endif

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
inline void IOMEMSET( void* dest, int c, int n )
{
    int ls = n % 2;
#if defined( ENV32BIT )
    #if defined(__MSVC__)
        __ASM_BEGIN__
            MOV    edi, dest
            MOV    eax, c
            MOV    ecx, n
            SHR    ecx, 1
            REP    STOSW
            MOV    ecx, ls
            REP    STOSB
            __ASM_END__
    #elif defined(__GNUC__)
        __ASM_BEGIN__
            //"    MOV    %0,        %%edi  \n\t"
            "    MOV    %1,        %%eax  \n\t"
            "    MOV    %1,        %%ebx  \n\t"
            "    SHL    $8,        %%ebx  \n\t"
            "    OR     %%ebx,     %%eax  \n\t"
            "    MOV    %2,        %%ecx  \n\t"
            "    SHR    $1,        %%ecx  \n\t"
            "    REP    STOSW             \n\t"
            "    MOV    %3,        %%ecx  \n\t"
            "    REP    STOSB             \n\t"
            : "=r"
            ( dest )
            : "r"( c ), "r"( n ), "r"( ls )
            __ASM_END__
    #endif
#else
    memset( dest, c, n );
#endif
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
// Sets buffers to a specified dword (NOTE: count is in bytes!)
inline void DWMEMSET( void* dest, int c, unsigned int n )
{
    // can't fnd STOSD in gnu why?
#if defined(__MSVC__)  && defined( ENV32BIT )
    __ASM_BEGIN__
        MOV    edi, dest
        MOV    eax, c
        MOV    ecx, n
        XOR    ebx, ebx
        SHR    ecx, 1
        ADC    bl, bl
        SHR    ecx, 1
        ADC    bh, bh
        REP    STOSD
        MOV    cl, bh
        REP    STOSW
        MOV    cl, bl
        REP    STOSB
        __ASM_END__
#elif defined(__GNUC__)
    IOMEMSET( dest, c, n );
    /*
    __ASM_BEGIN__
        //NF
        //"    MOV    dest,  %%edi  \n\t"
        "    MOV    %1,        %%eax  \n\t"
        "    MOV    %2,        %%ecx  \n\t"
        "    XOR    %%ebx,     %%ebx  \n\t"
        "    SHR    $1,        %%ecx  \n\t"
        "    ADC    %%bl,      %%bl   \n\t"
        "    SHR    %1,        %%ecx  \n\t"
        "    ADC    %%bh,      %%bh   \n\t"
        "    REP    STOSD             \n\t"
        "    MOV    %%bh,      %%cl   \n\t"
        "    REP    STOSW             \n\t"
        "    MOV    %%bl,      %%cl   \n\t"
        "    REP    STOSB             \n\t"
: "=r"
        ( dest )
: "r"( c ), "r"( n )
    __ASM_END__
    */
#else
    memset( dest, c, n );
#endif
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
inline void IOMAPCPY( ubyte_t* dest, int d_w, const ubyte_t* src, int s_w, int s_h, int n )
{
    int C_DIV_2 = n >> 2;
    int C_MOD_1 = n & 3;

    for( int row = 0; row < s_h; row++ )
    {
#if defined(__MSVC__) && defined( ENV32BIT )
        __ASM_BEGIN__
            MOV    esi, src
            MOV    edi, dest
            MOV    ecx, C_DIV_2
            REP    MOVSD
            MOV    ecx, C_MOD_1
            REP    MOVSB
            __ASM_END__
#elif defined(__GNUC__)
        __ASM_BEGIN__
            //NF
            //"    MOV    src,   %%esi  \n\t"
            //"    MOV    dest,  %%edi  \n\t"
            "    MOV    %2,        %%ecx  \n\t"
            "    REP    MOVSD             \n\t"
            "    MOV    %3,        %%ecx  \n\t"
            "    REP    MOVSB             \n\t"
            : "=r"
            ( dest )
            : "r"( src ), "r"( C_DIV_2 ), "r"( C_MOD_1 )
            __ASM_END__
#else
        memcpy( dest, src, n );
#endif
        dest += d_w; // 指向下一列 pixel 起始位址
        src += s_w;
    }
}


///////////////////////////////////////////////////////////////////////////////
//  MMX function
//

//=============================================================================
// NAME:
// DESC:1 byte pixel move
//=============================================================================
inline void MMX_IOMEMCPY( void* dest, void* src, int n )
{
#if defined(__MSVC__) && defined( ENV32BIT )
    __ASM_BEGIN__
        MOV    esi, src
        MOV    edi, dest
        MOV    eax, n
        //CMP    edx, 9
        CMP    eax, 8
        JL     END_MMX
        MOV    ecx, eax
        SHR    ecx, 3
        ALIGN 8
        LP0:
    MOVQ   mm0, [ esi ]
        ADD    esi, 8
        MOVQ[ edi ], mm0
        ADD    edi, 8
        DEC    ecx
        JNZ    LP0
        MOV    ecx, eax
        AND    ecx, 7
        REP    MOVSB
        EMMS
        END_MMX :
    __ASM_END__
        int C_MOD_7 = n & 7;
    if( C_MOD_7 != 0 )
    {
        int doready = n - C_MOD_7;
        __pfmemcpy( ( ubyte_t* )dest + doready, ( ubyte_t* )src + doready, C_MOD_7 );
    }
#elif defined(__GNUC__)
    __ASM_BEGIN__
        //"    MOV    %1,       %%esi    \n\t"
        //"    MOV    %0,       %%edi    \n\t"
        //"    MOV    %2,       %%eax    \n\t"
        "    CMP    $8,       %%eax    \n\t"
        "    JL     END_MMX            \n\t"
        "    MOV    %%eax,     %%ecx   \n\t"
        "    SHR    $3,        %%ecx   \n\t"
        //"    ALIGN 8                   \n\t"
        " LP0:                         \n\t"
        "    MOVQ   (%1),      %%mm0   \n\t"
        "    ADD    $8,        %1      \n\t"
        "    MOVQ   %%mm0,     (%0)    \n\t"
        "    ADD    $8,        %0      \n\t"
        "    DEC    %%ecx              \n\t"
        "    JNZ    LP0                \n\t"
        "    MOV    %%eax,     %%ecx   \n\t"
        "    AND    $7,        %%ecx   \n\t"
        "    REP    MOVSB              \n\t"
        "    EMMS                      \n\t"
        " END_MMX:                     \n\t"
        : "=D"
        ( dest )
        : "S"( src ), "a"( n )
        __ASM_END__
        int C_MOD_7 = n & 7;
    if( C_MOD_7 != 0 )
    {
        int doready = n - C_MOD_7;
        __pfmemcpy( ( ubyte_t* )dest + doready, ( ubyte_t* )src + doready, C_MOD_7 );
    }
#else
    memcpy( dest, src, n );
#endif
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
inline void MMX_IOMEMSET( void* dest, int c, int n )
{
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
inline void MMX_IOZEROMEMORY( void* dest, int n )
{
#if defined(__MSVC__) && defined( ENV32BIT )
    //ubyte_t* b_d_pt = (ubyte_t*)dest;
    __ASM_BEGIN__
        ALIGN 4
        MOV    edi, dest
        MOV    eax, n
        MOV    ebx, eax
        SHR    eax, 3
        AND    ebx, 7
        CMP    eax, 0
        JE     q1
        PXOR   mm1, mm1
        L1 :
    MOVQ[ edi ], mm1
        ADD    edi, 8
        DEC    eax
        JNE    L1
        q1 :
    TEST   ebx, 4
        JZ     q2
        STOSD
        q2 :
    TEST   ebx, 2
        JZ     q3
        STOSW
        q3 :
    TEST   ebx, 1
        JZ     q4
        STOSW
        q4 :
    EMMS
        __ASM_END__
#elif defined(__GNUC__)
    //ubyte_t* b_d_pt = (ubyte_t*)d_pt;
    __ASM_BEGIN__
        " ALIGN 4                      \n\t"
        //"    MOV    %0,       %%edi    \n\t"
        "    MOV    %1,       %%eax    \n\t"
        "    MOV    %%eax,    %%ebx    \n\t"
        "    SHR    $3,       %%eax    \n\t"
        "    AND    $7,       %%ebx    \n\t"
        "    CMP    $0,       %%eax    \n\t"
        "    JE     q1                 \n\t"
        "    PXOR   %%mm1,    %%mm1    \n\t"
        " L1:                          \n\t"
        "    MOVQ   %%mm1,    (%%edi)  \n\t"
        "    ADD    $8,       %%edi    \n\t"
        "    DEC    %%eax              \n\t"
        "    JNE    L1                 \n\t"
        " q1:                          \n\t"
        "    TEST   $4,       %%ebx    \n\t"
        "    JZ     q2                 \n\t"
        "    STOSD                     \n\t"
        " q2:                          \n\t"
        "    TEST   $2,       %%ebx    \n\t"
        "    JZ     q3                 \n\t"
        "    STOSW                     \n\t"
        " q3:                          \n\t"
        "    TEST   $1,       %%ebx    \n\t"
        "    JZ     q4                 \n\t"
        "    STOSW                     \n\t"
        " q4:                          \n\t"
        " EMMS                         \n\t"
        : "=r"
        ( dest )
        : "r"( n )
        __ASM_END__
#else
    memset( dest, 0, n );
#endif
}

///////////////////////////////////////////////////////////////////////////////
//  SSE function
//
//=============================================================================
// NAME:
// DESC:
//=============================================================================
inline void SSE_IOMEMCPY( void* dest, void* src, int n )
{
#if defined(__MSVC__) && defined( ENV32BIT )
    __ASM_BEGIN__
        MOV    esi, src
        MOV    edi, dest
        MOV    edx, n
        CMP    edx, 17        // 不滿16個byte 直接結束 (<17個)
        JL     END_SSE
        MOV    ecx, edx
        SHR    ecx, 4         // 一次處理 16個byte 所以 /16  == <<4
        //ALIGN 8             // 在macos上不會過 win32上拿掉也好像沒甚麼影響 那我那時候為何要加他?
        LP0:
    MOVDQU xmm0, [ esi ]
        MOVDQU[ edi ], xmm0
        ADD    esi, 16
        ADD    edi, 16
        ADD    eax, 8
        DEC    ecx
        JNZ    LP0
        END_SSE :
    __ASM_END__
        int cmod = n % 16;
    if( cmod > 0 )
    {
        int doready = n - cmod;
        __pfmemcpy( ( ubyte_t* )dest + doready, ( ubyte_t* )src + doready, cmod );
    }
#elif defined(__GNUC__)
    __ASM_BEGIN__
        //"    MOV    %1,       %%esi   \n\t"
        //"    MOV    %0,       %%edi   \n\t"
        //"    MOV    %2,       %%edx   \n\t"
        "    CMP    $16,      %%eax   \n\t"
        "    JL     END_SSE           \n\t"
        "    MOV    %%eax,    %%ecx   \n\t"
        "    SHR    $4,       %%ecx   \n\t"
        " LP0:                        \n\t"
        "    MOVDQU (%1),     %%xmm0  \n\t"
        "    MOVDQU %%xmm0,   (%0)    \n\t"
        "    ADD    $16,      %1      \n\t"
        "    ADD    $16,      %0      \n\t"
        "    ADD    $8,       %eax    \n\t"
        "    DEC    %%ecx             \n\t"
        "    JNZ    LP0               \n\t"
        " END_SSE:                    \n\t"
        : "=r"
        ( dest )
        : "r"( src ), "r"( n )
        __ASM_END__
        int cmod = n % 16;
    if( cmod > 0 )
    {
        int doready = n - cmod;
        __pfmemcpy( ( ubyte_t* )dest + doready, ( ubyte_t* )src + doready, cmod );
    }
#else
    memcpy( dest, src, n );
#endif
}