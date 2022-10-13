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
#include "CsLoginClient.h"
#include "CsLoginCommand.h"
#include "CsLoginClientDelegate.h"
#include <T_Lock.h>
#include <Debug.h>
//#include "CsGameCommand.h"

namespace Cyberspace
{


	T_Lock g_LoginStateLock;
	//===============================================================================//
	//
	//
	//===============================================================================//
	LoginClient::LoginClient()
	{
		Create( NULL );
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	LoginClient::~LoginClient()
	{
		{
			TS_Lock lock( &g_LoginStateLock );
			m_Loginstate = CSERR_NOT_CONNECTD;
		}
		Shutdown();
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void  LoginClient::Create( LoginClientDelegate* gc )
	{
		{
			TS_Lock lock( &g_LoginStateLock );
			m_Loginstate = CSERR_NOT_CONNECTD;
		}
		m_gc = gc;
		m_TimeoutBegin = 0;
		// 預設30秒
		m_TimeoutSec = 30;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	dword_t LoginClient::OnGetVersion()
	{
		if ( m_gc )
		{
			return m_gc->OnGetVersion();
		}

		return CsINVALID_ID;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	bool LoginClient::Connect( ipa_t host, unsigned short port )
	{
		//SetConnectionId(CsINVALID_ID);
		//FreeConnect();
		{
			TS_Lock lock( &g_LoginStateLock );
			m_Loginstate = CSERR_NOT_CONNECTD;
		}
		//m_HostList.clear();
		return Client::Connect( host, port );
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginClient::SetTimeoutTime( stimen_t t )
	{
		m_TimeoutSec = t;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	CSERR_CODE LoginClient::Login( const char_t* username, const char_t* password )
	{
		AsynLogin( username, password );

		//for (UINT i = 0; i < 30; ++i)
		while ( 1 )
		{
			// 不用sleep因為 上層有做timeout
			Update();
			CSERR_CODE code = GetLoginState();

			if ( ( code == CsINVALID_ID ) || ( code == CSERR_DURING_LOGIN ) || ( code == CSERR_NOT_CONNECTD ) )
			{
			}
			else if ( code == CSERR_TIMEOUT )
			{
				break;
			}
			else if ( code == CSERR_ACC_SUCCESS )
			{
				return CSERR_ACC_SUCCESS;
			}
			else
			{
				return code;
			}

			//
			// 			switch(m_Connected)
			// 			{
			// 			case  1: return true;
			// 			case -1: break;
			// 			default:
			// 			{
			// 			Sleep(500);
			// 			continue;
			// 			}
			// 			}
			//
		}

		//Client::Shutdown();
		Shutdown();
		return CSERR_TIMEOUT;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginClient::AsynLogin( const char_t* username, const char_t* password )
	{
		{
			TS_Lock lock( &g_LoginStateLock );

			if ( m_Loginstate == CSERR_DURING_LOGIN )
			{
				return;
			}

			m_Loginstate = CSERR_DURING_LOGIN;
		}
		CsResBuf buf;
		CsLoginMessage::Client::SetRequestMessage( buf, username, password );
		SendCsMessage( CsINVALID_ID, CsC_S_LOGIN_REQUEST, _GetResBufPtr( buf ), _GetResBufLen( buf ) );
		m_TimeoutBegin = time( NULL );
	}



	//===============================================================================//
	//
	//
	//===============================================================================//
	CSERR_CODE LoginClient::Link( const tchar_t* linkdata )
	{
		AsynLink( linkdata );

		while ( 1 )
		{
			// 不用sleep因為 上層有做timeout
			Update();
			CSERR_CODE code = GetLoginState();

			if ( ( code == CsINVALID_ID ) || ( code == CSERR_DURING_LOGIN ) || ( code == CSERR_NOT_CONNECTD ) )
			{
			}
			else if ( code == CSERR_TIMEOUT )
			{
				break;
			}
			else if ( code == CSERR_ACC_SUCCESS )
			{
				return CSERR_ACC_SUCCESS;
			}
			else
			{
				return code;
			}
		}

		Shutdown();
		return CSERR_TIMEOUT;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginClient::AsynLink( const tchar_t* linkdata )
	{
		{
			TS_Lock lock( &g_LoginStateLock );

			if ( m_Loginstate == CSERR_DURING_LOGIN )
			{
				return;
			}

			m_Loginstate = CSERR_DURING_LOGIN;
		}
		channelid_t id = 0;
		unsigned int len = 0;
		CsResBuf buf;
		CsBridgeMessage::Client::SetRequestMessage( buf, linkdata );
		SendCsMessage( CsINVALID_ID, CsC_S_LINK_REQUEST, _GetResBufPtr( buf ), _GetResBufLen( buf ) );
		m_TimeoutBegin = time( NULL );
	}

	const char* LoginClient::GetRetBuff()
	{
		return m_retbuf;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	CSERR_CODE LoginClient::GetLoginState( void )
	{
		TS_Lock lock( &g_LoginStateLock );

		if ( m_Loginstate == CSERR_DURING_LOGIN )
		{
			if ( m_TimeoutSec != 0 )
			{
				int passtime = time( NULL ) - m_TimeoutBegin;

				if ( passtime > m_TimeoutSec )
				{
					return m_Loginstate = CSERR_TIMEOUT;
				}
			}
		}

		// 		if((m_Loginstate == CSERR_ACC_SUCCESS)&&
		// 			(GetSessionId()==CsINVALID_ID))
		// 			return CSERR_DURING_LOGIN;
		return m_Loginstate;
	}



	//===============================================================================//
	//
	//
	//===============================================================================//
	/*
	void LoginClient::Logout(void)
	{
		//m_HostList.clear();
		{
			TS_Lock lock(&g_LoginStateLock);
			m_Loginstate=CSERR_NOT_CONNECTD;
		}

		CsCommandPacket hsysmsg;
		hsysmsg.Header.CommandId=CsC_S_LOGIN_REQUEST;
		hsysmsg.Header.CommandLen=1;
		//SEND_GAME_MESSAGE(this, (&hsysmsg));
		SendCsMessage(CsINVALID_ID, hsysmsg.Header.CommandId, hsysmsg.Buffer.Param, hsysmsg.Header.CommandLen);
		return;
	}
	*/


	//===============================================================================//
	//
	//! 通知SERVER 角色選擇完畢
	//===============================================================================//
	/*
	void LoginClient::Ready(void)
	{
		CsResBuf buf;
		CsLobbyCommand::CsReadyMessage::Client::SetRequestMessage(buf);
		//SEND_GAME_MESSAGE(this, (&hsysmsg));
		SendCsMessage(CsINVALID_ID, CsC_S_READY_REQUEST, _GetResBufPtr(buf), _GetResBufLen(buf));
		return;
	}
	*/



	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginClient::OnRecvDefineRequest( sessionid_t cid, uniqueid_t eid, channelid_t chid, const byte_t* command, size32_t len )
	{
		//TS_Lock lock(&m_UtLoginLock);
		switch ( chid )
		{
		case CsS_C_LOGIN_FEEDBACK:
			{
				CSERR_CODE loginstate;
				CsLoginMessage::Client::GetFeedback( command, loginstate );
				{
					TS_Lock lock( &g_LoginStateLock );
					m_Loginstate = loginstate;
				}

				if ( ( loginstate == CSERR_ACC_SUCCESS ) && ( GetSessionId() == CsINVALID_ID ) )
				{
					//m_Loginstate = CSERR_ACC_NOT_EXIST;
					//assert(0&&_T("登入成功 卻沒有ID? 很怪 除錯階段要注意"));
					//Shutdown();
				}

				if ( loginstate != CSERR_ACC_SUCCESS )
				{
					//FreeConnect();
					//SetConnectionId(CsINVALID_ID);
					Shutdown();
				}

				Pf_DEBUG( "client login feedback %d\n", m_Loginstate );
				m_gc->OnLogin( m_Loginstate, this );
				{
					TS_Lock lock( &g_LoginStateLock );
					m_Loginstate = loginstate;
				}
			}

			return;

		case CsS_C_LINK_FEEDBACK:
			{
				CSERR_CODE loginstate;
				CsBridgeMessage::Client::GetFeedback( command, loginstate, m_retbuf );
				m_Loginstate = loginstate;

				if ( ( m_Loginstate == CSERR_ACC_SUCCESS ) && ( GetSessionId() == CsINVALID_ID ) )
				{
					//m_Loginstate = CSERR_ACC_NOT_EXIST;
					//assert(0&&_T("登入成功 卻沒有ID? 很怪 除錯階段要注意"));
					//Shutdown();
				}

				if ( m_Loginstate != CSERR_ACC_SUCCESS )
				{
					Shutdown();
					//SetConnectionId(CsINVALID_ID);
				}

				m_gc->OnLogin( m_Loginstate, this );
				m_Loginstate = loginstate;
			}

			return;

		default:
			Client::OnRecvDefineRequest( cid, eid, chid, command, len );
			return;
		}

		return;
	}

	void LoginClient::OnListServer( const ServerRecord* pServerNode, int mt )
	{
		m_gc->OnListServer( pServerNode, mt );
	}


	/*
		//===============================================================================//
		//
		//
		//===============================================================================//
		unsigned int LoginClient::GetHostCount()
		{
			return m_HostList.size();
		}
	*/


	// ---------------------------------------------------------------------------
	// GetHostInfo
	// 取得註冊的伺服器資訊
	// ---------------------------------------------------------------------------
	/*
	bool LoginClient::GetHostInfo(const char* hostname,
								   int &n,
								   ipa_t ipa, ushort_t &port,
								   char* servername,
								   char* functionname,
								   unsigned int &maxconnect, unsigned int &usedconnect, int &load)
	{
		for(UINT i = 0; i<m_HostList.size(); i++)
		{
			if(strcmp(m_HostList[i]._hostname, hostname) == 0)
			{
				n = i;
				strcpy(ipa, m_HostList[n]._ip);
				port = m_HostList[n]._port;
				strcpy(servername, m_HostList[n]._objectname);
				strcpy(functionname, m_HostList[n]._functionname);
				maxconnect = m_HostList[n]._maxconnect;
				usedconnect = m_HostList[n]._usedconnect;
				if (m_HostList[n]._maxconnect == 0)
					load = 0;
				else
					load = m_HostList[n]._usedconnect * 100 / m_HostList[n]._maxconnect;
				return true;
			}
		}
		return false;
	}

	bool LoginClient::GetHostInfo(int n,
								   char* hostname,
								   ipa_t ipa, ushort_t &port,
								   char* servername,
								   char* functionname,
								   unsigned int &maxconnect, unsigned int &usedconnect, int &load)
	{
		if (n >= 0 &&n < GetHostCount())
		{
			strcpy(hostname, m_HostList[n]._hostname);
			strcpy(ipa, m_HostList[n]._ip);
			port = m_HostList[n]._port;
			strcpy(servername, m_HostList[n]._objectname);
			strcpy(functionname, m_HostList[n]._functionname);
			maxconnect = m_HostList[n]._maxconnect;
			usedconnect = m_HostList[n]._usedconnect;
			if (m_HostList[n]._maxconnect == 0)
				load = 0;
			else
				load = m_HostList[n]._usedconnect * 100 / m_HostList[n]._maxconnect;
			return true;
		}

		return false;
	}
	*/
	/*

	void LoginClient::ClearState()
	{
		//m_HostList.clear();
		{
			TS_Lock lock(&g_LoginStateLock);
			m_Loginstate=CSERR_NOT_CONNECTD;
		}
		//SetConnectionId(-1);
		Shutdown();
	}
	*/



	//! 取代上層功能
// 	void LoginClient::OnKnockout(void)
// 	{
// 		// 如果正常進入GAMESERVER後這邊被踢出應該是正常的
// 		//if(m_Loginstate!=CSERR_ACC_SUCCESS)
// 		//ClearState();
// 		FreeConnect();
// //      m_HostList.clear();
// // 		m_Loginstate=CSERR_NOT_CONNECTD;
// // 		SetConnectionId(-1);
// 		//m_GameClient->OnClientKnockout();
// 	}

	int LoginClient::Shutdown( void )
	{
		int ret = Cyberspace::Client::Shutdown();
		//FreeConnect();
		return ret;
	}


	//! 取代上層功能
	//! 被斷線以後會被喚起的 callback
	/*
	void LoginClient::OnDisconnection(void)
	{
		// 如果正常進入GAMESERVER後這邊被踢出應該是正常的
		//if(m_Loginstate!=CSERR_ACC_SUCCESS)
		//ClearState();
		FreeConnect();
	// 		m_HostList.clear();
	// 		m_Loginstate=CSERR_NOT_CONNECTD;
	// 		SetConnectionId(-1);
		//m_GameClient->OnClientDisconnection();
	}
	*/

};