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
#include "CsLoginCommand.h"
#include "CsGameCommand.h"

namespace Cyberspace
{

    //===============================================================================//
    //
    //
    //===============================================================================//
	ValidateClient::ValidateClient( IGameClient*  client )
	{
		m_GameClient = client;
		m_bCbBlock.SetValue(true);
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	ValidateClient::~ValidateClient()
	{

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
		int ret = SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buffer.Param, hsysmsg.Header.CommandLen );
		sleep( 200 );
		return ret;
	}
	
    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::CreateEntity( const char_t* name, const byte_t* prop, size_t len )
	{
		TS_Lock lock( &m_UtVaLock );
		//CsCommandPacket hsysmsg;
		CsResBuf buf;
		CsCreateEntityMessage::Client::SetRequestMessage( buf, name, prop, len );
		//SEND_GAME_MESSAGE( this, (&hsysmsg) );
		SendCsMessage( CsINVALID_ID, CsC_S_CREATENEWENTITY_REQUEST, _GetResBufPtr(buf), _GetResBufLen(buf) );
		sleep( 200 );
		return;
	}

    //===============================================================================//
    //
    // 告訴SERVER 你要選擇哪一個人
    //===============================================================================//
	void ValidateClient::SelectEntity( const char_t* name )
	{
		TS_Lock lock( &m_UtVaLock );
		//CsCommandPacket hsysmsg;
		CsResBuf buf;
		CsSelectEntityMessage::Client::SetRequestMessage( buf,name );

		//SEND_GAME_MESSAGE( this, (&hsysmsg) );
		SendCsMessage( CsINVALID_ID, CsC_S_SELECTENTITY_REQUEST, _GetResBufPtr(buf), _GetResBufLen(buf) );
		//SendCsMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buffer.Param, hsysmsg.Header.CommandLen );
		sleep( 500 );
		return;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::DeleteEntity( const char_t* name )
	{
		TS_Lock lock( &m_UtVaLock );
		//CsCommandPacket hsysmsg;
		CsResBuf buf;
		CsDeleteEntityMessage::Client::SetRequestMessage( buf, name );
		//SEND_GAME_MESSAGE( this, (&hsysmsg) );
		SendCsMessage( CsINVALID_ID, CsC_S_DELETEENTITY_REQUEST, _GetResBufPtr(buf), _GetResBufLen(buf) );
		sleep( 200 );
		return;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	int ValidateClient::Update( void )
	{
		if( m_bCbBlock.GetValue() )
		{
			return LoginClient::Update();
		}

		return 0;		
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	void ValidateClient::OnConnect( void )
	{
		// 還沒有正確登入之前也不用被叫起
		if( GetLoginState() == CSERR_ACC_SUCCESS )
		{
			if( m_bCbBlock )
			{
				m_GameClient->OnClientConnect();
			}
		}
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	void ValidateClient:: OnKnockout( void )
	{
		// 還沒有正確登入之前也不用被叫起
		if( GetLoginState() == CSERR_ACC_SUCCESS )
		{
			if( m_bCbBlock )
			{
				m_GameClient->OnClientKnockout();
			}
		}

	}	


	//===============================================================================//
	//
	//
	//===============================================================================//
	void ValidateClient::OnDisconnection( void )
	{
		// 還沒有正確登入之前也不用被叫起
		if( GetLoginState() == CSERR_ACC_SUCCESS )
		{
			if( m_bCbBlock )
			{
				m_GameClient->OnClientDisconnection();
			}

		}
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	void ValidateClient::OnAssingHost ( ipa_t host, u_short port,
		const char* hostname, 
		const char* servername, 
		const char* functionname, 
		u_int maxconnect, u_int usedconnect )
	{

		m_GameClient->JoinServer(hostname);
	}



	void ValidateClient::OnGetHostInfo( ipa_t host, u_short port, 					
		                                const char* hostname, 
		                                const char* servername, 
		                                const char* functionname, 
		                                u_int maxconnect, u_int usedconnect )
	{
	    m_GameClient->OnGetHostInfo( host, port, hostname, servername, functionname, maxconnect, usedconnect );
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	void ValidateClient::OnRecvDefineRequest( connectionid_t cid, uniqueid_t eid, channelid_t chid, const byte_t* command, size_t len )
	{
		TS_Lock lock( &m_UtVaLock );

		switch( chid )
		{

		case CsS_C_SELECTENTITY_FEEDBACK:
			{
			    m_GameClient->OnGetSelectEntityId( CsSelectEntityMessage::Client::GetEntityId( command ) );
			}
		    return;
		case CsS_C_CREATENEWENTITY_FEEDBACK:
			{
				size_t  l=0;
				const char*   name = CsCreateEntityMessage::Client::GetEntityName( command );
				const byte_t* propbuf  = CsCreateEntityMessage::Client::GetProperty( command, l );
				short         errcode  = CsCreateEntityMessage::Client::GetResult( command );
				if(name==NULL)
					return;
				m_GameClient->OnGetCreateEntity( name, (const entitybuf_t)propbuf, l, errcode );
			}
			return;
		////////////////////////////////////////////////
		////create entity or select all entity
		case CsS_C_GETENTITYRECORD_FEEDBACK:
			{
				size_t l=0;
				char*         name=(char*)CsGetRecordEntityMessage::Client::GetEntityName( command );
				const byte_t* propbuf = CsGetRecordEntityMessage::Client::GetProperty( command, l );

				if( (name==NULL) || (propbuf==NULL) )
					return;
				m_GameClient->OnGetRecordEntity( name, (const entitybuf_t)propbuf, l );
			}
			return;
		case CsS_C_DELETEENTITY_FEEDBACK:
			{
				m_GameClient->OnEntityDeleted( CsDeleteEntityMessage::Client::GetEntityName( command ) ,
					                           CsDeleteEntityMessage::Client::GetFeedbackRet( command ) );
			}
			return;
		default:
			LoginClient::OnRecvDefineRequest( cid, eid, chid, command, len );
			break;

		}
		
		return;
	}

	void ValidateClient::SetBlock( bool t )
	{
		m_bCbBlock= t ;
	}


	bool ValidateClient::GetBlock( void )
	{
		return m_bCbBlock;
	}


};