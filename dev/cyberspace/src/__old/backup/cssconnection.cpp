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
#include "CsSConnection.h"
#include "CsProtocol.h"
#include "MessageDispatcher.h"
#include "PacketFactory.h"
#include <PFTimer.h>
#include <Debug.h>

#include <memio.inl>
#include <assert.h>

namespace Cyberspace
{

	uint_t     CsSConnection::s_LimitSafeCounter=10000;
	mstimen_t  CsSConnection::s_MaxRecvHeadway  = 100;

	struct ServerMessageData
	{
		unsigned int eid;
		channelid_t  channelid;
		std::vector< char > buff;
	};




	//===============================================================================//
	//
	//
	//===============================================================================//
	CsSConnection::CsSConnection()
	{
		m_ConnectionDescriptor = NULL;
		m_SafeCounter          = 0;
        m_AccecpMSec           = 0;
        m_ErrorChennalCounter  = 0;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	CsSConnection::~CsSConnection()
	{
		//Release();
		Close();
		//delete accname;
		/*
		std::map< EntityId_t, CsEntity* >::iterator iter = m_Entities.begin();
		while(iter != m_Entities.end())
		{
		SAFE_DELETE(iter->second);
		iter++;
		}
		*/
		//m_Username[0]='\0';	// clear, there may be some bug
		//SAFE_DELETE_ARRAY(m_Username);

	}


	//===============================================================================//
	//
	//
	//===============================================================================
	socket_t CsSConnection::Create(int domain)
	{
		TS_Lock lock(&m_utCsLock);
		//m_bValidation = false;
		m_SafeCounter = 0;
		m_ConnectionDescriptor = NULL;
        

		//m_bInit= true;
		//ipn_t zipn;
		//memset(&zipn, 0, sizeof(ipn_t));
		//m_ConnectNode.Initial(CsINVALID_ID, 0, CsINVALID_ID, -1, GetPeerIPAddress(), 0, 0, 0, 0, this);
		/*		
		m_ConnectNode.SetCid(CsINVALID_ID);
		m_ConnectNode.SetMid(0);
		m_ConnectNode.SetSid(CsINVALID_ID);
		m_ConnectNode.SetPower(-1);
		m_ConnectNode.SetIndex(0);
		m_ConnectNode.ClearFlag();
		*/
		//m_ConnectNode.ClearState();
		//m_ConnectNode.SetStateBit(_CONNECTNODE_MASTER, true);
		//buildtime;                // 故意不清
		//createtime;               // 故意不清
		//m_Username="";
		//m_bValidation=false;
		//m_Key=NULL;     // 因為放在 onclose的地方 所以 這邊 不要 動 不然會有 發生的時間點的問題(因為 kickout 是擺在 另一個序中)
		//m_bInit= false;

		return CsSAuthenticSocket::Create(domain);
	}


	//===============================================================================//
	//
	//
	//===============================================================================
	int CsSConnection::Close(void)
	{
		return CsSAuthenticSocket::Close();
	}



	//===============================================================================//
	//
	//
	//===============================================================================//
// 	bool CsSConnection::GetInit(void)
// 	{
// 		TS_Lock lock(&m_utCsLock);
// 		return m_bInit;
// 	}


	/*
	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsSConnection::SetCId(connectionid_t id)
	{
		//TS_Lock lock(&m_utCsLock);
		m_ConnectNode.SetCid(id);
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsSConnection::SetMId(macn_t m)
	{
		//TS_Lock lock(&m_utCsLock);
		m_ConnectNode.SetMid(m);
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsSConnection::SetPower(int b)
	{
		//TS_Lock lock(&m_utCsLock);
		m_ConnectNode.SetPower(b);
	}
	*/


	//===============================================================================//
	//
	//
	//===============================================================================//
	CsConnectionDescriptor CsSConnection::GetDescriptor()
	{
		TS_Lock lock(&m_utCsLock);
		return m_ConnectionDescriptor;
	}


	/*
	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsSConnection::SetValidation(bool f)
	{
		TS_Lock lock(&m_utCsLock);
		m_bValidation = f;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	bool CsSConnection::GetValidation(void)
	{
		TS_Lock lock(&m_utCsLock);
		return m_bValidation;

	}


	
	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsSConnection::SetUsername(const char* username)
	{
		m_Username= username;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	const char* CsSConnection::GetUsername(void)
	{
		return m_Username.c_str();
	}
	*/


	//===============================================================================//
	//
	//
	//===============================================================================//
// 	mstimen_t CsSConnection::GetLastRecvMST(void)
// 	{
// 		TS_Lock lock(&m_utCsLock);
// 		return m_LastRecvMST;
// 	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	bool CsSConnection::CheckRecvLegal(mstimen_t currtime)
	{
        if(m_ErrorChennalCounter>30)
            return false;

		return true;
		// 避免封包連續式的攻擊
		// 如果連續很多次 送封包的訊息間隔時間都很短
		// 就把它當成不正當
		mstimen_t recvtime = 0;
		int      counter  = 0;

		TS_Lock lock(&m_utCsLock);


	    recvtime = GetLastRecvMST();
		counter  = m_SafeCounter;
		
		//scn->SetSafeCounter(counter);
		//scn->SetRecvTime(nowtime);

		mstimen_t mtime=currtime-recvtime;

		// 距離上次收到訊息的時間，低於 100ms
		if(mtime<s_MaxRecvHeadway)
		{
			counter++;				
		}
		else
		{
			counter-=100;
			if(counter<=0)
			    counter=0;
			//else
			//	counter--;
		}


		m_SafeCounter=counter;
	    //SetLastRecvMST(currtime);


		//m_SafeCounter.SetValue(counter);

		if(counter >s_LimitSafeCounter)
		{
			return false;
		}
		return true;
	}


	//    //===============================================================================//
	//    //
	//    //
	//    //===============================================================================//
	//	void CsSConnection::SetValidation(bool b)
	//	{
	//		m_bValidation = b;
	//	}
	//
	//
	//    //===============================================================================//
	//    //
	//    //
	//    //===============================================================================//
	//	bool CsSConnection::GetValidation(void)
	//	{
	//		return m_bValidation;
	//	}





	// 	//===============================================================================//
	//     //
	//     //
	//     //===============================================================================//
	// 	CsCommandPacket* CsSConnection::CreateBuffer(void)
	// 	{
	// 		TS_Lock lock(&m_lCommBuff);
	// 		SAFE_DELETE(m_CommBuff);
	// 		m_CommBuff = new CsCommandPacket;
	// 		IOMEMSET(m_CommBuff, 0, _MAX_SOCKET_BUFF);
	// 		return m_CommBuff;
	// 	}
	// 
	// 	//===============================================================================//
	// 	//
	// 	//
	// 	//===============================================================================//
	// 	CsCommandPacket* CsSConnection::GetBuffer(void)
	// 	{
	// 		TS_Lock lock(&m_lCommBuff);
	// 		if(m_CommBuff==NULL)
	// 		{
	// 			m_CommBuff = new CsCommandPacket;
	// 		}
	// 		IOMEMSET(m_CommBuff, 0, _MAX_SOCKET_BUFF);
	// 		return m_CommBuff; 
	// 	}
	// 
	//     //===============================================================================//
	//     //
	//     //
	//     //===============================================================================//
	// 	void CsSConnection::DeleteBuffer(void)
	// 	{
	// 		TS_Lock lock(&m_lCommBuff);
	// 		SAFE_DELETE(m_CommBuff);
	// 	}

	//===============================================================================//
	//
	//
	//===============================================================================//
// 	int CsSConnection::GetUsed(void)
// 	{
// 		TS_Lock lock(&m_lUsed);
// 		return m_Used;
// 	}

	//===============================================================================//
	//
	//
	//===============================================================================//
// 	void CsSConnection::AddUsed(int u)
// 	{
// 		TS_Lock lock(&m_lUsed);
// 		m_Used+=u;
// 	}

	//===============================================================================//
	//
	//
	//===============================================================================//
// 	bool CsSConnection::WaitForSingleUsed(int block)
// 	{
// 		int times=0;
// 		while(1)
// 		{
// 			{
// 				TS_Lock lock(&m_lUsed);
// 				if(m_Used<=0)
// 					return true;
// 				times++;
// 				if(block!=0)
// 				{
// 					if(times>block)
// 					{
// 						return false;
// 					}
// 				}
// 			}
// 			Sleep(20);
// 		}
// 	}


/*
	//===============================================================================//
	//
	//
	//===============================================================================//
	int CsSConnection::SendPacket(uniqueid_t eid, unsigned short cid, const byte_t* bp, size32_t len)
	{
		TS_Lock lock(&m_utCsLock);

		// 還是有機會對斷線的人送資料
		if(this->GetFD()==0)
			return 0;

		return m_PacketFactory->SendPacket(this, eid, cid, bp, len);
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	int CsSConnection::RecvPacket(uniqueid_t& eid, unsigned short& cid, byte_t** bp, size32_t& len)
	{
		TS_Lock lock(&m_utCsLock);
		m_LastRecvMST=PFTimer::GetTickMSTime();
		return m_PacketFactory->RecvPacket(this, eid, cid, bp, len);
	}
*/



}

