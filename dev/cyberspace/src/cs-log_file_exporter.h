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
#pragma once

#include "cs-def.h"
#include <t_lock.h>
#if defined(_WINDOWS)
    #include <io.h>
#endif

namespace CsLog
{
    class FileExporter
    {
        char             log_date_[ 256 ];
        char             log_folder_[ 256 ];
        char             log_fname_[ 256 ];
        char             log_ext_[ 256 ];
                         
        FILE*            logf_;
        bool             is_live_;

        T_Lock           log_lock_;
        _mbsvector       log_message_;

    public:

        FileExporter( const char* filename = "history.log" );
        ~FileExporter();

        void        Open( const char* filename = "history.log" );
        void        Close();
        void        OutputMessage( const char* string );
    private:
        static void DoUpdate( void* data );
    };
}
