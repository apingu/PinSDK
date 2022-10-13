


#include "CsConnectService.h"

namespace Cyberspace
{

	void ConnectService::PlayerLogin(int player_id,IO::TCPSocket *socket)
	{
		m_socket[player_id] = socket;
	}

	void ConnectService::SendCsCmdFromID(int player_id, void *packet)
	{
		IO::TCPSocket *socket = m_socket[player_id];
		socket->Send( (char*)packet, 1024, 1 );
	}


};	// namespace

