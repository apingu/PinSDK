//
//
//
//
//   作為server端最基本建立的socket 傳送 經過 加密 壓縮 加入header的資料
//    authentic binary socket CsAbtpCnSocket
//    connection socket =>TCP
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
#include <pf-def.h>
#include <net/net-socket.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-if_socket.h"
#include "cs-if_session.h"

namespace cyberspace
{
    class PacketFactory;
    
    class AbtpCnSocket : public net::TCPSocket, public CsIfSocket, public CsIfSession
    {
        typedef net::TCPSocket  BaseClass;
        
        //macn_t               m_MacAddress;
        PacketFactory*             v_packetfactory_;  // 用指標的目的只是不想要大家都需要include他
        //CsSessionDescriptor  session_descriptor_;

        CsSessionDescriptor        session_descriptor_;
                                  
    public:                       
                                  
        int                        num_check_lost_channel_; // 安全檢查 紀錄找不到相對應channel的訊息次數
        int                        num_check_jam_message_;  // 安全檢查 安全次數，為避免封包攻擊
                                  
        channel_uid_t              state_channel_uid_;
                                  
        static uint_t              max_num_jam_message_;   // 超過 次數 每次送上來的時間都很密集時
        static mstimen_t           max_recv_headway_;      // 距離上次收到訊息的時間，最少需要間隔秒數

        AbtpCnSocket();
        //AbtpCnSocket( CsSessionDescriptor sdp );
        ~AbtpCnSocket();
    
        virtual const _cs_char_t*  ObjectName() const { return ( "CsAbtpCnSocket" ); }
        virtual socket_t           Create( _IN int domain = AF_INET );
        virtual int                Close( void );
                                   
        void                       SetSessionUID( cs_session_uid_t sid );
        cs_session_uid_t           GetSessionUID();
        void                       SetDescriptor( CsSessionDescriptor sdp ) { session_descriptor_ = sdp; }
        CsSessionDescriptor        GetDescriptor() const                    { return session_descriptor_; };
        // 取得上一次收到訊息的時間    
        mstimen_t                  GetLastRecvMST( void );
                                   
        // 檢查封包收到的時間是否太短 次數是否太多
        // 超過 30次收到沒有對應通道的 訊息
        bool                       CheckRecvLegal();

        int                        SendPacket( _IN uniqueid_t eid, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len );
        // don't delete bp, class will delete by it self
        int                        RecvPacket( _OUT uniqueid_t& eid, _OUT channel_uid_t& cid, _OUT ubyte_t** bp, _OUT _cs_size_t& len );
        
    private:

        //////////////////////////////////////////////////////////////////////////
        // socketinterface
        //macn_t               GetMac();
        int                        Send( _IN const ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag = 0 );
        int                        Recv( _OUT ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag = 0 );   
    };
}

typedef cyberspace::AbtpCnSocket  CsAbtpCnSocket;
