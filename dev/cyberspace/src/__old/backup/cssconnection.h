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
#ifndef CSSCONNECTION_H
#define CSSCONNECTION_H


#include <list>
#include <safevalue.inl>
#include "CsSAuthenticSocket.h"
#include "CstCDataDef.h"



namespace Cyberspace
{
	class CsSConnection : public CsSAuthenticSocket
	{
		friend class CoreServer;

		//bool                             m_bValidation;
		//T_Lock                           m_lSafeCounter;
		int                                m_SafeCounter;  // 安全次數，為避免封包攻擊
		//T_Lock                           m_lLastRecvMST;
		//mstimen_t                         m_LastRecvMST;
		channelid_t                        m_StateChannel;
		short                              m_StateStage;
		//bool                             m_bInit;
		T_Lock                             m_utCsLock;
        mstimen_t                          m_AccecpMSec;

		// 連續接收封包合法的次數
		//  超過 次數 每次送上來的時間都很密集時
		static uint_t                      s_LimitSafeCounter;
		static mstimen_t                   s_MaxRecvHeadway;

		CsConnectionDescriptor             m_ConnectionDescriptor;

	public:

		CsSConnection();
		~CsSConnection();

        int                                m_ErrorChennalCounter;

    private:

		virtual socket_t                   Create(int domain = AF_INET);
		virtual int                        Close(void);

		// 
		bool                               CheckRecvLegal(mstimen_t currtime);

		void                               SetDescriptor(CsConnectionDescriptor sdp) {TS_Lock lock(&m_utCsLock); m_ConnectionDescriptor=sdp;}
        CsConnectionDescriptor             GetDescriptor()	                         {TS_Lock lock(&m_utCsLock); return m_ConnectionDescriptor;
        };
	};
}

#endif//CSSCONNECTION_H
