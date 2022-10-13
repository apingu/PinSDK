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
#include <assert.h>

#if defined(_WINDOWS)
#include <direct.h>
#endif
#if defined(_LINUX)
#include <stdarg.h>
#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <pf-debug.h>
#include <parser/xmlnode.h>
#include <algorithmic/crypt-crc32.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-arbitration_server.h"
#include "cs-uid_generator.h"
#include "cs-arbit_command.h"
#include "cs-packet_factory.h"
#include "cs-log_file_service.h"
#include "cs-pri_def.h"
#include "cs-entity_def.h"


namespace cyberspace
{
    namespace Arbitration
    {
        ///////////////////////////////////////////////////////////////////////////////////
        //
#define DIEDCYCLE   180

        CsArbitEntity             g_arbit_host_info;
        T_Lock                    g_arbit_lock;
        bool                      g_init_finish;
        int                       g_log_index;

        ///////////////////////////////////////////////////////////////////////////////////
        // 連線上來的 Server
        class HostNode: public CsSectionEntity
        {
        public:
            HostNode()
            {
                sectionid = 0;
                connecttime = 0;
                strcpy( hostname, "" );
                strcpy( objectname, "" );
                strcpy( functionname, "" );
                maxprocess = 0;
                limitconnect = 0;
                usedconnect = 0;
                maxmemorysize = 0;
                usedmemorysize = 0;
                _state = 0;

                sym_addrlen = 0;
                for( int i = 0; i < _MAX_IPNSIZE; i++ )
                {
                    memset( &sym_addr[ i ], 0, _cssizeof( sym_addr[ i ] ) );
                }
                strcpy( aliasname, "" );
                pub_port = 0;
                scs_port = 0;
            }

            ~HostNode()
            {};

            std::vector<_cs_char_t> buff;
        };

        //  連上來的每個server用的socket
        class PeerClient: public CsAbtpCnSocket
        {
            Arbitration::HostNode* m_Host;

        public:
            PeerClient()
            {
                m_Host = NULL;
            }

            ~PeerClient()
            {
                SAFE_DELETE( m_Host );
            }

            inline void  Initate( Arbitration::HostNode* host )
            {
                m_Host = host;
            }

            inline void  Release( void )
            {
                m_Host = NULL;
            }

            inline Arbitration::HostNode* GetHost( void )
            {
                return m_Host;
            }
        };
    }


    //=============================================================================
    //
    //
    //=============================================================================
    ArbitrationServer::ArbitrationServer()
    {
        //m_MaxOnline = 0;
        //log_fname = new CsLog::FileExporter;
        Arbitration::g_init_finish = false;
        //m_fLogMode = 0;
        //stime_startup_ = 0;
        SET_BIT_FLAG( def_log_mode_, LOGMODE_SAVEFILE );
        SET_BIT_FLAG( def_log_mode_, LOGMODE_PRINTOUT );
    }


    //=============================================================================
    //
    //
    //=============================================================================
    ArbitrationServer::~ArbitrationServer()
    {
        //m_MaxOnline = 0;
        //ClearAllCsPSockConn();
        //m_Watching = false;
        //SAFE_DELETE( log_fname );
        Shutdown();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    net::TCPSocket* ArbitrationServer::OnNewSocket( void )
    {
        return new Arbitration::PeerClient;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationServer::OnDelSocket( net::TCPSocket* skt )
    {
        Arbitration::PeerClient* obj = ( Arbitration::PeerClient* )skt;
        SAFE_DELETE( obj );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int ArbitrationServer::StartUp( unsigned int allowplayers, unsigned short port )
    {
        time_t startt = pftml::Time::GetSTime();
        //time( &startt );
        stime_startup_ = startt;
        // open a log history file
        mkdir( "log" );
        struct tm* p;
        p = localtime( &startt );
        _cs_char_t filename[ _MAX_PATH ];
        _stprintf( filename,
                   "log/arbit-server_history_%d_%d_%d_%d_%d_%d.log",
                   p->tm_year + 1900,
                   p->tm_mon + 1,
                   p->tm_mday,
                   p->tm_hour,
                   p->tm_min,
                   p->tm_sec );
        cyberspace::Arbitration::g_log_index = cyberspace::LogFileSerivce::Open( filename );
        net::TCPServer::Create();
        int ret = net::TCPServer::StartUp( allowplayers, port );
        srand( ( int )pftml::Time::GetSTime() );
        //srand( ( int ) time( 0 ) );
        GetLocalHostname( Arbitration::g_arbit_host_info.hostname, 32 );
        Arbitration::g_arbit_host_info.starttime = pftml::Time::GetTickMSTime();
        Arbitration::g_arbit_host_info.maxnodesize = GetMaxConnectSize();
        return ret;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int ArbitrationServer::Shutdown( void )
    {
        //OnServerShtudown();
        //ClearAllCsPSockConn();
        //m_Watching = false;
        TCPServer::Shutdown();
        Arbitration::g_init_finish = false;
        stime_startup_ = 0;
        cyberspace::LogFileSerivce::Close( cyberspace::Arbitration::g_log_index );
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool ArbitrationServer::LoadConfigure( const _cs_char_t* filename )
    {
        //m_Watching = false;
        Arbitration::g_init_finish = false;
        XMLNode xml;
        _cs_string log_msg;

        if( xml.LoadFromFile( filename ) )
        {
            unsigned int AllowedPlayers = 0;
            unsigned short port = 0;
            //bool flag = false;
            _cs_char_t host[ 255 ];
            XMLNode& ArbitCfg = xml[ ( "ArbitServer" ) ];

            if( ArbitCfg.text_.c_str() == NULL )
            {
                OutputMessage( def_log_mode_, "No arbit configure node arbit-server start failure!!!\n" );
                return false;
            }

            ArbitCfg[ ( "MaxNodes" ) ].text_ >> AllowedPlayers;
            ArbitCfg[ ( "Port" ) ].text_ >> port;
            log_msg = ( "||===========================================================================\n" );
            log_msg += ( "||  cyberspace 【 %s 】 server startup........\n" );
            log_msg += ( "||      build:  %s %s\n" );

            if( StartUp( AllowedPlayers, port ) != 1 )
            {
                log_msg += ( " >>  startup failure !\n" );
                log_msg += ( "||===========================================================================\n\n" );
                OutputMessage( def_log_mode_, log_msg.c_str(), ObjectName(), __DATE__, __TIME__ );
                return false;
            }

            time_t starttime = 0;
            stime_startup_ = starttime;
            log_msg += ( "||  Start on %s" );
            log_msg += ( "||--------------------------------------------------------------------------\n" );
            OutputMessage( def_log_mode_, log_msg.c_str(), ObjectName(), __DATE__, __TIME__, ctime( &starttime ) );
            //uint_t ipcount = 0;
            //ipn4_t  ipn;
            OutputMessage( def_log_mode_, "||  HostName : %s\n", GetLocalHostname( host, 255 ) );
            //std::vector<ipa_t> ip4list;
            //std::vector<ipa_t> ip6list;
            //GetLocalIPAddress( ip4list, ip6list );
            std::vector<EthIF> ethif_list;
            net::Socket::GetEthIFInfo( ethif_list );

            if( ethif_list.empty() )
            {
                log_msg = ( ">>  Can't find any ip!!...\n" );
                log_msg += ( "||===========================================================================\n\n" );
                OutputMessage( def_log_mode_, log_msg.c_str() );
                return false;
            }

            //int ifidex = 0;
            OutputMessage( def_log_mode_, ( "||  Enternet interface :\n" ) );

            for( uint_t ifi = 0; ifi < ethif_list.size(); ifi++ )
            {
                if( ethif_list[ ifi ].iftype == _IF_TYPE_ETHERNET )
                {
                    _cs_string addr_msg;

                    for( int ip4i = 0; ip4i < ethif_list[ ifi ].unicast_addrs.size(); ip4i++ )
                    {
                        sockaddr* skaddr = ( sockaddr* )&ethif_list[ ifi ].unicast_addrs[ ip4i ];

                        if( skaddr->sa_family == AF_INET )
                        {
                            _cs_char_t string[ _MAX_LONG_MSG ] = { "" };
                            ipn4_t ipn4 = net::Socket::GetSockAddrIPN4( skaddr );
                            _stprintf( string, ( "IPv4 = %-15s " ), net::Socket::IPN4ToA4( ipn4 ).c_str() );
                            addr_msg += string;
                            //OutputMessage( def_log_mode_, ( "IPv4 = %-15s " ), net::Socket::IPN4ToA4( ipn4 ).c_str() );
                            //ifidex++;
                        }

                        if( skaddr->sa_family == AF_INET6 )
                        {
                            _cs_char_t string[ _MAX_LONG_MSG ] = { "" };
                            ipn6_t ipn6 = net::Socket::GetSockAddrIPN6( skaddr );
                            _stprintf( string, ( "IPv6 = %-25s " ), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                            addr_msg += string;
                            //OutputMessage( def_log_mode_, ( "IPv6 = %-25s " ), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                        }
                    }

                    if( !addr_msg.empty() )
                    {
                        OutputMessage( def_log_mode_, ( "||    %-10s : %s\n" ), ethif_list[ ifi ].name.c_str(), addr_msg.c_str() );
                    }
                }
            }

            log_msg =  ( "||  Port : %d\n" );
            log_msg += ( "||  Max CsPSockConns : %d\n" );
            log_msg += ( "||  cyberspace 【 %s 】 server start successful.\n" );
            log_msg += ( "||===========================================================================\n\n" );
            OutputMessage( def_log_mode_, log_msg.c_str(), GetServicePort(), GetMaxConnectSize(), ObjectName() );
        }
        else
        {
            log_msg =  ( "||===========================================================================\n" );
            log_msg += ( ">>  Can't load configure file %s!!\n" );
            log_msg += ( "||  cyberspace 【 %s 】 server start failure.\n" );
            log_msg += ( "||===========================================================================\n\n" );
            OutputMessage( def_log_mode_, log_msg.c_str(), filename, ObjectName() );
            return false;
        }

        //m_Watching = true;
        Arbitration::g_init_finish = true;
        return true;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationServer::OnAccept( net::TCPSocket* skt )
    {
        //TS_Lock lock( &arbitrationserver_ut_lock_[0] );

        // it allow connect until server initial finish
        if( !Arbitration::g_init_finish )
        {
            return;
        }

        //_cs_char_t host[ 255 ];
        time_t timep = pftml::Time::GetSTime();
        //time( &timep );
        Arbitration::HostNode* hostnode = new Arbitration::HostNode;
        //memset(hostnode, 0, _cssizeof(Arbitration::HostNode));

        ( ( Arbitration::PeerClient* )skt )->Initate( hostnode );
        sockaddr_storage addr_storage;
        sockaddr* addr = skt->GetPeerAddr( addr_storage );

        if( addr != NULL )
        {
            ipa_t ipa = net::Socket::GetSockAddrIPA( addr );
            char hostname[ NI_MAXHOST ];
            char servinfo[ NI_MAXSERV ];
            net::Socket::GetAddrNameInfo( addr, hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
            _cs_string msg;
            msg =  ( " \n ||================================================\n" );
            msg += ( " ||    Join Unit Server\n" );
            msg += ( " ||------------------------------------------------\n" );
            msg += ( " ||  HostName : %s\n" );
            msg += ( " ||  Service  : %s\n" );
            msg += ( " ||  Time     : %s" );
            msg += ( " ||  Ip       : %s\n" );
            msg += ( " ||================================================\n" );
            OutputMessage( def_log_mode_, msg.c_str(), hostname, servinfo, asctime( localtime( &timep ) ), ipa.c_str() );
        }

        // 設定SERVER的ID
        //SetCsServerUID(skt);
        // 將目前的連線送給這台SERVER
        //SendCsPSockConnListTo(s);
        // 要求資料
        NotifySyncSection( skt );
        return;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //int ArbitrationServer::Kickout( net::TCPSocket* skt )
    void ArbitrationServer::OnKickout( net::TCPSocket* skt )
    {
        //TS_Lock lock( &arbitrationserver_ut_lock_[0] );
        Arbitration::HostNode* hostnode = ( ( Arbitration::PeerClient* )skt )->GetHost();

        if( hostnode == NULL )
        {
            return;
        }

        //host.Release();
        //post all client exit
        NotifyExitSection( skt );
        stimen_t timep = pftml::Time::GetSTime();
        //time( &timep );
        sockaddr_storage addr_storage;
        sockaddr* addr = skt->GetPeerAddr( addr_storage );

        hostnode->sectionid = cyberspace::uid_generator::GetSocketAddrUID( addr, timep );
        hostnode->connecttime = timep;
        if( addr != NULL )
        {
            time_t currtm = timep;
            ipa_t ipa = net::Socket::GetSockAddrIPA( addr );
            char hostname[ NI_MAXHOST ];
            char servinfo[ NI_MAXSERV ];
            net::Socket::GetAddrNameInfo( addr, hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
            _cs_string msg;
            msg =  ( " \n ||================================================\n" );
            msg += ( " ||    Exit Unit Server <%s>\n" );
            msg += ( " ||------------------------------------------------\n" );
            msg += ( " ||  HostName : %s\n" );
            msg += ( " ||  Service  : %s\n" );
            msg += ( " ||  Time     : %s" );
            msg += ( " ||  Ip       : %s\n" );
            msg += ( " ||================================================\n" );
            OutputMessage( def_log_mode_, msg.c_str(), hostnode->objectname, hostname, servinfo, asctime( localtime( &currtm ) ), ipa.c_str() );
        }

        ( ( Arbitration::PeerClient* )skt )->Release();
        SAFE_DELETE( hostnode );
        //TCPServer::Kickout( skt );
        return;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int ArbitrationServer::SendMessage( net::TCPSocket* skt, uniqueid_t who, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
    {
        int ret = 0;

        for( uint_t i = 0; i < _MAX_ERROR_RETRY_TIMES; i++ )
        {
            //if((ret = CsCore::cyberspace::Abtp::SendPacket(s, from, cid, (_cs_char_t*)bp, len, GetSyncTime())) >=0)
            if( ( ret = ( ( Arbitration::PeerClient* )skt )->SendPacket( who, cid, ( ubyte_t* )bp, len ) ) >= 0 )
            {
                return ret;
            }
        }

        return -1;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int ArbitrationServer::RecvMessage( net::TCPSocket* skt, uniqueid_t& who, channel_uid_t& cid, ubyte_t** bp, _cs_size_t& len )
    {
        // TODO: Pin
        // 這邊可以檢查 封包傳回來的時間
        return ( ( Arbitration::PeerClient* )skt )->RecvPacket( who, cid, bp, len );
        //return CsCore::cyberspace::Abtp::RecvPacket(s, who, cid, bp, len, mt);
    }

    //=============================================================================
    //
    //
    //=============================================================================
    stimen_t ArbitrationServer::GetSyncSec( void )
    {
        time_t timep = pftml::Time::GetSTime();
        //time( &timep );
        return timep;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    const CsSectionDescriptor ArbitrationServer::GetHostInfo( net::TCPSocket* skt )
    {
        NotifySyncSection( skt );
        CsSectionEntity* hn = ( ( Arbitration::PeerClient* )skt )->GetHost();
        //s->SetKey(hn);
        return hn;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationServer::NotifySyncSection( net::TCPSocket* skt )
    {
        //stimen_t timep;
        //time(&timep);
        CsSectionEntity* hostnode = ( ( Arbitration::PeerClient* )skt )->GetHost();

        _csbinbuf buf;
        CsArbitCommand::ReportHostInfo::Server::SetRequest( buf, hostnode->sectionid, &Arbitration::g_arbit_host_info );
        SendMessage( skt, CsINVALID_ID, CsS_C_REQUEST_REPORT_HOSTINFO, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationServer::NotifyJoinSection( net::TCPSocket* skt )
    {
        _csbinbuf buf;
        Arbitration::PeerClient* msn = ( Arbitration::PeerClient* )skt;
        {
            _cs_string group_name = msn->GetHost()->functionname;
            TS_Lock lock( &arbitrationserver_ut_lock_[ ( crc32::Encode( group_name.c_str(), group_name.size() ) % _MAX_BUF ) ] );

            for( std::set<net::TCPSocket*>::iterator it = host_bank_[ group_name ].begin(); it != host_bank_[ group_name ].end(); it++ )
            {
                // 通知連線上來的section 其他section的資料
                if( ( *it ) != skt )
                {
                    // 自己的不送
                    Arbitration::PeerClient* sn = ( Arbitration::PeerClient* )( *it );
                    CsSectionEntity* sentity = sn->GetHost();
                    ___SET_SECTION_NONLOCAL( sentity );
                    CsArbitCommand::PostJoinHostInfo::Server::SetRequest( buf, sentity );
                    SendMessage( skt, CsINVALID_ID, CsS_C_JOIN_HOSTINFO_POST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                }
                else
                {
                    Arbitration::PeerClient* sn = ( Arbitration::PeerClient* )( *it );
                    CsSectionEntity* sentity = sn->GetHost();
                    ___SET_SECTION_LOCAL( sentity );
                    CsArbitCommand::PostJoinHostInfo::Server::SetRequest( buf, sentity );
                    SendMessage( skt, CsINVALID_ID, CsS_C_JOIN_HOSTINFO_POST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                }

                // 通知連其他section 連上來section的資料
                {
                    CsSectionEntity* sentity = msn->GetHost();
                    ___SET_SECTION_NONLOCAL( sentity );
                    _csbinbuf buf;
                    CsArbitCommand::PostJoinHostInfo::Server::SetRequest( buf, sentity );
                    SendMessage( ( *it ), CsINVALID_ID, CsS_C_JOIN_HOSTINFO_POST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                }
            }
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    /*
    void ArbitrationServer::PublishMountHost( CsSectionEntity* hostnode )
    {
        //CsSectionEntity* hostnode = ( ( Arbitration::PeerClient* ) skt )->GetHost();
        if ( hostnode->sectionid == 0 )
        {
            return;
        }

        {
            _cs_string group_name = hostnode->functionname;
            TS_Lock lock( &arbitrationserver_ut_lock_[ (   crc32::Encode( group_name.c_str(), group_name.size() ) % _MAX_BUF ) ] );

            for ( std::set<net::TCPSocket*>::iterator it = host_bank_[ group_name ].begin(); it != host_bank_[ group_name ].end(); it++ )
            {
                // 自己也得送才行 不然 沒建立scsagnet
                _csbinbuf buf;
                CsArbitCommand::PostJoinHostInfo::Server::SetRequest( buf, hostnode );
                SendMessage( ( *it ), CsINVALID_ID, CsS_C_JOIN_HOSTINFO_POST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            }
        }
        return;
    }
    */

    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationServer::NotifyExitSection( net::TCPSocket* skt )
    {
        Arbitration::PeerClient* msn = ( Arbitration::PeerClient* )skt;

        if( msn->GetHost()->sectionid == 0 )
        {
            return;
        }

        {
            _cs_string group_name = msn->GetHost()->functionname;
            TS_Lock lock( &arbitrationserver_ut_lock_[ ( crc32::Encode( group_name.c_str(), group_name.size() ) % _MAX_BUF ) ] );

            for( std::set<net::TCPSocket*>::iterator it = host_bank_[ group_name ].begin(); it != host_bank_[ group_name ].end(); it++ )
            {
                // 自己不用送
                //if(operator[](i)==skt)
                // continue;
                _csbinbuf buf;
                CsArbitCommand::PostExitHostInfo::Server::SetRequest( buf, msn->GetHost()->sectionid );
                SendMessage( ( *it ), CsINVALID_ID, CsS_C_EXIT_HOSTINFO_POST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            }
        }

        return;
        //post host to all client
        //if(hostnode->visible)
        //{
        // for(uint_t i=0; i<GetConnectedSize(); i++)
        // {
        //     CsCommandPacket hPostMsg;
        //     CsArbitCommand::PostExitHostInfo::Server::SetRequest(&hPostMsg, hostnode->sectionid);
        //     SEND_ARBIT_MESSAGE((operator[](i)), hPostMsg);
        // }
        //}
    }


    //=============================================================================
    //
    //
    //=============================================================================
    /*
    void ArbitrationServer::SendCsPSockConnListTo(net::TCPSocket* skt)
    {
     bool result = false;
     _csbinbuf buf;
     TS_Lock lock(&m_lConnectVec);
     std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it = m_ConnectVec.begin();
     while(it!=m_ConnectVec.end())
     {
        CstArbitConnectNode* chnode=(CstArbitConnectNode*)it->second;
        if(chnode!=NULL)
        {
            _CsSessionEntity sendnode;
            sendnode.cid  = chnode->cid;
            sendnode.mac  = chnode->mac;
            sendnode.sid  = chnode->sid;
            sendnode.power   = chnode->power;
            sendnode.ipn  = chnode->ipn;
            sendnode.createsec  = chnode->createsec;
            //sendnode.createmst  = chnode->createmst;
            CsArbitCommand::JoinClient::Server::SetFeedback(buf, &sendnode, result);
            SendMessage(s, CsINVALID_ID, CsS_C_JOIN_CsPSockConn_FEEDBACK, _CsGetResBufPtr(buf), _CsGetResBufLen(buf));
        }
        it++;
     }
    }
    */


    //=============================================================================
    //
    //
    //=============================================================================
    /*
    void ArbitrationServer::ClearAllCsPSockConn(void)
    {
     TS_Lock lock(&m_lConnectVec);
    std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it = m_ConnectVec.begin();
    while(it != m_ConnectVec.end())
     {
        //也需該先把key 環給下面的人
        //..........
        SAFE_DELETE(it->second);
        it++;
     }
    }
    */




    //=============================================================================
    //
    //
    //=============================================================================
    /*
    void ArbitrationServer::DoJoinCsPSockConn(net::TCPSocket* skt, _CsSessionEntity* node)
    {
     bool result=false;
     if(IsCsInvalidId(node->cid))
     {
        printf("Join invalid id!\n");
        return;
     }

     TS_Lock lock(&m_lConnectVec);

     //CsCommandPacket hChMsg;
     _csbinbuf buf;
     net::TCPSocket* joinsocket=s; //加入這個連線的那台SERVER
     CstArbitConnectNode* chnode=NULL;
     std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it = m_ConnectVec.find(node->cid);


     // 這個連線已經加入過
     // 所以先找出來對舊的連線請求關閉
     // 然後在移除後 重新加入
     if(it != m_ConnectVec.end())
     {
        printf(" Join connect %lld already exist", node->cid);
        chnode = it->second;
        if(chnode->connecthost!=NULL)
        {
            // 跟原本所在的Server是不同的Server
            if(chnode->connecthost!=s)
            {
                net::TCPSocket* kickoutserver =NULL;
                if(chnode->createsec>node->createsec)
                {
                    //joinsocket=chnode->connecthost;
                    printf("....delay from new host, kickout new connect\n");
                    // 因為這次的連線是舊的，也許因為網路延遲的關西 所以後送到
                    // 因此視為無效
                    // 將新的請求連線關閉
                    CsArbitCommand::ExitClient::Server::SetFeedback(buf, chnode);
                    if(SendMessage(s, CsINVALID_ID, CsS_C_JOIN_CsPSockConn_FEEDBACK, _CsGetResBufPtr(buf), _CsGetResBufLen(buf)) <=0)
                    {
                        printf("Send close falure!!");
                    }

                    // 不用更新資料
                    return;
                }
            }
            else
            {
                // 有可能是再同一台Server上面更新資料
                printf("....from same host\n");
            }
        }
        else// no owner ???
        {
            printf("....no owner host???\n");
        }

        //find resource
        m_ConnectVec.erase(it);
     }
     // not find old connect node
     else
     {
        printf("Join new connect=%lld\n", node->cid);
        chnode = new CstArbitConnectNode;
        chnode->createhost = s;
     }

     chnode->cid=node->cid;
     chnode->mac=node->mac;
     chnode->ipn=node->ipn;
     chnode->sid=GetCsServerUID(s);
     chnode->power = node->power;
     chnode->createsec = node->createsec;
     chnode->index = node->index;
     //chnode->createmst = node->createmst;
     chnode->translate=1;
     chnode->connecthost = joinsocket;

     result=true;

     // 先加入在送給子SERVER
     m_ConnectVec.insert(std::map< cs_session_uid_t, CstArbitConnectNode* >::value_type(node->cid, chnode));
     // 送給加入 的那台實體SERVER以外的SERVER
     // 因為他自己會加入
     for(uint_t i=0; i<GetConnectedSize(); i++)
     {
        if((operator[](i))!=joinsocket)
        {
            //CsCommandPacket hPostMsg;
            CsArbitCommand::JoinClient::Server::SetFeedback(buf, chnode, result);
            SendMessage(operator[](i), CsINVALID_ID, CsS_C_JOIN_CsPSockConn_FEEDBACK, _CsGetResBufPtr(buf), _CsGetResBufLen(buf));
            //SEND_ARBIT_MESSAGE(operator[](i), hChMsg);
        }
     }

     m_MaxOnline = __max(m_ConnectVec.size(), m_MaxOnline);

     _cs_char_t msgstr[_MAX_MSG];
     _cs_string outputstr;


     _stprintf(msgstr, "\n========================================\n");   outputstr+=msgstr;
     _stprintf(msgstr, "||  Join CsAbtpCnSocket [ Id : %lld ]\n", chnode->cid);  outputstr+=msgstr;
     _stprintf(msgstr, "||--------------------------------------\n");     outputstr+=msgstr;
     maca_t maca = net::Socket::MACNToA(chnode->mac);
     _stprintf(msgstr, "||  mac :%02X-%02X-%02X-%02X-%02X-%02X\n",
              maca[0], maca[1], maca[2],
                          maca[3], maca[4], maca[5]);       outputstr+=msgstr;
     _stprintf(msgstr, "||  ip  :%s\n", net::Socket::IPN4ToA4(chnode->ipn));      outputstr+=msgstr;
     _stprintf(msgstr, "||  on time:%s", asctime(localtime(&chnode->createsec)));  outputstr+=msgstr;
     //_stprintf(msgstr, "||  on time:%s", chnode->createmst);        outputstr+=msgstr;

     _stprintf(msgstr, "||  Now online=%d\n", m_ConnectVec.size());    outputstr+=msgstr;
    _stprintf(msgstr, "||  Max online=%d\n", m_MaxOnline);      outputstr+=msgstr;
    _stprintf(msgstr, "||--------------------------------------\n");     outputstr+=msgstr;
    _stprintf(msgstr, "||     ....successful\n");      outputstr+=msgstr;
     _stprintf(msgstr, "========================================\n");      outputstr+=msgstr;


     if(_csstrlen(outputstr.c_str()) < _MAX_LONG_MSG)
        OutputMessage(outputstr.c_str());
     else
        OutputMessage(def_log_mode_,  " ========================================\n||  Join CsAbtpCnSocket [ Id : %d ]\n========================================\n\n", chnode->cid);



     // update server value
     // 告訴其他SERVER 某依台SERVER 增加了一個人
     printf("update server state\n");
     CsSectionEntity* hostnode = ((PeerClient*)joinsocket)->GetHost();
     hostnode->usedconnect++;

     PublishMountHost(joinsocket);
    }


      //=============================================================================
    //
    //
      //=============================================================================
    void ArbitrationServer::DoExitCsPSockConn(net::TCPSocket* skt, _CsSessionEntity* node)
    {
     if(IsCsInvalidId(node->cid))
     {
        printf("Exit invalid id!\n");
        return;
     }


     TS_Lock lock(&m_lConnectVec);

     _csbinbuf buf;
     std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it = m_ConnectVec.find(node->cid);
     // 要移除得連線原本就不存在?
     if(it == m_ConnectVec.end())
     {
        maca_t maca = net::Socket::MACNToA(node->mac);
        printf("Exit CsAbtpCnSocket\n  cid=%lld mac=%02X-%02X-%02X-%02X-%02X-%02X on sectionid=%d....not find\n",
              node->cid, maca[0], maca[1], maca[2], maca[3], maca[4], maca[5], node->sid);
        return;
     }


     const CsSectionEntity* requesthostnode = GetFeedback(s);
     CstArbitConnectNode* chnode = (CstArbitConnectNode*)it->second;

     // 先移除子SERVER在移除自己
     //自己就不用往下傳
     for(uint_t i=0; i<GetConnectedSize(); i++)
     {
        // 不送給要求的Server
        if(operator[](i)!=s)
        {
            printf("Exit CsAbtpCnSocket\n  cid=%lld on sectionid=%d from sectionid=%d\n", node->cid, chnode->connecthost->GetFD(), node->sid, requesthostnode->sectionid);
            // 別台server可以關調不屬於自己的連線
            CsArbitCommand::ExitClient::Server::SetFeedback(buf, chnode);
            SendMessage(operator[](i), CsINVALID_ID, CsS_C_EXIT_CsPSockConn_FEEDBACK, _CsGetResBufPtr(buf), _CsGetResBufLen(buf));
        }
     }

     _cs_char_t msgstr[_MAX_MSG];
     _cs_string outputstr;

     stimen_t timep;
     time(&timep);

     _stprintf(msgstr, "\n========================================\n");  outputstr+=msgstr;
     _stprintf(msgstr, "||  Exit CsAbtpCnSocket [ Id : %lld ]\n", chnode->cid); outputstr+=msgstr;
     _stprintf(msgstr, "||--------------------------------------\n");    outputstr+=msgstr;
     _stprintf(msgstr, "||  cid :%lld\n", chnode->cid);      outputstr+=msgstr;

     maca_t maca = net::Socket::MACNToA(chnode->mac);
     _stprintf(msgstr, "||  mac :%02X-%02X-%02X-%02X-%02X-%02X\n",
              maca[0], maca[1], maca[2],
                          maca[3], maca[4], maca[5]);      outputstr+=msgstr;

     _stprintf(msgstr, "||  on time:%s", asctime(localtime(&timep)));    outputstr+=msgstr;
     _stprintf(msgstr, "||  on   sectionid:%d\n", node->sid);       outputstr+=msgstr;
    _stprintf(msgstr, "||  from sectionid:%d\n", requesthostnode->sectionid);   outputstr+=msgstr;
    _stprintf(msgstr, "||  Now online=%d\n", m_ConnectVec.size());   outputstr+=msgstr;
    _stprintf(msgstr, "||  Max online=%d\n", m_MaxOnline);     outputstr+=msgstr;
     _stprintf(msgstr, "||--------------------------------------\n");    outputstr+=msgstr;
     _stprintf(msgstr, "||     ....successful\n");     outputstr+=msgstr;
     _stprintf(msgstr, "========================================\n");     outputstr+=msgstr;

     if(_csstrlen(outputstr.c_str()) < _MAX_LONG_MSG)
        OutputMessage(outputstr.c_str());
     else
        OutputMessage(def_log_mode_,  " ========================================\n||  Exit CsAbtpCnSocket [ Id : %d ]\n========================================\n\n", chnode->cid);


     //update server value
     printf("update server state\n");
     CsSectionEntity* hostnode = ((PeerClient*)chnode->connecthost)->GetHost();
     if(hostnode->usedconnect>0)
        hostnode->usedconnect--;




     // 因為 移除了SERVER 少了一個連線，所以更新一次連線的資料
     PublishMountHost(chnode->connecthost);

     m_ConnectVec.erase(it);
     SAFE_DELETE(chnode);
    }
    */


    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationServer::OnReceive( net::TCPSocket* skt )
    {
        uniqueid_t     eid = CsINVALID_ID;
        channel_uid_t  command_uid = 0;
        ubyte_t* recvbuf = NULL;
        _cs_size_t      command_len = 0;
        RecvMessage( skt, eid, command_uid, &recvbuf, command_len );

        //CsCore::cyberspace::Abtp::RecvPacket(s, eid, hChMsg.command_uid, &recvbuf, hChMsg.command_len);
        //RECV_ARBIT_MESSAGE(s, eid, hChMsg);
        switch( command_uid )
        {
            case CsC_S_FEEDBACK_REPORT_HOSTINFO:
                {
                    // 有一台經過認證的SERVER 報告他的狀況
                    CsSectionEntity* hostnode = ( ( Arbitration::PeerClient* )skt )->GetHost();
                    // 因為ArbitPeer送上來的資料沒有包含
                    // SeverId跟connectime(已這邊為主，所以沒有送的必要)
                    // 所以不能被蓋掉


                    cs_section_uid_t sectionid = hostnode->sectionid;
                    stimen_t   connecttime = hostnode->connecttime;
                    _cs_string  old_group = hostnode->functionname;
                    // 檢查是不是送 相同的資料上來
                    {
                        // 不比對這個
                        hostnode->sectionid = 0;
                        hostnode->connecttime = 0;
                        CsSectionEntity newhostnode;
                        CsArbitCommand::ReportHostInfo::Server::GetFeedback( recvbuf, newhostnode );

                        // 一樣 重複設定
                        if( memcmp( &newhostnode, hostnode, _cssizeof( CsSectionEntity ) ) == 0 )
                        {
                            return;
                        }

                        memcpy( hostnode, &newhostnode, _cssizeof( CsSectionEntity ) );
                    }

                    hostnode->sectionid = sectionid;
                    hostnode->connecttime = connecttime;

                    //bool fistjoin = true;
                    {
                        // erase old group location
                        {
                            _cs_string group_name = old_group;
                            TS_Lock lock( &arbitrationserver_ut_lock_[ ( crc32::Encode( group_name.c_str(), group_name.size() ) % _MAX_BUF ) ] );
                            std::map<_cs_string, SockeList>::iterator it = host_bank_.find( group_name );

                            if( it != host_bank_.end() )
                            {
                                SockeList::iterator sit = it->second.find( skt );

                                if( sit != it->second.end() )
                                {
                                    //fistjoin = false;
                                    it->second.erase( sit );
                                }

                                if( it->second.empty() )
                                {
                                    host_bank_.erase( it );
                                }
                            }
                        }
                        //hostnode->connecttime = connecttime;
                        {
                            _cs_string group_name = hostnode->functionname;
                            TS_Lock lock( &arbitrationserver_ut_lock_[ ( crc32::Encode( group_name.c_str(), group_name.size() ) % _MAX_BUF ) ] );
                            std::map<_cs_string, SockeList>::iterator it = host_bank_.find( group_name );

                            if( it == host_bank_.end() )
                            {
                                SockeList newsocklist;
                                host_bank_.insert( std::map<_cs_string, SockeList>::value_type( hostnode->functionname, newsocklist ) );
                            }

                            host_bank_[ hostnode->functionname ].insert( std::set<net::TCPSocket*>::value_type( skt ) );
                        }
                    }
                    Pf_INFO( "Get mount host report!!\n" );
                    // 我想要 更新就重連
                    NotifyJoinSection( skt );
                    //// 把其他人告訴我
                    //if ( fistjoin )
                    //{
                    //    NotifyJoinSection( skt );
                    //}
                    ////告訴其他的SERVER有一台SERVER的狀態新增或更改
                    //// 把我告訴其他人
                    //PublishMountHost( hostnode );
                }
                break;
                //case CsC_S_REQUEST_GETHOSTINFOLIST:
                //    {
                //        NotifyJoinSection( skt );
                //    }
                //    break;
                /*
                case CsC_S_REQUEST_REGISTERSERVERDATA:
                {
                #ifdef _DEBUG
                    printf( "Get register data!!\n" );
                #endif
                _cs_size_t len;
                const char* buf = CsArbitCommand::RegisterUserData::Server::GetUserData( recvbuf, len );
                Arbitration::HostNode* hostnode = ( ( PeerClient* ) skt )->GetHost();
                hostnode->buff.resize( len );

                if ( len > 0 )
                {
                memcpy( &hostnode->buff[ 0 ], buf, len );
                }
                }
                break;
                */
                /*
                case CsC_S_JOIN_CsPSockConn_REQUEST:
                 {
                    //_cs_char_t buff[255];
                    //TS_Lock lock(&Arbitration::g_arbit_lock);
                    _CsSessionEntity node;
                    CsArbitCommand::JoinClient::Server::GetRequest(recvbuf, node);
                    DoJoinCsPSockConn(s, &node);
                 }
                 break;


                case CsC_S_EXIT_CsPSockConn_REQUEST:
                 {
                    //TS_Lock lock(&Arbitration::g_arbit_lock);
                    _CsSessionEntity node;
                    CsArbitCommand::ExitClient::Server::GetRequest(recvbuf, node);
                  DoExitCsPSockConn(s, &node);
                 }
                 break;
                */
                //     case CsC_S_ESTABLISH_CsPSockConn_REQUEST:
                //         {
                //             _CsSessionEntity node;
                //             cs_session_uid_t cid = CsINVALID_ID;
                //             CsArbitCommand::EstablishConnect::Server::GetRequest(&hChMsg, node);
                //
                //             if(OnEstablishCsPSockConn(&node))
                //             {
                //                 std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it = m_ConnectVec.find(node.cid);
                //
                //                 //find resource
                //                 CstArbitConnectNode* chnode=NULL;
                //                 if(it != m_ConnectVec.end())
                //                 {
                //                     chnode = (CstArbitConnectNode*)it->second;
                //                 }
                //                 else
                //                 {
                //                     chnode = new CstArbitConnectNode;
                //                  m_ConnectVec.insert(std::map< cs_session_uid_t, CstArbitConnectNode* >::value_type(node.cid, chnode));
                //                 }
                //
                //                 chnode->cid=node.cid;
                //                 //chnode->aid=node.aid;
                //                 chnode->mac=node.mac;
                //
                //                 chnode->translate=1;
                //                 chnode->createhost = s;
                //                 chnode->connecthost = NULL;
                //
                //                 pftml::Timer timer;
                //                 chnode->buildtime = timer.GetTickMSTime();
                //
                //                 printf("Establish one CsAbtpCnSocket\n  cid=%d mac=%02X-%02X-%02X-%02X-%02X-%02X..successful\n",
                //                      node.cid, node.mac[0], node.mac[1], node.mac[2], node.mac[3], node.mac[4], node.mac[5]);
                //             }
                //             else
                //                 printf("Establish one CsAbtpCnSocket\n  cid=%d mac=%02X-%02X-%02X-%02X-%02X-%02X..failure\n",
                //                   node.cid, node.mac[0], node.mac[1], node.mac[2], node.mac[3], node.mac[4], node.mac[5]);
                //
                //
                //             //CsArbitCommand::EstablishConnect::Server::SetFeedback(&hChMsg, node);
                //             //SEND_ARBIT_MESSAGE(s, hChMsg);
                //
                //         }
                //         break;
                //     case CsC_S_JOIN_CsPSockConn_REQUEST:
                //         {
                //             //_cs_char_t buff[255];
                //             _CsSessionEntity node;
                //             CsArbitCommand::JoinClient::Server::GetRequest(&hChMsg, node);
                //             bool result=false;
                //
                //             std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it = m_ConnectVec.find(node.cid);
                //
                //         //find resource
                //    if((it != m_ConnectVec.end()) &&
                //              (node.cid == it->second->cid) &&
                //                 (node.mac == it->second->mac))
                //                 //&& (node.aid == CsINVALID_ID))
                //             {
                //                 CstArbitConnectNode* chnode = (CstArbitConnectNode*)it->second;
                //                 chnode->connecthost = s;
                //              result=true;
                //                 node.cid=chnode->cid;
                //                 //node.aid=chnode->aid;
                //                 chnode->translate--;
                //              printf("Join one CsAbtpCnSocket\n  cid=%d mac=%02X-%02X-%02X-%02X-%02X-%02X..successful\n",
                //                      node.cid, node.mac[0], node.mac[1], node.mac[2], node.mac[3], node.mac[4], node.mac[5]);
                //             }
                //             else
                //             {
                //                 //node.aid=CsINVALID_ID;
                //                 printf("Join one CsAbtpCnSocket\n  cid=%d mac=%02X-%02X-%02X-%02X-%02X-%02X..failure\n",
                //                      node.cid, node.mac[0], node.mac[1], node.mac[2], node.mac[3], node.mac[4], node.mac[5]);
                //             }
                //
                //    CsArbitCommand::JoinClient::Server::SetFeedback(&hChMsg, node.cid, result);
                //    SEND_ARBIT_MESSAGE(s, hChMsg);
                //         }
                //         break;
        }

        //SAFE_DELETE_ARRAY(recvbuf);
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void ArbitrationServer::OnDisconnect( net::TCPSocket* skt )
    //{
    //    //_cs_char_t host[255];
    //    //_cs_char_t addr[255];
    //    //     CsSocketKeyContainer<Arbitration::HostNode*> host(s);
    //    //  if(host.Wait(5000)==NULL)
    //    //     {
    //    //   printf("DisCsPSockConn get host, failure\n");
    //    //   return;
    //    //     }
    //    TS_Lock lock( &arbitrationserver_ut_lock_ );
    //    Arbitration::HostNode* hostnode = ( ( PeerClient* )skt )->GetHost();
    //
    //    if ( hostnode == NULL )
    //    {
    //        return;
    //    }
    //
    //    assert( hostnode && "host can't be NULL" );
    //    //host.Release();
    //    //post all client exit
    //    NotifyExitSection( skt );
    //    time_t timep;
    //    time( &timep );
    //    OutputMessage( def_log_mode_, " \n ||================================================\n" );
    //    OutputMessage( def_log_mode_, " ||  Close cyberspace <%s> server\n", hostnode->objectname );
    //    OutputMessage( def_log_mode_, " ||------------------------------------------------\n" );
    //    OutputMessage( def_log_mode_, " ||  name : %s\n", hostnode->hostname );
    //    OutputMessage( def_log_mode_, " ||  Time : %s", asctime( localtime( &timep ) ) );
    //    OutputMessage( def_log_mode_, " ||  Id   : %d\n", hostnode->sectionid );
    //
    //    for ( uint_t n = 0; n < hostnode->iplen; n++ )
    //    {
    //        OutputMessage( def_log_mode_, " ||  Ip   : %s\n", net::Socket::IPN4ToA4( hostnode->ip[n] ) );
    //    }
    //
    //    OutputMessage( def_log_mode_, " ||================================================\n" );
    //    /*
    //    TS_Lock clock(&m_lConnectVec);
    //    std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it = m_ConnectVec.begin();
    //    while(it!=m_ConnectVec.end())
    //    {
    //     _CsSessionEntity* cnode = (_CsSessionEntity*)it->second;
    //     it++;
    //     if(cnode->sid==hostnode->sectionid)
    //     {
    //        printf("Remove connect %lld build on %s\n", cnode->GetCid(), asctime(localtime(&cnode->createsec)));
    //        m_ConnectVec.erase(cnode->GetCid());
    //        SAFE_DELETE(cnode);
    //     }
    //
    //    }
    //    */
    //    ( ( PeerClient* )skt )->Release();
    //    SAFE_DELETE( hostnode );
    //}


    //=============================================================================
    //
    //
    //=============================================================================
    //void ArbitrationServer::OnShutDown(void)
    //{
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationServer::OutputMessage( CsLogMode m, const _cs_char_t* format, ... )
    {
        //_cs_char_t string[ _MAX_LONG_MSG ] = { "" };
        //FILE str;
        //register FILE *outfile = &str;
        va_list arglist;
        //register int retval;
        //const _cs_char_t* i = format;
        va_start( arglist, format );
        //assert( string != NULL );
        //assert( format != NULL );
        //vsprintf( string, format, arglist );
        cyberspace::LogFileSerivce::OutputMessageV( cyberspace::Arbitration::g_log_index, m, format, arglist );
        va_end( arglist );
        //cyberspace::LogSerivce::OutputMessage( cyberspace::Arbitration::g_log_index, m, format, arglist );        return;
        return;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    //void ArbitrationServer::SetLogMode( CsLogMode m )
    //{
    //    m_fLogMode = m;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //CsLogMode ArbitrationServer::GetLogMode( void )
    //{
    //    return m_fLogMode;
    //}


    /*
    void __cdecl ArbitrationServer::Watching( void* data )
    {
        Server* server = ( Server* )data;
        _csbinbuf buf;

        while(server->m_Watching)
        {
         //TS_Lock lock(&Arbitration::g_arbit_lock);
         //Arbitration::g_arbit_lock.Lock();
         //mstimen_t now= pftml::Time::GetTickMSTime();
         stimen_t now;
         time(&now);

         TS_Lock clock(&server->m_lConnectVec);
         std::map< cs_session_uid_t, CstArbitConnectNode* >::iterator it=server->m_ConnectVec.begin();
         while(it != server->m_ConnectVec.end())
         {
            // 閒置過久?
            CstArbitConnectNode* node = (CstArbitConnectNode*)it->second;
            if((node->translate>=2) && (now-node->createsec)>DIEDCYCLE)
            {
             server->m_ConnectVec.erase(it);
                SAFE_DELETE(node);
             CsArbitCommand::ExitClient::Server::SetFeedback(buf, (_CsSessionEntity*)node);
             server->SendMessage(node->createhost, CsINVALID_ID, CsS_C_EXIT_CsPSockConn_FEEDBACK, _CsGetResBufPtr(buf), _CsGetResBufLen(buf));
            }
            ++it;
            pfstd::MSSleep(0);
         }
         //Arbitration::g_arbit_lock.UnLock();
         pfstd::MSSleep(30000);
        }
    }
    */
}
;

