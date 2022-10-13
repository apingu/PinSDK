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
#include "CsLoginServer.h"
#include "CsLoginCommand.h"

#include "CsDBFunction.h"
#include "CsArbitrationClient.h"

#include <XMySqlDB.h>
#include <PfTimer.h>
#include <Debug.h>


namespace Cyberspace
{
#define INVALID_CHARACTER_NAME(c)    (((c)==NULL)||(strcmp((c), "")==0))

	//#define SEND_LOBBY_MESSAGE(s, m)  SendCsMessage(s, CsINVALID_ID, (m)->Header.CommandId, (m)->Buff, (m)->Header.CommandLen)
	//#define SEND_LOBBY_MESSAGE(s, m)  CsCore::CsSendPacket(s, CsINVALID_ID, (m)->Header.CommandId, (m)->Buffer.Param, (m)->Header.CommandLen)
	class LoginField
	{
	public:
		static bool   dbg_no_check_login2; // 不檢查重複登入
		bool          m_AllowLogin;

		bool          m_bTimeoutClose;
		mstimen_t     m_TimeoutCycle;
		T_Lock        m_lTimeout;
		std::set<CsSessionDescriptor> m_TimeoutSeq;

		T_Lock        m_lConnectCount;
		uint_t        m_ConnectCount;

		T_Lock        m_lLoginCount;
		uint_t        m_LoginCount;

		T_Lock        m_lLoginErrorCount;
		uint_t        m_LoginErrorCount;

		T_Lock        m_lLoginUnKnowErrorCount;
		uint_t        m_LoginUnKnowErrorCount;

		T_Lock        m_lTimeoutCount;
		uint_t        m_TimeoutCount;


		stimen_t       m_LessLogoutSec; // 至少要登出多久後 才能在登入 SEC

		LoginField()
		{
			m_AllowLogin = true;
			m_TimeoutCycle = 1000 * 60 * 3;
			m_bTimeoutClose = false;
			m_TimeoutCount = 0;
			m_LoginErrorCount = 0;
			m_LessLogoutSec = 10;
		}

		~LoginField()
		{
			m_bTimeoutClose = true;
		}
	};

	bool LoginField::dbg_no_check_login2 = true; // true 不設定及檢查 DB 之登入紀錄, 對 debug 有好處

	//===============================================================================//
	//
	//
	//===============================================================================//
	LoginServer::LoginServer()
	{
		m_pLoginField = new LoginField;
		//m_bSaveLogoutSec=false;
		//((ArbitPeer*)m_pAribtAgent)->m_Visible=false;
		//((ArbitPeer*)m_pAribtAgent)->StartTimeoutPolling();
		Pflag hostflag = 0;
		SET_BIT_FLAG( hostflag, _HOST_FLAG_HIDE_ );
		SetFlag( hostflag );
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	LoginServer::~LoginServer()
	{
		SAFE_DELETE( m_pLoginField );
		//((ArbitPeer*)m_pAribtAgent)->CloseTimeoutPolling();
		//ClearAllLock();
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	int LoginServer::StartUp( uint_t allowplayers, unsigned short port )
		//void LoginServer::OnServerInitialing(void)
	{
		int ret = 0;
		ret = Cyberspace::Server::StartUp( allowplayers, port );
		RegisterUserDefChannel( CsC_S_LOGIN_REQUEST, DoLogin );
		RegisterUserDefChannel( CsC_S_LOGIN_LOGOUT_REQUEST, DoLogout );
		{
			TS_Lock  lock( &m_pLoginField->m_lConnectCount );
			m_pLoginField->m_ConnectCount = 0;
			TS_Lock llock0( &m_pLoginField->m_lLoginCount );
			m_pLoginField->m_LoginCount = 0;
			TS_Lock llock1( &m_pLoginField->m_lLoginErrorCount );
			m_pLoginField->m_LoginErrorCount = 0;
			TS_Lock llock2( &m_pLoginField->m_lLoginUnKnowErrorCount );
			m_pLoginField->m_LoginUnKnowErrorCount = 0;
		}
		return ret;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	int LoginServer::Shutdown()
		//int LoginServer::Shutdown(void)
	{
		int ret = Cyberspace::Server::Shutdown();
		if ( ret != 1 )
		{
			return ret;
		}
		UnRegisterUserDefChannel( CsC_S_LOGIN_REQUEST );
		UnRegisterUserDefChannel( CsC_S_LOGIN_LOGOUT_REQUEST );
		return ret;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginServer::SetVoidConnectCycle( mstimen_t t )
	{
		TS_Lock lock( &m_pLoginField->m_lTimeout );
		m_pLoginField->m_TimeoutCycle = t;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	uint_t LoginServer::GetTotalConnectCount( void )
	{
		TS_Lock llock( &m_pLoginField->m_lConnectCount );
		return m_pLoginField->m_ConnectCount;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	uint_t LoginServer::GetTotalLoginCount( void )
	{
		TS_Lock llock( &m_pLoginField->m_lLoginCount );
		return m_pLoginField->m_LoginCount;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	uint_t LoginServer::GetTotalLoginErrorCount( void )
	{
		TS_Lock llock( &m_pLoginField->m_lLoginErrorCount );
		return m_pLoginField->m_LoginErrorCount;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	uint_t LoginServer::GetTotalLoginUnknowErrorCount( void )
	{
		TS_Lock llock( &m_pLoginField->m_lLoginUnKnowErrorCount );
		return m_pLoginField->m_LoginUnKnowErrorCount;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	uint_t LoginServer::GetTimeoutCount( void )
	{
		TS_Lock llock( &m_pLoginField->m_lTimeoutCount );
		return m_pLoginField->m_TimeoutCount;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	bool LoginServer::RegisterUserDefChannel( channelid_t id, ServerMessageChannelFunction func )
	{
		return Server::RegisterUserDefChannel( id, ( ServerMessageChannelFunction ) func );
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	bool LoginServer::UnRegisterUserDefChannel( channelid_t id )
	{
		return Server::UnRegisterUserDefChannel( id );
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	XDataBase* LoginServer::OnCreateCommonDB( void )
	{
		return ( XDataBase* )new XMySqlDB;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginServer::OnDeleteCommonDB( XDataBase* db )
	{
		XMySqlDB* xdb = ( XMySqlDB* ) db;
		SAFE_DELETE( xdb );
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	XDataBase* LoginServer::OnCreateCustomDB( void )
	{
		return ( XDataBase* )new XMySqlDB;
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginServer::OnDeleteCustomDB( XDataBase* db )
	{
		XMySqlDB* xdb = ( XMySqlDB* ) db;
		SAFE_DELETE( xdb );
	}

	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginServer::OnClientConnect( CsSessionDescriptor sdp )
	{
		time_t timep;
		time( &timep );
		if ( IsMaster( sdp ) )
		{
			SetLogMessage( GetLogMode(), " Master client connected from %s on %s", Net::Socket::GetIPNToA( GetSessionIpn( sdp ) ), asctime( localtime( &timep ) ) );
			{
				TS_Lock lock( &m_pLoginField->m_lTimeout );
				m_pLoginField->m_TimeoutSeq.insert( std::set<CsSessionDescriptor>::value_type( sdp ) );
			}
			{
				TS_Lock lock( &m_pLoginField->m_lConnectCount );
				m_pLoginField->m_ConnectCount++;
			}
		}
		else
		{
			SetLogMessage( GetLogMode(), " Clone client connected from %s on %s", Net::Socket::GetIPNToA( GetSessionIpn( sdp ) ), asctime( localtime( &timep ) ) );
		}
	};


	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginServer::OnClientDisconnect( CsSessionDescriptor sdp )
	{
		time_t timep;
		time( &timep );
		SetLogMessage( GetLogMode(), " Client from %s disconnect on %s", Net::Socket::GetIPNToA( GetSessionIpn( sdp ) ), asctime( localtime( &timep ) ) );
		TS_Lock lock( &m_pLoginField->m_lTimeout );
		m_pLoginField->m_TimeoutSeq.erase( sdp );
		//DoLogout(sdp);
		return;
	};


	/*

	//===============================================================================//
	//  when server add one connect, must tell server Member id of the user
	//
	//===============================================================================//
	bool LoginServer::DoVerifyJoinSession(CsSessionDescriptor sdp)
	{
		return true;
	}

	//===============================================================================//
	//  when server add one connect, must tell server Member id of the user
	//
	//===============================================================================//
	void LoginServer::DoVerifyExitSession(CsSessionDescriptor sdp)
	{
		Server::DoVerifyExitSession(sdp);
	};
	*/

	//===============================================================================//
	//  DoLogin
	//
	//===============================================================================//
	int LoginServer::DoLogin( const ServerEvocator* evocator, const void* pData, unsigned int len )
	{
		CsSessionDescriptor sdp = evocator->Sessiondescriptor;
		LoginServer*  pServer = ( LoginServer* ) evocator->pserver;
		sessionid_t   Memberid = GetSessionSessionId( sdp );
		tchar_t username[ 255 ];
		tchar_t password[ 255 ];
		tchar_t buff[ 256 ];
		sprintf( buff, "Login get value %d\n", len );
		if ( !CsLoginMessage::Server::GetRequest( ( const byte_t* ) pData, len, username, password ) )
		{
			pServer->Kickout( evocator->Sessiondescriptor );
			return -1;
		}
		if ( !pServer->m_pLoginField->m_AllowLogin ) // 還不准 login
		{
			pServer->Kickout( evocator->Sessiondescriptor );
			return -1;
		}
		sessionid_t ssid = CsINVALID_ID;
		if ( Memberid != CsINVALID_ID )
		{
			pServer->SetLogMessage( LOGMODE_DEFINE, " Exist Member %lld try to do login\n", Memberid );
			{
				TS_Lock llock( &pServer->m_pLoginField->m_lLoginErrorCount );
				pServer->m_pLoginField->m_LoginErrorCount++;
			}
			pServer->Kickout( evocator->Sessiondescriptor );
			return -1;
		}
		int usedb = rand() % 10;
		if ( pServer->GetCommonDB( usedb ) == NULL )
		{
			pServer->Kickout( evocator->Sessiondescriptor );
			return -1;
		}
		//CSERR_CODE retcode=0;
		std::string userstr = pServer->GetCommonDB( usedb )->OnEscapeString( username, strlen( username ) );
		std::string pawdstr = pServer->GetCommonDB( usedb )->OnEscapeString( password, strlen( password ) );
		Memberid = pServer->GetMemberSessionId( userstr.c_str() );
		CSERR_CODE retcode = pServer->OnVerifyLogin( userstr.c_str(), pawdstr.c_str() );
		if ( retcode == CSERR_ACC_SUCCESS )
		{
			int mtype = 0;
			{
				macn_t  macadd = 0;
				int  state = 0;
				stimen_t stime = 0;
				//DBMember::dbGetCheckQualification(GetCommonDB(Memberid), userstr.c_str(), state, macadd, mtype, stime);
				DBMember::dbGetCheckQualification( pServer->GetCommonDB( Memberid ), Memberid, state, macadd, mtype, stime );
			}
			macn_t checkmacadd = GetSessionMac( sdp );
			if ( ( mtype != 2 ) && ( checkmacadd == 0 ) ) // 要檢查MAC 不允許暱MAC登入
			{
				retcode = CSSERR_BAD_MAC;
			}
			else
			{
				// 假如該帳號之前已經有人連線 先踢除除
				CsSessionDescriptor oldced = pServer->GetSessionDescriptor( Memberid );
				if ( oldced )
				{
					retcode = CSERR_ALREADY_LOGIN;
				}
				else
				{
					// TODO: 2010 1 25
					// 先設定為 offline 並填入 登入時間
					// 如果game server 再登入時間的差距 幾分鐘內 就允許登入
					DBMember::dbSetMemberAllowLogin( pServer->GetCommonDB( Memberid ), Memberid, checkmacadd );
					pServer->SetLogMessage( LOGMODE_DEFINE, " Create Member login %lld:%s succsssful\n", Memberid, Net::Socket::GetMACNToA( checkmacadd ) );
					// 設定ID給client
					pServer->BindSession( sdp, Memberid );
				}
			}
		}
		if ( retcode != CSERR_ACC_SUCCESS )
		{
			//DoInitJoinSession(sdp, errcid, GetSessionMac(sdp));
		}
		else
		{
		}
		// 可以強制改變
		retcode = pServer->OnLogin( Memberid, retcode );
		bool post = false;
		if ( retcode == CSERR_ACC_SUCCESS )
		{
			{
				TS_Lock llock( &pServer->m_pLoginField->m_lLoginCount );
				pServer->m_pLoginField->m_LoginCount++;
			}
			pServer->PublishServerList( sdp );
			pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Id = %lld login successful\n", userstr.c_str(), Memberid );
			post = true;
		}
		else
		{
			switch ( retcode )
			{
			case CSERR_ALREADY_LOGIN:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] request validate....double login\n", userstr.c_str() );
					// 把重複登入的踢掉
					pServer->Kickout( pServer->GetSessionDescriptor( Memberid ) );
				}
				break;
			case CSERR_ACC_NOT_EXIST:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Not registered user!!\n", userstr.c_str() );
				}
				break;
			case CSERR_BAD_PASSWD:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Bad password!\n", userstr.c_str() );
				}
				break;
			case ACCSTATE_FREEZE:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] is Banned!\n", userstr.c_str() );
				}
				break;
			case CSERR_LOGOUT_TOO_SHORT:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] logout lapsed too short!\n", userstr.c_str() );
				}
				break;
			case CSERR_EMPTY_JOIN_SERVER:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] empty server list!\n", userstr.c_str() );
				}
				break;
			case CSSERR_BAD_MAC:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Not allow NULL MAC\n", userstr.c_str() );
				}
				break;
			default:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] request validate....unknow error %d\n", userstr.c_str(), retcode );
					break;
				}
			}
			//retcode=CSERR_ACC_INVALID;
			//pServer->SetLogMessage(LOGMODE_DEFINE, " List server failure\n");
			//DebugOutMessage("lobbylogin", "login 10\n");
			if ( retcode == CSERR_ACC_UNKNOW_ERROR )
			{
				TS_Lock llock( &pServer->m_pLoginField->m_lLoginUnKnowErrorCount );
				pServer->m_pLoginField->m_LoginUnKnowErrorCount++;
			}
			else
			{
				TS_Lock llock( &pServer->m_pLoginField->m_lLoginErrorCount );
				pServer->m_pLoginField->m_LoginErrorCount++;
			}
			post = false;
		}
		CsResBuf buf;
		CsLoginMessage::Server::SetFeedbackMessage( buf, retcode );
		pServer->SendCsMessage( sdp, CsINVALID_ID, CsS_C_LOGIN_FEEDBACK, _GetResBufPtr( buf ), _GetResBufLen( buf ) );
		Pf_INFO( "Set ID:%lld login %d\n", Memberid, retcode );
		// 新的做法 由loginserver斷線
		pServer->Kickout( evocator->Sessiondescriptor );
		return -1;
	}



	//===============================================================================//
	//  logout this socket
	//
	//===============================================================================//
	int LoginServer::DoLogout( const ServerEvocator* evocator, const void* pData, unsigned int len )
	{
		CsSessionDescriptor sdp = evocator->Sessiondescriptor;
		LoginServer*     pServer = ( LoginServer* ) evocator->pserver;   //      OnRemoveSession(sdp);
		//bool ret=WaitForSingleRequest(socket, 2000);
		pServer->SetLogMessage( LOGMODE_DEFINE, " Client from %s logout\n", Net::Socket::GetIPNToA( pServer->GetSessionIpn( sdp ) ) );
		TS_Lock lock( &pServer->m_pLoginField->m_lTimeout );
		pServer->m_pLoginField->m_TimeoutSeq.erase( sdp );
		pServer->Kickout( evocator->Sessiondescriptor );
		return -1;
	}


	//===============================================================================//
	//  when server add one connect, must tell server Member id of the user
	//
	//===============================================================================//
	CSERR_CODE LoginServer::OnVerifyLogin( const tchar_t* username, const tchar_t* password )
	{
		// 運作:
		// 1. 運用 username 查出密碼以及帳號ID,停用狀態
		// 2. 檢查帳號是否已經登入?
		// 3. 檢查帳號是否是特殊帳號, 並且來源有無問題?
		// 4. 檢查帳號是否是停用中
		// 5. 檢查密碼是否正確.
		// 以上檢查只要是失敗, 就立即退出並傳回錯誤.
		int retcode = CSERR_ACC_SUCCESS;
		sessionid_t cid = CsINVALID_ID;
		if ( !DBMember::dbCheckMember( GetCommonDB( 0 ), username, password, cid ) )
		{
			cid = CsINVALID_ID;
		}
		if ( cid == CsINVALID_ID )
		{
			if ( DBMember::dbGetMemberId( GetCommonDB( 0 ), username ) == CsINVALID_ID )
			{
				retcode = CSERR_ACC_NOT_EXIST;
			}
			else
			{
				retcode = CSERR_BAD_PASSWD;
			}
		}
		else
		{
			// has entry!
			CseMEMBERSTATE result = ACCSTATE_ERROR;
			stimen_t lastlogout = 0;
			stimen_t currlogin = 0;
			stimen_t currtime = 0;
			DBMember::dbGetLoginState( GetCommonDB( cid ), cid, result, lastlogout, currlogin, currtime );
			if ( result == ACCSTATE_OFFLINE )
			{
				retcode = CSERR_ACC_SUCCESS;
			}
			else if ( result == ACCSTATE_ONLINE )
			{
				// 資料庫記錄重複登入的狀況下
				if ( GetSessionDescriptor( cid ) != NULL )
				{
					retcode = CSERR_ALREADY_LOGIN;
				}
				else //應該是資料庫沒有清乾淨
				{
					DBMember::dbSetMemberState( GetCommonDB( cid ), cid, ACCSTATE_OFFLINE );
					retcode = CSERR_ACC_SUCCESS;
				}
			}
			else if ( result == ACCSTATE_FREEZE )
			{
				retcode = CSERR_BANNED;
			}
			/*
			else if(result==ACCSTATE_LEAVING)
			{
			 retcode=CSERR_LEAVING;
			}
			*/
			else
			{
				retcode = CSERR_NOT_ALLOW;
			}
			if ( GetSCSServerSize() == 1 )
			{
				// 沒有遊戲伺服器可以進入
				retcode = CSERR_EMPTY_JOIN_SERVER;
			}
			if ( lastlogout != 0 ) // 第一次登入
			{
				if ( ( currtime - lastlogout ) < m_pLoginField->m_LessLogoutSec )
				{
					retcode = CSERR_LOGOUT_TOO_SHORT;
				}
			}
		}
		if ( retcode != CSERR_ACC_SUCCESS )
		{
			SetLogMessage( 0, "Login failure code %d\n", retcode );
		}
		return retcode;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	void LoginServer::OnExecute( void )
	{
		std::vector<CsSessionDescriptor> eraselist;
		//Sleep((server->m_pLoginField->m_TimeoutCycle/5));
		{
			TS_Lock lock( &m_pLoginField->m_lTimeout );
			std::set<CsSessionDescriptor>::iterator iter = m_pLoginField->m_TimeoutSeq.begin();
			while ( iter != m_pLoginField->m_TimeoutSeq.end() )
			{
				mstimen_t time = GetLastRecvMST( ( *iter ) );
				mstimen_t dtime = PfTimer::GetTickMSTime() - GetLastRecvMST( ( *iter ) );
				if ( ( time != 0 ) && ( ( dtime ) > ( m_pLoginField->m_TimeoutCycle ) ) )
				{
					Kickout( ( *iter ) );
				}
				iter++;
			}
		}
	}


	//===============================================================================//
	//  logout this socket
	//
	//===============================================================================//
	bool LoginServer::DoLink( const ServerEvocator* evocator, const void* pData, unsigned int len )
	{
		CsSessionDescriptor cdp = evocator->Sessiondescriptor;
		const tchar_t*      linkdata = ( const tchar_t* ) pData;
		LoginServer*        pServer = ( LoginServer* ) evocator->pserver;
		//      char errout[256];
		//      DWORD time1 = ::GetTickCount();
		char namebuf[ 512 ];
		memset( namebuf, 0, 512 );
		powerid_t   pwd = 0;
		macn_t      mac = 0;
		long        flag = 0;
		sessionid_t cid = CsINVALID_ID;
		char retbuf[ 512 ];
		if ( ( cid = pServer->OnLink( linkdata, namebuf, 512, pwd, mac, flag, retbuf, 512 ) ) == CsINVALID_ID )
		{
			return false;
		}
		int usedb = rand() % 10;
		char buff[ 1024 ];
		sprintf( buff, _T( "SELECT MemberID FROM member WHERE MemberId = '%lld'" ), cid );
		XDBResult result = pServer->GetCommonDB( usedb )->ExecQuery( buff );
		if ( pServer->GetCommonDB( usedb )->GetResultEmpty( result ) )
		{
			pServer->GetCommonDB( usedb )->FreeResult( result );
			// 建立部用驗證的帳號
			int tp = 0;
			if ( mac == 0 )
			{
				tp = 2;
			}
			sprintf( buff, _T( "INSERT INTO member( MemberId, Power, Username, Macaddress,  Flag, CurrLoginSec, LastLogoutSec ) \
							 VALUES( '%lld',   '%d',  '%s',  '%lld',  '%d',  UNIX_TIMESTAMP(NOW()), 0)" ),
					 cid, pwd, namebuf, mac, flag, tp );
			result = pServer->GetCommonDB( usedb )->ExecQuery( buff );
			pServer->GetCommonDB( usedb )->FreeResult( result );
		}
		else
		{
			pServer->GetCommonDB( usedb )->FreeResult( result );
			sprintf( buff, _T( "UPDATE member SET Power = '%d', \
							 Username = '%s', \
							 Macaddress = '%lld', \
							 CurrLoginSec = UNIX_TIMESTAMP(NOW()) WHERE MemberId = '%lld'" ), pwd, namebuf, mac, cid );
			result = pServer->GetCommonDB( usedb )->ExecQuery( buff );
			pServer->GetCommonDB( usedb )->FreeResult( result );
		}
		/////////
		CSERR_CODE retcode = CSERR_ACC_SUCCESS;
		if ( retcode == CSERR_ACC_SUCCESS )
		{
			int mtype = 0;
			{
				macn_t  macadd = 0;
				int  state = 0;
				stimen_t stime = 0;
				//DBMember::dbGetCheckQualification(GetCommonDB(cid), namebuf, state, macadd, mtype, stime);
				DBMember::dbGetCheckQualification( pServer->GetCommonDB( cid ), cid, state, macadd, mtype, stime );
			}
			macn_t checkmacadd = GetSessionMac( cdp );
			if ( ( mtype != 2 ) && ( checkmacadd == 0 ) ) // 要檢查MAC 不允許暱MAC登入
			{
				retcode = CSSERR_BAD_MAC;
			}
			else
			{
				// 假如該帳號之前已經有人連線 先踢除除
				CsSessionDescriptor oldced = pServer->GetSessionDescriptor( cid );
				if ( oldced )
				{
					retcode = CSERR_ALREADY_LOGIN;
				}
				else
				{
					// TODO: 2010 1 25
					// 先設定為 offline 並填入 登入時間
					// 如果game server 再登入時間的差距 幾分鐘內 就允許登入
					DBMember::dbSetMemberAllowLogin( pServer->GetCommonDB( cid ), cid, checkmacadd );
					pServer->SetLogMessage( LOGMODE_DEFINE, " Create Member login %lld:%s succsssful\n", cid, Net::Socket::GetMACNToA( checkmacadd ) );
					// 設定ID給client
					pServer->BindSession( cdp, cid );
				}
			}
		}
		if ( retcode != CSERR_ACC_SUCCESS )
		{
			//DoInitJoinSession(sdp, errcid, GetSessionMac(sdp));
		}
		else
		{
		}
		// 可以強制改變
		retcode = pServer->OnLogin( cid, retcode );
		bool post = false;
		if ( retcode == CSERR_ACC_SUCCESS )
		{
			{
				TS_Lock llock( &pServer->m_pLoginField->m_lLoginCount );
				pServer->m_pLoginField->m_LoginCount++;
			}
			pServer->PublishServerList( cdp );
			pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Id = %lld login successful\n", namebuf, cid );
			post = true;
		}
		else
		{
			switch ( retcode )
			{
			case CSERR_ALREADY_LOGIN:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] request validate....double login\n", namebuf );
					// 把重複登入的踢掉
					pServer->Kickout( pServer->GetSessionDescriptor( cid ) );
				}
				break;
			case CSERR_ACC_NOT_EXIST:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Not registered user!!\n", namebuf );
				}
				break;
			case CSERR_BAD_PASSWD:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Bad password!\n", namebuf );
				}
				break;
			case ACCSTATE_FREEZE:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] is Banned!\n", namebuf );
				}
				break;
			case CSERR_LOGOUT_TOO_SHORT:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] logout lapsed too short!\n", namebuf );
				}
				break;
			case CSERR_EMPTY_JOIN_SERVER:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] empty server list!\n", namebuf );
				}
				break;
			case CSSERR_BAD_MAC:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] Not allow NULL MAC\n", namebuf );
				}
				break;
			default:
				{
					pServer->SetLogMessage( LOGMODE_DEFINE, " Player : [%s] request validate....unknow error %d\n", namebuf, retcode );
					break;
				}
			}
			//retcode=CSERR_ACC_INVALID;
			//pServer->SetLogMessage(LOGMODE_DEFINE, " List server failure\n");
			//DebugOutMessage("lobbylogin", "login 10\n");
			if ( retcode == CSERR_ACC_UNKNOW_ERROR )
			{
				TS_Lock llock( &pServer->m_pLoginField->m_lLoginUnKnowErrorCount );
				pServer->m_pLoginField->m_LoginUnKnowErrorCount++;
			}
			else
			{
				TS_Lock llock( &pServer->m_pLoginField->m_lLoginErrorCount );
				pServer->m_pLoginField->m_LoginErrorCount++;
			}
			post = false;
		}
		CsResBuf buf;
		CsLoginMessage::Server::SetFeedbackMessage( buf, retcode );
		pServer->SendCsMessage( cdp, CsINVALID_ID, CsS_C_LOGIN_FEEDBACK, _GetResBufPtr( buf ), _GetResBufLen( buf ) );
		Pf_INFO( "Set ID:%lld login %d\n", cid, retcode );
		// 新的做法 由loginserver斷線
		pServer->Kickout( evocator->Sessiondescriptor );
		return -1;
	}

	//===============================================================================//
	//
	//  封鎖起來不給gameserver的user使用
	//===============================================================================//
	// register the user define message channel
	/*
	bool LoginServer::RegisterUserDefChannel(channelid_t id, ServerMessageChannelFunction func)
	{
	   return Server::RegisterUserDefChannel(id, func);
	}
	*/




	//===============================================================================//
	//
	//
	//===============================================================================//
	//  void __cdecl LoginServer::TryToTimeout(void* data)
	//  {
	//      PfTimer  LobbyTimeoutClock;
	//      LoginServer* server = (LoginServer*)data;
	//      while (!server->m_pLoginField->m_bTimeoutClose)
	//      {
	//          std::vector<CsSessionDescriptor> eraselist;
	//          Sleep((server->m_pLoginField->m_TimeoutCycle/5));
	//          {
	//              TS_Lock lock(&server->m_pLoginField->m_lTimeout);
	//              std::set< CsSessionDescriptor >::iterator iter=server->m_pLoginField->m_TimeoutSeq.begin();
	//              while(iter!=server->m_pLoginField->m_TimeoutSeq.end())
	//              {
	//                  mstimen_t time=server->GetLastRecvMST((*iter));
	//                  if((time!=0)&&
	//                      ((LobbyTimeoutClock.GetTickMSTime()-time)>server->m_pLoginField->m_TimeoutCycle))
	//                  {
	//                      server->Kickout((*iter));
	//                  }
	//                  iter++;
	//              }
	//          }
	//      }
	//  }






};

