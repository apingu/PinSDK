//
//
//   作為server端最基本建立的socket 傳送 經過 加密 壓縮 加入header的資料
//
//
//
//
//

#include "CsSAuthenticSocket.h"
#include "PacketFactory.h"
#include <PFTimer.h>

namespace Cyberspace
{
	//===============================================================================//
	//
	//
	//===============================================================================//
	CsSAuthenticSocket::CsSAuthenticSocket()
	{
		m_PacketFactory = new PacketFactory;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	CsSAuthenticSocket::~CsSAuthenticSocket()
	{
        Close();
		SAFE_DELETE(m_PacketFactory);
	}

	/*
	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsSAuthenticSocket::Initate(void)
	{
		m_PacketFactory->Initate();
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsSAuthenticSocket::Release(void)
	{
		m_PacketFactory->Initate();
	}
	*/

	//===============================================================================//
	//
	//
	//===============================================================================
	socket_t CsSAuthenticSocket::Create(int domain)
	{
        if(m_PacketFactory==NULL)
            return 0;
		m_PacketFactory->Initate();
		return BaseClass::Create(domain);
	}


	//===============================================================================//
	//
	//
	//===============================================================================
	int CsSAuthenticSocket::Close(void)
	{
        if(m_PacketFactory==NULL)
            return 0;
		m_PacketFactory->Initate();
		return BaseClass::Close();
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	int  CsSAuthenticSocket::SendPacket(uniqueid_t eid, unsigned short cid, const byte_t* bp, size32_t len)
	{
		//TS_Lock lock(&m_AKLock);

		// 還是有機會對斷線的人送資料
		if(this->GetFD()==0)
			return 0;
		return m_PacketFactory->SendPacket(this, eid, cid, bp, len);
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	int  CsSAuthenticSocket::RecvPacket(uniqueid_t& eid, unsigned short& cid, byte_t** bp, size32_t& len)
	{
		return m_PacketFactory->RecvPacket(this, eid, cid, bp, len);
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	/*
	void CsSAuthenticSocket::SetLastRecvMST(mstimen_t t)
	{
		TS_Lock lock(&m_AKLock);
		m_LastRecvMST=t;
	}
	*/

	//===============================================================================//
	//
	//
	//===============================================================================//
	mstimen_t CsSAuthenticSocket::GetLastRecvMST(void)
	{
		//TS_Lock lock(&m_AKLock);
		//return m_LastRecvMST;
		return m_PacketFactory->GetLastRecvMST();
	}

}

