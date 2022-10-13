//pf-debug.h header
//
//
//  print debug message in file "MyDebug.txt"
//
//
//
//
//
//
//
//
//
//              Copyright (C) 1999 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <stdio.h>
#include <stdlib.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


#define _Debugfname  _T("./MyDebug.txt")

#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.

//static char _Debugfname[_MAX_LPATH] = "./MyDebug.txt";
//  0 : none  1 : PfOutputDebugString 2: printf: 3 1&&2
//void    SetDebugOutType     ( int o );

void    DebugDelf           ( void );
//void    DebugOutPutf        ( const char* msg, const char* title );
//void    DebugOutPutf        ( const char* fname, const char* msg, const char* title );

//#define DebugMessage(s, t)      DebugOutPutf(s, t)
//#define DebugOutMessage(s, t)   printf("Warring: %s => %s\n", t, s); DebugOutPutf(s, t)
//#define DebugBreakPoint(s, t)   DebugOutPutf(s, t)

namespace PfTrace
{
    enum TRACE_TYPE
    {
        TRACE_INFO = 0,     //! 通常
        TRACE_WARNING,      //! 警告
        TRACE_ERROR,        //!
        TRACE_ALERT,        //! action must be taken immediately
        //TRACE_INFO,       //! 情報
        TRACE_DEBUG,        //! debug-level message
        
        TRACE_TYPE_NUM,
    };
    
    extern dword_t color[ PfTrace::TRACE_TYPE_NUM];

    namespace Debug
    {
        // buffer size and buffer number
        int  ShowConsole( unsigned int cdx, unsigned int cdy );
        //void OutputString( dword_t color, const char* format, ... );
        void OutputString( TRACE_TYPE type, const char* format, ... );
        void OutputString( TRACE_TYPE type, const wchar_t* format, ... );
    }
    

    //////////////////////////////////////////////////////////////////////////
    namespace Footprint
    {
        // foot print
        typedef void ( _CALLBACK *OnOutput ) ( _IN const char* funcname, _IN TRACE_TYPE type, _IN const char* text, _IN uint_t maxstep, _IN uint_t curstep );

        void SetFunction( _IN OnOutput func );
        void OutputString( _IN const char* funcname, _IN TRACE_TYPE type, _IN const char* text, _IN uint_t maxstep, _IN uint_t curstep );
    }
}

//#define PfOutputDebugString( c, f, ... )  PfDebug::OutputString( c, f, ... )
//#define Pf_ABORT( ... )
//#define Pf_ASSERT( ... )
#define Pf_INFO( ... )                 PfTrace::Debug::OutputString( PfTrace::TRACE_INFO, __VA_ARGS__ )
//#define Pf_DEBUG( ... )                PfTrace::Debug::OutputString( PfTrace::TRACE_DEBUG, __VA_ARGS__ )
#define Pf_WARNING( ... )              PfTrace::Debug::OutputString( PfTrace::TRACE_WARNING, __VA_ARGS__ )
#define Pf_ERROR( ... )                PfTrace::Debug::OutputString( PfTrace::TRACE_ERROR, __VA_ARGS__ )




/*
#ifdef _WINDOWS
//#include <io.h>
#ifdef _DEBUG
#define PfOutputDebugString(s)  OutputDebugString(s)
#endif
#ifdef _NODEBUG
#define PfOutputDebugString(s)  printf("%s", s);
#endif
#else
#define PfOutputDebugString(s)  printf("%s", s);
#endif
*/