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
#ifndef PICNOMESSAGERCLIENT_H
#define PICNOMESSAGERCLIENT_H

#pragma warning (disable : 4786)
#include <map>
#include <PMSGDef.h>
#include <Cyberspace_client.h>
#include <PicnoMessagerConference.h>


namespace Picno
{

		class MessagerClient : public Cyberspace::Client
		{
			short m_LoginResult;
			std::map< ConferenceId_t, MessagerConference* >  m_ConferenceList;

		public:

			MessagerClient();
			~MessagerClient();

			//void CreateAccount( const char* username, const char* password );
			//void DeleteAccount( const char* username );
			void          Login( const char* username, const char* password );
			void          Logout( void );
			
			// when someone logout			
			virtual void  OnLogout( userid_t uid ){};

			virtual void  OnEnumFriendId( userid_t uid ){}

			virtual void  OnUpdateState( userid_t friendid, short state ){};
			virtual void  OnUpdateEvent( userid_t friendid, short event ){};
			
			// login return eid;
			// else return 0;
			short         GetState( void );

			// you will get a current id after login successful
			userid_t      GetUserId( void );

			// 
			bool          SendUpdateNickname( const char* nickname );

			// callback function when friend change his/her nickname
			virtual void  OnUpdateNickname( userid_t friendid, const char* nickname ){};


			//
			void          SendUpdatePicture( const char* filename );



			///////////////////////////////////////////////////////////////////////////
			//

			// send shock message to other
			void           SendShockToUser( userid_t user );
			// callback when shock from other
			virtual void   OnShockFromUser( userid_t user ){}

			
			///////////////////////////////////////////////////////////////////////////
			// char room

			// create one chat room with one entity
			ConferenceId_t CreateConference( void );

			// delete one chat room with one entity
			void           LeaveConference( ConferenceId_t eid );
			virtual void   OnChatLeaveConference( ConferenceId_t cid, userid_t user ){}

		
			
			// invite one user to your chatroom
			void           InviteConference( ConferenceId_t roomnum, userid_t user );

			// Accept  return 1
			// Decline return 0
			virtual short  OnChatInvitedByFriend( ConferenceId_t roomnum, userid_t user ){ return 0; }
			virtual short  OnChatInvitedReply( ConferenceId_t roomnum, userid_t user, short reply ){ return 0; }

			// callback function when friend change his/her picture
			// online call when click friend conference
			virtual void   OnUpdateFriendPicture( userid_t friendid, BYTE* image ){};



			// send message to a conference
			void           SendConferenceMessage( ConferenceId_t roomnum, const char* msg );
			virtual void   OnRecvConferenceMessage( ConferenceId_t roomnum, userid_t from, const char* msg ){}


			///////////////////////////////////////////////////////////////////////////
			// friend list

			// send request for someone ask tobe friend by friend name
			void           SendRequestMakeFriend( userid_t with );


		    // callback when SendRequestAddnewFriend, the friend that want add will call this function
			// return true for accept or false for decline
			virtual bool   OnRequestMakeFriendWith( userid_t from ){ return false; };


			// callback when OnRequestMakeFriendWith,  successuf or not
			virtual void   OnFeedbackAddnewFriend( userid_t with, bool result ){}


			// send request for delete friend, no anyfeedback, it must successful
			void           SendRequestDeleteFriend( const char* name ){}


			//void           SendEnumAllFriends( void );
			// callback function when login and get all friend list from server
			//virtual void   OnEnumFriend( userid_t friendid, const char* nickname, short state ){}

			// callback function when one friend online
			//void           OnFriendOnline( userid_t friendid, const char* nickname ){};

			// callback function when one friend offline
			//void           OnFriendOffline( userid_t friendid, const char* nickname ){};

			bool           ConnectMessagerServer( void );


		private:

		    void OnGetHostInfo( ipa_t host, u_short port,
			                    const char* hostname, 
							    const char* servicename, 
							    u_int maxconnect, u_int usedconnect );

			void OnRecvRequest( IO::TCPSocket* cs, ChannelId_t cid, const char* command, size_t len );

			//static void  LoginFeedbackCB( long who, void* pData, unsigned int len );


			ConferenceId_t m_CreateCId;
			T_Lock  m_ConferenceLock;


		};

};




#endif  //PICNOMESSAGERCLIENT_H