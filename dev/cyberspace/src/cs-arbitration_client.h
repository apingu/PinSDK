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
//    Level 1 = Arbitration-server
//    Level 2 = scs-Host
//    Level 3 = Client
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#pragma warning(disable:4786)

//01.== C 系統文件/C++ 系統文件
#include <string>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <template/safevalue.inl>
#include <pf-thread.h>
#include <net/net-client.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-arbitration_delegate.h"


namespace cyberspace
{
    //class UnitServer;
    //class commbuf_t;
    //class PacketFactory;
    
    typedef UINT( _CALLBACK *OnHostNode )( unsigned int order, CsSectionEntity* pHost, void* combin );
    
    //////////////////////////////////////////////////////////////////////////
    //
    class ArbitrationClient: private net::TCPClient
    {
        T_Lock                                arbitrationclient_ut_lock_;
        cs_section_uid_t                       server_id_;             // set by arbit
        ArbitrationDelegate*                  delegate_;
        //PacketFactory*                        v_packetfactory_;
        CsArbitEntity                         arbit_entity_;        // arbit host
        _cs_string                            host_name_;              // arbit server address
        ushort_t                              host_port_;              // arbit port number
        //T_Lock                                m_lCommVec;
        //std::vector<commbuf_t*>               m_tCommVec;
        CsLogMode                             log_mode_;
        std::vector<pfkrl::Thread::Handle>    threads_handle_;
        
    public:
    
        ArbitrationClient( ArbitrationDelegate* s );
        ~ArbitrationClient();
        
    public:
    
        // ! set the host can't be see by other host
        void                SetVisible( bool b );
        cs_section_uid_t     GetLocalServerUID( void ){ return server_id_; }        
        
        /////////////////////////////
        // 同步Arbitration端的資料
        int                 StartUp( _cs_char_t* hname, unsigned short hport ); // 允許幾個scs server連線 與連線的port
        //
        int                 Shutdown( void );
       
    protected:
    
        bool                Ping( void );
        
    private:

        int                 Close();
        void                OnReceive();
   
        // 沒有加密
        int                 SendMessage( channel_uid_t cid, const ubyte_t* buff, _cs_size_t len );
        int                 RecvMessage( channel_uid_t& cid, ubyte_t** buff, _cs_size_t& len );
        
        safevalue<bool>     m_RetryConnectToArbit;
        static void         TryToConnect( void* data );
        
        // scs 的處理介面
        //cs_section_uid_t          OnGetLocalServerUID();
        //void                OnPassSendMessage( CsSessionDescriptor sdp, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len );
        //void                OnPassRecvMessage( CsSessionDescriptor sdp, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len );
        //void                OnKickout( CsSessionDescriptor sdp );
        //void                OnClientConnect( CsSessionDescriptor sdp );
        ////void                OnClientKickout( CsSessionDescriptor sdp );
        //void                OnOutputMessage( const _cs_char_t* str );
        //void                OnRecvDefineRequest( CsSectionDescriptor skt, uniqueid_t who, channel_uid_t cid, ubyte_t* command, _cs_size_t len );
        //void                OnSectionJoin( cs_section_uid_t sserverid );
        //void                OnSectionExit( cs_section_uid_t sserverid );
    };
};