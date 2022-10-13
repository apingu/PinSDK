#ifndef PICNOMESSENGERSERVER_H
#define PICNOMESSENGERSERVER_H

#pragma warning (disable : 4786)
#include <map>
#include "PMSGDef.h"
#include <Cyberspace_server.h>




namespace Picno
{
	class MessengerServer : public Cyberspace::Server
	{
	public:

		MessengerServer();
		~MessengerServer();

		const char* ServerName() const { return "MessengerServer"; }


		void         PublishMessageToConference( ConferenceId_t cid,
			                                     userid_t from,
											     channelid_t mid,
			                                      const char* buff, size_t len );
		
		void         PublishMessageToFriends   ( userid_t from,
			                                     channelid_t cid,
							    			     const char* buff, size_t len );
		    
		    
	    virtual bool OnLogin( ConnectionId_t acc ){ return 1; }
		    
		    
	private:

		XDataBase*          OnCreateDatabase( void );
	    void                OnDeleteDatabase( XDataBase* db );

		virtual bool        DoVerifyJoinConnection( IO::TCPSocket* cs ){ return true; }
		virtual void        DoVerifyExitConnection( IO::TCPSocket* cs ){}		

		virtual void        OnClientConnect( IO::TCPSocket* socket );
		virtual void        OnClientDisconnect( IO::TCPSocket* socket );

		
		void                PublishUserNicknameToFriends( userid_t uid );
		void                PublishUserStateToFriends( userid_t uid );
		void                PublishUserEventToFriends( userid_t uid );
		
	
	    void        OnRecvRequest( IO::TCPSocket* cs, channelid_t cid, const char* command, size_t len );
	};
};

#endif