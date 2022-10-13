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
#if defined(_WINDOWS)  /* Windows platforms */
    #include <Windows.h>
#endif

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pf-share_library.h"


namespace pfkrl
{
    namespace ShareLibrary
    {
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Handle Open( const tchar_t* filename, int flag )
        {
#if defined(_WINDOWS)
            return LoadLibrary( filename );
#endif
#if defined(_LINUX)
            return dlopen( filename, flag );
#endif
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void* GetFunction( Handle handle, const tchar_t* funcname )
        {
#if defined(_WINDOWS)
            return GetProcAddress( handle, funcname );
#endif
#if defined(_LINUX)
            return dlsym( handle, funcname );
#endif
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Close( Handle handle )
        {
#if defined(_WINDOWS)
            return FreeLibrary( handle );
#endif
#if defined(_LINUX)
            return dlclose( handle );
#endif
        }
    }
}