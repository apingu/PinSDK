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

#pragma once

#pragma warning(disable:4786)

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//
#include <memio.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-err.h"
#include "cs-protocol.h"
#include "cs-entity_def.h"


//#include "CsCommandFactory.h"


#define CsID_SCS  (_CsMAXCHANNEL+15000)
//#define SEND_LOBBY_MESSAGE(s, m)  (s->SendPacket(CsINVALID_ID, (m)->Header.command_uid, (m)->Buff, (m)->Header.command_len)
//
enum CsSCSCOMMAND
{
    CsS_C_SCS_SEND_MESSAGE = CsID_SCS + 1, // 發送訊息 到連接在別台機器上的連線
    CsS_C_SCS_RECV_MESSAGE,
    CsC_S_SYNC_HOST_REQUEST,

    CsC_S_INSERT_CLONE_REQUEST,
    CsC_S_REMOVE_CLONE_REQUEST,

    CsC_C_CREATE_SHAREBUFFER_REQUEST,      // 要求建立 sharebuffer
    CsC_C_DELETE_SHAREBUFFER_REQUEST,      // 要求刪除 sharebuffer

    CsC_C_READ_SHAREBUFFER_REQUEST,        // 讀取
    CsC_C_SAVE_SHAREBUFFER_REQUEST,        // 儲存

    //CsC_S_SYNC_MASTER_REQUEST,
};

namespace CsSCSCommand
{
    //=====================================================================================
    //
    // DESC: Block client can't send message
    //=====================================================================================
    namespace SCSSendRecvMessage
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, cs_session_uid_t& cid, channel_uid_t& chid, ubyte_t* bp, _cs_size_t& len )
            {
                ubyte_t* param = buff;
                cid = ( *( cs_session_uid_t* )param );
                param += _cssizeof( cs_session_uid_t );
                chid = ( *( channel_uid_t* )param );
                param += _cssizeof( channel_uid_t );
                len = ( *( _cs_size_t* )param );
                param += _cssizeof( _cs_size_t );
                bp = param;
                return;
            }
        };

        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, cs_session_uid_t cid, channel_uid_t chid, const ubyte_t* bp, _cs_size_t len )
            {
                if( ( bp == NULL ) || ( len == 0 ) )
                {
                    return 0;
                }

                buf.resize( _cssizeof( cs_session_uid_t ) + _cssizeof( channel_uid_t ) + _cssizeof( int ) + len );
                ubyte_t* param = _CsGetResBufPtr( buf );
                ( *( cs_session_uid_t* )param ) = cid;
                param += _cssizeof( cs_session_uid_t );
                ( *( channel_uid_t* )param ) = chid;
                param += _cssizeof( channel_uid_t );
                ( *( _cs_size_t* )param ) = len;
                param += _cssizeof( int );
                IOMEMCPY( param, bp, len );
                return buf.size();
            }
        };
    }



    //=====================================================================================
    // name:
    // DESC: 同步連線的資訊
    //=====================================================================================
    namespace SyncHost
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, cs_section_uid_t& sid )
            {
                //int nlen = 0;
                CsFactory::SplitArgument( buff, "%h", &sid );
                return;
            }
        };

        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, cs_section_uid_t sid )
            {
                buf.resize( _cssizeof( cs_section_uid_t ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%h", sid );
            }

        };
    };
    //=====================================================================================
    // name:
    // DESC: 加入/移除 一個MASTER的連線
    //=====================================================================================
    namespace InsertRemoveMaster
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, CsSessionEntity*& nodevec, _cs_size_t& len )
            {
                int nlen = 0;
                CsFactory::SplitArgument( buff, "%d%p", &len, &( nodevec ), &nlen );
                return;
            }

        };

        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, const CsSessionEntity* nodevec, _cs_size_t len )
            {
                int nlen = len * _cssizeof( CsSessionEntity );
                buf.resize( _cssizeof( int ) + sizeofcsbuf( nlen ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%d%v", len, nodevec, nlen );
            }

        };
    };


    //=====================================================================================
    // name:
    // DESC: 建立 一個sharebuffer
    //=====================================================================================
    namespace CreateShareBuffer
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, uniqueid_t& uid, int8_t& gid )
            {
                CsFactory::SplitArgument( buff, "%q%h", &uid, &gid );
                return;
            }
        };


        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buff, uniqueid_t uid, int8_t gid )
            {
                int nlen = _cssizeof( uniqueid_t ) + _cssizeof( short );
                buff.resize( nlen );
                ubyte_t* param = _CsGetResBufPtr( buff );
                return CsFactory::MakeArgument( param, "%q%h", uid, gid );
            }
        };
    }

    namespace DeleteShareBuffer
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, uniqueid_t& uid, int8_t& gid )
            {
                CsFactory::SplitArgument( buff, "%q%h", &uid, &gid );
                return;
            }
        };


        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buff, uniqueid_t uid, int8_t gid )
            {
                int nlen = _cssizeof( uniqueid_t ) + _cssizeof( short );
                buff.resize( nlen );
                ubyte_t* param = _CsGetResBufPtr( buff );
                return CsFactory::MakeArgument( param, "%q%h", uid, gid );
            }
        };
    }


    //=====================================================================================
    // name:
    // DESC: 讀取 一個sharebuffer
    //=====================================================================================
    namespace ReadShareBuffer
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, uniqueid_t& uid, int8_t& gid )
            {
                CsFactory::SplitArgument( buff, "%q%h", &uid, &gid );
                return;
            }
        };


        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buff, uniqueid_t uid, int8_t gid )
            {
                int nlen = _cssizeof( uniqueid_t ) + _cssizeof( short );
                buff.resize( nlen );
                ubyte_t* param = _CsGetResBufPtr( buff );
                return CsFactory::MakeArgument( param, "%q%h", uid, gid );
            }
        };
    }


    //=====================================================================================
    // name:
    // DESC: 儲存 一個sharebuffer
    //=====================================================================================
    /*
    namespace SaveShareBuffer
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, uniqueid_t& uid, int8_t& gid, ubyte_t* buf, _cs_size_t& len )
            {
                CsFactory::SplitArgument( buff, "%q%h%v", &uid, &gid, buf, &len );
                return;
            }
        };


        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buff, uniqueid_t uid, int8_t gid, const ubyte_t* buf, _cs_size_t len )
            {
                int nlen = _cssizeof( uniqueid_t ) + _cssizeof( short ) + sizeofcsbuf( len );
                buff.resize( nlen );
                char* param = _CsGetResBufPtr( buff );
                return CsFactory::MakeArgument( param, "%q%h%v", uid, gid, buf, len );
            }
        };
    }
    */
};