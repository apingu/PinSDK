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
#include "PicnoMessagerClient.h"
#include "PMSGCommand.h"
#include "CsLobbyCommand.h"
#include <direct.h>



namespace Picno
{
	struct Host_t
	{
		char			_ip[64];
		unsigned short	_port;
		unsigned int	_maxconnect;
		unsigned int	_usedconnect;

		Host_t()	{	memset(this, 0, sizeof(Host_t));	}
		Host_t(const char* ip, const unsigned short port, unsigned int maxconnect, unsigned int usedconnect)
		{		strcpy(_ip, ip);	_port = port;	_maxconnect = maxconnect;	_usedconnect = usedconnect;		}
		Host_t(const Host_t& rhs)
		{		memcpy(this, &rhs, sizeof(Host_t));		}
	};

	std::vector<Host_t>      g_HostList;


		//===============================================================================//
		//
		//
		//===============================================================================//
		MessagerClient::MessagerClient()
		{
			mkdir( "./Resource" );
			mkdir( "./Resource/Profiles" );
			mkdir( "./Resource/Cache" );
			mkdir( "./Resource/History" );
			mkdir( "./Resource/Skin" );
			mkdir( "./Resource/Media" );
			mkdir( "./Resource/Media/Smileys" );
			mkdir( "./Resource/Media/Image" );
			mkdir( "./Resource/Media/Sound" );
			mkdir( "./Resource/Media/Animation" );
			m_LoginResult=CSERR_ACC_UNKNOW_ERROR;

		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		MessagerClient::~MessagerClient()
		{
			m_LoginResult=CSERR_ACC_UNKNOW_ERROR;
			
			while( 1 )
			{
				std::map< ConferenceId_t, MessagerConference* >::iterator iter = m_ConferenceList.begin();
				if( iter == m_ConferenceList.end() )
					break;
				LeaveConference( iter->first );
			}
			m_ConferenceList.clear();
		}

		///////////////////////////////////////////////////////////////////////////////
		// login/logout

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::Login( const char* username, const char* password )
		{
			CsCommandMessage hsysmsg;// = (CsCommandMessage2 *)&m_send_cmd;
		    CsLoginMessage::Client::SetRequestMessage(&hsysmsg,username,password);
		    SendMessage( CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buff, hsysmsg.Header.CommandLen );
		    sleep( 200 );

			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::Logout( void )
		{
			char buff[512];
			size_t len = PMSGLogoutMessage::Client::SetRequest( buff, GetUserId() );
			SendMessage( CsINVALID_ID, PMSG_C_S_LOGIN_REQUEST, buff, len );
			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		short MessagerClient::GetState( void )
		{
			return m_LoginResult;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		userid_t MessagerClient::GetUserId( void )
		{
			return GetConnectionId();
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::SendShockToUser( userid_t user )
		{
			char buff[_MAX_MESSAGE_BUFF];
			size_t len = PMSGShockFriendMessage::Client::SetRequest( buff, user );
			SendMessage( CsINVALID_ID, PMSG_C_S_SHOCKUSER_REQUEST, buff, len );
			return;
		}

		


		///////////////////////////////////////////////////////////////////////////////
		// update name/picture

	    //===============================================================================//
		//
		//
		//===============================================================================//
		bool MessagerClient::SendUpdateNickname( const char* nickname )
		{
			if( strlen(nickname) > 1024 )
				return false;
			char buff[1032];
			size_t len = PMSGUpdateNicknameMessage::Client::SetRequest( buff, GetUserId(), nickname );
			SendMessage( CsINVALID_ID, PMSG_C_S_UPDATENICKNAME_REQUEST, buff, len );
			return true;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::SendUpdatePicture( const char* filename )
		{
			if( strcmp(filename, "" )==0 )
				return;
				
			FILE* imagef=fopen( filename, "rb" );
			if( imagef==NULL )
				return;

			int flen=_filelength( fileno(imagef) );

			char* buff=new char[flen+2];

			fread( (buff+2), flen, 1, imagef );

			(*(unsigned short*)(buff))=flen;

			fclose( imagef );
			SendMessage( CsINVALID_ID, PMPG_C_S_UPDATEPICTURE_REQUEST, (char*)buff, flen+2 );
			delete [] buff;

			sleep( 500 );

			return;
		}


		bool MessagerClient::ConnectMessagerServer()
		{
			for (int i = 0; i < 30; ++i)	// 重試30次
			{
				if (g_HostList.size() != 0)
				{
					if (Connect(g_HostList[0]._ip, g_HostList[0]._port))
					{
						return true;
					}
				}
				sleep(100);
			}
			return false;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::OnGetHostInfo( ipa_t host, u_short port,
		                                    const char* hostname, 
									        const char* servicename, 
									        u_int maxconnect, u_int usedconnect )
		 {
			if( strcmp( "MessagerServer", servicename )==0 )
			    g_HostList.push_back(Host_t(host, port, maxconnect, usedconnect));
			return;
		 }





		///////////////////////////////////////////////////////////////////////////////
		// chat

		//===============================================================================//
		//
		//
		//===============================================================================//
		ConferenceId_t MessagerClient::CreateConference( void )
		{
			char buff[1024];
			m_CreateCId=CsINVALID_ID;
			size_t len = PMSGChatCreateConference::Client::SetRequest( buff );
			SendMessage( CsINVALID_ID, PMSG_C_S_CREATE_CONFERENCE_REQUEST, buff, len );

			for( UINT i=0; i<30; i++ )
			{
				if( m_CreateCId!=CsINVALID_ID )
				{
					UINT cid=m_CreateCId;
					m_CreateCId=CsINVALID_ID;
					return cid;
				}
				sleep( 50 );
			}
			return m_CreateCId;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::LeaveConference( ConferenceId_t cid )
		{
			char buff[1024];
			size_t len = PMSGChatLeaveConference::Client::SetRequest( buff, cid );
			SendMessage( CsINVALID_ID, PMSG_C_S_LEAVE_CONFERENCE_REQUEST, buff, len );

			std::map< ConferenceId_t, MessagerConference* >::iterator iter = m_ConferenceList.find( cid );
			if( iter != m_ConferenceList.end() )
			{
				SAFE_DELETE( iter->second );
				m_ConferenceList.erase( iter );
			}

		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::InviteConference( ConferenceId_t roomnum, userid_t with )
		{
			char buff[32];
			size_t len = PMSGChatInviteConference::Client::SetRequest( buff, roomnum, with );
			SendMessage( CsINVALID_ID, PMSG_C_S_INVITE_CONFERENCE_REQUEST, buff, len );
			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::SendConferenceMessage( ConferenceId_t roomnum, const char* msg )
		{
			char buff[_MAX_MESSAGE_BUFF];
			size_t len = PMSGChatSendMessage::Client::SetRequest( buff, roomnum, msg );
			SendMessage( CsINVALID_ID, PMSG_C_S_CONFERENCEMESSAGE_REQUEST, buff, len );
			return;
		}


		///////////////////////////////////////////////////////////////////////////////
		// make friend
		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::SendRequestMakeFriend( userid_t with )
		{
			char buff[32];
			size_t len = PMSGMakefriendMessage::Client::SetRequest( buff, with );
			SendMessage( CsINVALID_ID, PMPG_C_S_MAKEFRIEND_REQUEST, buff, len );
			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessagerClient::OnRecvRequest( IO::TCPSocket* cs, ChannelId_t cid, const char* command, size_t len )
		{
			switch( cid )
			{
			case CsS_C_LOGIN_FEEDBACK:
				{
					CsLoginMessage::Client::GetFeedback( (char*)command, m_LoginResult );
				}
				return;

			case PMSG_S_C_LOGIN_FEEDBACK:
				{
				    PMSGLoginMessage::Client::GetFeedback( (char*)command, m_LoginResult );
				}
				return;
				
		    case PMSG_S_C_LOGOUT_REQUEST:
		        {
		        	userid_t fid=CsINVALID_ID;
		        	PMSGLogoutMessage::Client::GetRequest( (char*)command, fid );
		        	OnLogout( fid );
		    	}
		    	return;
				
			case PMPG_S_C_ENUMFRIENDID_REQUEST:
				{
					userid_t fid=CsINVALID_ID;
					PMSGEnumFriendIdMessage::Client::GetRequest( (char*)command, fid );
					OnEnumFriendId( fid );
				}
				return;

			case PMPG_S_C_UPDATENICKNAME_REQUEST:
				{
					userid_t fid=CsINVALID_ID;
					char     nickname[512];
					PMSGUpdateNicknameMessage::Client::GetRequest( (char*)command, fid, nickname );
					OnUpdateNickname( fid, nickname );
				}
				return;

			case PMSG_S_C_UPDATESTATE_REQUEST:
				{
					userid_t fid=CsINVALID_ID;
					short    state=0;
					PMSGUpdateStateMessage::Client::GetRequest( (char*)command, fid, state );
					OnUpdateState( fid, state );
				}
				return;
				
			case PMSG_S_C_UPDATEEVENT_REQUEST:
				{
					userid_t fid=CsINVALID_ID;
					short    event=0;
					PMSGUpdateStateMessage::Client::GetRequest( (char*)command, fid, event );
					OnUpdateEvent( fid, event );
				}
				return;

			case PMSG_S_C_UPDATEPICTURE_REQUEST:
				{
					//PMSGUpdatePictureMessage::Client::GetRequest( (char*)command, 


				}
				return;


			///////////////////////////////////////////////////////////////////////////
			// create conference

			case PMSG_S_C_CREATE_CONFERENCE_FEEDBACK:
				{
					m_ConferenceLock.Lock();
					m_CreateCId=CsINVALID_ID;
					PMSGChatCreateConference::Client::GetFeedback( (char*)command, m_CreateCId );
					m_ConferenceList.insert( std::map<ConferenceId_t, MessagerConference*>::value_type( m_CreateCId, new MessagerConference(m_CreateCId) ) );
					while( m_CreateCId!=CsINVALID_ID )
					{
					}
					m_ConferenceLock.Lock();
				}
				return;

			case PMSG_S_C_INVITE_CONFERENCE_REQUEST:
				{
					ConferenceId_t cid;
					userid_t       from;
					PMSGChatInviteConference::Client::GetRequest( (char*)command, cid, from );
					short reply=OnChatInvitedByFriend( cid, from );

					char buff[32];
					size_t len = PMSGChatInviteConference::Client::SetFeedback( buff, cid, from, reply );
					SendMessage( CsINVALID_ID, PMSG_C_S_INVITE_CONFERENCE_FEEDBACK, buff, len );
				}
				return;

			case PMSG_S_C_INVITE_CONFERENCE_FEEDBACK:
				{
					ConferenceId_t cid=CsINVALID_ID;
					userid_t       with=CsINVALID_ID;
					short          reply=0;
					PMSGChatInviteConference::Client::GetFeedback( (char*)command, cid, with, reply );
					OnChatInvitedReply( cid, with, reply );
				}
				return;

			case PMSG_S_C_LEAVE_CONFERENCE_REQUEST:
				{
					ConferenceId_t cid=CsINVALID_ID;
					userid_t       uid=CsINVALID_ID;
					PMSGChatLeaveConference::Client::GetRequest( (char*)command, cid, uid );
					OnChatLeaveConference( cid, uid );
				}
				return;

			case PMSG_S_C_CONFERENCEMESSAGE_REQUEST:
				{
					char msg[_MAX_MESSAGE_BUFF];
					ConferenceId_t cid=CsINVALID_ID;
					userid_t       from=CsINVALID_ID;
					PMSGChatSendMessage::Client::GetRequest( (char*)command, cid, from, msg );
					OnRecvConferenceMessage( cid, from, msg );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// make friend
			case PMPG_S_C_MAKEFRIEND_REQUEST:
				{
                    userid_t uid=CsINVALID_ID;
					PMSGMakefriendMessage::Client::GetRequest( (char*)command, uid );
					bool ret=OnRequestMakeFriendWith( uid );
					char msg[_MAX_MESSAGE_BUFF];
					size_t size=PMSGMakefriendMessage::Client::SetFeedback( msg, uid, ret );
					SendMessage( CsINVALID_ID, PMPG_C_S_MAKEFRIEND_FEEDBACK, msg, size );
				}
				return;

			case PMPG_S_C_MAKEFRIEND_FEEDBACK:
				{
                    userid_t uid=CsINVALID_ID;
					bool     ret=false;
					PMSGMakefriendMessage::Client::GetFeedback( (char*)command, uid, ret );
					OnFeedbackAddnewFriend( uid, ret );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// make shockuser
			case PMSG_S_C_SHOCKUSER_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					PMSGShockFriendMessage::Client::SetRequest( (char*)command, uid );
					OnShockFromUser( uid );
				}
				return;

		    case CsC_S_SERVERINFI_POST:
			{
				ipn_t host;
				u_short port;
				u_int maxconnect;
				u_int usedconnect;
				char  hostbuff[512];
				char  servicebuff[512];
				CsLobbyCommand::ServerInfo::Client::GetRequest( (char*)command, host, port, hostbuff, servicebuff, maxconnect, usedconnect );
				OnGetHostInfo( get_ipntoa( host ), port, hostbuff, servicebuff, maxconnect, usedconnect );
			}
			break;
				
			}


			Client::OnRecvRequest( cs, cid, command, len );
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
/*
		void Client::LoginFeedbackCB( long who, void* pData, unsigned int len )
		{
			Picno::Messager::Client* client = (Picno::Messager::Client*)who;

			bool ret=CsINVALID_ID;
	        ConnectionId_t cid=CsINVALID_ID;
			PMSGLoginMessage::Client::GetFeedback( pData, ret, cid );
			if( result )
			    client->m_UserId= cid;
			else
				

		}

		// callback function when friend change his/her nickname
		void     FriendUpdateNicknameCB( long who, void* pData, unsigned int len )
		{
			Picno::Messager::Client* client = (Picno::Messager::Client*)who;

			userid_t uid=CsINVALID_ID;
			char nickname[512];
		
    		PMSGUpdateNicknameMessage::Client::GetRequest( pData, uid, nickname );
			client->OnFriendUpdateNickname( uid, nickname );
		}

		// callback when shock from other
		void     ShockFromUserCB( long who, void* pData, unsigned int len )
		{
			Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}
		
		void     InvitedByFriendCB( long who, void* pData, unsigned int len )
		{
			Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}

		// callback function when friend change his/her picture
		// online call when click friend conference
		void     UpdateFriendPictureCB( long who, void* pData, unsigned int len )
		{
			Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}
		

		// send message to a conference
		void     RecvConferenceMessageCB( long who, void* pData, unsigned int len )
		{
			Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}


		///////////////////////////////////////////////////////////////////////////
		// friend list

	    // callback when SendRequestAddnewFriend, the friend that want add will call this function
		// return true for accept or false for decline
		bool     RequestMakeFriendWithCB( long who, void* pData, unsigned int len )
		{
			Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}


		// callback when OnRequestMakeFriendWith,  successuf or not
		void     FeedbackAddnewFriendCB( long who, void* pData, unsigned int len )
		{
            Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}


		// callback function when login and get all friend list from server
		void     EnumFriendCB( long who, void* pData, unsigned int len )
		{
            Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}

		// callback function when one friend online
		void     FriendOnlineCB( long who, void* pData, unsigned int len )
		{
            Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}

		// callback function when one friend offline
		void     FriendOfflineCB( long who, void* pData, unsigned int len )
		{
            Picno::Messager::Client* client = (Picno::Messager::Client*)who;
		}
		*/


};
