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
//////////////////////////////////////////////////////////////////////////

//01.== C 系統文件/C++ 系統文件
#include <time.h>
#include <assert.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-core_server.h"
#include "cs-utility.h"
#include "cs-log_file_service.h"

namespace cyberspace
{
    uint_t  CoreServer::s_num_startup_server_ = 0;
    int     g_log_index = 0;
    //cyberspace::OutletExchange CoreServer::s_OutletExchange;

    namespace
    {
        // rember which command was be used for debug
        int       g_update_stage = 0; //for debug
        //int UpdateMsg = 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    CoreServer::CoreServer( UnitServer* server )
    {
        //state_.Set_Iinitate_Mstime( 0 );
        //memset( &m_tServerReport, 0, _cssizeof( ServerInfo ) );
        //m_bLogThread = false;
        v_thread_log_ = NULL;
        // 不同步接收資料
        is_base_sync_ = false;
        v_scssystem_ = new cyberspace::scs::System( this );
        v_aribt_agent_ = new ArbitrationClient( this );
        v_unitserver_ = server;
        SET_BIT_FLAG( log_mode_, LOGMODE_OUTDEBUG );
        SET_BIT_FLAG( log_mode_, LOGMODE_SAVEFILE );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    CoreServer::~CoreServer()
    {
        SAFE_DELETE( v_scssystem_ );
        SAFE_DELETE( v_aribt_agent_ );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int CoreServer::StartUp( unsigned int allowplayers, unsigned short port )
    {
        //PfDebug::ShowConsole(100, 100);
        net::TCPServer::Create();
        int ret = net::TCPServer::StartUp( allowplayers, port );

        if( ret == 1 )
        {
            v_thread_log_ = new ThreadLog;
            //pfkrl::Thread::BeginProcess(CoreServer::DoUpdateTimerOperator, this);
            s_num_startup_server_++;
            state_.Set_Startup_Stime( pftml::Time::GetSTime() );
            //time( &stime_startup_ );
            struct tm* p;
            p = localtime( NULL );
            mkdir( "log" );
            tchar_t filename[ _MAX_PATH ];
            sprintf( filename, "log/cs-%s_History_%d_%d_%d_%d_%d_%d.log", v_unitserver_->ObjectName(),
                p->tm_year + 1900,
                p->tm_mon + 1,
                p->tm_mday,
                p->tm_hour,
                p->tm_min,
                p->tm_sec );
            cyberspace::g_log_index = cyberspace::LogFileSerivce::Open( filename );
            socket_supervis_.DoStartUp( v_unitserver_->ObjectName(), state_.Get_Startup_Stime() );
            threads_handle_.push_back( pfkrl::Thread::BeginProcess( CoreServer::DoTestTimeoutConnect, this, _T( "CoreServer::StartUp::DoTestTimeoutConnect" ) ) );
            threads_handle_.push_back( pfkrl::Thread::BeginProcess( CoreServer::DoExecute, this, _T( "CoreServer::StartUp::DoExecute" ) ) );
        }
        return ret;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int CoreServer::Shutdown( void )
    {
        if( GetFD() == 0 )
        {
            return 0;
        }

        int ret = 0;
        {
            socket_supervis_.DoClose();
            //TS_Lock lock(&m_lValidateConnect);
            //m_tVoidConnectList.clear();
            //m_tRiskAddressList.clear();
        }
        {
            paper_system_.Shutdown();
        }
        SAFE_DELETE( v_thread_log_ );

        //int ret = 0;
        //ret = net::TCPServer::Shutdown();
        if( ret == 1 )
        {
            s_num_startup_server_--;
        }

        cyberspace::LogFileSerivce::Close( cyberspace::g_log_index );
        ret = net::TCPServer::Shutdown();

        while( 1 )
        {
            if( pfkrl::Thread::TestProcessExit( threads_handle_ ) )
            {
                break;
            }

            pfstd::MsSleep( 100 );
        }

        //SCSDestroy();
        return ret;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int CoreServer::SCSStartUp( tchar_t* arbit_host, ushort_t arbit_port, ushort_t scs_port )
    {
        TS_Lock lock( &coreserver_ut_lock_ );

        if( v_scssystem_ == NULL )
        {
            return -1;
        }

        if( v_aribt_agent_ == NULL )
        {
            return -1;
        }

        if( v_scssystem_->StartUp( scs_port ) == 1 )
        {

            return v_aribt_agent_->StartUp( arbit_host, arbit_port );
        }

        return -1;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int CoreServer::SCSShutdown()
    {
        if( v_scssystem_ != NULL )
        {
            v_scssystem_->Shutdown();
        }

        if( v_aribt_agent_ == NULL )
        {
            v_aribt_agent_->Shutdown();
        }
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void CoreServer::GetAllLocalSessionDescriptor( std::vector<CsSessionDescriptor>& CsPSockConnlist )
    {
        std::vector<net::TCPSocket*> socketlist;
        GetAllConnectedSocket( socketlist );

        for( uint_t i = 0; i < socketlist.size(); i++ )
        {
            CsPSockConnlist.push_back( SkToCd( socketlist[ i ] ) );
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void CoreServer::GetLocalServerReport( cyberspace::ServerState& sinfo )
    //{
    //    TS_Lock  syslock1( &m_lSystemNum[ _INITATE_MSTIME ] );
    //    TS_Lock  syslock2( &m_lSystemNum[ _MAX_PROCESS_CHANNEL ] );
    //    TS_Lock  syslock3( &m_lSystemNum[ _MAX_UPDATE_MSTIME ] );
    //    TS_Lock  syslock4( &m_lSystemNum[ _MAX_CONNECTED_CLIENT ] );
    //    //TS_Lock  syslock5( &m_lSystemNum[ _LIVING_THREAD_COUNT ] );
    //    TS_Lock  syslock6( &m_lSystemNum[ _CNT_LONG_PROCESS ] );
    //    //TS_Lock  syslock7( &m_lSystemNum[ _SUMOFRECV_COUNT ] );
    //    memcpy( &sinfo, &m_tServerReport, _cssizeof( cyberspace::ServerState ) );
    //    sinfo.cnt_living_thread = GetLivingThreadSize();
    //    sinfo.limit_allow_connected = GetLimitConnectedSize();
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    void CoreServer::SetThreadHistory( bool t )
    {
        if( v_thread_log_ )
        {
            v_thread_log_->SetLogThread( t );
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    cs_size_t CoreServer::GetThreadHistory( std::vector<channel_uid_t>& chrd, std::vector<mstimen_t>& tmrd )
    {
        if( v_thread_log_ )
        {
            return v_thread_log_->GetThreadLog( chrd, tmrd );
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int CoreServer::Update( void )
    {
        // 自己做的事 應該自己負責 所以 把 這些 都是自己做的工作
        // 都擺在自己呼叫的 function中
        pfstd::MsSleep( 1000 );
        return 1;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void CoreServer::DoExecute( void* data )
    {
        CoreServer* server = ( CoreServer* )data;

        while( server->GetFD() )
        {
            //int updatesize = 0;
            // 由這邊來執行是為了保證
            // SCSconnect 做資源釋放時 不會被Update
            //updatesize += ( ( cyberspace::Converger* )server )->Update();
            server->v_unitserver_->OnExecute();
            pfstd::MsSleep( _MIN_SLEEP );
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void CoreServer::DoTestTimeoutConnect( void* data )
    {
        CoreServer* server = ( CoreServer* )data;

        while( server->GetFD() )
        {
            server->socket_supervis_.Update( server );
            pfstd::MsSleep( 100 );
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void CoreServer::BindSession( CsSessionDescriptor sdp, cs_session_uid_t sessionid )
    //{
    //    CsAbtpCnSocket* scn = OutletExchange::CdToSc( sdp );
    //    scn->SetSessionUid( sessionid );
    //}

    //=============================================================================
    //
    //! Key為上層使用的資料, 設定連結傳回舊的key 值
    //=============================================================================
    //CsSessionNote* CoreServer::SetSessionNote( CsSessionDescriptor cdp, CsSessionNote* key )
    //{
    //    return cyberspace::Converger::SetSessionNote( cdp, key );
    //}
    //
    //void CoreServer::DelSessionNote( CsSessionDescriptor cdp, CsSessionNote* key )
    //{
    //    cyberspace::Converger::DelSessionNote( cdp, key );
    //}

    //=============================================================================
    //
    //! Key為上層使用的資料, 取得連結
    //=============================================================================
    //CsSessionNote* CoreServer::GetSessionNote( CsSessionDescriptor cdp, notetid_t notetid )
    //{
    //    return cyberspace::Converger::GetSessionNote( cdp, notetid );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    net::TCPSocket* CoreServer::OnNewSocket( void )
    {
        CsAbtpCnSocket* psockconn = new CsAbtpCnSocket();
        return psockconn;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void CoreServer::OnDelSocket( net::TCPSocket* s )
    {
        CsAbtpCnSocket* scn = SkToSc( s );
        SAFE_DELETE( scn );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void CoreServer::OnAccept( net::TCPSocket* skt )
    {
        if( state_.Get_Iinitate_Mstime() == 0 )
        {
            return;
        }

        //CsAbtpCnSocket* scn = SkToSc( skt );
        //TS_Lock lock(&scn->coreserver_ut_lock_);
        //scn->Initate();
        skt->Set_tcp_buf_size( SO_RCVBUF, _MAX_SOCKET_BUFF );
        skt->Set_tcp_buf_size( SO_SNDBUF, _MAX_SOCKET_BUFF );
        //tchar_t addr[32];
        // 看樣子一定要再這邊寫IP
        sockaddr_storage addr_storage;
        sockaddr* addr = skt->GetPeerAddr( addr_storage );
        ipa_t     ipa = net::Socket::GetSockaddrIPA( addr );
        ushort_t  port = net::Socket::GetSockaddrPort( addr );

        // 安全性檢查
        if( !socket_supervis_.DoAccept( skt ) )
        {
            v_unitserver_->OutputMessage( log_mode_, _T( "Deny connect from %s:%d\n" ), ipa.c_str(), port );
            net::TCPServer::Kickout( skt );
            return;
        }

        state_.Set_Max_Connected_Client( this );
        time_t timep = pftml::Time::GetSTime();
        //time( &timep );
        //long32_t md = 0;
        v_unitserver_->OutputMessage( log_mode_, _T( "Connect client %d from %s:%d on %s  online: current [%d/%d]  max [%d]\n" ),
            skt->GetFD(), ipa.c_str(), port, asctime( localtime( &timep ) ), GetConnectedSize(), GetLimitConnectedSize(), state_.Get_Max_Connected_Client() );
        return;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void CoreServer::OnDisconnect( net::TCPSocket* skt )
    //{
    //    // 因為已經在Kickout裏面值皆做 所以應該部用鎖
    //    // 否則如果在UPDAT裏面呼叫Kickout 就會卡住
    //    // 如果 OnSCSconnect 是在kicking序裏面 這邊就要鎖
    //    // 如果不是 就不能鎖
    //    // 2009 10 28
    //    // 試著擺在 Kickout裏面做
    //    // 2009 10 31
    //    // 改成 kicking的做法
    //    // 因為 這邊 跟update之間 還是用 唯一個 CoreServer::s_utLock 的lock來鎖 應該會比較不容易當機
    //    // 原本的想法是 如果一個帳號在 斷線登出的同時 又被登入 可能會 照成 取用entity的問題
    //    // 但其實這個狀況 只鎖 socket的utlock是沒有用的 因為 此時登入的socket 不可能跟登出的socket 雖然邏輯上是同一個ID 卻不會是同一個socket
    //    // 因此 還是整個鎖起來比較保險
    //    PeerSocket* scn = SkToSc( skt );
    //    CsSessionDescriptor sdp = scn->GetDescriptor();
    //    cs_session_uid_t lsessionid = v_unitserver_->GetSessionSessionUid( sdp );
    //    TS_Lock lock( &m_utBaseServerLock[lsessionid % 1000] );
    //
    //    if ( sdp )
    //    {
    //        v_unitserver_->OnClientKickout( sdp );
    //    }
    //
    //    // 2011 07 18 從這邊移除 以免DoTestTimeout 發生 deadlock
    //    socket_supervis_.DoDisconnect( skt );
    //    // 2011 07 17 改到 kickout 裡面做
    //    //{
    //    //TS_Lock lock(&m_utBaseServerLock);
    //    scn->SetDescriptor( NULL );
    //
    //    if ( sdp )
    //	{
    //		cyberspace::Converger::SCSDeleteSessionDescriptor( sdp );
    //	}
    //
    //    //}
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //int CoreServer::Kickout( net::TCPSocket* skt )
    void CoreServer::OnKickout( net::TCPSocket* skt )
    {
        /*
        // kickout 的內容
        if ( skt == NULL )
        {
            assert( skt == NULL && _T( "kickout cant be NULL" ) );
        }

        // 只有 本機有
        //TS_Lock lock( &m_InOutLock );
        //TS_Lock lock(&m_KickoutLock);
        if ( skt->GetFD() == 0 )
        {
            return 0;
        }

        CsSessionDescriptor sdp = SkToCd( skt );
        return Kickout( sdp );
        */
        // 2011 07 18 從這邊移除 以免DoTestTimeout 發生 deadlock
        socket_supervis_.DoKickout( skt );
        CsAbtpCnSocket* scn = SkToSc( skt );
        CsSessionDescriptor sdp = scn->GetDescriptor();
        Pf_INFO( "remove session %%llu\n", cyberspace::CoreServer::GetSessionSessionUid( sdp ) );
        _csbinbuf buf;
        CsBaseCommand::ServerKickout::Server::SetRequest( buf );
        SendPacket( sdp, CsINVALID_ID, CsS_C_KICKOUT_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
        sockaddr_storage addr_storage;
        sockaddr* addr = skt->GetPeerAddr( addr_storage );
        ipa_t     ipa = net::Socket::GetSockaddrIPA( addr );
        ushort_t  port = net::Socket::GetSockaddrPort( addr );
        time_t timep = pftml::Time::GetSTime();
        //time( &timep );
        v_unitserver_->OutputMessage( log_mode_, _T( "Kickout client %d from %s:%d on %s  online: current [%d/%d]  max [%d]\n" ),
            skt->GetFD(), ipa.c_str(), port, asctime( localtime( &timep ) ), GetConnectedSize(), GetLimitConnectedSize(), state_.Get_Max_Connected_Client() );
        if( v_unitserver_ )
        {
            v_unitserver_->OnClientKickout( sdp );
        }

        cyberspace::CoreServer::DelectSession( sdp );
    }

    //=============================================================================
    //
    //
    //===============================================================================/
    // 會紀錄的踢除
    int CoreServer::Kickout( CsSessionDescriptor sdp )
    {
        if( sdp == NULL )
        {
            return 0;
        }

        //TS_Lock lock( &m_InOutLock );
        //if ( cyberspace::Converger::SCSGetSessionStateBit( sdp, _CONNECTNODE_KICKOUT ) )
        //{
        //    return 0;
        //}
        //
        //cyberspace::Converger::SCSSetSessionStateBit( sdp, _CONNECTNODE_KICKOUT, true );
        int ret = 0;
        // 是一個請求別人去做 clone不用真的踢除
        net::TCPSocket* skt = CdToSk( sdp );

        if( skt )
        {
            ret = net::TCPServer::Kickout( skt );
        }
        else
        {
            // 通知該機器 剔除就好
            Pf_INFO( "remove clone connect %llu\n", cyberspace::CoreServer::GetSessionSessionUid( sdp ) );
            cyberspace::CoreServer::DelectSession( sdp );
        }

        // TODO : Pin
        //if ( cyberspace::Converger::SCSGetSessionIndex( sdp ) != 0 )
        {
            //cs_session_uid_t sessionid = cyberspace::CoreServer::GetSessionSessionUid( sdp );
            //DBHistory::dbEndMemberHistory( v_unitserver_->GetCommonDB( sessionid ), cyberspace::Converger::SCSGetSessionIndex( sdp )/*, duration_sec*/ );
        }
        return ret;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void CoreServer::OnReceive( net::TCPSocket* skt )
    {
        CsAbtpCnSocket* revconn = SkToSc( skt ); // 這裡進來的 一定是 PeerSocket

        // server未完成 初始化
        if( state_.Get_Iinitate_Mstime() == 0 )
        {
            Kickout( revconn->GetDescriptor() );
            return;
        }

        revconn->state_channel_uid_ = 0;
        //revconn->m_StateStage = 0;
        uniqueid_t   uid = CsINVALID_ID;
        channel_uid_t  command_uid = 0;
        ubyte_t*     recvbuff = NULL;
        unsigned int CommandLen = 0;
        //mstimen_t  procmst = 0;
        state_.Add_Cnt_Recv_Message();

        if( revconn->RecvPacket( uid, command_uid, &recvbuff, CommandLen ) >= 0 )
        {
            //char debugbuff[512];
            //PfOutputDebugString(debugbuff);
            mstimen_t currmst = pftml::Time::GetTickMSTime();
            revconn->state_channel_uid_ = command_uid;
            //revconn->m_StateStage = -1;
            // 紀錄有哪些Thread 在執行 幾號的channel
            cyberspace::ThreadLog::ChannelLog* chrd = NULL;

            if( v_thread_log_ )
            {
                v_thread_log_->InsertChannel( command_uid, currmst );
            }


            //if ( cdp )
            //{
            /*
            Login有可能
            if(revconn->GetCId()==CsINVALID_ID)
            {
            char buff[512];
            sprintf(buff, "Can't find Invalid ConnectId ChannelId = %d, UID=%llu!!\n", command_uid, uid);
            DebugOutPutf(buff, "CoreServer::OnReceive");
            Kickout(skt);
            }
            */
            // 沒有認證前(revconn->m_bValidation=true)，只能做認證，否則踢掉
            switch( command_uid )
            {
            case CsC_S_CONNECT_REQUEST:
                {
                    /*
                    {
                    TS_Lock vclock(&m_lValidateConnect);
                    m_tVoidConnectList.erase(revconn);
                    // 送了驗證的封包上來 所以 從高風險的ip列表中移除
                    ulonglong_t fipn = skt->GetInternetAddress().S_un.S_addr;
                    m_tRiskAddressList.erase(fipn);
                    }
                    */
                    // 這邊是>= 因為 有塞假資料
                    if( CommandLen >= ( _cssizeof( cs_session_uid_t ) + _cssizeof( macn_t ) + _cssizeof( dword_t ) ) )
                    {
                        cs_session_uid_t sessionid;
                        macn_t      macaddr;
                        dword_t     ver = 0;
                        CsBaseCommand::ActiveConnect::Server::GetRequest( recvbuff, sessionid, macaddr, ver );
                        //#ifdef _DEBUG
                        sessionid = revconn->GetSessionUid();
                        maca_t maca = net::Socket::MACNToA( macaddr );
                        // set log
                        {
                            sockaddr_storage addr_storage;
                            sockaddr* addr = skt->GetPeerAddr( addr_storage );
                            ipa_t     ipa = net::Socket::GetSockaddrIPA( addr );
                            ushort_t  port = net::Socket::GetSockaddrPort( addr );
                            char infobuff[ _MAX_MSG ];
                            sprintf( infobuff, _T( " [ SESSION_ID = %llu ][ MAC = %02X-%02X-%02X-%02X-%02X-%02X ][ IP = %s:%d ]" ), sessionid, maca[ 0 ], maca[ 1 ], maca[ 2 ], maca[ 3 ], maca[ 4 ], maca[ 5 ], ipa.c_str(), port );
                            v_unitserver_->OutputMessage( log_mode_, infobuff );
                        }

                        // 確保 clinet 與 server 版本相同的第一次檢查
                        if( ver != v_unitserver_->OnGetVersion() )
                        {
                            char infobuff[ _MAX_MSG ];
                            sprintf( infobuff, "SESSION_ID = %llu Version not match, Kickout", sessionid );
                            v_unitserver_->OutputMessage( log_mode_, infobuff );
                            //bkickout=true;
                            net::TCPServer::Kickout( skt );
                        }
                        else
                        {
                            // 已經 加入過了==> 先關閉之前的連線
                            // 同一個CID只能有一條連線
                            // 如果有重複 後連的 會把先連的踢掉
                            // 2. 其他的SERVER 從收到加入通知的地方去 踢 這樣 就不用多送一次 Kickout的訊息
                            bool bconnectf = false;
                            //cyberspace::Converger::FullCsPSockConn( SkToSc( skt )/*, sessionid, macaddr*/ );

                            if( v_unitserver_->OnClientAuthenication( sessionid ) )
                            {
                                bconnectf = true;
                                if( !IsCsInvalidId( sessionid ) )
                                {
                                    CreateSession( sessionid, SkToSc( skt ) );
                                }
                                // 從連線 的佇列中移除
                                socket_supervis_.SetSocketKeepAlive( skt );
                            }

                            if( bconnectf )
                            {
                                v_unitserver_->OnClientConnect( revconn->GetDescriptor() );
                            }
                            else
                            {
                                net::TCPServer::Kickout( revconn );
                            }
                        }
                    }
                    else
                    {
                        net::TCPServer::Kickout( revconn );
                    }
                }
                break;

            default:
                {
                    //v_unitserver_->Kickout(GetCsSessionDescriptor(revconn));
                    CsSessionDescriptor cdp = ScToCd( revconn );
                    if( cdp != NULL )
                    {
                        if( v_unitserver_->OnRecvDefineRequest( cdp, uid, command_uid, recvbuff, CommandLen ) <= 0 )
                        {
                            revconn->num_check_lost_channel_++;
                        }
                        else
                        {
                            revconn->num_check_lost_channel_ = 0;
                        }
                    }
                    //socket_supervis_.WriteRiskAddress( skt, 1 );
                    //net::TCPServer::Kickout( skt );
                }
                break;
            }

            mstimen_t runtime = pftml::Time::GetTickMSTime() - currmst;
            // 紀錄花費最多時間的 訊號 跟 時間
            state_.Set_Max_Proc_Channel( command_uid, runtime );
            // 處理超過一秒的
            state_.Add_Cnt_Long_Process();
            //}

            if( v_thread_log_ )
            {
                v_thread_log_->RemoveChannel( chrd );
            }

            if( !revconn->CheckRecvLegal() )
            {
                // 判定為有攻擊嫌疑的封包
                //socket_supervis_.WriteRiskAddress( skt, 2 );
                v_unitserver_->OutputMessage( log_mode_, "Warring: Client Id: %llu try to send message frequency_count [ <%d mstime,   >%d count ], server Kickout\n",
                    cyberspace::CoreServer::GetSessionSessionUid( ScToCd( revconn ) ), CsAbtpCnSocket::max_recv_headway_, CsAbtpCnSocket::max_num_jam_message_ );
                net::TCPServer::Kickout( skt );
            }

            //}
        }
        else
        {
            //socket_supervis_.WriteRiskAddress( skt, 3 );
            v_unitserver_->OutputMessage( log_mode_, "Warring: Error message packet from %s Kickout it!\n",
                net::Socket::IPN6ToA6( cyberspace::CoreServer::GetSessionIpn6( ScToCd( revconn ) ) ).c_str() );
            net::TCPServer::Kickout( skt );
        }

        //
        //revconn->m_StateStage = -2;
        //SAFE_DELETE_ARRAY(recvbuff);
    }


    //////////////////////////////////////////////////////////////////////////
    // scs
    //=============================================================================
    //
    //
    //=============================================================================
    //void CoreServer::OnSCSRecvDefineRequest( uniqueid_t who, channel_uid_t cid, const ubyte_t* command, cs_size_t len )
    //{
    //    scs::ServerEvocator evocator;
    //    evocator.uniqueid = who;
    //    evocator.pserver = v_unitserver_;
    //    evocator.sectionid = GetSessionSectionId( GetSessionDescriptor( (cs_session_uid_t)who ) );
    //    m_SCSMessageDispatcher.DispatchMessage( &evocator, cid, command, len );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    /*
    void CoreServer::WriteRiskAddress(net::TCPSocket* skt, int errnum)
    {
    if(skt==NULL)
    return;

    if(skt->GetFD()==NULL_SOCKET)
    return;

    ipn4_t   ipn;
    u_short port=0;
    skt->GetInternetAddress(ipn, port);
    stimen_t timep;
    time(&timep);
    char buff[_MAX_MSG];
    sprintf(buff, "risk client errno = %d from address %s:%d on %s\n", errnum, net::Socket::IPN4ToA4(ipn).c_str(), port, asctime(localtime(&timep)));
    printf(buff);

    TS_Lock lock(&m_RiskAddressFileLock);
    FILE* f = fopen(risk_address_fname_, "a+b");
    if(f)
    {
    fwrite(buff, _csstrlen(buff), 1, f);
    fclose(f);
    }
    */
}