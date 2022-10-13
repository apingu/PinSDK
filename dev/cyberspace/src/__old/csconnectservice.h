// ServBase.h
// Written by Wood Woo, Mar 26, 2004
// version 1.0

#ifndef _LOGIN_SERVICE_H_
#define _LOGIN_SERVICE_H_

#include "CSSERVICEBASE.H"
#include "IOSocket.h"
#include <map>


namespace Cyberspace {

	class ConnectService : public ServiceBase
	{
		map<int,IO::TCPSocket*> m_socket;
	protected:
	
	public:
		void PlayerLogin(int player_id,IO::TCPSocket *socket);

		void SendCsCmdFromID(int player_id, void *packet);
	
	};	// ServiceBase


};	// namespace

#endif	// _LOGIN_SERVICE_H_
