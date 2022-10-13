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
#include "PicnoMessagerServer.h"
#include "PMSGCommand.h"
#include "MessagerDBFuncion.h"
#include "CsArbitPeer.h"
#include "CstIdentity.h"
#include "PicnoMessagerConference.h"

		
namespace Cyberspace
{
    extern void SendSetConnectionCId( IO::TCPSocket* s, ConnectionId_t cid );
};

using namespace Cyberspace;


namespace Picno
{

	    T_Lock g_ConferenceListLock;

		std::map< ConferenceId_t, MessagerConference*  > g_Conferencelist;


		bool MessagerCreateConference( ConferenceId_t cid )
		{
			TS_Lock lock( &g_ConferenceListLock );

			std::map< ConferenceId_t, MessagerConference*  >::iterator iter = g_Conferencelist.find( cid );
			if( iter!=g_Conferencelist.end() )
				return false;
			g_Conferencelist.insert( std::map< ConferenceId_t, MessagerConference* >::value_type( cid, new MessagerConference(cid) ) );
			return true;
		}

		bool MessagerRemoveConference( ConferenceId_t cid )
		{
			TS_Lock lock( &g_ConferenceListLock );
			std::map< ConferenceId_t, MessagerConference*  >::iterator iter = g_Conferencelist.find( cid );
			if( iter!=g_Conferencelist.end() )
				return false;
			SAFE_DELETE( iter->second );
			g_Conferencelist.erase( cid );
			return true;
		}

		MessagerConference* GetMessagerConference( ConferenceId_t cid )
		{
            TS_Lock lock( &g_ConferenceListLock );
			std::map< ConferenceId_t, MessagerConference*  >::iterator iter = g_Conferencelist.find( cid );
			if( iter!=g_Conferencelist.end() )
				return iter->second;
			return NULL;
		}


		//===============================================================
		//
		//
		//===============================================================			
		MessagerServer::MessagerServer()
		{

		}


		//===============================================================
		//
		//
		//===============================================================			
		MessagerServer::~MessagerServer()
		{
			
		}

		//===============================================================
		//
		//
		//===============================================================
		void MessagerServer::OnClientConnect( IO::TCPSocket* socket )
		{
			char buff[_MAX_MESSAGE_BUFF];


			
			size_t size=0;

			std::vector<userid_t> friendlist;

		    //
			Picno::Messager::GetFriendlist( GetDataBase(), GetConnectionCId( socket ), friendlist );
			Picno::Messager::SetState( GetDataBase(), GetConnectionCId( socket ), PMSG_USER_ONLINE );

			for( UINT i=0; i< friendlist.size(); i++ )
			{
				size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, GetConnectionCId(socket),
			                                                        Picno::Messager::GetNickname( GetDataBase(), friendlist[i] ).c_str() );
				SendMessage( socket, CsSERVER_ID, PMPG_S_C_UPDATENICKNAME_REQUEST, buff, size );

				size=PMSGUpdateStateMessage::Server::SetRequest( buff, GetConnectionCId(socket),
		                                                         PMSG_USER_ONLINE );
				SendMessage( socket, CsSERVER_ID, PMSG_S_C_UPDATESTATE_REQUEST, buff, size );

			    size=PMSGUpdateEventMessage::Server::SetRequest( buff, GetConnectionCId(socket),
			                                                     0 );
			    SendMessage( socket, CsSERVER_ID, PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );
			}


			// 送自己的暱稱給朋友
			size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, GetConnectionCId(socket),
			                                                          Picno::Messager::GetNickname( GetDataBase(), GetConnectionCId( socket ) ).c_str() );
			SendMessageToFriends( GetConnectionCId( socket ), PMPG_S_C_UPDATENICKNAME_REQUEST, buff, size );

			// 送自己的狀態給朋友
			size=PMSGUpdateStateMessage::Server::SetRequest( buff, GetConnectionCId(socket),
			                                                       PMSG_USER_ONLINE );
			SendMessageToFriends( GetConnectionCId( socket ), PMSG_S_C_UPDATESTATE_REQUEST, buff, size );

			// 送自己的事件給朋友
			size=PMSGUpdateEventMessage::Server::SetRequest( buff, GetConnectionCId(socket),
			                                                       0 );
			SendMessageToFriends( GetConnectionCId( socket ), PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );


			//


		}

		//===============================================================
		//
		//
		//===============================================================
		void MessagerServer::OnClientDisconnect( IO::TCPSocket* socket )
		{
			char buff[_MAX_MESSAGE_BUFF];
			size_t size=PMSGLogoutMessage::Server::SetRequest( buff, GetConnectionCId( socket ) );
			Picno::Messager::SetState( GetDataBase(), GetConnectionCId( socket ), PMSG_USER_OFFLINE );

			// remove from all conference

			// for all friend
			SendMessageToFriends( GetConnectionCId(socket), PMSG_S_C_LOGOUT_REQUEST, buff, size );
		}


		//===============================================================
		//
		//
		//===============================================================
		void MessagerServer::SendMessageToConference( ConferenceId_t cid, userid_t from, ChannelId_t mid, const char* buff, size_t len )
		{
			MessagerConference* conf = GetMessagerConference( cid );
			if( conf==NULL )
			{
				assert ( 0||"Send message conference was a NULL");
			}

			TS_Lock loc( &conf->m_utLock );
			std::set< userid_t >::iterator iter=conf->m_Participants.begin();
			while( iter!=conf->m_Participants.end() )
			{
				SendMessage( (*iter), from, mid, buff, len );
				iter++;
			}
		
		}

		//===============================================================
		//
		//
		//===============================================================
		void MessagerServer::SendMessageToFriends( userid_t from, ChannelId_t cid, const char* buff, size_t len )
		{
			std::vector<userid_t> friendlist;

		    //
			Picno::Messager::GetFriendlist( GetDataBase(), from, friendlist );

			for( UINT i=0; i< friendlist.size(); i++ )
			{
				SendMessage( friendlist[i], from, cid, buff, len );
			}
		}

		//===============================================================
		//
		//
		//===============================================================
        void MessagerServer::OnRecvRequest( IO::TCPSocket* s, ChannelId_t cid, const char* command, size_t len )
		{
			switch( cid )
			{
 
			case PMSG_C_S_LOGOUT_REQUEST:
				return;

			case PMSG_C_S_UPDATENICKNAME_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					char nickname[512];
					PMSGUpdateNicknameMessage::Server::GetRequest( (char*)command, uid, nickname );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// Chat
			case PMSG_C_S_CREATE_CONFERENCE_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					PMSGChatCreateConference::Server::GetRequest( (char*)command, uid );
					// Create conference

					ConferenceId_t cid = MessagerCreateConference( uid );

					if( !MessagerCreateConference( cid ) )
						assert( 0||"Create conference falilure!!" );

					MessagerConference* conf = GetMessagerConference( cid );
					if( conf==NULL )
					{
						assert ( 0||"Create conference was a NULL");
					}
					conf->JoinParticipant( uid );
				
					// send conference id feedback
					char commandbuf[_MAX_MESSAGE_BUFF];
					size_t size = PMSGChatCreateConference::Server::SetFeedback( commandbuf, cid );
					SendMessage( uid, CsINVALID_ID, PMSG_S_C_CREATE_CONFERENCE_FEEDBACK, commandbuf, size );
				}
				return;

			case PMSG_C_S_INVITE_CONFERENCE_REQUEST:
				{
					ConferenceId_t cid=0;
					userid_t       with=CsINVALID_ID;
					PMSGChatInviteConference::Server::GetRequest( (char*)command, cid, with );

					char commandbuf[_MAX_MESSAGE_BUFF];
					size_t size = PMSGChatInviteConference::Server::SetRequest( commandbuf, cid, GetConnectionCId(s) );
					SendMessage( with, CsINVALID_ID, PMSG_S_C_INVITE_CONFERENCE_REQUEST, commandbuf, size );
				}
				return;

			case PMSG_C_S_INVITE_CONFERENCE_FEEDBACK:
				{
					ConferenceId_t cid=0;
					userid_t       with=CsINVALID_ID;
					short          reply=0;
					PMSGChatInviteConference::Server::GetFeedback( (char*)command, cid, with, reply );

					MessagerConference* conf = GetMessagerConference( cid );
					if( conf==NULL )
					{
						assert ( 0||"Leave conference was a NULL");
					}
				    conf->JoinParticipant( GetConnectionCId(s) );

					char commandbuf[_MAX_MESSAGE_BUFF];
					size_t size = PMSGChatInviteConference::Server::SetFeedback( commandbuf, cid, GetConnectionCId(s), reply );
					SendMessage( with, CsINVALID_ID, PMSG_S_C_INVITE_CONFERENCE_FEEDBACK, commandbuf, size );

				}
				return;

			case PMSG_C_S_LEAVE_CONFERENCE_REQUEST:
				{
					ConferenceId_t cid=0;
					PMSGChatLeaveConference::Server::GetRequest( (char*)command, cid );

					MessagerConference* conf = GetMessagerConference( cid );
					if( conf==NULL )
					{
						assert ( 0||"Leave conference was a NULL");
					}

					conf->ExitParticipant( GetConnectionCId(s) );
					if( conf->Empty() )
					{
						MessagerRemoveConference( cid );
					}
					else
					{
					    // for all friend
					    char commandbuf[_MAX_MESSAGE_BUFF];
				        size_t size=PMSGChatLeaveConference::Server::SetRequest( commandbuf, cid, GetConnectionCId(s) );
					    SendMessageToConference( cid, GetConnectionCId(s), PMSG_S_C_LEAVE_CONFERENCE_REQUEST, commandbuf, size );
					}
				}
				return;

			case PMSG_C_S_CONFERENCEMESSAGE_REQUEST:
				{
					ConferenceId_t cid=0;
					char msg[_MAX_MESSAGE_BUFF];
					PMSGChatSendMessage::Server::GetRequest( (char*)command, cid, msg );

					// for all chat user
					char commandbuf[_MAX_MESSAGE_BUFF];
					size_t size = PMSGChatSendMessage::Server::SetRequest( commandbuf, cid, GetConnectionCId(s), msg );
					SendMessageToConference( cid, GetConnectionCId(s), PMSG_S_C_CONFERENCEMESSAGE_REQUEST, msg, size );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// Friend 
			case PMPG_C_S_MAKEFRIEND_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					PMSGMakefriendMessage::Server::GetRequest( (char*)command, uid );

					char commandbuf[_MAX_MESSAGE_BUFF];
					size_t size = PMSGMakefriendMessage::Server::SetRequest( commandbuf, GetConnectionCId(s) );
					SendMessage( uid, CsINVALID_ID, PMPG_S_C_MAKEFRIEND_REQUEST, commandbuf, size );
				}
				return;

			case PMPG_C_S_MAKEFRIEND_FEEDBACK:
				{
					userid_t uid=CsINVALID_ID;
					bool     ret=false;
					PMSGMakefriendMessage::Server::GetFeedback( (char*)command, uid, ret );

					char buff[_MAX_MESSAGE_BUFF];
					size_t size = PMSGMakefriendMessage::Server::SetFeedback( buff, GetConnectionCId(s), ret );
					SendMessage( uid, CsINVALID_ID, PMPG_S_C_MAKEFRIEND_FEEDBACK, buff, size );
										// make friend with
					if( ret==1 )
					{
						std::vector<userid_t> friendlist;
						Picno::Messager::GetFriendlist( GetDataBase(), uid, friendlist );
						friendlist.push_back( GetConnectionCId(s) );
						Picno::Messager::SetFriendlist( GetDataBase(), uid, friendlist );

						Picno::Messager::GetFriendlist( GetDataBase(), GetConnectionCId(s), friendlist );
						friendlist.push_back( uid );
						Picno::Messager::SetFriendlist( GetDataBase(), GetConnectionCId(s), friendlist );

						// 送自己的暱稱給朋友
			            size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, GetConnectionCId(s),
			                                                                Picno::Messager::GetNickname( GetDataBase(), GetConnectionCId( s ) ).c_str() );
			            SendMessage( uid, CsSERVER_ID, PMPG_S_C_UPDATENICKNAME_REQUEST, buff, size );

						// 送自己的狀態給朋友
						size=PMSGUpdateStateMessage::Server::SetRequest( buff, GetConnectionCId(s),
																			   PMSG_USER_ONLINE );
						SendMessage( uid, CsSERVER_ID, PMSG_S_C_UPDATESTATE_REQUEST, buff, size );

						// 送自己的事件給朋友
						size=PMSGUpdateEventMessage::Server::SetRequest( buff, GetConnectionCId(s),
																			   0 );
						SendMessage( uid, CsSERVER_ID,  PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );


						// 送朋友的暱稱給自己
			            size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, uid,
			                                                                Picno::Messager::GetNickname( GetDataBase(), GetConnectionCId( s ) ).c_str() );
			            SendMessage( GetConnectionCId(s), CsSERVER_ID, PMPG_S_C_UPDATENICKNAME_REQUEST, buff, size );

						// 送朋友的狀態給自己
						size=PMSGUpdateStateMessage::Server::SetRequest( buff, uid,
																			   PMSG_USER_ONLINE );
						SendMessage( GetConnectionCId(s), CsSERVER_ID, PMSG_S_C_UPDATESTATE_REQUEST, buff, size );

						// 送朋友的事件給自己
						size=PMSGUpdateEventMessage::Server::SetRequest( buff, uid,
																			   0 );
						SendMessage( GetConnectionCId(s), CsSERVER_ID,  PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );
					
					}

				}
				return;

			case PMSG_C_S_SHOCKUSER_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					PMSGShockFriendMessage::Server::GetRequest( (char*)command, uid );

					char commandbuf[_MAX_MESSAGE_BUFF];
					size_t size = PMSGShockFriendMessage::Server::SetRequest( commandbuf, GetConnectionCId(s) );
					SendMessage( uid, CsINVALID_ID, PMSG_S_C_SHOCKUSER_REQUEST, commandbuf, size );
				}
				return;
			}
			
			Server::OnRecvRequest( s, cid, command, len );
			return;
		}
	

};