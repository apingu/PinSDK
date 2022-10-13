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
#include <NetSocket.h>
#include <PFTimer.h>
#include "CsPacketFactory.h"
#include "CsProtocol.h"


namespace Cyberspace
{

	//===============================================================================//
	//
	//
	//===============================================================================//
	PacketFactory::PacketFactory()
	{
        m_RecvBuf=NULL;
		Initate(CsINVALID_ID);
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	PacketFactory::~PacketFactory()
	{
        SAFE_DELETE_ARRAY(m_RecvBuf);
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
    connectionid_t PacketFactory::GetConnectionId()
    {
        return m_ConnectionId;
    }

	//===============================================================================//
	//
	//
	//===============================================================================//
	void PacketFactory::Initate(connectionid_t cid)
	{
		TS_Lock lock(&m_PFlock);
        m_ConnectionId=cid;
		m_LastRecvMST=0;
		m_LastSendFromMST=0;
		memset(m_RecvCryptologyKey, 0,  _CsMAX_CRYPTKEY_SIZE*sizeof(byte_t));
		memset(m_SendCryptologyKey, 0,  _CsMAX_CRYPTKEY_SIZE*sizeof(byte_t));
        SAFE_DELETE_ARRAY(m_RecvBuf);
        m_RecvLen=512;
        m_RecvBuf=new byte_t[m_RecvLen];
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	mstimen_t PacketFactory::GetLastRecvMST(void)
	{
		TS_Lock lock(&m_PFlock);
		return m_LastRecvMST;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	mstimen_t PacketFactory::GetLastSendFromMST(void)
	{
		TS_Lock lock(&m_PFlock);
		return m_LastSendFromMST;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	int PacketFactory::SendPacket(Net::TCPSocket* skt, uniqueid_t eid, channelid_t cid, const byte_t* bp, size32_t len)
	{
		TS_Lock lock(&m_PFlock);
		mstimen_t mt=PFTimer::GetTickMSTime();
		return CsCore::CsSendPacket(skt, m_ConnectionId, eid, cid, bp, len, mt, m_SendCryptologyKey);
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	int PacketFactory::RecvPacket(Net::TCPSocket* skt, uniqueid_t& eid, channelid_t& cid, byte_t** bp, size32_t& len)
	{
		TS_Lock lock(&m_PFlock);
		m_LastRecvMST=PFTimer::GetTickMSTime();
		int ret=CsCore::CsRecvPacket(skt, m_ConnectionId, eid, cid, &m_RecvBuf, m_RecvLen, len, m_LastSendFromMST, m_RecvCryptologyKey);
        (*bp)=m_RecvBuf;
        return ret;
	}
}

