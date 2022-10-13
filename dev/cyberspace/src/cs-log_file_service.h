//
//
//
//  寫在本地機器上 檔案的 系統紀錄
//
//
//
//
//
//
#pragma once

#include "cs-def.h"

namespace cyberspace
{
    namespace LogFileSerivce
    {
        uint_t Open( const char* filename = "history.log" );
        void   Close( uint_t  outid );
        void   OutputMessageV( uint_t outid, CsLogMode m, const _cs_char_t* format, va_list arglist );
        void   OutputMessage( uint_t outid, CsLogMode m, const _cs_char_t* format, ... );
    }
}