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
//
#include <NetClient.h>
#include <PfThread.h>
//#include "CstSDataDef.h"
#include "MessageDispatcher.h"
#include "CsUnitClient.h"
#include "CsProtocol.h"
#include "CsBaseCommand.h"
#include <memio.inl>
//#include "CsCommBuf.h"
#include <safevalue.inl>
#include <PfTime.h>
#include <PfTimer.h>
#include <Debug.h>
#include <list>
#include "CsCoreClient.h"

namespace Cyberspace
{
    ///////////////////////////////////////////////////////////////////////////////////
    //
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    UnitClient::UnitClient()
    {
        m_pCoreClient = new CoreClient( this );
        m_SendMode = 0;
    }
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    UnitClient::~UnitClient()
    {
        /*
        {
        	TS_Lock lock(&m_pCoreClient->m_UtLock);
        	m_pCoreClient->Close();
        }
        */
        Shutdown();
        SAFE_DELETE( m_pCoreClient );
        for ( int i = 0; i < m_HostList.size(); i++ )
        {
            SAFE_DELETE( m_HostList[ i ] );
        }
        m_HostList.clear();
    }
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    void UnitClient::SetCloseUntilSendFinish( bool b )
    {
        m_pCoreClient->m_bCloseUntilSendFinish.SetValue( b );
    }
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    bool UnitClient::RegisterChannel( channelid_t id, ClientMessageChannelFunction func )
    {
        return m_pCoreClient->RegisterChannel( id, func );
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    bool UnitClient::UnRegisterChannel( channelid_t id )
    {
        return m_pCoreClient->UnRegisterChannel( id );
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    int UnitClient::SendCsMessage( uniqueid_t who, channelid_t cid, const ubyte_t* bp, size32_t len )
    {
        if ( m_pCoreClient->GetBanned() )
        {
            return -1;
        }

        if ( m_SendMode == 1 )
        {
            m_pCoreClient->PostSendPacket( who, cid, bp, len );
        }
        else
        {
            m_pCoreClient->SendPacket( who, cid, bp, len );
        }        
        return 1;
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    void UnitClient::Ping( mstimen_t t )
    {
        CsResBuf resbuf;
        CsPingMessage::Client::SetRequest( resbuf, PfTime::GetTickMSTime() );
        SendCsMessage( CsINVALID_ID, CsC_S_PING_REQUEST, _CsGetResBufPtr( resbuf ), _CsGetResBufLen( resbuf ) );
        return;
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    void UnitClient::SetSessionId( sessionid_t cid )
    {
        m_pCoreClient->SetSessionId(cid);

    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    void UnitClient::ClearSessionId()
    {
        m_pCoreClient->ClearSessionId();
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    //bool UnitClient::Connect( ipa_t hname, unsigned short hport, ipa_t lname, unsigned short lport )
	bool UnitClient::Connect( ipa_t hname, unsigned short hport )
    {
        //TS_Lock lock(&m_pCoreClient->m_UtLock);
        // because the shudown will clear the m_pCoreClient->cid, buf
        // when we use the reconnect it maybe send the -1 to server
        // sessionid_t cid = m_pCoreClient->m_ConnectSymbol.GetCid();
        //_cstSession ConnectNode;
        //ConnectNode = m_pCoreClient->m_ConnectSymbol;
        //memcpy(&ConnectNode, &m_pCoreClient->m_ConnectSymbol, sizeof(_cstSession));
        /*
        powerid_t  PowerId=m_pCoreClient->m_PowerId;
        groupid_t  GroupId=m_pCoreClient->m_GroupId;
        
        m_pCoreClient->Close();
        
        m_pCoreClient->m_PowerId=PowerId;
        m_pCoreClient->m_GroupId=GroupId;
        */
        
        //m_pCoreClient->m_ConnectSymbol = ConnectNode;
        //memcpy( &m_pCoreClient->m_ConnectSymbol, &ConnectNode, sizeof(_cstSession));
        //m_pCoreClient->m_ConnectSymbol.mac=m_pCoreClient->GetTcpSocket()->GetLocalMACAddress();
        
        //strcpy(m_pCoreClient->m_CsBaseHost, hname);
        //m_pCoreClient->m_CsBasePort = hport;
        
        //if((m_pCoreClient->m_ConnectSymbol.GetMid() = m_pCoreClient->GetLocalMACAddress()) == 0)
        //	return false;
        //m_pCoreClient->m_ConnectSymbol.GetMid() = m_pCoreClient->GetLocalMACAddress();
        
        /*
        m_pCoreClient->m_ConnectSymbol.Initial(m_pCoreClient->m_ConnectSymbol.GetCid(), m_pCoreClient->GetLocalMACAddress(),
        									   m_pCoreClient->m_ConnectSymbol.GetSid(), m_pCoreClient->m_ConnectSymbol.GetPower(),
        									   m_pCoreClient->m_ConnectSymbol.GetIpn(), m_pCoreClient->m_ConnectSymbol.GetCreateSec(),
        									   m_pCoreClient->m_ConnectSymbol.GetIndex(), 0, m_pCoreClient->m_ConnectSymbol.GetFlag(), m_pCoreClient->m_ConnectSymbol.GetSConnection());
        */
        for ( int i = 0; i < m_pCoreClient->retry_count; ++i )
        {
            //if ( m_pCoreClient->Connect( hname.c_str(), hport, lname.c_str(), lport ) )
			if ( m_pCoreClient->Connect( hname.c_str(), hport ) )
            {
                //
                //m_pCoreClient->BeginSendTask();
                return true;
            }
            else
            {
                Sleep( m_pCoreClient->retry_time );
            }
        }
        
        return false;
    }
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    int UnitClient::Shutdown( void )
    {
        CsResBuf buf;
        CsBaseCommand::ClientKickout::Client::SetRequest( buf );
        m_pCoreClient->SendPacket( CsINVALID_ID, CsC_S_KICKOUT_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
        //m_pCoreClient->UndoSession();
        return m_pCoreClient->Shutdown( 1 );
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    bool UnitClient::IsConnected( void )
    {
        return ( m_pCoreClient->GetFD() == 0 ) ? false : true;
    }
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    int UnitClient::Update( void )
    {
        // must to put there
        return m_pCoreClient->Update();
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    void UnitClient::OnRecvUserDefRequest( sessionid_t cid, uniqueid_t who, channelid_t chid, const ubyte_t* command, size32_t len )
    {
        _cstSession session;
        m_pCoreClient->ToSession( session );
        ClientEvocator evocator;
        evocator.sessiondescriptor = ( CsSessionDescriptor ) ( &( session ) );
        evocator.uniqueid = who;
        evocator.pclient = this;
        m_pCoreClient->DispatchMessage( &evocator, chid, command, len );
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    void UnitClient::OnRecvDefineRequest( sessionid_t cid, uniqueid_t who, channelid_t chid, const ubyte_t* command, size32_t len )
    {
        switch ( chid )
        {
        case CsS_C_KICKOUT_REQUEST:
            {
                m_pCoreClient->Shutdown();
            }
            break;
            
        case CsS_C_PASSIVECONNECT_REQUEST:
            {
                char host[ 128 ];
                ushort_t port;
                CsBaseCommand::PassiveConnect::Client::GetRequest( command, host, port );
                Connect( host, port );
            }
            break;
            
        case CsS_C_BAN_CLIENT:
            {
                bool bBanned;
                CsBaseCommand::BanClient::Client::GetRequest( command, bBanned );
                m_pCoreClient->SetBanned( bBanned );
                Pf_INFO( "client block %d\n", bBanned );
            }
            break;
            
        case CsS_C_PING_FEEDBACK:
            {
                mstimen_t fromtime = 0;
                CsPingMessage::Client::GetFeedback( command, fromtime );
                OnPing( fromtime );
            }
            break;
            
        case CsC_S_PING_REQUEST:	// 目前沒用(給Server測量用)
            {
            }
            break;
            
        // 分散式的指派要送到哪裡去
        case CsS_C_ASSIGNSERVER_REQUEST:
        case CsS_C_LISTSERVER_FEEDBACK:
            {
                // 				ipn_t   host;
                // 				ushort_t port;
                // 				unsigned int   maxconnect;
                // 				unsigned int   usedconnect;
                // 				char    hostbuff[_MAX_HOSTNAME];
                // 				char    serverbuff[_MAX_SERVERNAME];
                // 				char    functionbuff[_MAX_FUNCTIONNAME];
                // 				CsLobbyCommand::ServerInfo::Client::GetRequest((char*)command, host, port, hostbuff, serverbuff, functionbuff, maxconnect, usedconnect);
                //m_HostList.push_back(ServerRecord(inet_ntoa(host), port, hostbuff, serverbuff, functionbuff, maxconnect, usedconnect));
                //m_gc->OnGetHostInfo(inet_ntoa(host), port, hostbuff, serverbuff, functionbuff, maxconnect, usedconnect, 1);
                std::vector<ServerInfo> SDataList;
                CsBaseCommand::ServerInfo::Client::GetRequest( command, SDataList );
                
                //m_HostList.clear();
                //m_HostList.push_back(ServerRecord(inet_ntoa(host), port, hostbuff, serverbuff, functionbuff, maxconnect, usedconnect));
                for ( int i = 0; i < m_HostList.size(); i++ )
                {
                    SAFE_DELETE( m_HostList[ i ] );
                }
                m_HostList.clear();

                for ( uint_t i = 0; i < SDataList.size(); i++ )
                {
                    ServerInfo* hostt = new ServerInfo( SDataList[i] );
                    m_HostList.push_back( hostt );
                }
                OnListServer();
            }
            
            return;
        }
        
        return;
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    void UnitClient::ListServer()
    {
        CsResBuf buf;
        CsBaseCommand::ServerInfo::Client::SetRequest( buf, GetSessionId() );
        SendCsMessage( CsINVALID_ID, CsC_S_LISTSERVER_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    int UnitClient::GetServerCount()
    {
        return m_HostList.size();
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    const ServerInfo* UnitClient::GetServerInfo( int i )
    {
        if( i <  m_HostList.size() )
            return m_HostList[i];
        return NULL;
    }    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    /*
    int UnitClient::RefreshMessage(void)
    {
    	return 0;
    
    	TS_Lock lock(&m_pCoreClient->m_UtsLock);
    
    	std::list< commbuf_t* >::iterator iter = m_pCoreClient->m_SendList.begin();
    	while (iter != m_pCoreClient->m_SendList.end())
    	{
    		commbuf_t* data = (commbuf_t*)(*iter);
    		SAFE_DELETE(data);
    		++iter;
    	}
    
    	m_pCoreClient->m_SendList.clear();
    	return 1;
    }
    */
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    sessionid_t UnitClient::GetSessionId( void )
    {
        return m_pCoreClient->GetSessionId();
    }
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    macn_t UnitClient::GetMACAddress( void )
    {
        return m_pCoreClient->GetMACAddress();
    }
    
    
    ////===============================================================================//
    ////
    ////
    ////===============================================================================//
    //powerid_t UnitClient::GetPowerId( void )
    //{
    //    return m_pCoreClient->GetPowerId();
    //}
    
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    //CsSessionDescriptor UnitClient::GetSessionDescriptor( void )
    //{
    //    _cstSession session;
    //    m_pCoreClient->ToSession( session );
    //    return ( CsSessionDescriptor ) ( &session );
    //}
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    int UnitClient::DoAssignMessage( const Evocator* evocator, channelid_t chid, const ubyte_t* command, unsigned int len )
    {
        return m_pCoreClient->DispatchMessage( evocator, chid, command, len );
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    /*
    void  UnitClient::FreeConnect()
    {
    	m_pCoreClient->Clear();
    }
    */
    
    
    // 	//===============================================================================//
    // 	//
    // 	//
    // 	//===============================================================================//
    // 	void UnitClient::SetConnectionId(sessionid_t cid)
    // 	{
    // 		m_pCoreClient->m_ConnectSymbol.GetCid() = cid;
    // 	}
    // 	//===============================================================================//
    // 	//
    // 	//
    // 	//===============================================================================//
    // 	void UnitClient::SetPowerId(powerid_t pid)
    // 	{
    // 		m_pCoreClient->m_ConnectSymbol.power = pid;
    // 	}
    
    //*/
};

