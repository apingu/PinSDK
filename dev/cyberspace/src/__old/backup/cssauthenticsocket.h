//
//
//   作為server端最基本建立的socket 傳送 經過 加密 壓縮 加入header的資料
//
//
//
//
//

#ifndef _CSSAUTHENTICSOCKET_H
#define _CSSAUTHENTICSOCKET_H

#include <basedef.h>
#include "CsDef.h"
#include <NetSocket.h>



namespace Cyberspace
{

    typedef Net::TCPSocket  CsSocket;

	class PacketFactory;

	class CsSAuthenticSocket : public Net::TCPSocket
	{
		typedef Net::TCPSocket   BaseClass;

		PacketFactory*     m_PacketFactory;// 用指標的目的只是不想要大家都需要include他

	public:

		CsSAuthenticSocket();
		~CsSAuthenticSocket();


		virtual socket_t     Create(int domain = AF_INET);

		virtual int          Close(void);

		int                  SendPacket(uniqueid_t eid, unsigned short cid, const byte_t* bp, size32_t len);
		int                  RecvPacket(uniqueid_t& eid, unsigned short& cid, byte_t** bp, size32_t& len);

		mstimen_t            GetLastRecvMST(void);
	};
}

#endif  //_CSSAUTHENTICSOCKET_H