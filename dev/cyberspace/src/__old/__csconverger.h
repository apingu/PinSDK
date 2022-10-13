//
// 做為 arbit and scs的交接處
// server 與其中 session 的同步機制
//
//
//
//
#pragma once

#include "CsDef.h"

#include "CsArbitrationClient.h"
#include "CsSCSSystem.h"

namespace Cyberspace
{
    class Converger : public SCS::SystemDelegate, public ArbitrationDelegate
    {
        friend Cyberspace::ArbitrationClient;
        friend SCS::System;
        
        T_Lock                            m_utLock;
        Cyberspace::ArbitrationClient*    m_pAribtAgent;
        SCS::System*                      m_pSCSSystem;
        MessageDispatcher                 m_SCSMessageDispatcher;
        
    public:
    
        Converger();
        ~Converger();
        
        //void                            SCSCreate( serverid_t serverid );
        //serverid_t                      GetLocalServerId();
        
        //int                             Update();
        //void                            UpdateHost();
        
        // 取得某一個 SCS Server的Id
        serverid_t                      SCSGetServerId( const tchar_t* servername );
        // 取得某一個 SCS Server的資料
        bool                            SCSGetServerInfo( serverid_t sid, CsSectionEntity& info );
        // 取得所有 SCS Server的資料
        void                            SCSGetAllSectionEntity( std::vector<CsSectionEntity>& hostlist );
        // 取得有多少個交換伺服器(包含自己)
        int                             SCSGetServerSize();
        // SCS send message指定Server為目的地
        int                             SCSSendMessage( serverid_t dest, uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len );
        // 給所有的 包含自己
        int                             SCSBroadcSCSRegisterUserDefChannelastMessage( uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len );
        // 給所有的 自己除外
        int                             SCSBroadcastMessageToOther( uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len );
        // 送訊息給除了自己以外且非隱藏的server
        int                             SCSBroadcastMessageToOtherAndVisible( uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len );
        // 送訊息給除了自己以外且類別名稱相同的server
        int                             SCSBroadcastMessageToOtherWithObject( const tchar_t* objectname, uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len );
        
        // register the SCS define message channel
        bool                            SCSRegisterUserDefChannel( channelid_t id, Cyberspace::SCS::ServerMessageChannelFunction func );
        //unregister the SCS define message channel
        bool                            SCSUnRegisterUserDefChannel( channelid_t id );
        
        //////////////////////////////////////////////////////////////////////////
        CsPSockConn*                    SCSCreatePeerClient();
        void                            SCSDeletePeerClient( _IN CsPSockConn* skt );
        void                            FullCsPSockConn( _INOUT CsPSockConn* sdp/*, _IN sessionid_t sid, _IN macn_t mac*/ );
        
        // 透過SCS的連線告訴大家有個新的CLIENT加入 必須要是一個Master 如果原先已在別台機器加入過 舊的連線會被踢除
        //bool                           SCSCreateSession( CsSessionDescriptor sdp );

        // TODO: 使用session的方式
        //bool                            SCSCreateSession( _IN sessionid_t id, _IN mtimen_t litetime = 180 );
        //bool                            SCSDeleteSession( _IN sessionid_t id );

        bool                            SCSCreateSession( _IN CsPSockConn* scn );
        
        // 透過SCS的連線通知大家有個要關閉一個clinet 可以不是自己的
        bool                            SCSDelectSession( _IN CsSessionDescriptor sdp );
        
        // 同步SessionDescriptor 裡面的資料
        void                            SCSSyncClient( CsSessionDescriptor sdp );
        
        // 取得有多少個client connect(目前不太準)
        _cssize_t                       GetConnectSize( CseCONNECT_TYPE t );
        
        CsSessionDescriptor             SCSGetSessionDescriptor( sessionid_t id );
        static sessionid_t              SCSGetSessionSessionId( CsSessionDescriptor sdp );
        //static macn_t                   SCSGetSessionMac( CsSessionDescriptor sdp );
        static ipn6_t                   SCSGetSessionIpn6( CsSessionDescriptor sdp );
        
        static ushort_t                 SCSGetSessionPort( CsSessionDescriptor sdp );
        static serverid_t               SCSGetSessionServerId( CsSessionDescriptor sdp );
        
        static stimen_t                 SCSGetSessionCreateSec( CsSessionDescriptor sdp );
        
        static bool                     SCSIsMaster( CsSessionDescriptor sdp );
        static bool                     SCSIsClone( CsSessionDescriptor sdp );
        static bool                     SCSIsInvalid( CsSessionDescriptor sdp );
        
        // 只有連在這台上的才有
        static CsPSockConn*  GetSessionPeerClient( CsSessionDescriptor sdp );
        static CsSessionNote*           SetSessionNote( CsSessionDescriptor sdp, CsSessionNote* key );
        static void                     DelSessionNote( CsSessionDescriptor sdp, CsSessionNote* key );
        static CsSessionNote*           GetSessionNote( CsSessionDescriptor sdp, notetid_t notetid );
              
        

    protected:

        virtual Cyberspace::UnitServer* DoGetServer() = 0;
        
        //=====================================================================
        //  建立section 用





        
        //       
        //virtual long32_t                    GetLogMode( void ) = 0; //開關輸出訊息
        virtual bool                    DoIsActive( void ) = 0;
        virtual int                     DoSendCsMessage( CsSessionDescriptor sdp, uniqueid_t uid, channelid_t cid, const ubyte_t* bp, _cssize_t len ) = 0;
        virtual int                     DoKickout( CsSessionDescriptor sdp ) = 0;
        virtual int                     OnRecvDefineRequest( CsSessionDescriptor sdp, uniqueid_t who, channelid_t cid, const ubyte_t* command, _cssize_t len ) = 0;
        virtual void                    OnClientConnect( CsSessionDescriptor sdp ) = 0;
        virtual void                    OnClientKickout( CsSessionDescriptor sdp ) = 0;
        virtual void                    OnSectionSync( serverid_t sid ) = 0;
        virtual void                    OnSectionFree( serverid_t sid ) = 0;
        
        
        
        //////////////////////////////////////////////////////////////////////////
        // SystemDelegate  的處理介面
        //
    private:

        //serverid_t                      OnSCSGetLocalServerId() { return GetLocalServerId(); }
        virtual void                    OnSCSClientConnect( CsSessionDescriptor sdp ) { OnClientConnect( sdp ); }
        virtual void                    OnSCSClientKickout( CsSessionDescriptor sdp ) { OnClientKickout( sdp ); }
        virtual void                    OnSCSSectionSync( serverid_t sserverid ) { OnSectionSync( sserverid ); };
        virtual void                    OnSCSSectionFree( serverid_t sserverid ) { OnSectionFree( sserverid ); };
        void                            OnSCSPassSendMessage( CsSessionDescriptor sdp, uniqueid_t uid, channelid_t cid, const ubyte_t* bp, _cssize_t len ) { SendCsMessage( sdp, uid, cid, bp, len ); }
        void                            OnSCSPassRecvMessage( CsSessionDescriptor sdp, uniqueid_t uid, channelid_t cid, const ubyte_t* bp, _cssize_t len ) { OnRecvDefineRequest( sdp, uid, cid, bp, len ); }
        void                            OnSCSKickout( CsSessionDescriptor sdp ) { Kickout( sdp ); }
        
        void                            OnSCSRecvDefineRequest( CsSectionDescriptor sdp, uniqueid_t who, channelid_t cid, const ubyte_t* command, _cssize_t len ); // SystemDelegate
        
        
        //////////////////////////////////////////////////////////////////////////
        // ArbitrationDelegate
    private:
    
        void                            OnSyncSection( CsSectionEntity& hnode );
        int                             OnJoinSection( CsSectionEntity* node );
        int                             OnExitSection( serverid_t sid );
        
        // both
        //void                            OnSetLogMessage( CsLogMode m, const tchar_t* str );
        
    };
    
}
