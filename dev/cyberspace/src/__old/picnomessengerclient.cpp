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
#include "PicnoMessengerClient.h"
#include "PMSGCommand.h"
#include "CsLoginCommand.h"
#include <direct.h>



namespace Picno
{
	struct Host_t
	{
		char	_ip[64];
		u_short _port;
		char	_hostname[_MAX_HOSTNAME];
		char	_servername[_MAX_SERVERNAME];
		char	_functionname[_MAX_FUNCTIONNAME];
		u_int	_maxconnect;
		u_int	_usedconnect;

		Host_t()	{	memset(this, 0, sizeof(Host_t));	}

		Host_t( const char* ip, const u_short port,
			    const char* hostname, 
			    const char* servername, 
			    const char* functionname, 
			    u_int maxconnect, u_int usedconnect)
		{		strcpy(_ip, ip);	_port = port;	
		        strcpy(_hostname, hostname);
		        strcpy(_servername, servername);
		        strcpy(_functionname, functionname);
		        _maxconnect = maxconnect;	_usedconnect = usedconnect;		}

		Host_t(const Host_t& rhs)
		{		memcpy(this, &rhs, sizeof(Host_t));		}
	};

	std::vector<Host_t>      g_HostList;

	T_Lock g_ConferenceLock;

	void InsertConference( std::vector<ConferenceId_t>& cl, ConferenceId_t cid )
	{
		TS_Lock lock( &g_ConferenceLock );
		cl.push_back( cid );
	}

	void RemoveConference( std::vector<ConferenceId_t>& cl, ConferenceId_t cid )
	{
		TS_Lock lock( &g_ConferenceLock );
		for( unsigned int i=0; i<cl.size(); i++ )
		{
			if( cl[i]==cid )
			{
				cl.erase( cl.begin()+i );
				return;
			}
		}
	}




		//===============================================================================//
		//
		//
		//===============================================================================//
		MessengerClient::MessengerClient()
		{
			_mkdir( "./Resource" );
			_mkdir( "./Resource/Profiles" );
			_mkdir( "./Resource/Cache" );
			_mkdir( "./Resource/History" );
			_mkdir( "./Resource/Skin" );
			_mkdir( "./Resource/Media" );
			_mkdir( "./Resource/Media/Smileys" );
			_mkdir( "./Resource/Media/Image" );
			_mkdir( "./Resource/Media/Sound" );
			_mkdir( "./Resource/Media/Animation" );
			m_LoginResult=CSERR_NOT_CONNECTD;

		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		MessengerClient::~MessengerClient()
		{
			m_LoginResult=CSERR_NOT_CONNECTD;
			
			for( UINT i=0; i<m_ConferenceList.size(); i++ )
			{
				LeaveConference( m_ConferenceList[i] );
			}
			
			m_ConferenceList.clear();
		}

		///////////////////////////////////////////////////////////////////////////////
		// login/logout

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::Login( const char* username, const char* password )
		{
			CsResBuf resbuf;
			m_Username = username;
			//CsCommandPacket hsysmsg;// = (CsCommandPacket2 *)&m_send_cmd;
		    CsLoginMessage::Client::SetRequestMessage( resbuf, username, password );
		    SendCsMessage( CsINVALID_ID, CsC_S_LOGIN_REQUEST, _GetResBufPtr(resbuf), _GetResBufLen(resbuf) );
		    //sleep( 200 );

			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::Logout( void )
		{
			m_Username="";
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGLogoutMessage::Client::SetRequest( buff, GetUserId() );
			SendCsMessage( CsINVALID_ID, PMSG_C_S_LOGOUT_REQUEST, buff, len );
			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		CSERR_CODE MessengerClient::GetState( void )
		{
			return m_LoginResult;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		userid_t MessengerClient::GetUserId( void )
		{
			return GetConnectionId();
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendShockToUser( ConferenceId_t cid, userid_t user )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGShockFriendMessage::Client::SetRequest( buff, cid, user );
			SendCsMessage( 0, PMSG_C_S_SHOCKUSER_REQUEST, buff, len );
			return;
		}

		
	    //===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendGetMyPicture( void )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGGetPictureMessage::Client::SetRequest( buff, GetUserId() );
			SendCsMessage( 0, PMSG_C_S_GET_USERPICTURE_REQUEST, buff, len );
		}

	    //===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendGetFriendPicture( userid_t uid )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGGetPictureMessage::Client::SetRequest( buff, uid );
			SendCsMessage( 0, PMSG_C_S_GET_USERPICTURE_REQUEST, buff, len );
		}

		///////////////////////////////////////////////////////////////////////////////
		// update name/picture

	    //===============================================================================//
		//
		//
		//===============================================================================//
		bool MessengerClient::SendUpdateNickname( const char* nickname )
		{
			if( strlen(nickname) > _SHORT_BUFFER_SIZE_ )
				return false;
			char buff[1032];
			size_t len = PMSGUpdateNicknameMessage::Client::SetRequest( buff, GetUserId(), nickname );
			SendCsMessage( 0, PMSG_C_S_UPDATE_MYNICKNAME_REQUEST, buff, len );
			return true;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendUpdatePicture( const char* filename )
		{
			if( strcmp(filename, "" )==0 )
				return;
				
			FILE* imagef=fopen( filename, "rb" );
			if( imagef==NULL )
				return;

			int flen=_filelength( _fileno(imagef) );

			char* buff=new char[flen];

			fread( (buff), flen, 1, imagef );

			//(*(unsigned short*)(buff))=flen;

			fclose( imagef );

			char* commbuff=new char[(flen)*2];
			
			size_t len = PMSGUpdatePictureMessage::Client::SetRequest( commbuff, GetUserId(), (char*)buff, flen );
			SendCsMessage( 0, PMSG_C_S_UPDATE_MYPICTURE_REQUEST, commbuff, len );
			delete [] buff;
			delete [] commbuff;

			sleep( 500 );

			OnUpdateMyPicture( GetUserId(), (BYTE*)buff, len );

			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendUpdateState( short state )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGUpdateStateMessage::Client::SetRequest( buff, GetUserId(), state );
			SendCsMessage( 0, PMSG_C_S_UPDATE_MYSTATE_REQUEST, buff, len );
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		bool MessengerClient::ConnectMessengerServer()
		{
			if (g_HostList.size() != 0)
			{
				if (Connect(g_HostList[0]._ip, g_HostList[0]._port))
				{
					return true;
				}
			}
//			for (int i = 0; i < 3; ++i)	// 重試30次
//			{
//
//				sleep(10);
//			}
			return false;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::OnGetHostInfo( ipa_t host, u_short port,
		                                     const char* hostname, 
									         const char* servername, 
											 const char* functionname, 
									         u_int maxconnect, u_int usedconnect )
		 {
			if( strcmp( "MessengerServer", servername )==0 )
			    g_HostList.push_back(Host_t( host, port, hostname, servername, functionname, maxconnect, usedconnect));
			return;
		 }





		///////////////////////////////////////////////////////////////////////////////
		// chat

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::CreateConferenceWith( userid_t user )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGChatCreateConferenceWith::Client::SetRequest( buff, user );
			SendCsMessage( 0, PMSG_C_S_CREATE_CONFERENCEWITH_REQUEST, buff, len );
			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::LeaveConference( ConferenceId_t cid )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGChatLeaveConference::Client::SetRequest( buff, cid );
			SendCsMessage( 0, PMSG_C_S_LEAVE_CONFERENCE_REQUEST, buff, len );
			RemoveConference( m_ConferenceList, cid );
			
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendInviteConference( ConferenceId_t roomnum, userid_t with )
		{
			char buff[32];
			size_t len = PMSGChatInviteConference::Client::SetRequest( buff, roomnum, with );
			SendCsMessage( 0, PMSG_C_S_INVITE_CONFERENCE_REQUEST, buff, len );
			return;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//		
		void MessengerClient::SendFeedbackChatInvitedByFriend( ConferenceId_t roomnum, userid_t user, PMSG_CONFERENCE_STATE reply )
		{
			char buff[32];
			size_t len = PMSGChatInviteConference::Client::SetFeedback( buff, roomnum, user, reply );
			SendCsMessage( 0, PMSG_C_S_INVITE_CONFERENCE_FEEDBACK, buff, len );
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendConferenceMessage( ConferenceId_t roomnum, const char* msg )
 		{
			char msgbuff[1024];

						
			if( strlen(msg)>1024 )
			{
				msgbuff[1023]=0;
				memcpy( msgbuff, msg, 1023 );
			}
			else
			{
				strcpy( msgbuff, msg );
			}
				

			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGChatSendMessage::Client::SetRequest( buff, roomnum, msgbuff );
			SendCsMessage( 0, PMSG_C_S_CONFERENCEMESSAGE_REQUEST, buff, len );
			return;
		}


		///////////////////////////////////////////////////////////////////////////////
		// make friend
		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendRequestMakeFriend( userid_t with )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			size_t len = PMSGMakefriendMessage::Client::SetRequest( buff, with );
			SendCsMessage( 0, PMPG_C_S_MAKEFRIEND_REQUEST, buff, len );
			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendFeedbackMakeFriend( userid_t from, bool ret )
		{
			char msg[_SHORT_BUFFER_SIZE_];
			size_t size=PMSGMakefriendMessage::Client::SetFeedback( msg, from, ret );
			SendCsMessage( 0, PMPG_C_S_MAKEFRIEND_FEEDBACK, msg, size );
			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::SendRequestDeleteFriend( userid_t uid )
		{
			char msg[_SHORT_BUFFER_SIZE_];
			size_t size=PMSGDeletefriendMessage::Client::SetRequest( msg, uid );
			SendCsMessage( 0, PMPG_C_S_DELETEFRIEND_REQUEST, msg, size );
			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void MessengerClient::OnRecvDefineRequest ( connectionid_t cid, uniqueid_t who, channelid_t chid, const char* command, size_t len )
		{
			switch( cid )
			{
			case CsS_C_LOGIN_FEEDBACK:
				{
					CsLoginMessage::Client::GetFeedback( (char*)command, m_LoginResult );
					if( m_LoginResult==CSERR_ACC_SUCCESS )
					{
						char dirname[_MAX_PATH];
						sprintf( dirname, "%s/%s", "./Resource/Profiles", m_Username.c_str() );
						_mkdir( dirname );
					}
				}
				return;

				
		    case PMSG_S_C_LOGOUT_REQUEST:
		        {
		        	userid_t fid=0;
		        	PMSGLogoutMessage::Client::GetRequest( (char*)command, fid );
		        	OnLogout( fid );
		    	}
		    	return;

				
			case PMSG_S_C_ENUMFRIENDID_REQUEST:
				{
					userid_t fid=0;
					PMSGEnumFriendIdMessage::Client::GetRequest( (char*)command, fid );
					OnEnumFriendId( fid );
				}
				return;


		    case PMSG_S_C_UPDATE_MYNICKNAME_REQUEST:
				{
					userid_t fid=0;
					char     nickname[512];
					PMSGUpdateNicknameMessage::Client::GetRequest( (char*)command, fid, nickname );
					m_MyNickname=nickname;
					OnUpdateMyNickname( fid, nickname );
				}
				return;


			case PMSG_S_C_UPDATE_MYSTATE_REQUEST:
				{
					userid_t fid=0;
					short    state=0;
					PMSGUpdateStateMessage::Client::GetRequest( (char*)command, fid, state );
					m_MyState=state;
					OnUpdateMyState( fid, state );
				}
				return;

			case PMSG_S_C_UPDATE_MYEVENT_REQUEST:
				{
					userid_t fid=0;
					short    event=0;
					PMSGUpdateStateMessage::Client::GetRequest( (char*)command, fid, event );
					m_MyEvent=event;
					OnUpdateMyEvent( fid, event );
				}
				return;

		    case PMSG_S_C_UPDATENICKNAME_REQUEST:
				{
					userid_t fid=0;
					char     nickname[512];
					PMSGUpdateNicknameMessage::Client::GetRequest( (char*)command, fid, nickname );
					OnUpdateNickname( fid, nickname );
				}
				return;

			case PMSG_S_C_UPDATESTATE_REQUEST:
				{
					userid_t fid=0;
					short    state=0;
					PMSGUpdateStateMessage::Client::GetRequest( (char*)command, fid, state );
					OnUpdateState( fid, state );
				}
				return;

			
			case PMSG_S_C_UPDATEEVENT_REQUEST:
				{
					userid_t fid=0;
					short    event=0;
					PMSGUpdateStateMessage::Client::GetRequest( (char*)command, fid, event );
					OnUpdateEvent( fid, event );
				}
				return;

			case PMSG_S_C_UPDATE_PICTURE_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					void*  imagebuf=NULL;
					size_t len=0;
					PMSGGetPictureMessage::Client::GetFeedback( (char*)command, uid, imagebuf, len );
					if( uid == GetUserId() )
					    OnUpdateMyPicture( uid, (BYTE*)imagebuf, len );
					else
						OnUpdateFriendPicture( uid, (BYTE*)imagebuf, len );

				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// create conference
		    case PMSG_S_C_CREATE_CONFERENCEWITH_FEEDBACK:
				{
					TS_Lock lock( &m_ConferenceLock );
					ConferenceId_t cid=0;
					userid_t       uid=0;
					short          ret=0;
					PMSGChatCreateConferenceWith::Client::GetFeedback( (char*)command, cid, uid, ret );
					InsertConference( m_ConferenceList, cid );
					OnCreateConferenceWith( cid, uid, (PMSG_CONFERENCE_STATE)ret );
//					MessengerConference* conference = new MessengerConference(cid);
//					conference->JoinParticipant( uid );
//					m_ConferenceList.insert( std::map<ConferenceId_t, MessengerConference*>::value_type( cid, conference ) );
				}
				return;

			case PMSG_S_C_INVITE_CONFERENCE_REQUEST:
				{
					TS_Lock lock( &m_ConferenceLock );
					ConferenceId_t cid;
					userid_t       from;
					PMSGChatInviteConference::Client::GetRequest( (char*)command, cid, from );
					OnChatInvitedByFriend( cid, from );
				}
				return;

			case PMSG_S_C_INVITE_CONFERENCE_FEEDBACK:
				{
					TS_Lock lock( &m_ConferenceLock );
					ConferenceId_t cid  =0;
					userid_t       with =0;
					short          reply=0;
					char nickname[_SHORT_BUFFER_SIZE_];
					PMSGChatInviteConference::Client::GetFeedback( (char*)command, cid, with, nickname, reply );
					OnChatInvitedReply( cid, with, nickname, (PMSG_CONFERENCE_STATE)reply );
				}
				return;

			case PMSG_S_C_LEAVE_CONFERENCE_REQUEST:
				{
					TS_Lock lock( &m_ConferenceLock );
					ConferenceId_t cid=0;
					userid_t       uid=0;
					PMSGChatLeaveConference::Client::GetRequest( (char*)command, cid, uid );
					OnChatLeaveConference( cid, uid );
				}
				return;

			case PMSG_S_C_CONFERENCEMESSAGE_REQUEST:
				{
					TS_Lock lock( &m_ConferenceLock );
					char msg[_SHORT_BUFFER_SIZE_];
					ConferenceId_t cid =0;
					userid_t       from=0;
					PMSGChatSendMessage::Client::GetRequest( (char*)command, cid, from, msg );
					OnRecvConferenceMessage( cid, from, msg );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// make friend
			case PMPG_S_C_MAKEFRIEND_REQUEST:
				{
					TS_Lock lock( &m_ConferenceLock );
                    userid_t uid=0;
					char nickname[256];
					PMSGMakefriendMessage::Client::GetRequest( (char*)command, nickname, uid );
					OnRequestMakeFriendWith( uid, nickname );
				}
				return;

			case PMPG_S_C_MAKEFRIEND_FEEDBACK:
				{
					TS_Lock lock( &m_ConferenceLock );
                    userid_t uid=0;
					char     nickname[256];
					short    ret=0;
					PMSGMakefriendMessage::Client::GetFeedback( (char*)command, nickname, uid, ret );
					OnFeedbackAddnewFriend( uid, nickname, ret );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// make shockuser
			case PMSG_S_C_SHOCKUSER_REQUEST:
				{
					TS_Lock lock( &m_ConferenceLock );
					ConferenceId_t cid=-1;
					userid_t uid=0;
					PMSGShockFriendMessage::Client::GetRequest( (char*)command, cid, uid );
					OnShockFromUser( cid, uid );
				}
				return;

		    case CsC_S_SERVERINFI_POST:
				{
					ipn_t host;
					u_short port;
					u_int maxconnect;
					u_int usedconnect;
					char  hostbuff[_MAX_HOSTNAME];
					char  serverbuff[_MAX_SERVERNAME];
					char  functionbuff[_MAX_FUNCTIONNAME];
					CsLobbyCommand::ServerInfo::Client::GetRequest( (char*)command, host, port, hostbuff, serverbuff, functionbuff, maxconnect, usedconnect );
					OnGetHostInfo( get_ipntoa( host ), port, hostbuff, serverbuff, functionbuff, maxconnect, usedconnect );
				}
				return;
				
			}

			Client::OnRecvDefineRequest( cid, who, chid, command, len );
			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
/*
		void Client::LoginFeedbackCB( long who, void* pData, unsigned int len )
		{
			Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;

			bool ret=0;
	        ConnectionId_t cid=0;
			PMSGLoginMessage::Client::GetFeedback( pData, ret, cid );
			if( result )
			    client->m_UserId= cid;
			else
				

		}

		// callback function when friend change his/her nickname
		void     FriendUpdateNicknameCB( long who, void* pData, unsigned int len )
		{
			Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;

			userid_t uid=0;
			char nickname[512];
		
    		PMSGUpdateNicknameMessage::Client::GetRequest( pData, uid, nickname );
			client->OnFriendUpdateNickname( uid, nickname );
		}

		// callback when shock from other
		void     ShockFromUserCB( long who, void* pData, unsigned int len )
		{
			Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}
		
		void     InvitedByFriendCB( long who, void* pData, unsigned int len )
		{
			Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}

		// callback function when friend change his/her picture
		// online call when click friend conference
		void     UpdateFriendPictureCB( long who, void* pData, unsigned int len )
		{
			Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}
		

		// send message to a conference
		void     RecvConferenceMessageCB( long who, void* pData, unsigned int len )
		{
			Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}


		///////////////////////////////////////////////////////////////////////////
		// friend list

	    // callback when SendRequestAddnewFriend, the friend that want add will call this function
		// return true for accept or false for decline
		bool     RequestMakeFriendWithCB( long who, void* pData, unsigned int len )
		{
			Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}


		// callback when OnRequestMakeFriendWith,  successuf or not
		void     FeedbackAddnewFriendCB( long who, void* pData, unsigned int len )
		{
            Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}


		// callback function when login and get all friend list from server
		void     EnumFriendCB( long who, void* pData, unsigned int len )
		{
            Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}

		// callback function when one friend online
		void     FriendOnlineCB( long who, void* pData, unsigned int len )
		{
            Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}

		// callback function when one friend offline
		void     FriendOfflineCB( long who, void* pData, unsigned int len )
		{
            Picno::Messenger::Client* client = (Picno::Messenger::Client*)who;
		}
		*/


};
