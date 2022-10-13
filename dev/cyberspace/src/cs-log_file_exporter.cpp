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

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-time.h>
#include <string.h>
#include <pf-thread.h>
#include <pf-stdlib.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-log_file_exporter.h"

namespace CsLog
{
    //=============================================================================
    //
    //
    //=============================================================================
    FileExporter::FileExporter( const char* filename )
    {
        logf_ = NULL;
        if( filename != NULL )
            Open( filename );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    FileExporter::~FileExporter()
    {
        Close();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void FileExporter::Open( const char* filename )
    {
        //char  m_filename[ 256 ];
        pftml::Time today;
        //strcpy( log_date_, today.GetDateSTR("%04d%02d%02d").c_str() );
        strcpy( log_date_, today.GetDateSTR( "%04d%02d%02d" ).c_str() );
        {
            char drive[ _MAX_DRIVE ];
            msplitfpath( filename, drive, log_folder_, log_fname_, log_ext_ );
        }
        is_live_ = true;
        pfkrl::Thread::BeginProcess( DoUpdate, this, ( "FileExporter::DoUpdate" ) );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void FileExporter::Close()
    {
        is_live_ = false;
        pfkrl::Thread::WaitAllExit( 30000, ( "FileExporter::DoUpdate" ) );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void FileExporter::OutputMessage( const char* string )
    {
        if( string == NULL )
        {
            return;
        }

        if( strcmp( string, "" ) == 0 )
        {
            return;
        }

        pftml::Time now_time;
        _cs_string message;
        pfstd::strprintf( message, "%s : %s", now_time.GetDateTimeTCS().c_str(), string );

        {
            TS_Lock lock( &log_lock_ );
            log_message_.push_back( message );
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void FileExporter::DoUpdate( void* data )
    {
        FileExporter* fobj = ( FileExporter* )data;

        //mstimen_t prev_time = pftml::Time::GetTickMSTime();
        pftml::Time begin_time;
        int t01_time = begin_time.GetDay();
        while( fobj->is_live_ )
        {
            // 判斷 開檔 換檔
            {
                // 不同天 換檔案
                pftml::Time now_time;
                int t02_time = now_time.GetDay();
                //if ( ( now_time.GetDateSTR( "%04d%02d%02d" ) != fobj->log_date_ ) || ( fobj->logf_ == NULL ) )
                if( ( t01_time != t02_time ) || ( fobj->logf_ == NULL ) )
                {
                    TS_Lock lock( &fobj->log_lock_ );
                    if( fobj->logf_ )
                    {
                        fflush( fobj->logf_ );
                        fclose( fobj->logf_ );
                        fobj->logf_ = NULL;
                    }

                    pfstd::MSSleep( 1000 );

                    //strcpy( fobj->log_date_, now_time.GetDateSTR( "%04d%02d%02d" ).c_str() );
                    strcpy( fobj->log_date_, now_time.GetDateSTR( "%04d%02d%02d" ).c_str() );

                    char  filename[ 256 ];
                    sprintf( filename, "%s%s_%s%s", fobj->log_folder_, fobj->log_fname_, fobj->log_date_, fobj->log_ext_ );
                    if( ( fobj->logf_ = fopen( filename, "ab" ) ) == NULL )
                    {
                        //exit( 0 );
                    }
                    t01_time = t02_time;
                }
            }

            // 10秒寫入一次 降低成本
            pfstd::MSSleep( 10000 );

            _mbsvector  logmessage;
            {
                TS_Lock lock( &fobj->log_lock_ );
                logmessage = fobj->log_message_;
                fobj->log_message_.clear();
            }

            for( int i = 0; i < logmessage.size(); i++ )
            {
                fwrite( logmessage[ i ].c_str(), logmessage[ i ].size(), 1, fobj->logf_ );
            }

            //mstimen_t curr_time = pftml::Time::GetTickMSTime();
            //// 10秒寫入一次 降低成本
            //if ( ( curr_time - prev_time ) > 10000 )
            //{
            //    fflush( fobj->logf_ );
            //    prev_time = curr_time;
            //
            //}
            fflush( fobj->logf_ );
        }

        // 餘下的寫完
        {
            _mbsvector  logmessage;
            {
                TS_Lock lock( &fobj->log_lock_ );
                logmessage = fobj->log_message_;
                fobj->log_message_.clear();
            }

            for( int i = 0; i < logmessage.size(); i++ )
            {
                fwrite( logmessage[ i ].c_str(), logmessage[ i ].size(), 1, fobj->logf_ );
            }

            fflush( fobj->logf_ );
            fclose( fobj->logf_ );
            fobj->logf_ = NULL;
        }
    }
}


