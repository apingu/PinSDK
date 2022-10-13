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
#include <set>
#include <list>
#include <assert.h>
#ifdef _WINDOWS
#include <direct.h>  //mkdir
#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-stdlib.h>
#include <net/net-server.h>
//
#include <memio.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-pri_def.h"
#include "cs-unit_server.h"
#include "cs-arbitration_client.h"
#include "cs-scs-system.h"
#include "cs-db_function.h"
#include "cs-utility.h"
#include "cs-paper_system.h"
#include "cs-base_command.h"
#include "cs-scs_command.h"


namespace cyberspace
{
    class ThreadLog;

    ///////////////////////////////////////////////////////////////////////////////////
    //
    class CoreServer: private net::TCPServer, public scs::SystemDelegate, public ArbitrationDelegate
    {
        T_Lock                                    coreserver_ut_lock_;

        cyberspace::ArbitrationClient*            v_aribt_agent_;
        scs::System*                              v_scssystem_;
        UnitServer*                               v_unitserver_;
        ThreadLog*                                v_thread_log_;
        cyberspace::SocketSupervise               socket_supervis_;

        static uint_t                             s_num_startup_server_;

        std::vector<pfkrl::Thread::Handle>        threads_handle_;
        CsLogMode                                 log_mode_;


    public:

        cyberspace::Paper::System                 paper_system_;

        // 將收到的訊息 同步傳送到每一台機器裡
        bool                                      is_base_sync_;

        // 建立 離開時不會去變更CommonDB 可使用在要銜接 別的帳號系統時
        cs_string                                 db_charset_;

        SafeServerState                           state_;
        //MessageDispatcher                         channel_functions_;
        std::map<channel_uid_t, unit_member_func>   channel_functions_;


    public:

        CoreServer( UnitServer* server );
        ~CoreServer();


        virtual int               StartUp( unsigned int allowplayers, unsigned short port );    //! start server
        virtual int               Shutdown( void );                                             //! close server
        int                       SCSStartUp( tchar_t* arbit_host, ushort_t arbit_port, ushort_t scs_port );
        int                       SCSShutdown();

        // 會紀錄的踢除
        virtual int               Kickout( CsSessionDescriptor sdp ) final;
        //inline int    SendPacket  (net::TCPSocket* skt, uniqueid_t eid,  unsigned short sessionid, const ubyte_t* bp, cs_size_t len){ return SkToSc(skt)->SendPacket( eid, sessionid, bp, len); }
        inline int                SendPacket( CsSessionDescriptor sdp, uniqueid_t eid, unsigned short sessionid, const ubyte_t* bp, cs_size_t len ) { return CdToSc( sdp )->SendPacket( eid, sessionid, bp, len ); }

        //void                      SetLogMode( CsLogMode m ) { m_fLogMode = m_fLogMode; }
        //CsLogMode                 GetLogMode( void ) { return m_fLogMode; }
        //void                      OutputMessage( CsLogMode m, const tchar_t* string );// { return v_unitserver_->OutputMessage( m_fLogMode, string ); }

    public:


        //void                      BindSession( CsSessionDescriptor sdp, cs_session_uid_t sessionid );

        //! Key為上層使用的資料, 設定連結傳回舊的key 值
        //static CsSessionNote*     SetSessionNote( CsSessionDescriptor cdp, CsSessionNote* key );
        //static void               DelSessionNote( CsSessionDescriptor cdp, CsSessionNote* key );
        //! Key為上層使用的資料, 取得連結
        //static CsSessionNote*     GetSessionNote( CsSessionDescriptor cdp, notetid_t notetid );

        // 在主緒中執行他
        int                       Update( void );

        // 加入一的定時執行的元件
        void                      GetAllLocalSessionDescriptor( std::vector<CsSessionDescriptor>& CsPSockConnlist );

        void                      GetLocalServerReport( cyberspace::ServerState& sinfo );

        void                      SetThreadHistory( bool t );                                                                        // 要不要紀錄執行緒處理訊息
        cs_size_t                 GetThreadHistory( std::vector<channel_uid_t>& chrd, std::vector<mstimen_t>& tmrd );                  // 目前有哪些執行緒在處理的 訊息編號 與 開始執行的時間

        static channel_uid_t      GetStageChannel( CsSessionDescriptor sdp ) { return CdToSc( sdp )->state_channel_uid_;; }               // 目前該 CsAbtpCnSocket 再處理的 訊息編號
        static mstimen_t          GetLastRecvMST( CsSessionDescriptor sdp ) { return CdToSc( sdp )->GetLastRecvMST(); }
        static bool               IsLive( CsSessionDescriptor sdp ) { return CdToSc( sdp )->IsLive(); }
        static bool               IsKickout( CsSessionDescriptor scn ) { return cyberspace::CoreServer::IsSessionInvalid( scn ); }

        void                      SetKeepAliveMstime( mstimen_t mst ) { socket_supervis_.SetKeepAliveMstime( mst ); }              // 設定連線上來到 未驗證前的時間 有多久 超過就踢除
        //void                      SetRiskAddress( int b ) { socket_supervis_.SetRiskAddress( b ); }
        void                      InsertAllowRemotePort( ushort_t b ) { socket_supervis_.InsertAllowRemotePort( b ); }

        cs_section_uid_t          GetLocalServerUid() { return v_aribt_agent_->GetLocalServerUid(); }

    public:

        //=====================================================================
        UINT                      GetPollingCount( void ) { return net::TCPServer::GetPollingCount(); }
        net::TCPSocket*           FindConnectedSocket( _IN socket_t fd ) { return net::TCPServer::FindConnectedSocket( fd ); }
        void                      GetAllConnectedSocket( _OUT std::vector<net::TCPSocket*>& CsPSockConnlist ) { return net::TCPServer::GetAllConnectedSocket( CsPSockConnlist ); }


        bool                      IsActive( void ) { return net::TCPServer::IsActive(); }
        int                       SendCsMessage( _IN CsSessionDescriptor sdp, _IN uniqueid_t uid, _IN channel_uid_t sessionid, _IN const ubyte_t* bp, _IN cs_size_t len ) { return v_unitserver_->SendCsMessage( sdp, uid, sessionid, bp, len ); }

        uint_t                    Get_Max_Connected_Client() { return state_.Get_Max_Connected_Client(); }                       // 特殊 取得 曾經最大的連線數量

        //
        //bool                      RegisterScsUserDefChannel( channel_uid_t id, cyberspace::scs::ServerAbtpChannelFunction func ) { return m_SCSMessageDispatcher.RegisterChannel( id, ( ChannelFunction )func ); }
        //bool                      UnRegisterScsUserDefChannel( channel_uid_t id ) { return m_SCSMessageDispatcher.UnRegisterChannel( id ); }



        //////////////////////////////////////////////////////////////////////////
        //
        CsSectionDescriptor       GetSectionDescriptor( _IN cs_section_uid_t sid ) { return v_scssystem_->GetSectionDescriptor( sid ); }
        cs_size_t                 GetSectionSize() { return v_scssystem_->GetSectionSize(); }
        cs_section_uid_t          GetSectionId( _IN const tchar_t* servername ) { return v_scssystem_->GetSectionId( servername ); }
        void                      GetAllSectionInfo( _OUT std::vector<ServerInfo>& section_list ) { return v_scssystem_->GetAllSectionInfo( section_list ); }

        int                       SendScsMessage( _IN cs_section_uid_t dest, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len ) { return v_scssystem_->SendMessage( dest, from, cid, bp, len ); }
        int                       BroadcastScsMessage( _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len ) { return v_scssystem_->BroadcastMessage( from, cid, bp, len ); }
        int                       BroadcastScsMessageToOther( _IN uniqueid_t from, _IN channel_uid_t cid, const _IN ubyte_t* bp, _IN cs_size_t len ) { return v_scssystem_->BroadcastMessageToOther( from, cid, bp, len ); }
        int                       BroadcastScsMessageToOtherAndVisible( _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len ) { return v_scssystem_->BroadcastMessageToOtherAndVisible( from, cid, bp, len ); }
        int                       BroadcastScsMessageToOtherWithObject( _IN const tchar_t* objectname, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len ) { return v_scssystem_->BroadcastMessageToOtherWithObject( objectname, from, cid, bp, len ); }

        static cs_string          GetSectionSectionName( _IN CsSectionDescriptor sdp ) { return cyberspace::scs::System::GetSectionSectionName( sdp ); }


        //
        CsSessionDescriptor       GetSessionDescriptor( _IN net::TCPSocket* skt ) { return SkToCd( skt ); }
        CsSessionDescriptor       GetSessionDescriptor( _IN cs_section_uid_t sid ) { return v_scssystem_->GetSessionDescriptor( sid ); }
        cs_size_t                 GetConnectSize( _IN CseCONNECT_TYPE t ) { return v_scssystem_->GetConnectSize( t ); }

        bool                      CreateSession( _IN cs_session_uid_t sessionid, _IN CsAbtpCnSocket* scn ) { return v_scssystem_->CreateSession( sessionid, scn ); }
        bool                      DelectSession( _IN CsSessionDescriptor sdp ) { return v_scssystem_->DeleteSession( cyberspace::scs::System::GetSessionSessionUid( sdp ) ); }

        int                       SendScsMessage( _IN CsSessionDescriptor dest, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len ) { return v_scssystem_->SendMessage( dest, from, cid, bp, len ); }


        //
        static CsAbtpCnSocket*    GetSessionSocket( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionSocket( sdp ); }
        static bool               IsSessionInvalid( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionIsInvalid( sdp ); }
        static cs_section_uid_t   GetSessionSectionId( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionSectionId( sdp ); }
        static cs_session_uid_t   GetSessionSessionUid( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionSessionUid( sdp ); }
        static ipn6_t             GetSessionIpn6( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionIpn6( sdp ); }
        static ushort_t           GetSessionPort( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionPort( sdp ); }
        static stimen_t           GetSessionCreateSec( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionCreateSec( sdp ); }
        static bool               GetSessionIsMaster( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionIsMaster( sdp ); }
        static bool               GetSessionIsClone( _IN CsSessionDescriptor sdp ) { return cyberspace::scs::System::GetSessionIsClone( sdp ); }


    private:

        inline int                RecvPacket( net::TCPSocket* skt, uniqueid_t& eid, unsigned short& sessionid, ubyte_t** bp, cs_size_t& len ) { return SkToSc( skt )->RecvPacket( eid, sessionid, bp, len ); }
        virtual int               Close( void ) { return v_unitserver_->Shutdown(); }
        //virtual int               Kickout( net::TCPSocket* skt );
        //int                       KickoutSocket( net::TCPSocket* skt );


        // TCPServer
        virtual net::TCPSocket*   OnNewSocket( void );
        virtual void              OnDelSocket( net::TCPSocket* s );
        virtual void              OnAccept( net::TCPSocket* skt );
        virtual void              OnKickout( net::TCPSocket* skt );
        virtual void              OnReceive( net::TCPSocket* skt );


        // Converger
        void                      OnClientConnect( CsSessionDescriptor sdp ) { v_unitserver_->OnClientConnect( sdp ); }
        void                      OnClientKickout( CsSessionDescriptor sdp ) { v_unitserver_->OnClientKickout( sdp ); }
        int                       OnRecvDefineRequest( CsSessionDescriptor sdp, uniqueid_t who, channel_uid_t cid, const ubyte_t* command, cs_size_t len ) { return v_unitserver_->OnRecvDefineRequest( sdp, who, cid, command, len ); }

        static void               DoExecute( void* argv );

        //----------------------------------------------------------------
        static void               DoTestTimeoutConnect( void* argv );                                                                   // 將連線 但逾時 沒有做驗證的clinet 踢除

    private:

        //=====================================================================
        // aribat
        int                       OnJoinSection( _IN CsSectionEntity* node ) { return v_scssystem_->InsertSection( node ); }            // 收到server 通知有一台section 加入
        int                       OnExitSection( _IN cs_section_uid_t sid ) { return v_scssystem_->RemoveSection( sid ); }               // 收到server 通知有一台section 離開
        void                      OnSyncSection( _OUT CsSectionEntity& node ) { v_scssystem_->UpdateSection( node ); }                  // 要求同步這台server的資料

        //=====================================================================
        // scs
        void                      OnSCSJoinSection( CsSectionDescriptor sectiondp ) { return v_unitserver_->OnJoinSection( sectiondp ); }
        void                      OnSCSExitSection( CsSectionDescriptor sectiondp ) { return v_unitserver_->OnExitSection( sectiondp ); }
        void                      OnSCSJoinSession( CsSessionDescriptor sessiondp ) { OnClientConnect( sessiondp ); };                  // 別台建立一個新連線
        void                      OnSCSExitSession( CsSessionDescriptor sessiondp ) { OnClientConnect( sessiondp ); };                  // 斷線
        void                      OnSCSPassRecvMessage( CsSessionDescriptor sdp, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len ) { OnRecvDefineRequest( sdp, uid, cid, bp, len ); }
        //void                      OnSCSRecvDefineRequest( uniqueid_t who, channel_uid_t cid, const ubyte_t* command, cs_size_t len );

    public:

        const tchar_t*            DoGetObjectName( void ) { return v_unitserver_->ObjectName(); }
        const tchar_t*            DoGetLocalHostname( _OUT tchar_t* buf, _IN cs_size_t len ) { return net::TCPServer::GetLocalHostname( buf, len ); }
        ushort_t                  DoGetLocalServicePort( void ) { return net::TCPServer::GetServicePort(); }
        const tchar_t*            DoGetLocalFunctionname( void ) { return v_unitserver_->GetLocalServiceName(); }
        const tchar_t*            DoGetLocalAliasname( void ) { return v_unitserver_->GetLocalAliasName(); }
        stimen_t                  DoGetCreateTime( void ) { return net::TCPServer::GetLastCreateTime(); }
        uint_t                    DoGetLocalLimitConnectSize( void ) { return net::TCPServer::GetLimitConnectedSize(); }
        uint_t                    DoGetLocalConnectedSize( void ) { return net::TCPServer::GetConnectedSize(); }
        Pflag                     DoGetState( void ) { return v_unitserver_->OnGetState(); }
    };
}