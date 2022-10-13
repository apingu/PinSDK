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
#ifndef PICCOMESSENGERCLIENT_H
#define PICNOMESSENGERCLIENT_H

#pragma warning (disable : 4786)
#include <map>
#include "PMSGDef.h"
#include "Cyberspace_client.h"
//#include "PicnoMessengerConference.h"


namespace Picno
{

		class MessengerClient : public Cyberspace::Client
		{
			CSERR_CODE                     m_LoginResult;
			std::string                    m_MyNickname;
			short                          m_MyState;
			short                          m_MyEvent;
			std::string                    m_MyPicture;
			std::vector< ConferenceId_t >  m_ConferenceList;
			std::string                    m_Username;

		public:

			MessengerClient();
			~MessengerClient();


			// login return eid;
			// else return 0;
			CSERR_CODE    GetState( void );

			//bool          Connect( char* host, unsigned short port );

			// you will get a current id after login successful
			userid_t      GetUserId( void );
			const char*   GetLoginname( void ) { return m_Username.c_str(); }
			const char*   GetMyNickname( void ){ return m_MyNickname.c_str(); }


			void          CreateAccount( const char* username, const char* password );
			void          DeleteAccount( const char* username );

			void          Login( const char* username, const char* password );
			void          Logout( void );


			// when someone logout			
			virtual void  OnLogout( userid_t uid ){};

			virtual void  OnEnumFriendId( userid_t uid ){}



			// =>OnUpdateNickname, =>OnUpdateMynickname;
			void          SendGetMyPicture( void );
			void          SendGetFriendPicture( userid_t uid );
			bool          SendUpdateNickname( const char* nickname );
			//
			void          SendUpdatePicture( const char* filename );

			void          SendUpdateState( short state );


			virtual void  OnUpdateMyNickname( userid_t myid, const char* nickname ){}
			virtual void  OnUpdateMyState( userid_t myid, short state ){}
			virtual void  OnUpdateMyEvent( userid_t myid, short event ){}
			virtual void  OnUpdateMyPicture( userid_t myid, BYTE* image, size_t len ){}

			// callback function when friend change his/her nickname
			virtual void  OnUpdateNickname( userid_t friendid, const char* nickname ){};
			virtual void  OnUpdateState( userid_t friendid, short state ){};
			virtual void  OnUpdateEvent( userid_t friendid, short event ){};





			///////////////////////////////////////////////////////////////////////////
			//

			// send shock message to other
			void           SendShockToUser( ConferenceId_t cid, userid_t user );
			// callback when shock from other
			virtual void   OnShockFromUser( ConferenceId_t cid, userid_t user ){}

			
			///////////////////////////////////////////////////////////////////////////
			// char room

			// create one chat room with one entity
			void           CreateConferenceWith( userid_t user );

			// 當有人要建立一個 會議( 有可能是自己 )
			virtual void   OnCreateConferenceWith( ConferenceId_t cid, userid_t user, PMSG_CONFERENCE_STATE result ){}

			// delete one chat room with one entity
			void           LeaveConference( ConferenceId_t eid );

			// 當有人要離開一個 會議( 有可能是自己 )
			virtual void   OnChatLeaveConference( ConferenceId_t cid, userid_t user ){}

	
			
			// invite one user to your chatroom
			void           SendInviteConference( ConferenceId_t roomnum, userid_t user );
			// 當有人要加入會議
		    // 因為有可能會不是朋友所以連 暱稱一起送下來
			virtual void   OnChatInvitedReply( ConferenceId_t roomnum, userid_t user, const char* nickname, PMSG_CONFERENCE_STATE reply ){}


			// Accept  return 1
			// Decline return 0
			// 當被邀請加入一個會議
			virtual void   OnChatInvitedByFriend( ConferenceId_t roomnum, userid_t user ){ return; }
			// 回覆願不願意加入會議
			void           SendFeedbackChatInvitedByFriend( ConferenceId_t roomnum, userid_t user, PMSG_CONFERENCE_STATE reply );


			// callback function when friend change his/her picture
			// online call when click friend conference
			virtual void   OnUpdateFriendPicture( userid_t friendid, BYTE* image, size_t len ){}



			// send message to a conference
			void           SendConferenceMessage( ConferenceId_t roomnum,
				                                  const char* msg );

			virtual void   OnRecvConferenceMessage( ConferenceId_t roomnum,
				                                    userid_t from,
				                                    const char* msg ){}


			///////////////////////////////////////////////////////////////////////////
			// friend list

			// send request for someone ask tobe friend by friend name
			void           SendRequestMakeFriend( userid_t with );
			void           SendFeedbackMakeFriend( userid_t from, bool ret );


		    // callback when SendRequestAddnewFriend, the friend that want add will call this function
			// return true for accept or false for decline
			virtual void   OnRequestMakeFriendWith( userid_t from, const char* nickname ){};


			// callback when OnRequestMakeFriendWith,  successuf or not
			virtual void   OnFeedbackAddnewFriend( userid_t with, const char* nickname, int result ){}


			// send request for delete friend, no anyfeedback, it must successful
			void           SendRequestDeleteFriend( userid_t uid );


			//void           SendEnumAllFriends( void );
			// callback function when login and get all friend list from server
			//virtual void   OnEnumFriend( userid_t friendid, const char* nickname, short state ){}

			// callback function when one friend online
			//void           OnFriendOnline( userid_t friendid, const char* nickname ){};

			// callback function when one friend offline
			//void           OnFriendOffline( userid_t friendid, const char* nickname ){};

			bool           ConnectMessengerServer( void );


		private:

		    void OnGetHostInfo( ipa_t host, u_short port,
			                    const char* hostname, 
								const char* servername, 
							    const char* functionname, 
							    u_int maxconnect, u_int usedconnect );

		    void  OnRecvDefineRequest ( connectionid_t cid, uniqueid_t who, channelid_t chid, const char* command, size_t len );

			//static void  LoginFeedbackCB( long who, void* pData, unsigned int len );


			T_Lock  m_ConferenceLock;


		};

};




#endif  //PICNOMessengerCLIENT_H