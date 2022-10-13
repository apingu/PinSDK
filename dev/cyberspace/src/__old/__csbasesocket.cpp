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
#include "CsBaseSocket.h"
#include "CsProtocol.h"


namespace Cyberspace
{

	//===============================================================================//
	//
	//
	//===============================================================================//
	BaseSocket::BaseSocket()
	{

	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	BaseSocket::~BaseSocket()
	{

	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	int BaseSocket::SendPacket( uniqueid_t eid, unsigned short cid, const byte_t* bp, size_t len )
	{
		TS_Lock lock(&m_lCryptologyKey);
		return CsCore::CsSendPacket( this, eid, cid, bp, len, m_CryptologyKey );
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	int BaseSocket::RecvPacket( uniqueid_t& eid, unsigned short& cid, byte_t** bp, size_t& len )
	{
		TS_Lock lock(&m_lCryptologyKey);
		return CsCore::CsRecvPacket( this, eid, cid, bp, len, m_CryptologyKey );
	}



}

