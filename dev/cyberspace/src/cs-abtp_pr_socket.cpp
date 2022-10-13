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
#include <pf-stdlib.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-abtp_pr_socket.h"
#include "cs-packet_factory.h"

namespace cyberspace
{
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    AbtpPrSocket::AbtpPrSocket()
    {
        v_packetfactory_ = new cyberspace::PacketFactory;
        //session_descriptor_ = NULL;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    AbtpPrSocket::~AbtpPrSocket()
    {
        SAFE_DELETE( v_packetfactory_ );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t AbtpPrSocket::Create( int domain )
    {
        if( v_packetfactory_ == NULL )
        {
            return 0;
        }

        //std::vector<EthIF> ethif_list;
        //net::Socket::GetEthIFInfo( ethif_list );
        //m_MacAddress = ethif_list[ 0 ].macn;
        v_packetfactory_->Initate();
        return BaseClass::Create( domain );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool AbtpPrSocket::Connect( _IN ipa_t hname, _IN unsigned short hport )
    {
        _cs_char_t buff[ 16 ];
        addrs_t addrs;
        net::Socket::SetAddress( hname.c_str(), itoa( hport, buff, 10 ), ( sockaddr* )&addrs, 0, SOCK_DGRAM );
        return ( SendnTo( NULL, 0, ( sockaddr* )&addrs, sizeof( sockaddr ) ) >= 0 ) ? true : false;

    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpPrSocket::Close( void )
    {
        if( v_packetfactory_ == NULL )
        {
            return -1;
        }

        v_packetfactory_->Initate();
        return BaseClass::Close();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void AbtpPrSocket::SetSessionUID( cs_session_uid_t cid )
    {
        if ( v_packetfactory_ == NULL )
        {
            return;
        }
        v_packetfactory_->SetSessionUID( cid );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    cs_session_uid_t AbtpPrSocket::GetSessionUID()
    {
        if( v_packetfactory_ == NULL )
        {
            return CsINVALID_ID;
        }
        return v_packetfactory_->GetSessionUID();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpPrSocket::SendPacket( const sockaddr* srcaddr, uniqueid_t eid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
    {
        //TS_Lock lock( &m_utCsLock );
        //TS_Lock lock(&m_AKLock);
        // 還是有機會對斷線的人送資料
        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }
        if( srcaddr->sa_family == AF_INET )
        {
            send_addr_length_ = _cssizeof( sockaddr_in );
        }
        else
        {
            send_addr_length_ = _cssizeof( sockaddr_in6 );
        }
        send_addr_buffer_ = srcaddr;
        //addrs_t addrs;
        //net::Socket::SetAddress( hname.c_str(), itoa( hport, buff, 10 ), ( sockaddr* )&send_addr_buffer_, 0, SOCK_DGRAM );
        return v_packetfactory_->SendPacket( this, eid, cid, bp, len );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpPrSocket::RecvPacket( sockaddr* srcaddr, uniqueid_t& eid, channel_uid_t& cid, ubyte_t** bp, _cs_size_t& len )
    {
        //TS_Lock lock( &m_utCsLock );
        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }
        if( srcaddr->sa_family == AF_INET )
        {
            recv_addr_length_ = _cssizeof( sockaddr_in );
        }
        else
        {
            recv_addr_length_ = _cssizeof( sockaddr_in6 );
        }
        recv_addr_buffer_ = srcaddr;
        return v_packetfactory_->RecvPacket( this, eid, cid, bp, len );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    mstimen_t AbtpPrSocket::GetLastRecvMST( void )
    {
        //TS_Lock lock(&m_AKLock);
        //return mtime_last_recv_;
        return v_packetfactory_->GetLastRecvMST();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //macn_t AbtpPrSocket::GetMac()
    //{
    //    return m_MacAddress;
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpPrSocket::Send( _IN const ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag )
    {
        return BaseClass::SendnTo( bp, len, send_addr_buffer_, send_addr_length_, flag );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpPrSocket::Recv( _OUT ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag )
    {
        return BaseClass::RecvnFrom( bp, len, recv_addr_buffer_, recv_addr_length_, flag );
    }
}