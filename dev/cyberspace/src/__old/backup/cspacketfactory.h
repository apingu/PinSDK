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


namespace Cyberspace
{
	class Net::TCPSocket;

	// 每個SOCKET 不管 CLIENT 或是 SERVER PEER 都應該有一個自己的
	// 每次都要重新建立一個KEY 當成是下一個封包加密的Key
	class PacketFactory
	{
		// 加密用的
		T_Lock               m_PFlock;

        connectionid_t       m_ConnectionId;

		mstimen_t            m_LastRecvMST;        // 上次接收到訊息封包的時間，為避免封包攻擊
		mstimen_t            m_LastSendFromMST;    // 最近一次接收到的封包 他從發送端送出的時間
		byte_t               m_SendCryptologyKey[_CsMAX_CRYPTKEY_SIZE];
		byte_t               m_RecvCryptologyKey[_CsMAX_CRYPTKEY_SIZE];

        byte_t*              m_RecvBuf;
        size32_t             m_RecvLen;

	public:

		PacketFactory();
		~PacketFactory();

        connectionid_t   GetConnectionId();

		void             Initate    (connectionid_t cid);       

		mstimen_t        GetLastRecvMST(void);
		mstimen_t        GetLastSendFromMST(void);

		// 加上 加解密的KEY
		int             SendPacket(Net::TCPSocket* skt, uniqueid_t eid, channelid_t cid, const byte_t* bp, size32_t len);
        // bp: 原先buffer的point   bl :原buffer的length 假如長度夠 就不會重新new 一個
		int             RecvPacket(Net::TCPSocket* skt, uniqueid_t& eid,channelid_t& cid, byte_t** bp, size32_t& len);

	};
};


#endif
