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
#ifndef CSBASESOCKET_H
#define CSBASESOCKET_H

#include "CsDef.h"
#include <NetSocket.h>


namespace Cyberspace
{
	class BaseSocket : public Net::TCPSocket
	{
		// 加密用的
		T_Lock             m_lCryptologyKey;
		byte_t             m_CryptologyKey[_CsMAX_CRYPTKEY_SIZE];

	public:

		BaseSocket();
		~BaseSocket();


		// 加上 加解密的KEY
		int             SendPacket( uniqueid_t eid, unsigned short cid, const byte_t* bp, size_t len );
		int             RecvPacket( uniqueid_t& eid, unsigned short& cid, byte_t** bp, size_t& len );

	};
};


#endif
