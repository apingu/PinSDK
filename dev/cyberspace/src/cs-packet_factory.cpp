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
#include <pf-debug.h>
#include <pf-time.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-packet_factory.h"
#include "cs-protocol.h"
#include "cs-abtp_cn_socket.h"


namespace cyberspace
{
    //=============================================================================
    //
    //
    //=============================================================================
    PacketFactory::PacketFactory()
    {
        recv_buffer_ = NULL;
        //recv_length_ = 512;
        //recv_buffer_ = new ubyte_t[ recv_length_ ];
        //recv_buffer_ = NULL;
        session_Id_ = CsINVALID_ID;
        //m_MacAddress=0;
        Initate();
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    PacketFactory::~PacketFactory()
    {
        SAFE_DELETE_ARRAY( recv_buffer_ );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    PacketFactory& PacketFactory::operator = ( PacketFactory& src )
    {
        session_Id_ = src.session_Id_;
        //m_MacAddress = src.m_MacAddress;
        mtime_last_recv_ = src.mtime_last_recv_;
        mtime_last_send_from_ = src.mtime_last_send_from_;
        return *this;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void PacketFactory::Initate()
    {
        char out_debug[ 1024 ];
        sprintf( out_debug, "call %s", __FUNCTION__ );
        Pf_INFO( out_debug );

        TS_Lock lock( &packetfactory_ut_lock_, 1 );
        //session_Id_ = CsINVALID_ID;
        //m_MacAddress = 0;
        mtime_last_recv_ = 0;
        mtime_last_send_from_ = 0;
        memset( recv_cryptology_key_, 0, _CsMAX_CRYPTKEY_SIZE * _cssizeof( ubyte_t ) );
        memset( send_cryptology_key_, 0, _CsMAX_CRYPTKEY_SIZE * _cssizeof( ubyte_t ) );
        SAFE_DELETE_ARRAY( recv_buffer_ );
        recv_length_ = 512;
        recv_buffer_ = new ubyte_t[recv_length_];
        memset( recv_buffer_, 0, recv_length_ );

        Pf_INFO( "____finish\n" );
    }
   
    //=============================================================================
    //
    //
    //=============================================================================
    void PacketFactory::SetSessionUID( cs_session_uid_t cid )
    {
        //char out_debug[ 1024 ];
        //sprintf( out_debug, "call %s", __FUNCTION__ );
        //Pf_DEBUG( out_debug );

        TS_Lock lock( &packetfactory_ut_lock_, 2 );
        session_Id_ = cid;

        //Pf_DEBUG( "____finish\n" );

    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    cs_session_uid_t PacketFactory::GetSessionUID()
    {
        return session_Id_;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    mstimen_t PacketFactory::GetLastRecvMST( void )
    {
        //char out_debug[ 1024 ];
        //sprintf( out_debug, "call %s", __FUNCTION__ );
        //Pf_DEBUG( out_debug );

        TS_Lock lock( &packetfactory_ut_lock_, 3 );

        //Pf_DEBUG( "____finish\n" );

        return mtime_last_recv_;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    mstimen_t PacketFactory::GetLastSendFromMST( void )
    {
        TS_Lock lock( &packetfactory_ut_lock_, 4 );
        return mtime_last_send_from_;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int PacketFactory::SendPacket( CsIfSocket* skt, uniqueid_t eid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
    {
        //char out_debug[1024];
        //sprintf( out_debug, "call %s, send, uid %lld, channel %d, size %d", __FUNCTION__, eid, cid, len );
        //Pf_DEBUG( out_debug );

        TS_Lock lock( &packetfactory_ut_lock_, 5 );
        mstimen_t mt = pftml::Time::GetTickMSTime();
        int ret = cyberspace::Abtp::SendPacket( skt, session_Id_, eid, cid, bp, len, mt, send_cryptology_key_ );

        //Pf_DEBUG( "____finish\n" );

        return ret;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int PacketFactory::RecvPacket( CsIfSocket* skt, uniqueid_t& eid, channel_uid_t& cid, ubyte_t** bp, _cs_size_t& len )
    {
        //char out_debug[ 1024 ];
        //sprintf( out_debug, "call %s, recv, ", __FUNCTION__ );
        //Pf_DEBUG( out_debug );

        TS_Lock lock( &packetfactory_ut_lock_, 6 );
        mtime_last_recv_ = pftml::Time::GetTickMSTime();
        //cs_session_uid_t currcid=CsINVALID_ID;
        // 如果recvbuf 足夠 就會沿用 不夠 會先delete 然後 再建立一個足夠的新 recvbuf
        //int ret = cyberspace::Abtp::RecvPacket( skt, session_Id_, m_MacAddress, eid, cid, &recv_buffer_, recv_length_, len, mtime_last_send_from_, recv_cryptology_key_ );
        int ret = cyberspace::Abtp::RecvPacket( skt, session_Id_, eid, cid, &recv_buffer_, recv_length_, len, mtime_last_send_from_, recv_cryptology_key_ );
        // 上一次的ID 不是初始ID 而且這次與上次不同
        //if((currcid!=session_Id_)&&
        // (session_Id_!=CsINVALID_ID)||
        // (currcid!=CsINVALID_ID))
        //{
        // memset(recv_buffer_, 0, len);
        // return -1;
        //}
        //session_Id_=currcid;
        ( *bp ) = recv_buffer_;

        //sprintf( out_debug, "uid %lld, channel %d, size %d____finish\n", eid, cid, len );
        //Pf_DEBUG( out_debug );

        return ret;
    }
}

