//
//
//
//
//
//
//
//        linux : Link with -ldl     (dl)
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#if defined(_WINDOWS)  /* Windows platforms */
    #include <wtypes.h>
    #include <minwinbase.h>
#endif
#if defined(_LINUX)    /* Linux platforms */
    #include <dlfcn.h>
#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


namespace pfkrl  // system kernel module
{
#if defined(_WINDOWS)  /* Windows platforms */
    enum SLType
    {
        RTLD_LAZY,     // 暂缓决定，等有需要时再解出符号
        RTLD_NOW,      // 立即决定，返回前解除所有未决定的符号。
        RTLD_LOCAL,
        RTLD_GLOBAL,   // 允许导出符号
        RTLD_GROUP,
        RTLD_WORLD
    };
#endif

    namespace ShareLibrary
    {
#if defined(_WINDOWS) /* Windows platforms */
    typedef HMODULE    Handle;
#endif
#if defined(_LINUX)  /* Unix/linux */
    typedef void*      Handle;
#endif
        Handle    Open( const tchar_t* filename, int flag = RTLD_LAZY );
        void*     GetFunction( Handle handle, const tchar_t* funcname );
        int       Close( Handle handle );
    }
}