//
//
//
//
//
//
//
//
//       主動發起方  ==> SetRequest
//         接收方      ==> GetRequest
//         接收方回覆  ==> SetFeedback
//       發起方接收  ==> GetFeedback
//
//
//////////////////////////////////////////////////////////////////////////

#pragma once

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//
#include <memio.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-protocol.h"
#include "cs-def.h"


#define CsID_BASE   (_CsMAXCHANNEL+15400)
//#define SEND_BASE_MESSAGE(s, m)  (s->SendPacket(CsINVALID_ID, (m)->Header.command_uid, (m)->Buff, (m)->Header.command_len))
//#define SEND_BASE_MESSAGE(s, m)  CsCore::cyberspace::Abtp::SendPacket(s, CsINVALID_ID, (m)->Header.command_uid, (m)->Buffer.Param, (m)->Header.command_len)

enum CsBASECOMMAND
{
    CsC_S_CONNECT_REQUEST = CsID_BASE + 1,
    CsS_C_CONNECT_FEEDBACK,

    CsS_C_PASSIVECONNECT_REQUEST,
    CsS_C_SWAPCONNECT_REQUEST,
    CsC_S_LISTSERVER_REQUEST,                 // 請求server的列表
    CsS_C_LISTSERVER_FEEDBACK,                // 把server資料傳給client clinet 收到要清除list    
    CsS_C_KICKOUT_REQUEST,                    //server kickout client
    CsC_S_KICKOUT_REQUEST,                    //client kickout server
    //CsC_S_FEEDBACK_AUTHERNTICCONNECT,       // client connect to server
    // Ping用, client:Req, server:S_FEEDBACK, client:measure time
    CsC_S_PING_REQUEST,
    CsS_C_PING_FEEDBACK,
    CsS_C_BAN_CLIENT,

    CsS_C_ASSIGNSERVER_REQUEST,               // 請clinet 連線到指定server
};

//using namespace CsCore;

namespace CsBaseCommand
{
    namespace ActiveConnect
    {
        namespace Client
        {
            inline void SetRequest( _OUT _csbinbuf& buf, _IN cs_session_uid_t key, _IN macn_t macaddr, _IN dword_t ver )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_len = _cssizeof(cs_session_uid_t)+_cssizeof(macn_t)+64+(rand()%64);
                //msg->Header.command_uid = CsC_S_CONNECT_REQUEST;
                buf.resize( _cssizeof( cs_session_uid_t ) + _cssizeof( macn_t ) + _cssizeof( dword_t ) + 64 + ( rand() % 64 ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                ( *( ( cs_session_uid_t* )param ) ) = key;
                memcpy( ( param + _cssizeof( cs_session_uid_t ) ), &macaddr, _cssizeof( macn_t ) );
                memcpy( ( param + _cssizeof( cs_session_uid_t ) + _cssizeof( macn_t ) ), &ver, _cssizeof( dword_t ) );
            }

            inline _cs_size_t GetFeedback( _IN ubyte_t* buff, _OUT short& result )
            {
                return CsFactory::SplitArgument( buff, "%h", &result );
            }
        };

        namespace Server
        {
            inline void GetRequest( _IN const ubyte_t* buff, _OUT cs_session_uid_t& key, _OUT macn_t& macaddr, _OUT dword_t& ver )
            {
                key = ( *( ( cs_session_uid_t* )buff ) );
                memcpy( &macaddr, ( buff + _cssizeof( cs_session_uid_t ) ), _cssizeof( macn_t ) );
                memcpy( &ver, ( buff + _cssizeof( cs_session_uid_t ) + _cssizeof( macn_t ) ), _cssizeof( dword_t ) );
                return;
            }

            inline void SetFeedback( _OUT _csbinbuf& buf, _IN short result )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_CONNECT_FEEDBACK;
                //msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%h", result);
                buf.resize( _cssizeof( short ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%h", result );
                return;
            }
        };
    };

    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace SwapConnect
    {
        namespace Server
        {
            inline void SetRequest( _OUT _csbinbuf& buf, _IN bool swap )
            {
                //msg->Header.who =CsINVALID_ID;
                //msg->Header.command_uid=CsS_C_SWAPCONNECT_REQUEST;
                //msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%c", swap);
                buf.resize( _cssizeof( bool ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%c", swap );
                return;
            }
        };

        namespace Client
        {
            inline void GetRequest( _IN const ubyte_t* buff, _OUT bool& swap )
            {
                CsFactory::SplitArgument( buff, "%c", &swap );
                return;
            }
        };
    };


    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace PassiveConnect
    {
        namespace Server
        {
            inline _cs_size_t SetRequest( _OUT _csbinbuf& buf, _IN char* host, _IN ushort_t port )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_PASSIVECONNECT_REQUEST;
                buf.resize( sizeofcsstr( host ) + _cssizeof( ushort_t ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%s%h", host, port );
            }
        };

        namespace Client
        {
            inline void GetRequest( _IN const ubyte_t* buff, _OUT char* host, _OUT ushort_t& port )
            {
                CsFactory::SplitArgument( buff, "%s%h", host, &port );
                return;
            }
        };

    };

    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace ServerKickout
    {
        namespace Server
        {
            inline void SetRequest( _OUT _csbinbuf& buf )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_KICKOUT_REQUEST;
                //msg->Header.command_len = 0;
                buf.resize( 0 );
            }
        };
    };


    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace ClientKickout
    {
        namespace Client
        {
            inline void SetRequest( _OUT _csbinbuf& buf )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsC_S_KICKOUT_REQUEST;
                //msg->Header.command_len = 0;
                buf.resize( 1 );
            }
        };
    };


    //=====================================================================================
    //
    // DESC: Block client can't send message
    //=====================================================================================
    namespace BanClient
    {
        namespace Server
        {
            inline _cs_size_t SetRequest( _OUT _csbinbuf& buf, _IN bool bBlock )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_BAN_CLIENT;
                //return (msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%c", bBlock));
                buf.resize( _cssizeof( bool ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%c", bBlock );
            }
        };

        namespace Client
        {
            inline void GetRequest( _IN const ubyte_t* buff, _OUT bool& bBlock )
            {
                CsFactory::SplitArgument( buff, "%c", &bBlock );
                return;
            }
        };
    }

    //////////////////////////////////////////////////////////////////////////////
    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace ServerInfo
    {
        namespace Client
        {
            inline void SetRequest( _csbinbuf& buf, cs_session_uid_t cid )
            {
                buf.resize( _cssizeof( cs_session_uid_t ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                ( *( ( cs_session_uid_t* )param ) ) = cid;
            }

            inline void GetRequest( _IN const ubyte_t* buff, _OUT std::vector<cyberspace::ServerInfo>& serverlist )
            {
                ubyte_t* param = ( ubyte_t* )buff;
                int len = ( *( ( short* )param ) );
                param += _cssizeof( short );

                for( int i = 0; i < len; i++ )
                {
                    cyberspace::ServerInfo* data = ( cyberspace::ServerInfo* )param;
                    serverlist.push_back( ( *data ) );
                    param += _cssizeof( cyberspace::ServerInfo );
                }
            }
        };

        namespace Server
        {
            inline _cs_size_t SetRequest( _OUT _csbinbuf& buf, _IN const std::vector<cyberspace::ServerInfo>* serverlist )
            {
                short len = serverlist->size();
                buf.resize( _cssizeof( short ) + ( _cssizeof( cyberspace::ServerInfo ) * len ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                ( *( ( short* )param ) ) = len;
                memcpy( ( param + _cssizeof( short ) ), &( ( *serverlist )[ 0 ] ), _cssizeof( cyberspace::ServerInfo ) * len );
                return buf.size();
            }
        };
    }

}



//==============================================
// For ping message
//==============================================
namespace CsPingMessage
{
    namespace Client
    {
        inline void SetRequest( _OUT _csbinbuf& buf, _IN mstimen_t t )
        {
            //buf->Header.command_uid = CsC_S_PING_REQUEST;
            //buf->Header.command_len = _cssizeof(int);
            buf.resize( _cssizeof( int ) );
            ubyte_t* param = _CsGetResBufPtr( buf );
            memcpy( param, &t, _cssizeof( int ) );
        }

        inline void GetFeedback( _IN const ubyte_t* buff, _OUT mstimen_t& t )
        {
            t = ( *( int* )buff );
        }
    }

    namespace Server
    {
        inline void SetFeedback( _OUT _csbinbuf& buf, _IN mstimen_t t )
        {
            //buf->Header.command_uid = CsC_S_PING_REQUEST;
            //buf->Header.command_len = _cssizeof(int);
            buf.resize( _cssizeof( int ) );
            ubyte_t* param = _CsGetResBufPtr( buf );
            memcpy( param, ( char* )&t, _cssizeof( int ) );
        }

        inline void GetRequest( _IN const ubyte_t* buff, _OUT mstimen_t& t )
        {
            t = ( *( int* )buff );
        }
    }
};