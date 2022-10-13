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
#include "CsValidateClient.h"
#include "CsLobbyCommand.h"
#include "CsGameCommand.h"

namespace Cyberspace
{



    //===============================================================================//
    //
    //
    //===============================================================================//
	ValidateClient::ValidateClient( GameClient*  client )
	{
		m_Loginstate=CSERR_NOT_CONNECTD;
		m_GameClient = client;
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	ValidateClient::~ValidateClient()
	{
		m_Loginstate=CSERR_NOT_CONNECTD;
		ShutDown();
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	bool ValidateClient::Connect( char* host, unsigned short port )
	{
		m_Loginstate=CSERR_NOT_CONNECTD;
		m_HostList.clear();
		return Client::Connect( host, port );
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::Login( const char* username, const char* password )
	{
		m_HostList.clear();

		channelid_t id=0;
		unsigned int len=0;
		m_Loginstate=CSERR_NOT_CONNECTD;
		CsCommandPacket hsysmsg;
		CsLoginMessage::Client::SetRequestMessage(&hsysmsg,username,password);
		SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buff, hsysmsg.Header.CommandLen );
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::Logout( void )
	{
		m_HostList.clear();
		m_Loginstate=CSERR_NOT_CONNECTD;

		CsCommandPacket hsysmsg;
		hsysmsg.Header.CommandId=CsC_S_LOGOUT_REQUEST;
		hsysmsg.Header.CommandLen=1;
		//SEND_GAME_MESSAGE( this, (&hsysmsg) );
		SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buff, hsysmsg.Header.CommandLen );
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	CSERR_CODE ValidateClient::GetLoginState( void )
	{
		return m_Loginstate;
	}
	
    //===============================================================================//
    //
    //
    //===============================================================================//
	bool ValidateClient::GetAllEntity( void )
	{
		TS_Lock lock( &m_UtVaLock );
		CsCommandPacket hsysmsg;
		hsysmsg.Header.CommandId = CsC_S_GETALLENTITYRECORD_REQUEST;
		hsysmsg.Header.CommandLen = 1;
		int ret = SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buff, hsysmsg.Header.CommandLen );
		sleep( 200 );
		return ret;
	}
	
    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::CreateEntity( const char* name, void* prop, size_t len )
	{
		TS_Lock lock( &m_UtVaLock );
		CsCommandPacket hsysmsg;
		CsCreateEntityMessage::Client::SetRequestMessage( &hsysmsg, name, prop, len );
		//SEND_GAME_MESSAGE( this, (&hsysmsg) );
		SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buff, hsysmsg.Header.CommandLen );
		sleep( 200 );
		return;
	}

    //===============================================================================//
    //
    // 告訴SERVER 你要選擇哪一個人
    //===============================================================================//
	void ValidateClient::SelectEntity( const char* name )
	{
		TS_Lock lock( &m_UtVaLock );
		CsCommandPacket hsysmsg;
		CsSelectEntityMessage::Client::SetRequestMessage(&hsysmsg,name);

		//SEND_GAME_MESSAGE( this, (&hsysmsg) );
		SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buff, hsysmsg.Header.CommandLen );
		sleep( 500 );
		return;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::DeleteEntity( const char* name )
	{
		TS_Lock lock( &m_UtVaLock );
		CsCommandPacket hsysmsg;
		CsDeleteEntityMessage::Client::SetRequestMessage( &hsysmsg, name );
		//SEND_GAME_MESSAGE( this, (&hsysmsg) );
		SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buff, hsysmsg.Header.CommandLen );
		sleep( 200 );
		return;
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::OnRecvDefineRequest( Net::TCPSocket* socket, entityid_t eid, channelid_t cid, const char* command, size_t len )
	{
		TS_Lock lock( &m_UtVaLock );

		switch( cid )
		{
		case CsS_C_LOGIN_FEEDBACK:
			{
				CsLoginMessage::Client::GetFeedback( (char*)command, m_Loginstate );
				if( ( m_Loginstate == CSERR_ACC_SUCCESS ) && ( m_Id == -1 ) )
				{
					m_Loginstate = CSERR_ACC_INVALID;
				}
				if( m_Loginstate != CSERR_ACC_SUCCESS )
				{
					m_Id = -1;
				}
			}
		    return;
		case CsC_S_SERVERINFI_POST:
			{
				ipn_t   host;
				u_short port;
				u_int   maxconnect;
				u_int   usedconnect;
				char    hostbuff[512];
				char    servicebuff[512];
				CsLobbyCommand::ServerInfo::Client::GetRequest( (char*)command, host, port, hostbuff, servicebuff, maxconnect, usedconnect );
				OnGetHostInfo( inet_ntoa( host ), port, hostbuff, servicebuff, maxconnect, usedconnect );
				m_GameClient->OnGetHostInfo( inet_ntoa( host ), port, hostbuff, servicebuff, maxconnect, usedconnect );
			}
			return;
		case CsS_C_SELECTENTITY_FEEDBACK:
			{
			    m_GameClient->OnGetSelectEntityId( CsSelectEntityMessage::Client::GetEntityId( (char*)command ) );
			}
		    return;
		case CsS_C_CREATENEWENTITY_FEEDBACK:
			{
				size_t l=0;
				char* name=(char*)CsCreateEntityMessage::Client::GetEntityName( (char*)command );
				void* property = CsCreateEntityMessage::Client::GetProperty( (char*)command, l );
				//if(*name=='\0')	name=NULL;
				m_GameClient->OnGetCreateEntity( name, property, l );
			}
			return;
		////////////////////////////////////////////////
		////create entity or select all entity
		case CsS_C_GETENTITYRECORD_FEEDBACK:
			{
				size_t l=0;
				char* name=(char*)CsGetRecordEntityMessage::Client::GetEntityName( (char*)command );
				void* property = CsGetRecordEntityMessage::Client::GetProperty( (char*)command, l );
				//if(*name=='\0')	name=NULL;
				m_GameClient->OnGetRecordEntity( name, property, l );
			}
			return;
		case CsS_C_DELETEENTITY_FEEDBACK:
			{
				m_GameClient->OnEntityDeleted( CsDeleteEntityMessage::Client::GetEntityName( (char*)command ) ,
					                           CsDeleteEntityMessage::Client::GetFeedbackRet( (char*)command ) );
			}
			return;			
		}
		Client::OnRecvDefineRequest( socket, eid, cid, command, len );
		return;
	}


	//===============================================================================//
    //
    //
    //===============================================================================//
    u_int ValidateClient::GetHostCount()
	{
	    return m_HostList.size();
	}

	// ---------------------------------------------------------------------------
	// GetHostInfo
	// 取得註冊的伺服器資訊
	// ---------------------------------------------------------------------------
    bool ValidateClient::GetHostInfo( const char *hostname,
                                      int &n,
		                              ipa_t host, u_short& port,
							          char* servicename,
							          u_int& maxconnect, u_int& usedconnect, int& load )
	{
		for( UINT i=0; i<m_HostList.size(); i++ )
		{
			if( strcmp( m_HostList[i]._hostname, hostname ) == 0 )
			{
				n = i;
			    strcpy( host, m_HostList[n]._ip );
			    port = m_HostList[n]._port;
			    strcpy( servicename, m_HostList[n]._servicename);
			    maxconnect = m_HostList[n]._maxconnect;
			    usedconnect = m_HostList[n]._usedconnect;
			    if (m_HostList[n]._maxconnect == 0)
				    load = 0;
			    else
				    load = m_HostList[n]._usedconnect * 100 / m_HostList[n]._maxconnect;
				return true;
			}
		}
		return false;
	}

	bool ValidateClient::GetHostInfo( int n,
		                              ipa_t host, u_short& port,
									  char *hostname, char* servicename,
									  u_int& maxconnect, u_int& usedconnect, int& load )
	{
		if (n >= 0 && n < GetHostCount())
		{
			strcpy( host, m_HostList[n]._ip );
			port = m_HostList[n]._port;
			strcpy( hostname, m_HostList[n]._hostname);
			strcpy( servicename, m_HostList[n]._servicename);
			maxconnect = m_HostList[n]._maxconnect;
			usedconnect = m_HostList[n]._usedconnect;
			if (m_HostList[n]._maxconnect == 0)
				load = 0;
			else
				load = m_HostList[n]._usedconnect * 100 / m_HostList[n]._maxconnect;
			return true;
		} 
		
		return false;
	}




	void ValidateClient::OnGetHostInfo( ipa_t host, u_short port, 					
								        const char* hostname, 
								        const char* servicename, 
								        u_int maxconnect, u_int usedconnect )
	{
		m_HostList.push_back( Host_t(host, port, hostname, servicename, maxconnect, usedconnect) );
	}

	//! 取代上層功能
	void ValidateClient::OnKnockout( void )
	{
        m_HostList.clear();
		m_Loginstate=CSERR_NOT_CONNECTD;
		m_Id=-1;
		//m_GameClient->OnClientKnockout();
	}

	
	//! 取代上層功能
	//! 被斷線以後會被喚起的 callback
	void ValidateClient::OnDisconnection( void )
	{
		m_HostList.clear();
		m_Loginstate=CSERR_NOT_CONNECTD;
		m_Id=-1;
		//m_GameClient->OnClientDisconnection();
	}

};