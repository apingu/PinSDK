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
//   使用RUTP的協動
//   Peer Socket
//
//
//////////////////////////////////////////////////////////////////////////

#pragma once

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
#include <net/net-peer.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-if_socket.h"
#include "cs-if_session.h"

namespace cyberspace
{
    class PacketFactory;
    
    class AbtpPrSocket : public net::RUDPPeer, public CsIfSocket, public CsIfSession
    {
        typedef net::RUDPPeer  BaseClass;

        PacketFactory*       v_packetfactory_;  // 用指標的目的只是不想要大家都需要include他

        const sockaddr*      send_addr_buffer_;
        int                  send_addr_length_;
        sockaddr*            recv_addr_buffer_;
        int                  recv_addr_length_;
       
    public:

        AbtpPrSocket();
        ~AbtpPrSocket();

        virtual const _cs_char_t* ObjectName() const { return ( "CsAbtpPrSocket" ); }

        virtual socket_t       Create( int domain = AF_INET );
        virtual int            Close( void );
        virtual bool           Connect( _IN ipa_t hname, _IN unsigned short hport );        
        
        void                   SetSessionUID( _IN cs_session_uid_t cid );
        cs_session_uid_t       GetSessionUID();
        mstimen_t              GetLastRecvMST( void );

        int                    SendPacket( _IN const sockaddr* srcaddr, _IN uniqueid_t eid, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len );
        // don't delete bp, class will delete by it self
        int                    RecvPacket( _OUT sockaddr* srcaddr, _OUT uniqueid_t& eid, _OUT channel_uid_t& cid, _OUT ubyte_t** bp, _OUT _cs_size_t& len );
        

    private:

        //////////////////////////////////////////////////////////////////////////
        // socketinterface
        int                    Send( _IN const ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag = 0 );
        int                    Recv( _OUT ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag = 0 );
        //////////////////////////////////////////////////////////////////////////
        //    
        //virtual void           OnReceive( _IN net::RUDPPeer* s );
        
    };
}

typedef cyberspace::AbtpPrSocket  CsAbtpPrSocket;



