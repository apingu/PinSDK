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
//  Update:
//         Pin   2004/12/26
//              for package struct
//
//////////////////////////////////////////////////////////////////////////

#pragma once

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-if_socket.h"


namespace net
{
    class TCPSocket;
}


//enum CsMessageType    { CsCUSERDEFINE,
//         CsCSYSDEFINE = 0XFFFF };
//         CsCSYSDEFINE = MAX_MESSAGE_SIZE+1 };



///////////////////////////////////////////////////////////////////////////////////////
// command Net
namespace cyberspace
{
    namespace Abtp
    {
        void     Initialize( void );

        //==========================================================================================================
        //  name : cyberspace::Abtp::SendPacket
        //  Dest : send message func
        //  @Param key : 16bytes key, input oldkey to encrypt and output newkey to next encrypt
        //
        int      SendPacket( CsIfSocket* sockif, cs_session_uid_t sessionid, uniqueid_t who, channel_uid_t  cid, const ubyte_t* bufptr, _cs_size_t buflen, mstimen_t mt, char* key = NULL );


        //==========================================================================================================
        //  name : cyberspace::Abtp::RecvPacket
        //  Dest : recv message func if buflen not enought will delete bufptr first and renew one
        //  @Param key : 16bytes key, input oldkey to decrypt and output newkey to
        //
        //int      cyberspace::Abtp::RecvPacket( CsIfSocket* sockif, cs_session_uid_t& sessionid, macn_t& mac, uniqueid_t& who, channel_uid_t& cid, ubyte_t** bufptr, _cs_size_t& buflen, _cs_size_t& recvlen, mstimen_t& mt, char* key = NULL );
        int      RecvPacket( CsIfSocket* sockif, cs_session_uid_t& sessionid, uniqueid_t& who, channel_uid_t& cid, ubyte_t** bufptr, _cs_size_t& buflen, _cs_size_t& recvlen, mstimen_t& mt, char* key = NULL );

        //==========================================================================================================
        //  name : CsRecvSocketUniqueId
        //  Dest : recv message sender uniqueid
        //  @Param key : 16bytes key, input oldkey to decrypt and output newkey to
        //
        //uniqueid_t CsRecvSocketUniqueId( CsIfSocket* sockif );
    }
}



///////////////////////////////////////////////////////////////////////////////////////
// command factory
// %d==>int,        ==> &value
// %l==>long64,     ==> &value
// %h==>short,      ==> &value
// %q==>longlong_t  ==> &value
// %c==>char,
// %s==>char*,                                              sizeofcsstr
// %w==>WCHAR*
// %v==>void*, two param ==> ex  ("%v", buff, buflen(int));                       01) buffer size must be sizeofcsbuf,   02) buff need new space
//
// SplitArgument (spical) % p==> like v but only set point to input param , two param ==> ex  ("%v", buff, buflen)  01) no need new space 02) only on SplitArgument
//                               inline void GetRequest( _IN const Databuf* data, _OUT _cs_string& keybuf, _OUT ubyte_t** valbuf, _OUT int& vallen, _OUT int& live_sec )
//                               {
//                                   CsFactory::SplitArgument( data->bufptr, "%s%p%d", kb, valbuf, &vallen, &live_sec );
//                               }
//                               inline void GetRequest( ubyte_t* buff, _CsSessionEntity*& nodevec, _cs_size_t& len )
//                               {
//                                   int nlen = 0;
//                                   CsFactory::SplitArgument( buff, "%d%p", &len, &( nodevec ), &nlen );
//                                   return;
//                               }
namespace CsFactory
{
    _cs_size_t MakeArgument( ubyte_t* out, const char* fmt, ... );
    _cs_size_t SplitArgument( const ubyte_t* in, const char* fmt, ... );
};



//======================================
//  Message strcut
//======================================

#ifdef _WINDOWS
    #pragma pack ( push, 1 )
#else
    __attribute__( ( aligned( 1 ) ) )
#endif

typedef struct
{
    short len;
    char  string[ 1 ];
} NETSTR;

#ifdef _WINDOWS
    #pragma pack ( pop )
#else
    //__attribute__ ( ( packed ) )
#endif

#define sizeofcsstr( S ) (unsigned int)( _cssizeof( ushort_t ) + ( strlen(S) + 1 )* _cssizeof( _cs_char_t )  ) //%s
#define sizeofcsbuf( L ) (unsigned int)( _cssizeof( ushort_t ) + L )                                      //%v
#define NETSTR_SIZE(s)   (unsigned int)( s->len + _cssizeof( short ) )

/*
inline _cs_size_t CsCommandMessageLen(CsCommandPacket* h)
{
    _cs_size_t len=(_cssizeof(CsCommandHeader)+(h->Header.command_len));
    return (len<_MAX_SOCKET_BUFF)? len+(h->Header.command_len):0;
}
*/
//#define CsCommandMessageLen(h)   (_cssizeof(CsCommandHeader)+((h)->Header.command_len)>MAX_MESSAGE_SIZE)? _cssizeof(CsCommandHeader)+((h)->Header.command_len):
