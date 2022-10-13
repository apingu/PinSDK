#ifndef PICNOMESSAGERSERVER_H
#define PICNOMESSAGERSERVER_H

#pragma warning (disable : 4786)
#include <map>
#include "PMSGDef.h"
#include <Cyberspace_server.h>
#include <XMySqlDB.h>



namespace Picno
{
	class MessagerServer : public Cyberspace::Server
	{
	public:

		MessagerServer();
		~MessagerServer();

		const char* ServerName() const { return "MessagerServer"; }


		void         SendMessageToConference( ConferenceId_t cid,
			                                  userid_t from,
											  ChannelId_t mid,
			                                  const char* buff, size_t len );
		
		void         SendMessageToFriends   ( userid_t from,
			                                  ChannelId_t cid,
											  const char* buff, size_t len );
		    
		    
	    virtual bool OnLogin( ConnectionId_t acc ){ return 1; }
		    
		    
	private:

		XDataBase*          OnCreateDatabase( void ) { return (XDataBase*)new XMySqlDB;	}
	    void                OnDeleteDatabase( XDataBase* db ) { XMySqlDB* xdb = (XMySqlDB*)db; SAFE_DELETE( xdb );	}

		virtual bool        DoVerifyJoinConnection( IO::TCPSocket* cs ){ return true; }
		virtual void        DoVerifyExitConnection( IO::TCPSocket* cs ){}		

		virtual void        OnClientConnect( IO::TCPSocket* socket );
		virtual void        OnClientDisconnect( IO::TCPSocket* socket );
		
	
	    void        OnRecvRequest( IO::TCPSocket* cs, ChannelId_t cid, const char* command, size_t len );
	};
};

#endif