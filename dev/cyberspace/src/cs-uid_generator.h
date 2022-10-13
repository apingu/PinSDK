//
//
//
//
//
//  各種ID的產生
// 
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

#pragma once

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <net/net-socket.h>
#include <algorithmic/crypt-crc32.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"


namespace cyberspace
{
    namespace uid_generator
    {
        inline cs_session_uid_t GetSessionUID( _IN const _cs_char_t* name )
        {
            _cs_string currname = ( "CsSession_" );
            currname.append( name );
            return crc32::Encode( ( const char* )currname.c_str(), ( unsigned int )( currname.size() * _cssizeof( _cs_char_t ) ) );
        }

        inline cs_session_uid_t GetPaperUID( _IN const _cs_char_t* key )
        {
            _cs_string currname = ( "CsPaper_" );
            currname.append( key );
            return crc32::Encode( ( const char* )currname.c_str(), ( unsigned int )( currname.size() * _cssizeof( _cs_char_t ) ) );
        }

        inline cs_section_uid_t GetServerUID( _IN const _cs_char_t* ip, _IN ushort_t port )
        {
            _cs_string currname = ( "CsServer_" );
            _pf_tstrprintfa( currname, ( "%s%d" ), ip, port );
            return crc32::Encode( ( const char* )currname.c_str(), currname.size() * _cssizeof( _cs_char_t ) );
        }

        // 用ip port createtime r建立一個ip
        inline uniqueid_t GetSocketAddrUID( _IN sockaddr* addr, _IN mstimen_t createtime )
        {
            sockaddr_in* addrin = ( sockaddr_in* )addr;
            std::string key;
            pfstd::strprintf( key, ( "CsSocket_%u:%d:lld" ), addrin->sin_addr.s_addr, addrin->sin_port, createtime );
            return crc32::Encode( ( const char* )key.c_str(), ( unsigned int )( key.size() * _cssizeof( _cs_char_t ) ) );
        }
    }
}