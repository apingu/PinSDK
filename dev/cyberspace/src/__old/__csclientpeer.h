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
//
//
//
//
//
#ifndef CSCLIENTPEER_H
#define CSCLIENTPEER_H


#include "IOSocket.h"
//#include "UniqueIdFactory.h"


namespace Cyberspace
{
	//! 這個東西有請 Pin 註解
	class ConnectObj : public IO::TCPSocket
	{
		//! 每次分配的連線 ID
		ConnectionId_t  m_CId;
		//! 網路卡資料, 作為追蹤紀錄用
		MACAddress_t    m_Macaddr;
		//! 已驗證?
		bool            m_Credible;
		//! 收到未經驗証的連線訊息次數
		unsigned int    m_IncredibleMessageCount;
		
	public:
		ConnectObj();
		~ConnectObj();

		const char* SocketName() { return "CsConnectObj";}

		//! 設定連線 ID
		void            SetId( ConnectionId_t id );
		//! 讀取連線 ID
		ConnectionId_t  GetId( void );
		//! 驗證狀態設定
		void SetAutherntic( bool b );
		//! 驗證狀態讀取
		bool GetAutherntic( void );
		//! 設定收到未經驗証的連線訊息次數
		void         SetMaxFictitiousMessage( unsigned int imc );
		//! 讀取收到未經驗証的連線訊息次數
		unsigned int GetMaxFictitiousMessage( void );

	};

	///////////////////////////////////////////////////////////////////////////////////
	//

	//=============================================================================
	//  
	//
	//=============================================================================
	ConnectObj::ConnectObj()
	{
		//m_Id       = CsINVALID_AID;
		//m_UKey     = 0;
		m_Credible = false;
		m_IncredibleMessageCount = 0;
	}

	//=============================================================================
	//  
	//
	//=============================================================================
	ConnectObj::~ConnectObj()
	{
	
	}

	//=============================================================================
	//  
	//
	//=============================================================================
	void ConnectObj::SetId( ConnectionId_t id )
	{
		m_CId = id;
	}

	//=============================================================================
	//  
	//
	//=============================================================================
	ConnectionId_t ConnectObj::GetId( void )
	{
		return m_CId;
	}

	/*

    //=============================================================================
	//  
	//
	//=============================================================================
	void ConnectObj::SetUKey( ukey_t v )
	{
		m_UKey = v;
	}

	//=============================================================================
	//  
	//
	//=============================================================================
	ukey_t ConnectObj::GetUKey( void )
	{
		return m_UKey;
	}
	*/

	//=============================================================================
	//  
	//
	//=============================================================================	
	void ConnectObj::SetAutherntic( bool b )
	{
		m_Credible = b;
	}

	//=============================================================================
	//  
	//
	//=============================================================================
	bool ConnectObj::GetAutherntic( void )
	{
		return m_Credible;
	}

    //=============================================================================
	//  
	//
	//=============================================================================
	void ConnectObj::SetMaxFictitiousMessage( unsigned int imc )
	{
		m_IncredibleMessageCount = imc;
	}

	//=============================================================================
	//  
	//
	//=============================================================================
	unsigned int ConnectObj::GetMaxFictitiousMessage( void )
	{
		return m_IncredibleMessageCount;
	}

};

#endif
