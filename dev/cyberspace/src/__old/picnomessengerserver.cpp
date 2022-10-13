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
#include "PicnoMessengerServer.h"
#include "PMSGCommand.h"
#include "MessengerDBFuncion.h"
#include "CsArbitPeer.h"
#include "CstIdentity.h"
#include "PicnoMessengerConference.h"
#include <XMySqlDB.h>

		

using namespace Cyberspace;


namespace Picno
{
	    UINT g_ConferenceIdBase = 1;

	    T_Lock g_ConferenceListLock;

		std::map< ConferenceId_t, MessengerConference*  > g_Conferencelist;

		MessengerConference* MessengerCreateConference( ConferenceId_t cid )
		{
			TS_Lock lock( &g_ConferenceListLock );

			std::map< ConferenceId_t, MessengerConference*  >::iterator iter = g_Conferencelist.find( cid );
			if( iter!=g_Conferencelist.end() )
				return NULL;

			MessengerConference* conference = new MessengerConference(cid);
			g_Conferencelist.insert( std::map< ConferenceId_t, MessengerConference* >::value_type( cid, conference ) );
			return conference;
		}

		bool MessengerRemoveConference( ConferenceId_t cid )
		{
			TS_Lock lock( &g_ConferenceListLock );
			std::map< ConferenceId_t, MessengerConference*  >::iterator iter = g_Conferencelist.find( cid );
			if( iter!=g_Conferencelist.end() )
				return false;
			SAFE_DELETE( iter->second );
			g_Conferencelist.erase( cid );
			return true;
		}

		MessengerConference* MessengerGetConference( ConferenceId_t cid )
		{
            TS_Lock lock( &g_ConferenceListLock );
			std::map< ConferenceId_t, MessengerConference*  >::iterator iter = g_Conferencelist.find( cid );
			if( iter!=g_Conferencelist.end() )
				return iter->second;
			return NULL;
		}

		inline int FindFriend( std::vector<userid_t>* fslist, userid_t uid )
		{
			for( UINT i=0; i<fslist->size(); i++ )
			{
				if( (*fslist)[i]==uid )
					return i;
			}
			return -1;
		}


		//===============================================================
		//
		//
		//===============================================================			
		MessengerServer::MessengerServer()
		{

		}


		//===============================================================
		//
		//
		//===============================================================			
		MessengerServer::~MessengerServer()
		{
			
		}

		//===============================================================
		//
		//
		//===============================================================
		XDataBase* MessengerServer::OnCreateDatabase( void )
		{
			return (XDataBase*)new XMySqlDB;
		}
		
		//===============================================================
		//
		//
		//===============================================================
	    void MessengerServer::OnDeleteDatabase( XDataBase* db )
		{
			XMySqlDB* xdb = (XMySqlDB*)db; SAFE_DELETE( xdb );
		}

		//===============================================================
		//
		//
		//===============================================================
		void MessengerServer::OnClientConnect( IO::TCPSocket* socket )
		{
			char buff[_SHORT_BUFFER_SIZE_];
			
			size_t size=0;

			std::vector<userid_t> friendlist;

		    //
			Picno::Messenger::GetFriendlist( GetDataBase(), GetConnectionCId( socket ), friendlist );
			Picno::Messenger::SetState( GetDataBase(), GetConnectionCId( socket ), PMSG_USER_ONLINE );


//			// 送自己的暱稱給自己
//			size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, GetConnectionCId(socket),
//			                                                          Picno::Messenger::GetNickname( GetDataBase(), GetConnectionCId( socket ) ).c_str() );
//			SendMessage( GetConnectionCId( socket ), CsSERVER_ID, PMSG_S_C_UPDATE_MYNICKNAME_REQUEST, buff, size );
//
//			// 送自己的狀態給自己
//			size=PMSGUpdateStateMessage::Server::SetRequest( buff, GetConnectionCId(socket),
//			                                                       Picno::Messenger::GetState( GetDataBase(), GetConnectionCId(socket)) );
//			SendMessage( GetConnectionCId( socket ), CsSERVER_ID,PMSG_S_C_UPDATE_MYSTATE_REQUEST, buff, size );
//
//			// 送自己的事件給自己
//			size=PMSGUpdateEventMessage::Server::SetRequest( buff, GetConnectionCId(socket),
//				                                                   0 );
//			SendMessage( GetConnectionCId( socket ), CsSERVER_ID,PMSG_S_C_UPDATE_MYEVENT_REQUEST, buff, size );


			// 送朋友的資料給自己
			for( UINT i=0; i< friendlist.size(); i++ )
			{
				if( friendlist[i]!=0 )
				{
					std::string str=Picno::Messenger::GetNickname( GetDataBase(), friendlist[i] );
					if( str.empty() )
					{
						friendlist[i]=0;
						continue;
					}

					size=PMSGEnumFriendIdMessage::Server::SetRequest( buff, friendlist[i] );
					SendMessage( socket, CsSERVER_ID, PMSG_S_C_ENUMFRIENDID_REQUEST, buff, size );

					size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, friendlist[i],
																		str.c_str() );
					SendMessage( socket, CsSERVER_ID, PMSG_S_C_UPDATENICKNAME_REQUEST, buff, size );

					size=PMSGUpdateStateMessage::Server::SetRequest( buff, friendlist[i],
						                                             Picno::Messenger::GetState( GetDataBase(), friendlist[i] ) );

					SendMessage( socket, CsSERVER_ID, PMSG_S_C_UPDATESTATE_REQUEST, buff, size );

					size=PMSGUpdateEventMessage::Server::SetRequest( buff, friendlist[i],
																	 0 );

					SendMessage( socket, CsSERVER_ID, PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );
				}
			}

			Picno::Messenger::SetFriendlist( GetDataBase(), GetConnectionCId( socket ), &friendlist );


			// 送自己的ＩＤ給朋友
			size=PMSGEnumFriendIdMessage::Server::SetRequest( buff, GetConnectionCId(socket) );
			PublishMessageToFriends( GetConnectionCId( socket ), PMSG_S_C_ENUMFRIENDID_REQUEST, buff, size );

			// 送自己的暱稱給朋友包含自己
			PublishUserNicknameToFriends( GetConnectionCId(socket) );
//			size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, GetConnectionCId(socket),
//			                                                          Picno::Messenger::GetNickname( GetDataBase(), GetConnectionCId( socket ) ).c_str() );
//			PublishMessageToFriends( GetConnectionCId( socket ), PMSG_S_C_UPDATENICKNAME_REQUEST, buff, size );

			// 送自己的狀態給朋友
			PublishUserStateToFriends( GetConnectionCId(socket) );
//			size=PMSGUpdateStateMessage::Server::SetRequest( buff, GetConnectionCId(socket),
//			                                                       Picno::Messenger::GetState( GetDataBase(), GetConnectionCId(socket)) );
//			PublishMessageToFriends( GetConnectionCId( socket ), PMSG_S_C_UPDATESTATE_REQUEST, buff, size );

			// 送自己的事件給朋友
			PublishUserEventToFriends( GetConnectionCId(socket) );
//			size=PMSGUpdateEventMessage::Server::SetRequest( buff, GetConnectionCId(socket),
//			                                                       0 );
//			PublishMessageToFriends( GetConnectionCId( socket ), PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );
			//
		}

		//===============================================================
		//
		//
		//===============================================================
		void MessengerServer::OnClientDisconnect( IO::TCPSocket* socket )
		{
			Picno::Messenger::SetState( GetDataBase(), GetConnectionCId( socket ), PMSG_USER_OFFLINE );

			char buff[_SHORT_BUFFER_SIZE_];
			size_t size=PMSGLogoutMessage::Server::SetRequest( buff, GetConnectionCId( socket ) );
			
			// remove from all conference

		    g_ConferenceListLock.Lock();
			std::vector< ConferenceId_t > eraselist;

			std::map< ConferenceId_t, MessengerConference* >::iterator iter = g_Conferencelist.begin();
			while( iter!=g_Conferencelist.end() )
			{
				iter->second->ExitParticipant( GetConnectionCId( socket ) );
				if( iter->second->Empty() )
				{
					eraselist.push_back( iter->first );
				}
				iter++;
			}
			g_ConferenceListLock.UnLock();


			for( UINT i=0; i<eraselist.size(); i++ )
			{
				MessengerRemoveConference( eraselist[i] );
				//Picno::Messenger::RemoveConference( GetDataBase(), eraselist[i] );
			}


			// for all friend
			PublishMessageToFriends( GetConnectionCId(socket), PMSG_S_C_LOGOUT_REQUEST, buff, size );
		}


		//===============================================================
		//
		//
		//===============================================================
		void MessengerServer::PublishMessageToConference( ConferenceId_t cid, userid_t from, channelid_t mid, const char* buff, size_t len )
		{
			MessengerConference* conf = MessengerGetConference( cid );
			if( conf==NULL )
			{
				assert ( 0&&"Send message conference was a NULL");
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
		void MessengerServer::PublishMessageToFriends( userid_t from, channelid_t cid, const char* buff, size_t len )
		{
			std::vector<userid_t> friendlist;

		    //
			Picno::Messenger::GetFriendlist( GetDataBase(), from, friendlist );

			for( UINT i=0; i< friendlist.size(); i++ )
			{
				SendMessage( friendlist[i], from, cid, buff, len );
			}
		}


		//===============================================================
		//
		//
		//===============================================================
		void MessengerServer::PublishUserNicknameToFriends( userid_t uid )
		{
			char buff[512];

			size_t size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, uid,
			                                                           Picno::Messenger::GetNickname( GetDataBase(), uid ).c_str() );
			PublishMessageToFriends( uid, PMSG_S_C_UPDATENICKNAME_REQUEST, buff, size );
			SendMessage( uid, CsSERVER_ID, PMSG_S_C_UPDATE_MYNICKNAME_REQUEST, buff, size );
			
		}


		//===============================================================
		//
		//
		//===============================================================
		void MessengerServer::PublishUserStateToFriends( userid_t uid )
		{
			char buff[512];

    		// 送自己的狀態給朋友
			size_t size=PMSGUpdateStateMessage::Server::SetRequest( buff, uid,
			                                                        Picno::Messenger::GetState( GetDataBase(), uid ) );
			PublishMessageToFriends( uid, PMSG_S_C_UPDATESTATE_REQUEST, buff, size );
			SendMessage( uid, CsSERVER_ID, PMSG_S_C_UPDATE_MYSTATE_REQUEST, buff, size );
		}

		//===============================================================
		//
		//
		//===============================================================
		void MessengerServer::PublishUserEventToFriends( userid_t uid )
		{
			char buff[512];
			size_t size=PMSGUpdateEventMessage::Server::SetRequest( buff,uid,
			                                                       0 );
			PublishMessageToFriends( uid, PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );			
			SendMessage( uid, CsSERVER_ID, PMSG_S_C_UPDATE_MYEVENT_REQUEST, buff, size );
		}


		//===============================================================
		//
		//
		//===============================================================
		void SendConferenceUserNameToPlayer( userid_t uid, void* pdata )
		{
			std::vector<userid_t>* ulist=(std::vector<userid_t>*)pdata;
			ulist->push_back( uid );
		}


        void MessengerServer::OnRecvRequest( IO::TCPSocket* s, channelid_t cid, const char* command, size_t len )
		{
			switch( cid )
			{
 
			case PMSG_C_S_LOGOUT_REQUEST:
				return;

			case PMSG_C_S_UPDATE_MYNICKNAME_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					char nickname[512];
					PMSGUpdateNicknameMessage::Server::GetRequest( (char*)command, uid, nickname );

					Picno::Messenger::SetNickname( GetDataBase(), uid, nickname );
					printf( "%d update nickname: %s\n", uid, nickname );
					// TODO: Pin 要給大家知道
					PublishUserNicknameToFriends( uid );
					
				}
				return;
				
			case PMSG_C_S_UPDATE_MYSTATE_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					short    state=0;

					PMSGUpdateStateMessage::Server::GetRequest( (char*)command, uid, state );

					Picno::Messenger::SetState( GetDataBase(), uid, state );
					printf( "%d update state: %d\n", uid, state );
					// TODO: Pin 要給大家知道
					PublishUserStateToFriends( uid );
				}
				return;

		    // 照片的更新
			case PMSG_C_S_UPDATE_MYPICTURE_REQUEST:
				{
					void*  pic=NULL;
					size_t len=0;
					userid_t uid=CsINVALID_ID;

					PMSGUpdatePictureMessage::Server::GetRequest( (char*)command, uid, pic, len );

					printf( "%d update picutre\n", uid );
									
					// TODO: Pin
					// 先存到資料庫就好
					// 圖形檔案太大不要主動更新
					Picno::Messenger::SetPicture( GetDataBase(), uid, pic, len );
				}
				return;

			case PMSG_C_S_GET_USERPICTURE_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					PMSGGetPictureMessage::Server::GetRequest( (char*)command, uid );
					DBResBuf resbuf = Picno::Messenger::GetPicture( GetDataBase(), uid );

					char* sendbuff=new char[resbuf.size()+512];

					size_t len=PMSGGetPictureMessage::Server::SetFeedback( sendbuff, uid, &(resbuf[0]), resbuf.size() );
					SendMessage( GetConnectionCId(s), CsINVALID_ID, PMSG_S_C_UPDATE_PICTURE_REQUEST, sendbuff, len );
					SAFE_DELETE_ARRAY( sendbuff );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// Chat
			case PMSG_C_S_CREATE_CONFERENCEWITH_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					PMSGChatCreateConferenceWith::Server::GetRequest( (char*)command, uid );


					printf( "Request create conference %d => %d....", GetConnectionCId(s), uid );

					PMSG_CONFERENCE_STATE result=PMSG_CONFERENCE_SUCCESS;

					if( GetAccountState( uid )!=ACCSTATE_ONLINE ) 
					{
                        cid=0;
						uid=0;
						result=PMSG_CONFERENCE_OFFLINE;
						printf( "create conference Id: %d user %d offline....failure", cid );
						return;
					}

					// Create conference
					ConferenceId_t cid = g_ConferenceIdBase;//Picno::Messenger::CreateConference( GetDataBase(), GetConnectionCId(s) );
					g_ConferenceIdBase++;
					if( cid==0 )
					{
						assert( 0&&"can't create as null" );
						return;
					}
					MessengerConference* conference = MessengerCreateConference( cid );

					

					if( !conference)
					{
						cid=0;
						uid=0;
						assert( 0||"Create conference falilure!!" );
						result=PMSG_CONFERENCE_FAILURE;
						printf( "create conference Id: %d ....failure", cid );
					}
					else
					{
						conference->JoinParticipant( GetConnectionCId(s) );
					    conference->JoinParticipant( uid );
						printf( "create conference Id: %d....successful", cid );
					}
				
					// send conference id feedback
					char commandbuf[_SHORT_BUFFER_SIZE_];
					size_t size = PMSGChatCreateConferenceWith::Server::SetFeedback( commandbuf, cid, uid, (short)result );
					SendMessage( GetConnectionCId(s), CsINVALID_ID, PMSG_S_C_CREATE_CONFERENCEWITH_FEEDBACK, commandbuf, size );

					size = PMSGChatCreateConferenceWith::Server::SetFeedback( commandbuf, cid, GetConnectionCId(s), (short)result );
					SendMessage( uid, CsINVALID_ID, PMSG_S_C_CREATE_CONFERENCEWITH_FEEDBACK, commandbuf, size );


					// TODO: Pin
					// send picture....
				}
				return;

			// 當要邀請 with 加入 會議 cid
			case PMSG_C_S_INVITE_CONFERENCE_REQUEST:
				{
					ConferenceId_t cid=0;
					userid_t       with=CsINVALID_ID;
					PMSGChatInviteConference::Server::GetRequest( (char*)command, cid, with );

					printf( "Request invite conference: %d => user: %d\n", cid, with );

					char commandbuf[_SHORT_BUFFER_SIZE_];
					size_t size = PMSGChatInviteConference::Server::SetRequest( commandbuf, cid, GetConnectionCId(s) );
					SendMessage( with, CsINVALID_ID, PMSG_S_C_INVITE_CONFERENCE_REQUEST, commandbuf, size );
				}
				return;

			// 當被邀請的人回覆的時候
			case PMSG_C_S_INVITE_CONFERENCE_FEEDBACK:
				{
					ConferenceId_t cid=0;
					userid_t       with=CsINVALID_ID;
					short          reply=0;
					PMSGChatInviteConference::Server::GetFeedback( (char*)command, cid, with, reply );
					
					printf( "Feedback invite conference: %d => user: %d reply=%d\n", cid, with, reply );

					char commandbuf[_SHORT_BUFFER_SIZE_];
					size_t size = 0;
					
					if( reply )
					{
						// 先送建立一個會議給自己
						size = PMSGChatCreateConferenceWith::Server::SetFeedback( commandbuf, cid, with, PMSG_CONFERENCE_SUCCESS );
						SendMessage( GetConnectionCId(s), CsINVALID_ID, PMSG_S_C_CREATE_CONFERENCEWITH_FEEDBACK, commandbuf, size );

						MessengerConference* conf = MessengerGetConference( cid );
					    if( conf==NULL )
						{
						    assert ( 0||"Invite conference was a NULL");
						}
						// 把這個會議裡面的人名都給準備要加入的那個傢伙
						std::vector<userid_t> ulist;
						conf->ForAllUserId( SendConferenceUserNameToPlayer, &ulist );
						for( UINT i=0; i<ulist.size(); i++ )
						{
						    size = PMSGChatInviteConference::Server::SetFeedback( commandbuf, cid,
							                                                      ulist[i],
							                                                      Picno::Messenger::GetNickname( GetDataBase(), ulist[i] ).c_str(),
																			      PMSG_CONFERENCE_SUCCESS );
							SendMessage( GetConnectionCId(s), CsINVALID_ID, PMSG_S_C_INVITE_CONFERENCE_FEEDBACK, commandbuf, size );
						}




						// 如果同意告訴這個會議裡面所有的人同意
						size = PMSGChatInviteConference::Server::SetFeedback( commandbuf, cid,
							                                                  GetConnectionCId(s),
							                                                  Picno::Messenger::GetNickname( GetDataBase(), GetConnectionCId(s) ).c_str(),
																			  PMSG_CONFERENCE_SUCCESS );
						PublishMessageToConference( cid, GetConnectionCId(s), PMSG_S_C_INVITE_CONFERENCE_FEEDBACK, commandbuf, size );

						conf->JoinParticipant( GetConnectionCId(s) );

					}
					else
					{
						//如果不同意告訴只告訴邀請的人
						size = PMSGChatInviteConference::Server::SetFeedback( commandbuf, cid,
							                                                  GetConnectionCId(s),
																			  Picno::Messenger::GetNickname( GetDataBase(), GetConnectionCId(s) ).c_str(),
																			  reply );
						SendMessage( with, CsINVALID_ID, PMSG_S_C_INVITE_CONFERENCE_FEEDBACK, commandbuf, size );
					}

				}
				return;

			case PMSG_C_S_LEAVE_CONFERENCE_REQUEST:
				{
					ConferenceId_t cid=0;
					PMSGChatLeaveConference::Server::GetRequest( (char*)command, cid );

					printf( "Request leave conference: %d => user: %d\n", cid, GetConnectionCId(s));

					MessengerConference* conf = MessengerGetConference( cid );
					if( conf!=NULL )
					{
						conf->ExitParticipant( GetConnectionCId(s) );
						// for all participant
			            char commandbuf[_SHORT_BUFFER_SIZE_];
				        size_t size=PMSGChatLeaveConference::Server::SetRequest( commandbuf, cid, GetConnectionCId(s) );
					    PublishMessageToConference( cid, GetConnectionCId(s), PMSG_S_C_LEAVE_CONFERENCE_REQUEST, commandbuf, size );
					
					    if( conf->Empty() )
						{
						    MessengerRemoveConference( cid );
						    //Picno::Messenger::RemoveConference( GetDataBase(), cid );
						}
					}
					else
				        assert ( 0&&"Leave conference was a NULL");

				}
				return;

			case PMSG_C_S_CONFERENCEMESSAGE_REQUEST:
				{
					ConferenceId_t cid=0;
					char  msg[_SHORT_BUFFER_SIZE_];
					PMSGChatSendMessage::Server::GetRequest( (char*)command, cid, msg  );

					// for all chat user
					char commandbuf[_SHORT_BUFFER_SIZE_];
					size_t size = PMSGChatSendMessage::Server::SetRequest( commandbuf, cid, GetConnectionCId(s), msg );
					PublishMessageToConference( cid, GetConnectionCId(s), PMSG_S_C_CONFERENCEMESSAGE_REQUEST, commandbuf, size );
				}
				return;

			///////////////////////////////////////////////////////////////////////////
			// Friend 
			case PMPG_C_S_MAKEFRIEND_REQUEST:
				{
					userid_t uid=CsINVALID_ID;
					PMSGMakefriendMessage::Server::GetRequest( (char*)command, uid );

					std::vector< userid_t > friendlist;
					Picno::Messenger::GetFriendlist( GetDataBase(), GetConnectionCId(s), friendlist );

					printf( "Request make friend %d => %d\n", GetConnectionCId(s), uid );

					// already in friendlist
					if( FindFriend( &friendlist, uid )!=-1 )
					{
					    char buff[_SHORT_BUFFER_SIZE_];
						size_t size = PMSGMakefriendMessage::Server::SetFeedback( buff, Picno::Messenger::GetNickname( GetDataBase(), uid ).c_str(),
							                                                            uid,
																						PMSG_MF_EXIST );
					    SendMessage( GetConnectionCId(s), CsINVALID_ID, PMPG_S_C_MAKEFRIEND_FEEDBACK, buff, size );
					}
					else
					{
					    char commandbuf[_SHORT_BUFFER_SIZE_];
					    size_t size = PMSGMakefriendMessage::Server::SetRequest( commandbuf, Picno::Messenger::GetNickname( GetDataBase(), GetConnectionCId(s) ).c_str(),
							                                                                 GetConnectionCId(s) );
					    SendMessage( uid, CsINVALID_ID, PMPG_S_C_MAKEFRIEND_REQUEST, commandbuf, size );
					}
				}
				return;

			case PMPG_C_S_MAKEFRIEND_FEEDBACK:
				{
					userid_t uid=CsINVALID_ID;
					short     ret=false;
					PMSGMakefriendMessage::Server::GetFeedback( (char*)command, uid, ret );

					printf( "Feedback make friend %d => %d result => %d\n", GetConnectionCId(s), uid, ret );

					char buff[_SHORT_BUFFER_SIZE_];
					if( ret )
					{
						ret=PMSG_MF_SUCCESS;
					}
					else
					{
						ret=PMSG_MF_FAILURE;
					}
					
				    size_t size = PMSGMakefriendMessage::Server::SetFeedback( buff, Picno::Messenger::GetNickname( GetDataBase(), GetConnectionCId(s) ).c_str(),
						                                                            GetConnectionCId(s),
																					ret );
				    SendMessage( uid, CsINVALID_ID, PMPG_S_C_MAKEFRIEND_FEEDBACK, buff, size );

										// make friend with
					if( ret==PMSG_MF_SUCCESS )
					{
						std::vector<userid_t> friendlist;
						Picno::Messenger::GetFriendlist( GetDataBase(), uid, friendlist );
						friendlist.push_back( GetConnectionCId(s) );
						Picno::Messenger::SetFriendlist( GetDataBase(), uid, &friendlist );

						Picno::Messenger::GetFriendlist( GetDataBase(), GetConnectionCId(s), friendlist );
						friendlist.push_back( uid );
						Picno::Messenger::SetFriendlist( GetDataBase(), GetConnectionCId(s), &friendlist );

						// 送自己的ID給朋友
						size=PMSGEnumFriendIdMessage::Server::SetRequest( buff, GetConnectionCId(s) );
					    SendMessage( uid, CsSERVER_ID, PMSG_S_C_ENUMFRIENDID_REQUEST, buff, size );


						// 送自己的暱稱給朋友
						PublishUserNicknameToFriends( GetConnectionCId(s) );
//			            size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, GetConnectionCId(s),
//			                                                                Picno::Messenger::GetNickname( GetDataBase(), GetConnectionCId( s ) ).c_str() );
//			            SendMessage( uid, CsSERVER_ID, PMSG_S_C_UPDATENICKNAME_REQUEST, buff, size );

						// 送自己的狀態給朋友
						PublishUserStateToFriends( GetConnectionCId(s) );
//						size=PMSGUpdateStateMessage::Server::SetRequest( buff, GetConnectionCId(s),
//																			   PMSG_USER_ONLINE );
//						SendMessage( uid, CsSERVER_ID, PMSG_S_C_UPDATESTATE_REQUEST, buff, size );

						// 送自己的事件給朋友
						PublishUserEventToFriends( GetConnectionCId(s) );
//						size=PMSGUpdateEventMessage::Server::SetRequest( buff, GetConnectionCId(s),
//																			   0 );
//						SendMessage( uid, CsSERVER_ID,  PMSG_S_C_UPDATEEVENT_REQUEST, buff, size );


						// 送朋友的ID給自己
						size=PMSGEnumFriendIdMessage::Server::SetRequest( buff, uid );
					    SendMessage( GetConnectionCId(s), CsSERVER_ID, PMSG_S_C_ENUMFRIENDID_REQUEST, buff, size );


						// 送朋友的暱稱給自己
			            size=PMSGUpdateNicknameMessage::Server::SetRequest( buff, uid,
			                                                                Picno::Messenger::GetNickname( GetDataBase(), uid ).c_str() );
			            SendMessage( GetConnectionCId(s), CsSERVER_ID, PMSG_S_C_UPDATENICKNAME_REQUEST, buff, size );

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

			case PMPG_C_S_DELETEFRIEND_REQUEST:
				{
					userid_t uid=0;
					PMSGMakefriendMessage::Server::GetRequest( (char*)command, uid );

					printf( "Request delete friend %d => %d\n", GetConnectionCId(s), uid );

					std::vector<userid_t> friendlist;
					Picno::Messenger::GetFriendlist( GetDataBase(), GetConnectionCId(s), friendlist );
					for( UINT i=0; i<friendlist.size(); i++ )
					{
						if( friendlist[i]==uid)
						{
							friendlist.erase( friendlist.begin()+i );
							break;
						}
					}
					Picno::Messenger::SetFriendlist( GetDataBase(), GetConnectionCId(s), &friendlist );
				}
				return;

			case PMSG_C_S_SHOCKUSER_REQUEST:
				{
					ConnectionId_t cid=CsINVALID_ID;
					userid_t       uid=CsINVALID_ID;
					PMSGShockFriendMessage::Server::GetRequest( (char*)command, cid, uid );

					printf( "Request shock friend %d => %d On Conference %d\n", GetConnectionCId(s), uid, cid );

					char commandbuf[_SHORT_BUFFER_SIZE_];
					size_t size = PMSGShockFriendMessage::Server::SetRequest( commandbuf, cid, GetConnectionCId(s) );
					SendMessage( uid, CsINVALID_ID, PMSG_S_C_SHOCKUSER_REQUEST, commandbuf, size );
				}
				return;
			case PMSG_C_S_CREATE_USER:
				{
					short ret=0;
					char username[_MAX_MSG];
					char password[_MAX_MSG];
					PMSGCreateUserMessage::Server::GetRequest( (char*)command, username, password );


					char query[2048];

					sprintf( query, "SELECT AccountId FROM account WHERE Username = '%s'", username );

				    XDBResult result = GetAccountDB()->ExecQuery( query );
					GetAccountDB()->FreeResult( result );

					if( !GetAccountDB()->GetXDBResultEmpty( result ) )
					{
						ret=1;
						sprintf( query, "INSERT INTO account( Username, Password ) VALUES( '%s', '%s' ) ", username, username  );

						XDBResult result0 = GetAccountDB()->ExecQuery( query );
						GetAccountDB()->FreeResult( result0 );

						int cid = GetAccountCId( username );

						UINT friends[100];
						memset( friends, 0, sizeof(UINT)*100 );
						UINT buff[200];
						size_t buflen = MakeResToString( (char*)buff, 800, (char*)friends, 400 );
						sprintf( query, "INSERT INTO participator( UserId, Nickname, Friends ) VALUES( '%d', '%s', '", cid, username );
						char* ptr=query;
						ptr+=strlen( query );
						memcpy( ptr, buff, buflen );
						ptr+=buflen;
						sprintf( ptr, "')" );

						XDBResult result1 = GetDataBase()->ExecQuery( query );
						GetDataBase()->FreeResult( result1 );
					}
				
					size_t size=PMSGCreateUserMessage::Server::SetFeedback( (char*)command, username, ret );
					SendMessage( s, CsINVALID_ID, PMSG_S_C_CREATE_USER, command, size );
				}
				return;

			case PMSG_C_S_DELETE_USER:
				{
					short ret=0;
					char username[_MAX_MSG];
					char password[_MAX_MSG];
					PMSGCreateUserMessage::Server::GetRequest( (char*)command, username, password );


					char query[512];

					sprintf( query, "SELECT AccountId FROM account WHERE Username = '%s'", username );

				    XDBResult result = GetAccountDB()->ExecQuery( query );
					GetAccountDB()->FreeResult( result );

					if( !GetAccountDB()->GetXDBResultEmpty( result ) )
					{
						ret=1;
						ConnectionId_t accid = atoi( GetAccountDB()->GetQueryResult( result, 0, 0).c_str() );
						
						sprintf( query, "DELETE FROM account WHERE AccountId = '%d'",  accid );
						XDBResult result2 = GetAccountDB()->ExecQuery( query );
						GetAccountDB()->FreeResult( result2 );

						sprintf( query, "DELETE FROM participator WHERE UserId = '%d'", accid );
						XDBResult result3 = GetDataBase()->ExecQuery( query );
						GetDataBase()->FreeResult( result3 );
					}

					size_t size=PMSGCreateUserMessage::Server::SetFeedback( (char*)command, username, ret );
					SendMessage( s, CsINVALID_ID, PMSG_S_C_DELETE_USER, command, size );

				}
				return;

			}
			
			Server::OnRecvRequest( s, cid, command, len );
			return;
		}
	

};