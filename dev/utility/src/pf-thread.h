// process thread
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
//       * in linux must link  libpthread.a  (pthread)
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <vector>
#if defined(_WINDOWS)  /* Windows platforms */
    //#include <process.h>
#endif
#if defined(_LINUX)    /* Linux platforms */
#include <pthread.h>
#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>
#include <t_lock.h>

typedef void( _CALLBACK *func_address )( void* argv );

namespace pfkrl
{
    namespace Thread
    {
#if defined(_WINDOWS) /* Windows platforms */
        typedef ulong64_t     Handle;
#endif
#if defined(_LINUX)  /* Unix/linux */
        typedef pthread_t     Handle;
#endif
        Handle   BeginProcess( func_address func, void* data, const char* name );
        int      GetProcessSize( void );
        bool     TestProcessRun( Handle threadid );
        bool     TestProcessExit( std::vector<Handle> threadids );
        bool     WaitAllExit( mstimen_t waittime, const char* name = NULL );
    };

};