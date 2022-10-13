//
// 做為 arbit and scs的交接處
//
//
//
//
//

#include "CsConverger.h"

namespace Cyberspace
{

#define WAIT_TRY_TIME 180000

    //=============================================================================
    //
    //
    //=============================================================================
    Converger::Converger()
    {
        m_pSCSSystem  = new Cyberspace::SCS::System( this );
        m_pAribtAgent = new ArbitrationClient( this );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    Converger::~Converger()
    {
        //if ( m_pSCSSystem != NULL )
        //{
        //    m_pSCSSystem->Shutdown();
        //}
        //
        //if ( m_pAribtAgent != NULL )
        //{
        //    m_pAribtAgent->Shutdown();
        //}

        //m_pAribtAgent = NULL;
        //m_pSCSSystem = NULL;
        
        SAFE_DELETE( m_pSCSSystem );
        SAFE_DELETE( m_pAribtAgent );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    //serverid_t Converger::GetLocalServerId()
    //{
    //    return m_pAribtAgent->GetLocalServerId();
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    //void Converger::SCSCreate( serverid_t serverid )
    //{
    //
    //}
    
    int Converger::SCSStartUp( tchar_t* arbit_host, unsigned short arbit_port, ushort_t scs_port )
    {
        TS_Lock lock( &m_utLock );
        
        if ( m_pSCSSystem == NULL )
        {
            return -1;
        }
        
        if ( m_pAribtAgent == NULL )
        {
            return -1;
        }
        
        if ( m_pSCSSystem->StartUp( scs_port ) == 1 )
        {
            //return m_pAribtAgent->StartUp( hname, port );
            //Cyberspace::Arbitration::Client ac( GetLocalServerId(), this );
            //ac.StartUp( hname, port );
            return m_pAribtAgent->StartUp( arbit_host, arbit_port );
        }
        
        return -1;
    }

    int Converger::SCSShutdown()
    {
        if ( m_pSCSSystem != NULL )
        {
            m_pSCSSystem->Shutdown();
        }

        if ( m_pAribtAgent == NULL )
        {
            m_pAribtAgent->Shutdown();
        }
        return 0;
    }
    
    
    //void Converger::SCSDestroy()
    //{
    //    if ( m_pSCSSystem != NULL )
    //    {
    //        m_pSCSSystem->Close();
    //    }
    //
    //    if ( m_pAribtAgent != NULL )
    //    {
    //        m_pAribtAgent->Shutdown();
    //    }
    //
    //    SAFE_DELETE( m_pSCSSystem );
    //    SAFE_DELETE( m_pAribtAgent );
    //    //m_StartUpSec = 0;
    //}
    

    //int Converger::Update()
    //{
    //    if ( m_pSCSSystem != NULL )
    //    {
    //        m_pSCSSystem->Update();
    //    }
    //
    //    if ( m_pAribtAgent != NULL )
    //    {
    //        return m_pAribtAgent->Update();
    //    }
    //    
    //    return 0;
    //}
    //
    //void Converger::UpdateHost()
    //{
    //    if ( m_pAribtAgent == NULL )
    //    {
    //        return;
    //    }
    //
    //    m_pAribtAgent->UpdateHost();
    //}
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    // 取得某一個 SCS Server的Id
    serverid_t Converger::SCSGetServerId( const tchar_t* servername )
    {
        if ( m_pSCSSystem == NULL )
        {
            return CsINVALID_ID;
        }
        
        return m_pSCSSystem->GetServerId( servername );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool Converger::SCSGetServerInfo( serverid_t sid, CsSectionEntity& info )
    {
        if ( m_pSCSSystem == NULL )
        {
            return false;
        }
        
        return m_pSCSSystem->GetServerInfo( sid, info );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void Converger::SCSGetAllSectionEntity( std::vector <CsSectionEntity>& hostlist )
    {
        if ( m_pSCSSystem == NULL )
        {
            return;
        }
        
        m_pSCSSystem->GetAllServerInfo( hostlist );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::SCSGetServerSize()
    {
        if ( m_pSCSSystem == NULL )
        {
            return 0;
        }
        
        return m_pSCSSystem->GetSectionSize();
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::SCSSendMessage( serverid_t dest, uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len )
    {
        if ( m_pSCSSystem == NULL )
        {
            return 0;
        }
        
        return m_pSCSSystem->SendMessage( dest, from, cid, bp, len );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::SCSBroadcastMessage( uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len )
    {
        if ( m_pSCSSystem == NULL )
        {
            return 0;
        }
        
        return m_pSCSSystem->BroadcastMessage( from, cid, bp, len );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::SCSBroadcastMessageToOther( uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len )
    {
        if ( m_pSCSSystem == NULL )
        {
            return 0;
        }
        
        return m_pSCSSystem->BroadcastMessageToOther( from, cid, bp, len );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::SCSBroadcastMessageToOtherAndVisible( uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len )
    {
        if ( m_pSCSSystem == NULL )
        {
            return 0;
        }
        
        return m_pSCSSystem->BroadcastMessageToOtherAndVisible( from, cid, bp, len );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::SCSBroadcastMessageToOtherWithObject( const tchar_t* objectname, uniqueid_t from, channelid_t cid, const ubyte_t* bp, _cssize_t len )
    {
        if ( m_pSCSSystem == NULL )
        {
            return 0;
        }
        
        return m_pSCSSystem->BroadcastMessageToOtherWithObject( objectname, from, cid, bp, len );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool Converger::SCSRegisterUserDefChannel( channelid_t id, Cyberspace::SCS::ServerMessageChannelFunction func )
    {
        return m_SCSMessageDispatcher.RegisterChannel( id, ( MessageChannelFunction )func );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool Converger::SCSUnRegisterUserDefChannel( channelid_t id )
    {
        return m_SCSMessageDispatcher.UnRegisterChannel( id );
    }
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    CsPSockConn* Converger::SCSCreatePeerClient()
    {
        if ( m_pSCSSystem == NULL )
        {
            return NULL;
        }
        
        return m_pSCSSystem->CreateCsPSockConn();
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void Converger::SCSDeletePeerClient( _IN CsPSockConn* skt )
    {
        if ( m_pSCSSystem == NULL )
        {
            return;
        }
        
        m_pSCSSystem->DeleteCsPSockConn( skt );
        return;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void  Converger::FullCsPSockConn( _INOUT CsPSockConn* sdp/*, _IN sessionid_t sid, _IN macn_t mac*/ )
    {
        Cyberspace::SCS::System::FullCsPSockConn( sdp/*, sid, mac*/ );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    //CsSessionDescriptor Converger::SCSCreateSessionDescriptor( const _CsSessionEntity* sn )
    //{
    //    if ( m_pSCSSystem == NULL )
    //    {
    //        return NULL;
    //    }
    //
    //    return m_pSCSSystem->CreateSessionDescriptor( sn );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    //void Converger::SCSDeleteSessionDescriptor( CsSessionDescriptor sdp )
    //{
    //    if ( m_pSCSSystem == NULL )
    //    {
    //        return;
    //    }
    //
    //    m_pSCSSystem->DeleteSessionDescriptor( sdp );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    //bool Converger::SCSCreateSession( CsSessionDescriptor sdp )
    //{
    //    if ( m_pSCSSystem == NULL )
    //    {
    //        return false;
    //    }
    //
    //    return m_pSCSSystem->CreateSession( sdp );
    //}
    
    bool Converger::SCSCreateSession( CsPSockConn* scn )
    {
        if ( m_pSCSSystem == NULL )
        {
            return false;
        }        
        return m_pSCSSystem->CreateSession( scn );
    }
    
    
    //=============================================================================
    // NAME :
    // DESC :
    //=============================================================================
    bool Converger::SCSDelectSession( CsSessionDescriptor sdp )
    {
        if ( m_pSCSSystem == NULL )
        {
            return false;
        }
        
        return m_pSCSSystem->DeleteSession( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    //void Converger::SCSSyncClient( CsSessionDescriptor sdp )
    //{
    //    if ( m_pSCSSystem == NULL )
    //    {
    //        return;
    //    }
    //    
    //    m_pSCSSystem->SyncSession( sdp );
    //}
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    _cssize_t Converger::GetConnectSize( CseCONNECT_TYPE t )
    {
        if ( m_pSCSSystem == NULL )
        {
            return 0;
        }
        
        return m_pSCSSystem->GetConnectSize( t );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    CsSessionDescriptor Converger::SCSGetSessionDescriptor( sessionid_t id )
    {
        if ( m_pSCSSystem == NULL )
        {
            return NULL;
        }
        
        return m_pSCSSystem->GetSessionDescriptor( id );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    sessionid_t Converger::SCSGetSessionSessionId( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::GetSessionSessionId( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    //macn_t Converger::SCSGetSessionMac( CsSessionDescriptor sdp )
    //{
    //    //return Cyberspace::SCS::System::GetSessionMac( sdp );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    ipn6_t Converger::SCSGetSessionIpn6( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::GetSessionIpn6( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    //ipa_t Converger::SCSGetSessionIpa( CsSessionDescriptor sdp )
    //{
    //    return Net::Socket::IPN4ToA4( Cyberspace::SCS::System::GetSessionIpn4( sdp ) );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    ushort_t Converger::SCSGetSessionPort( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::GetSessionPort( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    serverid_t Converger::SCSGetSessionServerId( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::GetSessionServerId( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    //groupid_t Converger::SCSGetSessionGroup( CsSessionDescriptor sdp )
    //{
    //    return Cyberspace::SCS::System::GetSessionGroup( sdp );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    //powerid_t Converger::SCSGetSessionPower( CsSessionDescriptor sdp )
    //{
    //    return Cyberspace::SCS::System::GetSessionPower( sdp );
    //}
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    //long32_t Converger::SCSGetSessionFlag( CsSessionDescriptor sdp )
    //{
    //    return Cyberspace::SCS::System::GetSessionFlag( sdp );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    //uniqueid_t Converger::SCSGetSessionIndex( CsSessionDescriptor sdp )
    //{
    //    return Cyberspace::SCS::System::GetSessionIndex( sdp );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    CsPSockConn* Converger::GetSessionPeerClient( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::GetSessionPeerClient( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    stimen_t Converger::SCSGetSessionCreateSec( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::GetSessionCreateSec( sdp );
    }
    
    ////=============================================================================
    ////
    ////
    ////=============================================================================
    //void Converger::SCSSetSessionFlag( CsSessionDescriptor sdp, long32_t f )
    //{
    //    Cyberspace::SCS::System::SetSessionFlag( sdp, f );
    //}
    //
    ////=============================================================================
    ////
    ////
    ////=============================================================================
    //void Converger::SCSSetSessionStateBit( CsSessionDescriptor sdp, ushort_t bit, bool n )
    //{
    //    Cyberspace::SCS::System::SetSessionStateBit( sdp, bit, n );
    //}
    //
    ////=============================================================================
    ////
    ////
    ////=============================================================================
    //bool Converger::SCSGetSessionStateBit( CsSessionDescriptor sdp, ushort_t bit )
    //{
    //    return Cyberspace::SCS::System::GetSessionStateBit( sdp, bit );
    //}
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool Converger::SCSIsMaster( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::IsMaster( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool Converger::SCSIsClone( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::IsClone( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool Converger::SCSIsInvalid( CsSessionDescriptor sdp )
    {
        return Cyberspace::SCS::System::IsInvalid( sdp );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    CsSessionNote* Converger::SetSessionNote( CsSessionDescriptor sdp, CsSessionNote* note )
    {
        return Cyberspace::SCS::System::SetSessionNote( sdp, note );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void Converger::DelSessionNote( CsSessionDescriptor sdp, CsSessionNote* note )
    {
        Cyberspace::SCS::System::DelSessionNote( sdp, note );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    CsSessionNote* Converger::GetSessionNote( CsSessionDescriptor sdp, notetid_t notetid )
    {
        return Cyberspace::SCS::System::GetSessionNote( sdp, notetid );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void  Converger::OnSCSRecvDefineRequest( CsSectionDescriptor sdp, uniqueid_t who, channelid_t cid, const ubyte_t* command, _cssize_t len )
    {
        SCS::ServerEvocator evocator;
        evocator.uniqueid = who;
        evocator.pserver = GetServer();
        evocator.serverid = m_pSCSSystem->GetServerId( sdp );
        m_SCSMessageDispatcher.DispatchMessage( &evocator, cid, command, len );
    }
    
    ////===============================================================================//
    ////
    ////
    ////===============================================================================//
    //void  Converger::OnSCSSectionJoin(serverid_t sserverid)
    //{
    //}
    //
    ////===============================================================================//
    ////
    ////
    ////===============================================================================//
    //void  Converger::OnSCSSectionExit(serverid_t sserverid)
    //{
    //}
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    void Converger::OnSyncSection( CsSectionEntity& hnode )
    {
        return m_pSCSSystem->UpdateSection( &node );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::OnJoinSection( CsSectionEntity* node )
    {
        return m_pSCSSystem->InsertSection( node );
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Converger::OnExitSection( serverid_t sid )
    {
        return m_pSCSSystem->RemoveSection( sid );
    }
    
    
}
