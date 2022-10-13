//
//
//
//
//   作為server端最基本建立的socket 傳送 經過 加密 壓縮 加入header的資料
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
#include <pf-timer.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-abtp_cn_socket.h"
#include "cs-packet_factory.h"


namespace cyberspace
{
    uint_t    AbtpCnSocket::max_num_jam_message_;
    mstimen_t AbtpCnSocket::max_recv_headway_;
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    AbtpCnSocket::AbtpCnSocket()
    {
        v_packetfactory_ = new cyberspace::PacketFactory;
        //session_descriptor_ = NULL;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //AbtpCnSocket( CsSessionDescriptor sdp )
    //{
    //    v_packetfactory_ = new cyberspace::PacketFactory;
    //    session_descriptor_ = sdp;
    //}
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    AbtpCnSocket::~AbtpCnSocket()
    {
        Close();
        SAFE_DELETE( v_packetfactory_ );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t AbtpCnSocket::Create( int domain )
    {
        if ( v_packetfactory_ == NULL )
        {
            return 0;
        }
       
        v_packetfactory_->Initate();
        return BaseClass::Create( domain );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpCnSocket::Close( void )
    {
        if ( v_packetfactory_ == NULL )
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
    void AbtpCnSocket::SetSessionUID( cs_session_uid_t cid )
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
    cs_session_uid_t AbtpCnSocket::GetSessionUID()
    {
        if ( v_packetfactory_ == NULL )
        {
            return CsINVALID_ID;
        }        
        return v_packetfactory_->GetSessionUID();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    mstimen_t AbtpCnSocket::GetLastRecvMST( void )
    {
        //TS_Lock lock(&m_AKLock);
        //return mtime_last_recv_;
        return v_packetfactory_->GetLastRecvMST();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //macn_t AbtpCnSocket::GetMac()
    //{
    //    return m_MacAddress;
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool AbtpCnSocket::CheckRecvLegal()
    {
        if( num_check_lost_channel_ > 30 )
        {
            return false;
        }
        return true;

        mstimen_t currtime = v_packetfactory_->GetLastRecvMST();
        // 避免封包連續式的攻擊
        // 如果連續很多次 送封包的訊息間隔時間都很短
        // 就把它當成不正當
        mstimen_t recvtime = 0;
        uint_t    counter = 0;
        //TS_Lock lock( &m_utCsLock );
        recvtime = GetLastRecvMST();
        counter = num_check_jam_message_;
        //scn->SetSafeCounter(counter);
        //scn->SetRecvTime(nowtime);
        mstimen_t mtime = currtime - recvtime;

        // 距離上次收到訊息的時間，低於 100ms
        if( mtime < max_recv_headway_ )
        {
            counter++;
        }
        else
        {
            counter -= 100;

            if( counter <= 0 )
            {
                counter = 0;
            }

            //else
            //   counter--;
        }

        num_check_jam_message_ = counter;

        //SetLastRecvMST(currtime);
        //num_check_jam_message_.SetValue(counter);
        if( counter > max_num_jam_message_ )
        {
            return false;
        }

        return true;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpCnSocket::SendPacket( uniqueid_t eid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
    {
        //TS_Lock lock(&m_AKLock);
        // 還是有機會對斷線的人送資料
        if ( GetFD() == NULL_SOCKET )
        {
            return -1;
        }        
        return v_packetfactory_->SendPacket( this, eid, cid, bp, len );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpCnSocket::RecvPacket( uniqueid_t& eid, channel_uid_t& cid, ubyte_t** bp, _cs_size_t& len )
    {
        if ( GetFD() == NULL_SOCKET )
        {
            return -1;
        }        
        return v_packetfactory_->RecvPacket( this, eid, cid, bp, len );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    /*
    void AbtpCnSocket::SetLastRecvMST(mstimen_t t)
    {
        TS_Lock lock(&m_AKLock);
        mtime_last_recv_=t;
    }
    */

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpCnSocket::Send( _IN const ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag )
    {
        return BaseClass::Sendn( bp, len, flag );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AbtpCnSocket::Recv( _OUT ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag )
    {
        return BaseClass::Recvn( bp, len, flag );
    }
}

