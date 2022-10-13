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
#ifndef CSIDENTITY_H
#define CSIDENTITY_H


#include "CsBaseSocket.h"
#include <list>
#include <SafeValue.tl>
#include "CstHostNode.h"


namespace Cyberspace
{
	class CsIdentity : public CstConnectNode, public BaseSocket
	{
		friend class Server;
		friend class DISNode;

		std::string          m_Username;   //Account name
		powerid_t            m_Power;      //權力代號
		
		std::list< void* >   m_MessageVec;
		int                  m_StateChannel;
		int                  m_StateStage;

		SafeValue<bool>      m_bValidation;
		SafeValue<mstime_t>  m_RecvTime;
		SafeValue<u_int>     m_SafeCounter;

		//T_Lock             m_lCommBuff;
		//CsCommandPacket*   m_CommBuff;


		T_Lock             m_lKey;
		void*              m_Key;

		//  使用狀態的 lock
		T_Lock             m_lUsed;
		int                m_Used;


	public:

		CsIdentity( connectionid_t id );
		CsIdentity();
		~CsIdentity();

		SafeValue<bool>  m_Kick;

		
		connectionid_t    GetCId( void );
		MacAddress_t      GetMId( void );
		const char*       GetUsername( void );
		int               GetPower( void );
		mstime_t          GetRecvTime( void ){ return m_RecvTime.GetValue();}

// 		CsCommandPacket*  CreateBuffer( void );
// 		CsCommandPacket*  GetBuffer( void );
// 		void              DeleteBuffer( void );

		virtual void      Clear( void );

		//T_Lock          m_Lock;
		char work[4];	// 這個供一些 function 用來做工作區, 記得不要依賴它(例如網路傳送時的標籤)
		unsigned int    m_IncredibleMessageCount;//收到未經驗証的連線訊息次數




		int             GetUsed( void );
		void            AddUsed( int u );
		bool            WaitForSingleUsed( int block );

	
	private:

		void            SetCId( connectionid_t id );
		void            SetMId( MacAddress_t m );
		void            SetUsername( const char* username );
		void            SetPower( int b );

		void            SetSafeCounter( u_int c ){ m_SafeCounter.SetValue(c); }
		u_int           GetSafeCounter( void ){ return m_SafeCounter.GetValue(); }

		void            SetRecvTime( mstime_t t ){ m_RecvTime.SetValue(t); }

        void            OnCreate( void );
        void            OnClose( void );

	};
}

#endif//CSACCOUNT_H
