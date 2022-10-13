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

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <net/net-socket.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-if_socket.h"

namespace cyberspace
{
        
    // 每個SOCKET 不管 CLIENT 或是 SERVER PEER 都應該有一個自己的
    // 每次都要重新建立一個KEY 當成是下一個封包加密的Key
    class PacketFactory
    {
    
        // 加密用的
        T_Lock          packetfactory_ut_lock_;
        
        cs_session_uid_t session_Id_;
        //macn_t      m_MacAddress;
        
        char             send_cryptology_key_[ _CsMAX_CRYPTKEY_SIZE ];
        char             recv_cryptology_key_[ _CsMAX_CRYPTKEY_SIZE ];
        mstimen_t        mtime_last_recv_;     // 上次接收到訊息封包的時間，為避免封包攻擊
        mstimen_t        mtime_last_send_from_; // 最近一次接收到的封包 他從發送端送出的時間
        
        ubyte_t*         recv_buffer_;
        _cs_size_t        recv_length_;
        
    public:
    
        PacketFactory();
        ~PacketFactory();
        
        virtual PacketFactory& operator = ( _IN PacketFactory& src );

        void             Initate();
        void             Finalize();

        void             SetSessionUID( cs_session_uid_t cid );
        cs_session_uid_t GetSessionUID();
        
        mstimen_t        GetLastRecvMST( void );
        mstimen_t        GetLastSendFromMST( void );
        
        
        // 加上 加解密的KEY
        int              SendPacket( CsIfSocket* skt, uniqueid_t eid,  channel_uid_t cid, const ubyte_t* bp, _cs_size_t len );
        // bp: 原先buffer的point   bl :原buffer的length 假如長度夠 就不會重新new 一個
        int              RecvPacket( CsIfSocket* skt, uniqueid_t& eid, channel_uid_t& cid, ubyte_t** bp, _cs_size_t& len );
        
    };
};