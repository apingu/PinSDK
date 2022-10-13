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
#if defined(_WINDOWS)
#include <Windows.h>
#endif
#if defined(_LINUX)
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#endif
#include <string.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pf-def.h"
#include "pf-debug.h"
#include "pf-stdlib.h"
#include "graphic2d.h"


//T_Lock   g_Debuglock;
int mydebugouttype = 0;

void DebugDelf( void )
{
#if defined(_WINDOWS)
    _unlink( _Debugfname );
#endif
#if defined(_LINUX)
    unlink( _Debugfname );
#endif
}

/*
void SetDebugOutType( int o )
{
    mydebugouttype = o;
}


void DebugOutPutf( const char* msg, const char* title )
{
    //TS_Lock lock(&g_Debuglock);
    DebugOutPutf( _Debugfname, msg, title );
}

void DebugOutPutf( const char* fname, const char* msg, const char* title )
{
    FILE* df;

    if( ( df = fopen( fname, _T( "ab" ) ) ) == NULL )
    {
        //OutPutMessage("can't open debug file  -_-b ", "open failure");
        return ;
    }

    time_t timep;
    time( &timep );
    //struct tm* p = localtime(&timep);
    tchar_t message[1024];

    int inlen = strlen( msg );
    if( inlen < 950 )
    {
        _stprintf( message, _T( "%s" ), ctime( &timep ) );
        int len = strlen( message );
        _stprintf( ( message + len - 1 ), _T( " - %s : %s \n" ), title, msg );
        fwrite( message, strlen( message ), 1, df );
        fclose( df );
        if( mydebugouttype == 1 )
        {
            PfOutputDebugString( message );
        }
        else if( mydebugouttype == 2 )
        {
            printf( message );
        }
        else if( mydebugouttype == 3 )
        {
            PfOutputDebugString( message );
            printf( message );
        }
    }
    else
    {
        _stprintf( message, _T( "%s" ), ctime( &timep ) );
        int len = strlen( message );
        _stprintf( ( message + len - 1 ), _T( " - %s : Message too long32_t\n" ), title );
        fwrite( message, strlen( message ), 1, df );
        fclose( df );
        if( mydebugouttype == 1 )
        {
            PfOutputDebugString( message );
        }
        else if( mydebugouttype == 2 )
        {
            printf( message );
        }
        else if( mydebugouttype == 3 )
        {
            PfOutputDebugString( message );
            printf( message );
        }
    }
    return;
}
*/


namespace PfTrace
{
    dword_t color[ TRACE_TYPE_NUM ] =
    {
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_INTENSITY,
        FOREGROUND_RED,
        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    };


#ifdef _LINUX
    int priority[ TRACE_TYPE_NUM ] =
    {
        LOG_USER | LOG_INFO,    // TRACE_INFO = 0,
        LOG_USER | LOG_WARNING, //  TRACE_WARNING,
        LOG_USER | LOG_ERR,     //  TRACE_ERROR,
        LOG_USER | LOG_ALERT,   //  TRACE_ALERT,
        LOG_USER | LOG_DEBUG,   //  TRACE_DEBUG,
    };
#endif

    namespace Debug
    {
#ifdef _LINUX
        static class DebugObj
        {
            tchar_t szLogFilename[ _MAX_LFNAME ];

        public:
            DebugObj()
            {
                strcpy( szLogFilename, program_invocation_short_name );
                openlog( _T( szLogFilename ), LOG_PID | LOG_CONS, LOG_USER );
            }

            ~DebugObj()
            {
                closelog();
            }
        }sDebugObj;

#endif

        //namespace Singolten
        bool     s_Init = false;
        bool     s_ShowConsole = false;
        uint32_t s_ConsoleCoordX = 180;
        uint32_t s_ConsoleCoordY = 1000;
        LG::Rect s_ConsoleRect( 0, 0, 1023, 191 );
        char console_name[ 256 ] = "Debug Console";

#if defined(_WINDOWS)
        HWND     s_hConsole = NULL;
#endif

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int ShowConsole( unsigned int cdx, unsigned int cdy )
        {
            s_ShowConsole = false;
            s_ConsoleCoordX = cdx;
            s_ConsoleCoordY = cdy;
            s_ConsoleRect.top = 0;
            s_ConsoleRect.left = 0;
            s_ConsoleRect.right = (long32_t)( ( ( (float)s_ConsoleCoordX ) * 1.05 ) - 1 );
            s_ConsoleRect.bottom = (long32_t)( ( ( (float)s_ConsoleCoordY ) * 1.05 ) - 1 );
#if defined(_WINDOWS)

            if ( AllocConsole() )
            {
                SetConsoleTitle( console_name );

                for ( ;; )
                {
                    pfstd::MSSleep( 5 );
                    s_hConsole = FindWindow( NULL, console_name );

                    if ( s_hConsole )
                    {
                        HANDLE chandle = GetStdHandle( STD_OUTPUT_HANDLE );
                        COORD cd;
                        cd.X = s_ConsoleCoordX;
                        cd.Y = s_ConsoleCoordY;
                        SetConsoleScreenBufferSize( chandle, cd );
                        MoveWindow( s_hConsole, s_ConsoleRect.right, s_ConsoleRect.top,
                            s_ConsoleRect.width(), s_ConsoleRect.height(), TRUE );
                        SetForegroundWindow( s_hConsole );
                        break;
                    }
                }

                s_ShowConsole = true;
            }

#endif
            return 1;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void OutputString( TRACE_TYPE type, const char* format, ... )
        {
            va_list marker;
            char buf[ 1024 ];
            va_start( marker, format );
            vsnprintf( buf, sizeof( buf ) - 1, format, marker );
            va_end( marker );

            if ( s_ShowConsole )
            {
#if defined(_WINDOWS)
                //dword_t color[MtTraceLog::TRACE_TYPE_NUM] =
                //{
                //    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
                //    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                //    FOREGROUND_RED | FOREGROUND_INTENSITY,
                //    FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
                GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &ConsoleScreenBufferInfo );
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (word_t)color[ type ] );
                ::printf( buf );
                OutputDebugStringA( buf );
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), ConsoleScreenBufferInfo.wAttributes );
#endif
#if defined(_LINUX)
                ::printf( buf );
#endif
            }
            else
            {
#if defined(_WINDOWS)
                OutputDebugStringA( buf );
#endif
#if defined(_LINUX)
                syslog( PfTrace::priority[ type ], _T( "%s" ), buf );
                //printf(priority[type], buf );
                //printf( buf );
#endif
            }
        }

        void OutputString( TRACE_TYPE type, const wchar_t* format, ... )
        {
            va_list marker;
            wchar_t buf[ 1024 ];
            va_start( marker, format );
            vswprintf( buf, sizeof( buf ) - 1, format, marker );
            va_end( marker );

            if ( s_ShowConsole )
            {
#if defined(_WINDOWS)
                //dword_t color[MtTraceLog::TRACE_TYPE_NUM] =
                //{
                //    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
                //    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                //    FOREGROUND_RED | FOREGROUND_INTENSITY,
                //    FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
                GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &ConsoleScreenBufferInfo );
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (word_t)color[ type ] );
                ::wprintf( buf );
                OutputDebugStringW( buf );
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), ConsoleScreenBufferInfo.wAttributes );
#endif
#if defined(_LINUX)
                ::wprintf( buf );
#endif
            }
            else
            {
#if defined(_WINDOWS)
                OutputDebugStringW( buf );
#endif
#if defined(_LINUX)
                syslog( PfTrace::priority[ type ], _T( "%s" ), buf );
                //printf(priority[type], buf );
                //printf( buf );
#endif
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // 
    namespace Footprint
    {
        OnOutput g_stepfunction = NULL;

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void SetFunction( OnOutput func )
        {
            g_stepfunction = func;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void OutputString( const char* funcname, TRACE_TYPE type, const char* text, uint_t maxstep, uint_t curstep )
        {
            if ( g_stepfunction )
                g_stepfunction( funcname, type, text, maxstep, curstep );
        }
    }
}
