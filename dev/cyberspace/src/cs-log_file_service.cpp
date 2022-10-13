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

//01.== C 系統文件/C++ 系統文件
#include <assert.h>
#include <list>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-thread.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-log_file_service.h"
#include "cs-log_file_exporter.h"


#ifdef _LINUX
    #include <stdarg.h>
#endif

namespace cyberspace
{
    namespace LogFileSerivce
    {
        std::vector<CsLog::FileExporter*> g_logfiles;
        bool g_is_live = true;
        struct LogData
        {
            CsLog::FileExporter* fileexporter;
            _cs_char_t string[ _MAX_LONG_MSG ];            
        };

        std::list<LogData*> g_LogQueue;
        
        uint_t Open( const char* filename )
        {
            int idx = -1;
            CsLog::FileExporter* lf = new CsLog::FileExporter;
            lf->Open( filename );
            
            for ( _cs_size_t i = 0; i < g_logfiles.size(); i++ )
            {
                if ( g_logfiles[i] == NULL )
                {
                    g_logfiles[ i ] = lf;
                    idx = i;
                    break;
                }
            }
            
            if ( idx == -1 )
            {
                idx = g_logfiles.size();
                g_logfiles.push_back( lf );
                // 由執行續寫檔
                //pfkrl::Thread::BeginProcess( DoUpdate, lf, ( "LogFileSerivce::DoUpdate" ) );
            }
            
            return idx;
        }
        
        void Close( uint_t outid )
        {
            g_is_live = false;
            //pfkrl::Thread::WaitAllExit( 30000 );
            if ( outid < g_logfiles.size() )
            {
                CsLog::FileExporter* lf = g_logfiles[ outid ];
                SAFE_DELETE( lf );
                g_logfiles[ outid ] = NULL;
            }
        }
        
        void OutputMessageV( uint_t outid, CsLogMode m, const _cs_char_t* format, va_list arglist )
        {
            _cs_char_t string[ _MAX_LONG_MSG ] = { "" };
            assert( format != NULL );
            vsprintf( string, format, arglist );
            if ( strcmp( string, "" ) == 0 )
            {
                return;
            }
            
            if ( IF_BIT_FLAG( m, LOGMODE_PRINTOUT ) )
            {
                printf( string );
            }
            
            if ( IF_BIT_FLAG( m, LOGMODE_OUTDEBUG ) )
            {
                Pf_INFO( string );
            }
            
            //TS_Lock lock( &g_lLogMessage );
            if ( IF_BIT_FLAG( m, LOGMODE_SAVEFILE ) )
            {
                if ( outid < g_logfiles.size() )
                {
                    if ( g_logfiles[ outid ] != NULL )
                    {
                        g_logfiles[ outid ]->OutputMessage( string );
                    }
                }
            }
        }
        
        void OutputMessage( uint_t outid, CsLogMode m, const _cs_char_t* format, ... )
        {
            //_cs_char_t string[ _MAX_LONG_MSG ] = { "" };
            //FILE str;
            //register FILE *outfile = &str;
            va_list arglist;
            //register int retval;
            //const _cs_char_t* i = format;
            va_start( arglist, format );
            OutputMessageV( outid, m, format, arglist );
            va_end( arglist );
        }
    }
}
