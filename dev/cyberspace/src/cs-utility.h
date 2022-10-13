//
//
//
//
//  各種輔助的類別
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

#pragma once

//01.== C 系統文件/C++ 系統文件
#include <vector>
#include <set>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-time.h>
#include <net/net-server.h>
#include <template/vmapl.inl>
#include <template/collectpool.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-entity_def.h"
#include "cs-abtp_cn_socket.h"


namespace cyberspace
{
    //////////////////////////////////////////////////////////////////////////
    //
    // socket 防護物件
    // 1. 連線過久 沒有認證
#define _MAX_RISK_INSECIRE   10

    class SocketSupervise
    {
        T_Lock                                insecure_lock_[ _MAX_RISK_INSECIRE ];         // 連線驗證的 lock 下面兩個共用
        std::map<net::TCPSocket*, mstimen_t>  insecure_ctnr_[ _MAX_RISK_INSECIRE ];
        mstimen_t                             keepalive_mstime_[ _MAX_RISK_INSECIRE ];     // time out 的機制  為了不想有一個同時鎖住的lock
        std::vector<ushort_t>                 allow_remote_port_[ _MAX_RISK_INSECIRE ];

        _cs_char_t                               risk_address_fname_[ _MAX_FNAME ];
        mstimen_t                             risk_log_time_;          // 多久寫一次記錄檔
        _mbsvector                            risk_log_text_;
        //T_Lock                                m_RiskAddressFileLock;  // 記錄高風險ip的記錄檔
        //std::map<ipa_t, int>                  m_tRiskAddressList;     // 記錄高風險的ip 該列表中的ip 一次只能加入一個


        mstimen_t                             update_time_;             // 一秒檢查一次好了

    public:

        SocketSupervise()
        {
            SetKeepAliveMStime( 5000 );
            //risk_address_ = 1;
            risk_log_time_ = pftml::Time::GetTickMSTime();
            update_time_ = pftml::Time::GetTickMSTime();
            //allow_remote_port_.clear();
            strcpy( risk_address_fname_, "" );
        }

        ~SocketSupervise()
        {
        }

        void DoStartUp( const char* servername, time_t startupsec )
        {
            struct tm* p;
            p = localtime( &startupsec );
            sprintf( risk_address_fname_, "log/cs-%s_risk-address_%04d%02d%02d_%02d%02d%02d.log", servername,
                p->tm_year + 1900,
                p->tm_mon + 1,
                p->tm_mday,
                p->tm_hour,
                p->tm_min,
                p->tm_sec );
        }

        void DoClose()
        {
            for( int i = 0; i < _MAX_RISK_INSECIRE; i++ )
            {
                TS_Lock lock( &insecure_lock_[ i ] );
                insecure_ctnr_[ i ].clear();
                allow_remote_port_[ i ].clear();
            }
            //m_tRiskAddressList.clear();

        }

        bool DoAccept( net::TCPSocket* skt )
        {
            sockaddr_storage addrin;
            sockaddr*    addr = skt->GetPeerAddr( addrin );
            ipa_t        fipn = net::Socket::GetSockAddrIPA( addr );
            ushort_t     port = net::Socket::GetSockAddrPort( addr );
            //ulonglong_t  fipn = net::Socket:: ipa.s_addr;
            {
                int sidx = skt->GetUID() % _MAX_RISK_INSECIRE;

                TS_Lock lock( &insecure_lock_[ sidx ] );

                // 鎖住來源port
                for( _cs_size_t pi = 0; pi < allow_remote_port_[ sidx ].size(); pi++ )
                {
                    if( port != allow_remote_port_[ pi ][ sidx ] )
                    {
                        return false;
                    }
                }

                // 放入檢查駔佇列中
                if( keepalive_mstime_[ sidx ] > 0 )
                {
                    insecure_ctnr_[ sidx ].insert( std::map<net::TCPSocket*, mstimen_t>::value_type( skt, pftml::Time::GetTickMSTime() ) );
                }
            }
            //if( risk_address_ > 0 )
            {
                // 太複雜又囉嗦 拿掉
                //std::map<ipa_t, int>::iterator it = m_tRiskAddressList.find( fipn );
                //if ( it != m_tRiskAddressList.end() )
                //{
                //    // 目前的加入次數 大於限制次數
                //    // 目前該IP 一次只能限制1的連線 同時存在 但未作驗證的動作
                //    if ( it->second > 0 )
                //    {
                //        //v_unitserver_->SetMessage(LOGMODE_PRINTOUT, ("Deny connect from %s:%d, %d times\n"), net::Socket::IPN4ToA4(ipn).c_str(), port, it->second);
                //        return false;
                //    }
                //    // 小於限制次數 讓你過
                //    else
                //    {
                //        it->second = 1;
                //    }
                //}
                //insecure_ctnr_.insert( std::map<net::TCPSocket*, mstimen_t>::value_type( skt, pftml::Time::GetTickMSTime() ) );
            }

            return true;
        }

        void DoKickout( net::TCPSocket* skt )
        {
            sockaddr_storage addr_storage;
            sockaddr*    addr = skt->GetPeerAddr( addr_storage );
            ipa_t        fipn = net::Socket::GetSockAddrIPA( addr );
            ushort_t     port = net::Socket::GetSockAddrPort( addr );
            //ulonglong_t fipn   = ipn.s_addr;
            //CsSSession* scn = cyberspace::SkToSc(skt);
            {
                int sidx = skt->GetUID() % _MAX_RISK_INSECIRE;
                TS_Lock lock( &insecure_lock_[ sidx ] );
                insecure_ctnr_[ sidx ].erase( skt );
            }
            // 檢查是不是在 高風險 ip 中
            //std::map<ipa_t, int>::iterator it = m_tRiskAddressList.find( fipn );
            //if ( it != m_tRiskAddressList.end() )
            //{
            //    it->second = 0;
            //}
        }

        // 驗證的連線，將他從要檢查的佇列中移除
        void SetSocketKeepAlive( net::TCPSocket* skt )
        {
            {
                int sidx = skt->GetUID() % _MAX_RISK_INSECIRE;
                TS_Lock vclock( &insecure_lock_[ sidx ] );
                insecure_ctnr_[ sidx ].erase( skt );
            }

            // 送了驗證的封包上來 所以 從高風險的ip列表中移除
            sockaddr_storage addr_storage;
            sockaddr*   addr = skt->GetPeerAddr( addr_storage );
            ipa_t       fipn = net::Socket::GetSockAddrIPA( addr );
            //ulonglong_t fipn = ipn.s_addr;
            //m_tRiskAddressList.erase( fipn );
        }

        //設多久沒有驗證 就視為 危險的連線kickout
        void SetKeepAliveMStime( mstimen_t t )
        {
            for( int i = 0; i < _MAX_RISK_INSECIRE; i++ )
            {
                TS_Lock lock( &insecure_lock_[ i ] );
                if( t == 0 )
                {
                    insecure_ctnr_[ i ].clear();
                }
                keepalive_mstime_[ i ] = t;
            }
        }

        uint_t GeInsecureSize()
        {
            uint_t ss = 0;
            for( int i = 0; i < _MAX_RISK_INSECIRE; i++ )
            {
                TS_Lock lock( &insecure_lock_[ i ] );
                ss += insecure_ctnr_[ i ].size();
            }
            return ss;
        }

        //  設定可以連線上來的port 連上來的client 必須綁定這在個port出發
        void InsertAllowRemotePort( ushort_t p )
        {
            for( int i = 0; i < _MAX_RISK_INSECIRE; i++ )
            {
                TS_Lock lock( &insecure_lock_[ i ] );
                allow_remote_port_[ i ].push_back( p );
            }
        }

        void Update( net::TCPServer* p_server )
        {
            std::vector<net::TCPSocket*> eraselist;

            // 控制在一秒檢查一次
            mstimen_t currtm = pftml::Time::GetTickMSTime();
            if( ( currtm - update_time_ ) < 1000 )
            {
                //pfstd::MSSleep( 100 );
                return;
            }

            update_time_ = currtm;

            {
                uint_t    connectsize = 0;
                mstimen_t timeouttime = 0;
                for( int i = 0; i < _MAX_RISK_INSECIRE; i++ )
                {
                    TS_Lock lock( &insecure_lock_[ i ] );
                    connectsize = insecure_ctnr_[ i ].size();
                    timeouttime = keepalive_mstime_[ i ];
                    if( ( connectsize > 0 ) )
                    {
                        for( std::map<net::TCPSocket*, mstimen_t>::iterator it = insecure_ctnr_[ i ].begin(); it != insecure_ctnr_[ i ].end(); it++ )
                        {
                            net::TCPSocket* scn = it->first;
                            mstimen_t accepttime = it->second;
                            if( ( ( currtm - accepttime ) > timeouttime ) || ( scn->GetFD() == 0 ) )
                            {
                                eraselist.push_back( it->first );
                            }
                            //it++;
                        }
                    }
                }

                if( !eraselist.empty() )
                {
                    Pf_INFO( "clear timeout size = %d time = %lld\n", eraselist.size(), timeouttime );
                }
                else
                {
                    return;
                }
            }

            // 紀錄
            {
                pftml::Time logtime;
                for( uint_t i = 0; i < eraselist.size(); i++ )
                {
                    sockaddr_storage addr_storage;
                    sockaddr* addr = eraselist[ i ]->GetPeerAddr( addr_storage );
                    ipa_t     ipa = net::Socket::GetSockAddrIPA( addr );
                    ushort_t  port = net::Socket::GetSockAddrPort( addr );
                    time_t    timep = pftml::Time::GetSTime();

                    _cs_string log_string;
                    pfstd::strprintf( log_string, "%s risk client errno = %d from address %s:%d\n", logtime.GetDateTimeSTR().c_str(), 4, ipa.c_str(), port );
                    printf( "%s", log_string.c_str() );
                    risk_log_text_.push_back( log_string );
                }
            }

            // 檢查要不要寫入檔案中
            {
                // 30秒再寫一次
                if( ( currtm - risk_log_time_ ) > 30000 )
                {
                    if( !risk_log_text_.empty() )
                    {
                        FILE* f = fopen( risk_address_fname_, "a+b" );
                        if( f )
                        {
                            for( int i = 0; i < risk_log_text_.size(); i++ )
                            {
                                fwrite( risk_log_text_[ i ].c_str(), risk_log_text_[ i ].size(), 1, f );
                            }
                            fclose( f );
                        }
                    }
                    risk_log_time_ = currtm;
                    risk_log_text_.clear();
                }
            }

            for( uint_t i = 0; i < eraselist.size(); i++ )
            {
                // 加入高風險ip中
                //sockaddr_storage addr_storage;
                //sockaddr*   addr = eraselist[ i ]->GetPeerAddr( addr_storage );
                //ipa_t       fipn = net::Socket::GetSockAddrIPA( addr );
                //ulonglong_t fipn = ipn.s_addr;
                //std::map<ipa_t, int>::iterator it = m_tRiskAddressList.find( fipn );
                //if( it == m_tRiskAddressList.end() )
                {
                    // 還是不清除 但 把他的 次數設為0
                    //WriteRiskAddress( eraselist[ i ], 4 );
                    //m_tRiskAddressList.insert( std::map<ipa_t, int>::value_type( fipn, 0 ) );
                    // 2011 07 18 原先在lock外的 改在在這邊做 因為發生
                    // 在收到驗證訊號時 同時被踢 造成的
                    // CsSCSytem.CreateSession ipn取不出來
                    // kickout 裡面會產生deadlock 所以 也不能這樣做
                }
                p_server->Kickout( eraselist[ i ] );
            }
        }
    };

    //////////////////////////////////////////////////////////////////////////
    //
    class ThreadLog
    {
    public:
        //
        struct ChannelLog
        {
            channel_uid_t chid;
            mstimen_t   rmst;
        };

    private:

        T_Lock                                    threadlog_ut_lock;
        bool                                      is_log_alive_thread_;      // 要不要紀錄 緒還有沒有活著
        std::set<ThreadLog::ChannelLog*>          thread_channel_;           // 目前在運行的序編號 與 開始的時間

    public:

        ThreadLog() {};
        ~ThreadLog()
        {
            for( std::set<ThreadLog::ChannelLog*>::iterator it = thread_channel_.begin(); it != thread_channel_.end(); it++ )
            {
                ThreadLog::ChannelLog* chlog = ( *it );
                SAFE_DELETE( chlog );
            }
            thread_channel_.clear();
        }

        void SetLogThread( bool f )
        {
            TS_Lock lock( &threadlog_ut_lock );
            is_log_alive_thread_ = f;
        }

        const ThreadLog::ChannelLog* InsertChannel( channel_uid_t chid, mstimen_t rmst )
        {
            if( is_log_alive_thread_ )
            {
                ThreadLog::ChannelLog* chlog = new ThreadLog::ChannelLog;
                chlog->chid = chid;
                chlog->rmst = rmst;
                return chlog;
            }

            return NULL;
        }

        void RemoveChannel( ThreadLog::ChannelLog*  chlog )
        {
            if( chlog == NULL )
            {
                return;
            }

            thread_channel_.erase( chlog );
            SAFE_DELETE( chlog );
        }

        _cs_size_t GetThreadLog( std::vector<channel_uid_t>& chrd, std::vector<mstimen_t>& tmrd )
        {
            chrd.clear();
            tmrd.clear();
            TS_Lock lock( &threadlog_ut_lock );

            if( !is_log_alive_thread_ )
            {
                return 0;
            }

            std::set<ThreadLog::ChannelLog*>::iterator it = thread_channel_.begin();

            while( it != thread_channel_.end() )
            {
                if( ( *it ) != NULL )
                {
                    chrd.push_back( ( *it )->chid );
                    tmrd.push_back( ( *it )->rmst );
                }

                it++;
            }

            return min( chrd.size(), tmrd.size() );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // 轉換連線的socket 
    // CsAbtpCnSocket 同 net::TcpSocket 
    // net::TcpSocket 同 CsAbtpCnSocket 
    // cyberspace::_SessionEntity*        CsSessionDescriptor
    class OutletExchange
    {

    public:

        OutletExchange() {}
        ~OutletExchange() {}

#define SkToSc( skt )  (( ( skt != NULL ) && ( strcmp( skt->ObjectName(), ( "AbtpCnSocket" ) ) == 0 ) )? ( CsAbtpCnSocket* )( skt ): NULL)
#define CdToSc( sdp )  (( sdp != NULL )? SkToSc( cyberspace::CoreServer::GetSessionSocket( sdp ) ) : NULL)
#define ScToSk( scn )  (( scn != NULL )? ( scn ): NULL)
#define CdToSk( sdp )  (ScToSk( CdToSc( sdp ) ))
#define ScToCd( scn )  (( scn != NULL )? scn->GetDescriptor(): NULL)
#define SkToCd( skt )  (ScToCd( SkToSc( skt ) ))
    };

    //////////////////////////////////////////////////////////////////////////
    //
    enum ServerReportType
    {
        _STARTUP_STTIME,         // 紀錄開始的時間
        _INITATE_MSTIME,         // 紀錄開始的時間
        _MAX_PROCESS_CHANNEL,    // 紀錄花費最多時間的 訊號 跟 時間
        _MAX_UPDATE_MSTIME,      // 執行update 最長的時間
        _MAX_CONNECTED_CLIENT,   // 最多連線的數量
        //_LIVING_THREAD_COUNT,
        _CNT_LONG_PROCESS,       // 處理超過一秒的 channel數量
        _CNT_RECV_MESSAGE,       // 收到的訊息數量
        _MAX_SERVER_REPORT_TYPE,
    };

    class SafeServerState: private ServerState
    {
        T_Lock  m_lSystemCount[ _MAX_SERVER_REPORT_TYPE ];

    public:
        SafeServerState() { memset( this, 0, sizeof( ServerState ) ); }
        ~SafeServerState() {}

        stimen_t    Get_Startup_Stime() { TS_Lock lock( &m_lSystemCount[ _STARTUP_STTIME ] ); return startup_stime; }
        void        Set_Startup_Stime( _IN stimen_t v ) { TS_Lock lock( &m_lSystemCount[ _STARTUP_STTIME ] ); startup_stime = v; }
        mstimen_t   Get_Iinitate_Mstime() { TS_Lock lock( &m_lSystemCount[ _INITATE_MSTIME ] ); return initate_mstime; }
        void        Set_Iinitate_Mstime( _IN  mstimen_t v ) { TS_Lock lock( &m_lSystemCount[ _INITATE_MSTIME ] ); initate_mstime = v; }
        channel_uid_t Get_Max_Proc_Channel_Id() { TS_Lock lock( &m_lSystemCount[ _MAX_PROCESS_CHANNEL ] ); return max_proc_channel_id; }
        mstimen_t   Get_Max_Proc_Channel_Mstime() { TS_Lock lock( &m_lSystemCount[ _MAX_PROCESS_CHANNEL ] ); return max_proc_channel_mstime; }
        void        Set_Max_Proc_Channel( _IN channel_uid_t v1, mstimen_t v2 ) { TS_Lock lock( &m_lSystemCount[ _MAX_PROCESS_CHANNEL ] ); max_proc_channel_id = v1, max_proc_channel_mstime = v2; }
        mstimen_t   Get_Max_Update_Mstime() { TS_Lock lock( &m_lSystemCount[ _MAX_UPDATE_MSTIME ] ); return max_update_mstime; }
        void        Set_Max_Update_Mstime( _IN mstimen_t v ) { TS_Lock lock( &m_lSystemCount[ _MAX_UPDATE_MSTIME ] ); max_update_mstime = __max( max_update_mstime, pftml::Time::GetTickMSTime() - v ); }
        uint_t      Get_Max_Connected_Client() { TS_Lock lock( &m_lSystemCount[ _MAX_CONNECTED_CLIENT ] ); return max_connected_client; }
        void        Set_Max_Connected_Client( _IN net::TCPServer* p_server ) {
            TS_Lock lock( &m_lSystemCount[ _MAX_CONNECTED_CLIENT ] ); max_connected_client = __max( max_connected_client, p_server->GetConnectedSize() );
        }
        //uint_t      Get_Cnt_Living_Thread()                      { TS_Lock lock(&m_lSystemCount[_STARTUP_STTIME]); return cnt_living_thread; }
        //void        Set_Cnt_Living_Thread( uint_t v );
        uint_t      Get_Cnt_Long_Process() { TS_Lock lock( &m_lSystemCount[ _CNT_LONG_PROCESS ] ); return cnt_long_process; }
        void        Set_Cnt_Long_Process( _IN uint_t v ) { TS_Lock lock( &m_lSystemCount[ _CNT_LONG_PROCESS ] ); cnt_long_process = v; }
        void        Add_Cnt_Long_Process() { TS_Lock lock( &m_lSystemCount[ _CNT_LONG_PROCESS ] ); cnt_long_process++; }
        uint_t      Get_Cnt_Recv_Message() { TS_Lock lock( &m_lSystemCount[ _CNT_RECV_MESSAGE ] ); return cnt_recv_message; }
        void        Set_Cnt_Recv_Message( _IN uint_t v ) { TS_Lock lock( &m_lSystemCount[ _CNT_RECV_MESSAGE ] ); cnt_recv_message = v; }
        void        Add_Cnt_Recv_Message() { TS_Lock lock( &m_lSystemCount[ _CNT_RECV_MESSAGE ] ); cnt_recv_message++; }
        //uint_t      Get_Max_Allow_Connected()                    { TS_Lock lock(&m_lSystemCount[_STARTUP_STTIME]); return limit_allow_connected; }
        //void        Set_Max_Allow_Connected( uint_t v );

        void        GetServerLog( _IN net::TCPServer* p_server, _OUT ServerState& v )
        {
            TS_Lock  syslock1( &m_lSystemCount[ _INITATE_MSTIME ] );
            TS_Lock  syslock2( &m_lSystemCount[ _MAX_PROCESS_CHANNEL ] );
            TS_Lock  syslock3( &m_lSystemCount[ _MAX_UPDATE_MSTIME ] );
            TS_Lock  syslock4( &m_lSystemCount[ _MAX_CONNECTED_CLIENT ] );
            //TS_Lock  syslock5( &m_lSystemCount[ _LIVING_THREAD_COUNT ] );
            TS_Lock  syslock6( &m_lSystemCount[ _CNT_LONG_PROCESS ] );
            //TS_Lock  syslock7( &m_lSystemCount[ _SUMOFRECV_COUNT ] );
            memcpy( &v, this, _cssizeof( cyberspace::ServerState ) );
            max_connected_client = __max( max_connected_client, p_server->GetConnectedSize() );
            v.cnt_living_thread = p_server->GetLivingThreadSize();
            v.limit_allow_connected = p_server->GetMaxConnectSize();
        }
    };
}