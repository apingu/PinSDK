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

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-timer.h>
#include <pf-system.h>
#include <parser/xmlnode.h>
#include <net/net-server.h>
#include <database/xdb-mysqldb.h>
//
#include <memio.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-base_command.h"
#include "cs-scs_command.h"
#include "cs-log_file_exporter.h"
#include "cs-arbitration_client.h"
#include "cs-core_server.h"
//#include "MessageDispatcher.h"
#include "cs-log_file_service.h"
#include "cs-unit_server.h"


#if defined(_WINDOWS)
#include <direct.h>
#endif

#if  defined(_LINUX)
#include <stdarg.h>
#endif

#include <assert.h>
#include <set>
#include <list>



#define _MAX_CsPSockConn_LOCK  10000
#define _MAX_DB_CONNECT    50

#define _LONG_PROC_TIME    1000

namespace cyberspace
{
    extern int     g_log_index;

    namespace
    {
        CsLogMode      g_log_mode = 0;
    }


    //-----------------------------------------------------------------------------------------
    // Global value

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //

    //=============================================================================
    //
    //
    //=============================================================================
    UnitServer::UnitServer( const tchar_t* name )
    {
        cyberspace::Abtp::Initialize();
        srand( ( int )pftml::Time::GetSTime() );

        SET_BIT_FLAG( g_log_mode, LOGMODE_OUTDEBUG );
        SET_BIT_FLAG( g_log_mode, LOGMODE_SAVEFILE );

        //strcpy( name_service_, "BaseServer" );
        //strcpy( name_alias_, "" );
        //srand( ( int ) time( 0 ) );

        if( name != NULL )
        {
            name_class_ = name;
        }
        else
        {
            TCHAR szPath[ _MAX_LPATH ];
            pfkrl::GetCurrentProcessFullName( szPath, _MAX_LPATH );
            TCHAR szFName[ _MAX_LFNAME ];
            getpathfname( szFName, szPath );
            name_class_ = szFName;
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    UnitServer::~UnitServer()
    {
        Shutdown();
        //SAFE_DELETE( v_coreserver_ );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::StartUp( uint_t allowplayers, unsigned short port )
    {
        RegisterUserDefChannel( CsC_S_PING_REQUEST, ( unit_member_func )&UnitServer::DoPing );
        RegisterUserDefChannel( CsC_S_LISTSERVER_REQUEST, ( unit_member_func )&UnitServer::DoListServer );
        return v_coreserver_->StartUp( allowplayers, port );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::Shutdown( void )
    {
        if( v_coreserver_ == NULL )
            return 0;

        v_coreserver_->SCSShutdown();
        int ret = v_coreserver_->Shutdown();
        if( ret != 0 )
        {
            return ret;
        }

        OnServerShtudown();
        //for ( uint_t i = 0; i < database_[ 0 ].size(); i++ )
        //{
        //    OnDeleteCommonDB( database_[ 0 ][ i ] );
        //}
        //
        //database_[ 0 ].clear();
        //
        //for ( uint_t i = 0; i < database_[ 1 ].size(); i++ )
        //{
        //    OnDeleteCommonDB( database_[ 1 ][ i ] );
        //}
        //
        //database_[ 1 ].clear();
        UnRegisterUserDefChannel( CsC_S_PING_REQUEST );
        UnRegisterUserDefChannel( CsC_S_LISTSERVER_REQUEST );

        v_coreserver_->Shutdown();
        SAFE_DELETE( v_coreserver_ );
        //v_coreserver_->SCSDestroy();
        pfstd::MsSleep( 50 );
        return ret;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SetName( const tchar_t* name )
    //{
    //    name_class_ = name;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::LoadConfigure( const tchar_t* filename )
    {
        // 設定server 未初始化
        //v_coreserver_->SetInitateMstime( 0 );
        //bool loadret = true;
        cs_string val;
        XMLNode xml;
        unsigned int AllowedPlayers = 0;
        tchar_t host[ 255 ];
        unsigned short port = 0;
        //bool flag = false;
        unsigned int AllowedSCSServer = 0;
        unsigned short SCSport = 0;
        //cs_section_uid_t sectionid = CsINVALID_ID;
        cs_string servicename;
        cs_string aliasname;
        std::vector<ushort_t> remoteportvec;
        bool baasesync = false;
        mstimen_t riskmstime = 3000;
        //int db_max_CsPSockConns = 0;
        //cs_string db_name;
        //cs_string db_hostname;
        //cs_string db_username;
        //cs_string db_password;
        //cs_string db_charset;
        //unsigned short db_port = 0;
        std::map<cs_string, cs_string> setting_node;
        CsLogMode log_mode = 0;
        SET_BIT_FLAG( log_mode, LOGMODE_PRINTOUT );

        //char logbuf[512];
        if( xml.LoadFromFile( filename ) )
        {
            //std::vector<cs_string> setting_node_text;
            XMLNode* GroupCfg = xml.FindFirstNode( _T( "ServerGroup" ) );

            if( GroupCfg != NULL )
            {
                //////////////////////////////////////////////////////////////////////////
                // Read configure file
                {
                    XMLNode* ServerCfg = GroupCfg->FindFirstNode( ObjectName() );

                    while( ServerCfg )
                    {
                        XMLAttribute* pa = ServerCfg->FindAttribute( _T( "name" ) );

                        if( ( name_class_ == "" ) || ( pa == NULL ) )
                        {
                            break;
                        }
                        else if( pa->value_ == name_class_.c_str() )
                        {
                            break;
                        }

                        ServerCfg = xml.FindNextNode( _T( ObjectName() ) );
                    }

                    //if(ServerCfg.text_.c_str()!=NULL)
                    if( ServerCfg )
                    {
                        for( uint_t ni = 0; ni < ServerCfg->GetNodeSize(); ni++ )
                        {
                            XMLNode* SettingNode = ServerCfg->FindNode( ni );

                            if( SettingNode != NULL )
                            {
                                //if ( SettingNode->name_ == _T( "Id" ) )
                                //{
                                //    SettingNode->text_ >> sectionid;
                                //}
                                if( SettingNode->name_ == _T( "MaxNodes" ) )
                                {
                                    SettingNode->text_ >> AllowedPlayers;
                                }
                                else if( SettingNode->name_ == _T( "Port" ) )
                                {
                                    SettingNode->text_ >> port;
                                }
                                else if( SettingNode->name_ == _T( "Service" ) )
                                {
                                    servicename = SettingNode->text_;
                                }
                                else if( SettingNode->name_ == _T( "Alias" ) )
                                {
                                    aliasname = SettingNode->text_;
                                }
                                else if( SettingNode->name_ == _T( "RemotePort" ) )
                                {
                                    tchar_t* pBuff = new tchar_t[ _csstrlen( SettingNode->text_.c_str() ) + 1 ];
                                    strcpy( pBuff, SettingNode->text_.c_str() );
                                    tchar_t* p = strtok( pBuff, "," );

                                    while( p )
                                    {
                                        ushort_t rport = atoi( p );

                                        if( rport == 0 )
                                        {
                                            remoteportvec.clear();
                                        }
                                        else
                                        {
                                            remoteportvec.push_back( atoi( p ) );
                                        }

                                        p = strtok( NULL, "," );
                                    }

                                    SAFE_DELETE_ARRAY( pBuff );
                                }
                                else if( SettingNode->name_ == _T( "SyncMsg" ) )
                                {
                                    XMLNode* SyncMsgCfg = SettingNode;
                                    baasesync = ( SyncMsgCfg->text_ == _T( "1" ) ) ? true : false;
                                }
                                else if( SettingNode->name_ == _T( "RiskMSTime" ) )
                                {
                                    SettingNode->text_ >> riskmstime;
                                }
                                else if( SettingNode->name_ == _T( "scs" ) )
                                {
                                    XMLNode* SCSCfg = SettingNode;
                                    ( *SCSCfg )[ _T( "Port" ) ].text_ >> SCSport;
                                }
                                else
                                {
                                    XMLNode* CfgNode = SettingNode;
                                    //XMLNode* ParNode = CfgNode;
                                    std::vector<XMLNode*> ParNodes;
                                    ParNodes.push_back( NULL );
                                    XMLNode* SubNode = CfgNode;

                                    while( SubNode != NULL )
                                    {
                                        //XMLNode* SubNode = CfgNode->FindFirstNode(NULL);
                                        if( SubNode->GetNodeSize() == 0 )
                                        {
                                            cs_string nodename;

                                            for( uint_t pi = 1; pi < ParNodes.size(); pi++ )
                                            {
                                                nodename += ParNodes[ pi ]->name_ + cs_string( _T( ">>" ) );
                                            }

                                            nodename += SubNode->name_;
                                            setting_node.insert( std::map<cs_string, cs_string>::value_type( nodename, SubNode->text_ ) );
                                            SubNode = ParNodes.back();
                                            continue;
                                        }
                                        else
                                        {
                                            if( SubNode->GetFindNodeIndex() == -1 )
                                            {
                                                ParNodes.push_back( SubNode );
                                            }

                                            SubNode = SubNode->FindNextNode( NULL );

                                            if( SubNode == NULL )
                                            {
                                                ParNodes.pop_back();
                                                SubNode = ParNodes.back();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        OutputMessage( log_mode, _T( ">>  Can't find node [ %d ] on configure!!\n" ), ObjectName() );
                        //loadret = false;
                        goto _LOADCONFIGURE_EXIT;
                    }
                }
            }
            else
            {
                OutputMessage( log_mode, _T( ">>  Can't find node [ ServerGroup ] on configure!!\n" ) );
                //loadret = false;
                goto _LOADCONFIGURE_EXIT;
            }
        }
        else
        {
            OutputMessage( log_mode, _T( ">>  Can't load configure file %s!!\n" ), filename );
            //loadret = false;
            goto _LOADCONFIGURE_EXIT;
        }

        //////////////////////////////////////////////////////////////////////////
        //
        {
            //v_coreserver_->SCSCreate( sectionid );
            v_coreserver_ = new CoreServer( this );
            cs_string log_msg;
            log_msg = _T( "||===========================================================================\n" );
            log_msg += _T( "||  cyberspace 【 %s 】 server startup........\n" );
            log_msg += _T( "||      build:  %s %s\n||\n" );

            if( StartUp( AllowedPlayers, port ) != 1 )
            {
                log_msg += _T( ">>  start up failure !\n" );
                log_msg += _T( "||===========================================================================\n\n" );
                OutputMessage( log_mode, log_msg.c_str(), ObjectName(), __DATE__, __TIME__ );
                //loadret = false;
                goto _LOADCONFIGURE_EXIT;
            }

            time_t startupsec = v_coreserver_->state_.Get_Startup_Stime();
            log_msg += _T( "||  Start on %s" );
            log_msg += _T( "||---------------------------------------------------------------------------\n" );
            OutputMessage( log_mode, log_msg.c_str(), ObjectName(), __DATE__, __TIME__, ctime( &startupsec ) );
            log_msg = _T( "" );
            //uint_t  ipcount = 0;
            //ipn4_t   ipn;

            //if ( GetLocalServerUid() == 0 )
            //{
            //    OutputMessage( log_mode, _T( ">>  UnitServer Id can't be 0!!...\n" ) );
            //    //loadret = false;
            //    goto _LOADCONFIGURE_EXIT;
            //}

            //OutputMessage( log_mode, _T( "||  ServerID : %d\n" ), GetLocalServerUid() );
            OutputMessage( log_mode, _T( "||  HostName : %s\n" ), GetLocalHostname( host, 255 ) );
            // 自己乖乖防錯
            //memset( v_coreserver_->name_service_, 0, _MAX_FUNCTIONNAME );
            name_service_ = servicename;
            OutputMessage( log_mode, _T( "||  Service  : %s\n" ), name_service_.c_str() );
            //
            //cs_string aliasname = (*ServerCfg)[_T("Alias")].text_.c_str();
            name_alias_ = aliasname;
            OutputMessage( log_mode, _T( "||  Alias    : %s\n" ), name_alias_.c_str() );
            //std::vector<ipa_t> ip4list;
            //std::vector<ipa_t> ip6list;
            //net::TCPServer::GetLocalIPAddress( ip4list, ip6list );
            std::vector<EthIf> ethif_list;
            net::Socket::GetEthIfinfo( ethif_list );

            if( ethif_list.empty() )
            {
                OutputMessage( log_mode, _T( ">>  Can't find any ip!!...\n" ) );
                //loadret = false;
                goto _LOADCONFIGURE_EXIT;
            }

            OutputMessage( log_mode, _T( "||  Enternet interface :\n" ) );

            for( cs_size_t ifi = 0; ifi < ethif_list.size(); ifi++ )
            {
                if( ethif_list[ ifi ].iftype == _IF_TYPE_ETHERNET )
                {
                    if( !ethif_list[ ifi ].unicast_addrs.empty() )
                    {
                        OutputMessage( log_mode, _T( "||    %-10s : " ), ethif_list[ ifi ].name.c_str() );

                        for( cs_size_t ip4i = 0; ip4i < ethif_list[ ifi ].unicast_addrs.size(); ip4i++ )
                        {
                            sockaddr* skaddr = ( sockaddr* )&ethif_list[ ifi ].unicast_addrs[ ip4i ];

                            if( skaddr->sa_family == AF_INET )
                            {
                                ipn4_t ipn4 = net::Socket::GetSockaddrIPN4( skaddr );
                                OutputMessage( log_mode, _T( "IPv4 = %-15s " ), net::Socket::IPN4ToA4( ipn4 ).c_str() );
                                //ifidex++;
                            }

                            if( skaddr->sa_family == AF_INET6 )
                            {
                                ipn6_t ipn6 = net::Socket::GetSockaddrIPN6( skaddr );
                                OutputMessage( log_mode, _T( "IPv6 = %-25s " ), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                                //OutputMessage( g_DefLogMode, _T( "||  Ip %-12s     : %-15s = %s\n" ), ethif_list[ ifi ].name.c_str(), net::Socket::IPN4ToA4( ipn4 ).c_str(), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                                //ifidex++;
                            }
                        }

                        OutputMessage( log_mode, _T( "\n" ) );
                    }
                }
            }

            OutputMessage( log_mode, _T( "||  Port : %d\n" ), GetLocalServicePort() );
            OutputMessage( log_mode, _T( "||  Max CsPSockConns : %d\n" ), GetLocalLimitConnectSize() );
            //
            //cs_string remoteportname = (*ServerCfg)[_T("RemotePort")].text_.c_str();
            v_coreserver_->is_base_sync_ = baasesync;
            OutputMessage( log_mode, _T( "||  Synchronize message : %s\n" ), ( v_coreserver_->is_base_sync_ == 1 ) ? "Y" : "N" );
            SetKeepAliveMstime( riskmstime );
            OutputMessage( log_mode, _T( "||  Risk mstime : %d\n" ), riskmstime );
            OutputMessage( log_mode, _T( "||  Allow remote port : " ) );

            if( remoteportvec.empty() )
            {
                OutputMessage( log_mode, "0" );
            }
            else
            {
                for( uint_t rp = 0; rp < remoteportvec.size(); rp++ )
                {
                    //ushort_t remoteport = 0;
                    //remoteportname >> remoteport;
                    v_coreserver_->InsertAllowRemotePort( remoteportvec[ rp ] );
                    OutputMessage( log_mode, _T( "%d, " ), remoteportvec[ rp ] );
                }
            }

            OutputMessage( log_mode, _T( "\n" ) );
        }

        //OutputMessage( log_mode, _T( "||  Load user configure....\n" ) );
        //////////////////////////////////////////////////////////////////////////
        // load extern config setting
        if( !OnLoadConfigure( setting_node ) )
        {
            OutputMessage( log_mode, _T( "||      ....failure\n" ) );
            //loadret = false;
            goto _LOADCONFIGURE_EXIT;
        }

        //OutputMessage( log_mode, _T( "||      ....successful\n" ) );
        //////////////////////////////////////////////////////////////////////////
        // Connect to ArbitServer and start scs system
        {
            XMLNode& DPCfg = xml[ _T( "ArbitServer" ) ];

            if( DPCfg.GetNodeSize() > 0 )
            {
                tchar_t  dpaddr[ 255 ];
                unsigned short dpport;
                strcpy( dpaddr, DPCfg[ _T( "Hostname" ) ].text_.c_str() );
                dpport = atoi( DPCfg[ _T( "Port" ) ].text_.c_str() );
                DPCfg[ _T( "MaxNodes" ) ].text_ >> AllowedSCSServer;

                if( SCSport == 0 )
                {
                    SCSport = dpport;
                }

                OutputMessage( log_mode, _T( "||\n||  startup scs-system on port:%d\n||  connect to Arbitration server %s:%d ...." ), SCSport, dpaddr, dpport );

                switch( v_coreserver_->SCSStartUp( dpaddr, dpport, SCSport ) )
                {
                case 1:
                    {
                        OutputMessage( log_mode, _T( "successful\n" ), host );
                    }
                    break;

                case 0:
                    {
                        OutputMessage( log_mode, _T( "failure, being to try connect..\n" ) );
                    }
                    break;

                default:
                    {
                        OutputMessage( log_mode, _T( "failure\n" ) );
                        //loadret = false;
                        goto _LOADCONFIGURE_EXIT;
                    }
                }
            }
            else
            {
                OutputMessage( log_mode, _T( "read arbitserver configure failure\n" ) );
                //loadret = false;
                goto _LOADCONFIGURE_EXIT;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // Connect to PaperServer and start scs system
        {
            std::vector<cs_string>       ips;
            std::vector<unsigned short> ports;
            XMLNode* GroupCfg = xml.FindFirstNode( _T( "PaperGroup" ) );

            if( GroupCfg != NULL )
            {
                //////////////////////////////////////////////////////////////////////////
                // Read configure file
                OutputMessage( log_mode, _T( "||\n||  connect to Paper server....\n" ) );
                XMLNode* ServerCfg = GroupCfg->FindFirstNode( _T( "PaperServer" ) );

                while( ServerCfg )
                {
                    for( uint_t ni = 0; ni < ServerCfg->GetNodeSize(); ni++ )
                    {
                        XMLNode* SettingNode = ServerCfg->FindNode( ni );

                        if( SettingNode != NULL )
                        {
                            if( SettingNode->name_ == _T( "Hostname" ) )
                            {
                                ips.push_back( SettingNode->text_ );
                            }
                            else if( SettingNode->name_ == _T( "AppPort" ) )
                            {
                                unsigned short port;
                                SettingNode->text_ >> port;
                                ports.push_back( port );
                            }
                        }
                    }

                    ServerCfg = xml.FindNextNode( _T( ObjectName() ) );
                }
            }
            else
            {
                OutputMessage( log_mode, _T( "**  Can't find configure file node [ PaperGroup ]!!\n" ) );
                //loadret = false;
                goto _LOADCONFIGURE_EXIT;
            }

            if( ( ips.size() != ports.size() ) || ( ips.size() == 0 ) )
            {
                OutputMessage( log_mode, _T( "**  load configure file node [ PaperServer ] failure!!\n" ) );
                //loadret = false;
                goto _LOADCONFIGURE_EXIT;
            }
            else
            {
                std::map<tstring, unsigned short> hosts;

                for( cs_size_t i = 0; i < ips.size(); i++ )
                {
                    OutputMessage( log_mode, _T( "||    %s:%d\n" ), ips[ i ].c_str(), ports[ i ] );
                    hosts.insert( std::map<tstring, unsigned short>::value_type( ips[ i ], ports[ i ] ) );
                }

                if( v_coreserver_->paper_system_.StartUp( hosts ) > 0 )
                {
                    OutputMessage( log_mode, _T( "||    ....successful\n" ) );
                }
                else
                {
                    OutputMessage( log_mode, _T( "||    ....failure\n" ) );
                }
            }
        }
        //v_coreserver_->OnStartUp();
        OutputMessage( log_mode, _T( "||\n||  server initialing ....\n" ) );

        if( OnServerInitialing() != 1 )
        {
            OutputMessage( log_mode, _T( "||    ....failure\n" ) );
            //loadret = false;
            goto _LOADCONFIGURE_EXIT;
        }

        OutputMessage( log_mode, _T( "||    ....success\n" ) );
        v_coreserver_->state_.Set_Iinitate_Mstime( pftml::Time::GetTickMSTime() );
        OutputMessage( log_mode, _T( "||  cyberspace 【 %s 】 server start successful.\n||===========================================================================\n" ), ObjectName() );
        val = v_coreserver_->paper_system_.Set( _T( "Test" ), _T( "1234" ), 0 );
        val = v_coreserver_->paper_system_.Setnx( _T( "Test" ), _T( "5678" ), 0 );
        //val = v_coreserver_->paper_system_.Get( _T( "Test" ) );
        //val = v_coreserver_->paper_system_.Del( _T( "Test" ) );
        return true;
    _LOADCONFIGURE_EXIT:
        OutputMessage( log_mode, _T( "||  cyberspace 【 %s 】 server start falure.\n||===========================================================================\n" ), ObjectName() );
        Shutdown();
        return false;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::Update( void )
    {
        if( v_coreserver_ == NULL )
        {
            return -1;
        }
        v_coreserver_->Update();
        return 1;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    // sum of receive message
    uint_t UnitServer::GetSumRecv( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->state_.Get_Cnt_Recv_Message();
        return -1;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    uint_t UnitServer::GetLongProc( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->state_.Get_Cnt_Long_Process();
        return -1;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::SetThreadHistroy( bool t )
    {
        if( v_coreserver_ != NULL )
            v_coreserver_->SetThreadHistory( t );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    cs_size_t UnitServer::GetThreadHistory( std::vector<channel_uid_t>& chrd, std::vector<mstimen_t>& tmrd )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->GetThreadHistory( chrd, tmrd );
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    // live_sec state
    bool UnitServer::IsActive( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->IsActive();
        return false;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    /*
    pfkrl::PfSytem::Thread::Handle UnitServer::BeginProcess(func_address func, void* data)
    {
       //return v_coreserver_->BeginProcess(func, data);
       return pfkrl::Thread::BeginProcess(func, data);
    }
    */


    //=============================================================================
    //
    //
    //=============================================================================
    cs_string UnitServer::GetSectionName( CsSectionDescriptor sectiondp )
    {
        if( v_coreserver_ != NULL )
        {
            return v_coreserver_->GetSectionSectionName( sectiondp );
        }
        return "";
    }


    //=============================================================================
    //
    //! Key為上層使用的資料, 設定連結傳回舊的key 值
    //=============================================================================
    //void* UnitServer::SetSocketKey( CsSessionDescriptor sdp, void* key )
    //{
    //    if ( GetSessionIsMaster( sdp ) )
    //    {
    //        return CoreServer::SetSessionKey( sdp, key );
    //    }
    //    else
    //    {
    //        // TODO: Pin
    //        //
    //    }
    //
    //    return NULL;
    //}

    //=============================================================================
    //
    //! Key為上層使用的資料, 取得連結
    //=============================================================================
    //void* UnitServer::GetSocketKey( CsSessionDescriptor sdp )
    //{
    //    if ( GetSessionIsMaster( sdp ) )
    //    {
    //        return CoreServer::GetSessionKey( sdp );
    //    }
    //    else
    //    {
    //        // TODO: Pin
    //        //
    //    }
    //
    //    return NULL;
    //}


    //=============================================================================
    //
    //
    //=============================================================================
    //XDatabase*  UnitServer::OnCreateCommonDB( void )
    //{
    //    return ( XDatabase* )new xdb::MySqlDB;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::OnDeleteCommonDB( XDatabase* db )
    //{
    //    xdb::MySqlDB* xdb = ( xdb::MySqlDB* ) db;
    //    SAFE_DELETE( xdb );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //XDatabase* UnitServer::GetCustomDB( cs_session_uid_t session_uid )
    //{
    //    if ( database_[ 1 ].size() == 0 )
    //    {
    //        return NULL;
    //    }
    //
    //    return database_[ 1 ][ session_uid % database_[ 1 ].size() ];
    //}


    //=============================================================================
    //
    //
    //=============================================================================
    //XDatabase* UnitServer::GetCommonDB( cs_session_uid_t session_uid )
    //{
    //    if ( database_[ 0 ].size() == 0 )
    //    {
    //        return NULL;
    //    }
    //
    //    return database_[ 0 ][ session_uid % database_[ 0 ].size() ];
    //}


    //=============================================================================
    //
    //
    //=============================================================================
    stimen_t UnitServer::GetStartSec( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->DoGetCreateTime();
        return 0;
    }

    //=============================================================================
    //
    // 取得跟 所有機器同步的時間
    //=============================================================================
    stimen_t UnitServer::GetSyncSec( void )
    {
        time_t timep = pftml::Time::GetSTime();
        //time( &timep );
        return timep;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SetState( Pflag t )
    //{
    //    v_coreserver_->SetState( t );
    //    return;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //Pflag UnitServer::GetState( void )
    //{
    //    return v_coreserver_->GetState();
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::GetLocalIPAddress( _OUT std::vector<ipa_t>& ip4list, _OUT std::vector<ipa_t>& ip6list )
    //{
    //	net::TCPServer::GetLocalIPAddress( ip4list, ip6list );
    //
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    const tchar_t* UnitServer::GetLocalHostname( tchar_t* buf, cs_size_t len )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->DoGetLocalHostname( buf, len );
        return NULL;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    unsigned short UnitServer::GetLocalServicePort( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->DoGetLocalServicePort();
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    const tchar_t* UnitServer::GetLocalAliasName( void )
    {
        return name_alias_.c_str();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    const tchar_t* UnitServer::GetLocalServiceName( void )
    {
        return name_service_.c_str();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    uint_t UnitServer::GetLocalPollingCount( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->GetPollingCount();
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //  uint_t UnitServer::GetLocalLivingThreadSize(void)
    //  {
    //      return v_coreserver_->GetLivingThreadSize();
    //  }

    //=============================================================================
    //
    //
    //=============================================================================
    cs_section_uid_t UnitServer::GetLocalServerUid( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->GetLocalServerUid();
        return CsINVALID_ID;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    unsigned int UnitServer::GetLocalLimitConnectSize( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->DoGetLocalLimitConnectSize();
        return  0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    uint_t UnitServer::GetLocalConnectedSize( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->DoGetLocalConnectedSize();
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    uint_t UnitServer::GetLocalMaxConnectedSize( void )
    {
        if( v_coreserver_ != NULL )
            return v_coreserver_->Get_Max_Connected_Client();
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //  mstimen_t UnitServer::GetSystemMaxUpdateTime(void)
    //  {
    //      return v_coreserver_->m_MaxUpdateMSTime;
    //  }

    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::RegisterUserDefChannel( channel_uid_t session_uid, unit_member_func func )
    {
        if( ( session_uid < CsID_BASE ) && ( v_coreserver_ != NULL ) )
        {
            v_coreserver_->channel_functions_.insert( std::map<channel_uid_t, unit_member_func>::value_type( session_uid, func ) );
            return true;
            //return v_coreserver_->channel_functions_.RegisterChannel( session_uid, ( ChannelFunction ) func );
        }

        return false;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::UnRegisterUserDefChannel( channel_uid_t session_uid )
    {
        if( v_coreserver_ != NULL )
        {
            v_coreserver_->channel_functions_.erase( session_uid );
            return true;
            //return v_coreserver_->channel_functions_.UnRegisterChannel( session_uid );

        }

        return false;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::GetAllChannelId( std::vector<channel_uid_t>& channellist )
    {
        if( v_coreserver_ != NULL )
        {
            for( std::map<channel_uid_t, unit_member_func>::iterator iter = v_coreserver_->channel_functions_.begin(); iter != v_coreserver_->channel_functions_.end(); iter++ )
            {
                channellist.push_back( iter->first );
            }
            //return v_coreserver_->m_UserDefMessageDispatcher.GetAllChannelId( channellist );
        }
        return;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //uniqueid_t UnitServer::CreateUniqueId( int gid )
    //{
    //    return DBUnique::dbCreateUniqueId( GetCommonDB( gid ), gid );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //bool UnitServer::DeleteUniqueId( uniqueid_t uid )
    //{
    //    DBUnique::dbDeleteUniqueId( GetCommonDB( uid ), uid );
    //    return true;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    channel_uid_t UnitServer::GetStageChannel( CsSessionDescriptor sdp )
    {
        if( ( GetSessionIsClone( sdp ) ) && ( v_coreserver_ != NULL ) )
        {
            return v_coreserver_->GetStageChannel( sdp );
        }

        return CsINVALID_ID;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::GetLocalServerReport( ServerState& sinfo )
    {
        if( v_coreserver_ != NULL )
            v_coreserver_->GetLocalServerReport( sinfo );
    }


    //=============================================================================
    //
    //
    //=============================================================================
    cs_session_uid_t UnitServer::GetSessionSessionUid( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return cyberspace::CoreServer::GetSessionSessionUid( sdp );
        }

        return CsINVALID_ID;
        //return v_coreserver_->GetCId(s);
    }


    //=============================================================================
    //
    //
    //=============================================================================
    //macn_t UnitServer::GetSessionMac( CsSessionDescriptor sdp )
    //{
    //    if ( sdp != NULL )
    //    {
    //        return cyberspace::Converger::SCSGetSessionMac( sdp );
    //    }
    //    
    //    macn_t mac = CsINVALID_ID;
    //    return mac;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    ipn6_t UnitServer::GetSessionIpn6( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return cyberspace::CoreServer::GetSessionIpn6( sdp );
        }

        ipn6_t ipn;
        memset( &ipn, 0, _cssizeof( ipn6_t ) );
        return ipn;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ipa_t UnitServer::GetSessionIpa( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return net::Socket::IPN6ToA6( GetSessionIpn6( sdp ) );
        }

        return NULL;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ushort_t UnitServer::GetSessionPort( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return cyberspace::CoreServer::GetSessionPort( sdp );
        }

        return 0;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    cs_section_uid_t UnitServer::GetSessionServerUid( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return cyberspace::CoreServer::GetSessionSectionId( sdp );
        }

        return CsINVALID_ID;
    }

    ////===============================================================================//
    ////
    ////
    ////===============================================================================//
    //cs_group_uid_t UnitServer::GetSessionGroup( CsSessionDescriptor sdp )
    //{
    //    if ( sdp != NULL )
    //    {
    //        return cyberspace::Converger::SCSGetSessionGroup( sdp );
    //    }
    //
    //    return -1;
    //}

    ////===============================================================================//
    ////
    ////
    ////===============================================================================//
    //cs_authority_t UnitServer::GetSessionPower( CsSessionDescriptor sdp )
    //{
    //    if ( sdp != NULL )
    //    {
    //        return cyberspace::Converger::SCSGetSessionPower( sdp );
    //    }
    //
    //    return -1;
    //}

    ////===============================================================================//
    ////
    ////
    ////===============================================================================//
    //void UnitServer::SetSessionPower( CsSessionDescriptor sdp, cs_authority_t power )
    //{
    //    if ( sdp != NULL )
    //    {
    //        cyberspace::Converger::SCSGetSessionPower( sdp );
    //    }
    //
    //    return;
    //}

    //=============================================================================
    //
    //! Key為上層使用的資料, 設定連結傳回舊的key 值
    //=============================================================================
    //CsSessionNote* UnitServer::SetSessionNote( CsSessionDescriptor cdp, CsSessionNote* key )
    //{
    //    return cyberspace::CoreServer::SetSessionNote( cdp, key );
    //}
    //
    //void UnitServer::DelSessionNote( CsSessionDescriptor cdp, CsSessionNote* key )
    //{
    //    return cyberspace::CoreServer::DelSessionNote( cdp, key );
    //}


    //=============================================================================
    //
    //! Key為上層使用的資料, 取得連結
    //=============================================================================
    //CsSessionNote* UnitServer::GetSessionNote( CsSessionDescriptor cdp, notetid_t notetid )
    //{
    //    return cyberspace::CoreServer::GetSessionNote( cdp, notetid );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //long32_t UnitServer::GetSessionFlag( CsSessionDescriptor sdp )
    //{
    //	if ( sdp != NULL )
    //	{
    //		return cyberspace::Converger::SCSGetSessionFlag( sdp );
    //	}
    //
    //	return 0;
    //}


    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SetSessionFlag( CsSessionDescriptor sdp, long32_t flag )
    //{
    //	if ( sdp != NULL )
    //	{
    //		cyberspace::Converger::SCSSetSessionFlag( sdp, flag );
    //	}
    //
    //	return;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SyncSession( CsSessionDescriptor sdp )
    //{
    //    if ( sdp != NULL )
    //    {
    //        v_coreserver_->SCSSyncClient( sdp );
    //    }
    //    
    //    return;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    stimen_t UnitServer::GetConnectCreateTime( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return cyberspace::CoreServer::GetSessionCreateSec( sdp );
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    socket_t UnitServer::GetConnectSocketId( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            CsAbtpCnSocket* pSkt = cyberspace::CoreServer::GetSessionSocket( sdp );
            if( pSkt )
            {
                return pSkt->GetFD();
            }
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::GetSessionIsMaster( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return cyberspace::CoreServer::GetSessionIsMaster( sdp );
        }

        return false;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::GetSessionIsClone( CsSessionDescriptor sdp )
    {
        if( sdp != NULL )
        {
            return cyberspace::CoreServer::GetSessionIsClone( sdp );
        }

        return true;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::IsKickout( CsSessionDescriptor sdp )
    {
        return CoreServer::IsKickout( sdp ); //IF_BIT_FLAG(sdp->state, _CONNECTNODE_KICKOUT);
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::IsLive( CsSessionDescriptor sdp )
    {
        if( GetSessionIsClone( sdp ) )
        {
            return false;
        }

        if( sdp != NULL )
        {
            return CoreServer::IsLive( sdp );
        }

        return false;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    cs_size_t UnitServer::GetConnectSize( CseCONNECT_TYPE t )
    {
        return v_coreserver_->GetConnectSize( t );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    mstimen_t UnitServer::GetLastRecvMST( CsSessionDescriptor sdp )
    {
        if( GetSessionIsClone( sdp ) )
        {
            return 0;
        }

        if( sdp != NULL )
        {
            return CoreServer::GetLastRecvMST( sdp );
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    CsSessionDescriptor UnitServer::GetSessionDescriptor( const tchar_t* name )
    {
        assert( 0 && "not finish yet!" );
        return NULL;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::GetAllLocalSessionDescriptor( std::vector<CsSessionDescriptor>& CsPSockConnlist )
    {
        return v_coreserver_->GetAllLocalSessionDescriptor( CsPSockConnlist );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    CsSessionDescriptor UnitServer::GetLocalSessionDescriptor( socket_t skt )
    {
        return v_coreserver_->GetSessionDescriptor( v_coreserver_->FindConnectedSocket( skt ) );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::Kickout( CsSessionDescriptor sdp )
    {
        v_coreserver_->Kickout( sdp );
        return;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    //
    //
    //=============================================================================
    //cs_session_uid_t UnitServer::GetMemberSessionUid( const tchar_t* name )
    //{
    //    /*
    //    {
    //     TS_Lock lock(&v_coreserver_->m_lCIdMemberNameCache);
    //     std::map<cs_string, cs_session_uid_t>::iterator it=v_coreserver_->m_tCIdMemberNameCache.find(name);
    //     if(it!=v_coreserver_->m_tCIdMemberNameCache.end())
    //      return it->second;
    //    }
    //    */
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        cs_session_uid_t cid = cs_db_function::member::GetSessionUid( GetCommonDB( 0 ), name );
    //        /*
    //        {
    //         TS_Lock lock(&v_coreserver_->m_lCIdMemberNameCache);
    //         v_coreserver_->m_tCIdMemberNameCache.insert(std::map<cs_string, cs_session_uid_t>::value_type(name, cid));
    //        }
    //        */
    //        return cid;
    //    }
    //
    //    return CsINVALID_ID;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //cs_string UnitServer::GetMemberName( cs_session_uid_t session_uid )
    //{
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        return cs_db_function::member::GetUsername( GetCommonDB( session_uid ), session_uid );
    //    }
    //
    //    return _T( "" );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //CseMEMBER_STATE UnitServer::member::GetState( cs_session_uid_t session_uid )
    //{
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        return cs_db_function::member::dbcsmember::GetState( GetCommonDB( session_uid ), session_uid );
    //    }
    //
    //    return ACCSTATE_MAX;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::member::SetState( cs_session_uid_t session_uid, CseMEMBER_STATE state )
    //{
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        cs_db_function::member::dbcsmember::SetState( GetCommonDB( session_uid ), session_uid, state );
    //    }
    //
    //    return;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //cs_authority_t UnitServer::member::GetPower( cs_session_uid_t session_uid )
    //{
    //    cs_authority_t ret = 0;
    //
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        return cs_db_function::member::dbcsmember::GetPower( GetCommonDB( session_uid ), session_uid, ret );
    //    }
    //
    //    return ret;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //longlong_t UnitServer::member::GetPoint( cs_session_uid_t session_uid )
    //{
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        return cs_db_function::member::dbcsmember::GetPoint( GetCommonDB( session_uid ), session_uid );
    //    }
    //
    //    return 0;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::member::SetPoint( cs_session_uid_t session_uid, longlong_t point )
    //{
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        cs_db_function::member::dbcsmember::SetPoint( GetCommonDB( session_uid ), session_uid, point );
    //    }
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //longlong_t UnitServer::member::AddPoint( cs_session_uid_t session_uid, longlong_t point )
    //{
    //    if ( v_coreserver_->m_bDBMember )
    //    {
    //        return cs_db_function::member::dbcsmember::AddPoint( GetCommonDB( session_uid ), session_uid, point );
    //    }
    //
    //    return 0;
    //}


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Paper UnitServer
    //=============================================================================
    //
    // set value into paper server if exist return old key
    //=============================================================================
    cs_string UnitServer::PaperSet( _IN cs_string key, _IN cs_string val, _IN int live_sec )
    {
        return v_coreserver_->paper_system_.Set( key, val, live_sec );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    cs_string UnitServer::PaperSetnx( _IN cs_string key, _IN cs_string val, _IN int live_sec )
    {
        return v_coreserver_->paper_system_.Setnx( key, val, live_sec );
    }

    //=============================================================================
    //
    //
    //  @val: return current value
    // return:  1 : success, 0 : no any value, -1 : value over time
    //=============================================================================
    cs_string UnitServer::PaperGet( _IN cs_string key )
    {
        return v_coreserver_->paper_system_.Get( key );
    }

    //=============================================================================
    //
    //
    //  @val: return delete value
    //=============================================================================
    cs_string UnitServer::PaperDel( _IN cs_string key )
    {
        return v_coreserver_->paper_system_.Del( key );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    //
    //
    //=============================================================================
    CsSessionDescriptor UnitServer::GetSessionDescriptor( cs_session_uid_t session_uid )
    {
        return v_coreserver_->GetSessionDescriptor( session_uid );
    }

    /*
      //=============================================================================
    //
    //
      //=============================================================================
    macn_t UnitServer::GetSessionMac(cs_session_uid_t session_uid)
    {
        CsSessionDescriptor sdp = GetSession(session_uid);
        if(s == NULL)
        {
            macn_t addr;
            //DSM
            return addr;
        }
        return GetSessionMac(s);
    }

      //=============================================================================
    //
    //
      //=============================================================================
    ipn4_t UnitServer::GetSessionIpn4(cs_session_uid_t session_uid)
    {
        ipn4_t ipn;
        CsSessionDescriptor sdp = GetSession(session_uid);
        if(s!=NULL)
            ipn=s->GetInternetAddress();
        return ipn;
    }

      //=============================================================================
    //
    //
      //=============================================================================
    void UnitServer::Kickout(cs_session_uid_t cid)
    {
        OutputMessage(GetLogMode(), " Kickout CsAbtpCnSocket %lld\n", cid);
        CsSessionDescriptor sdp = GetSession(cid);
        if(s==NULL)
        {
            v_coreserver_->ArbitKickoutClient(cid);
         cs_db_function::member::dbcsmember::SetState(GetCommonDB(), cid, ACCSTATE_OFFLINE);
        }
        else
        {
            v_coreserver_->Kickout(s);
        }
        return;
    }
    */

    ///////////////////////////////////////////////////////////////////////////////////
    //

    //=============================================================================
    //
    //
    //=============================================================================
    //CsLogMode UnitServer::GetLogMode()
    //{
    //    return v_coreserver_->GetLogMode();
    //};

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SetLogMode( CsLogMode m )
    //{
    //    v_coreserver_->SetLogMode( m );
    //};

    //=============================================================================
    //
    //
    //=============================================================================
    CsLogMode UnitServer::GetLogMode()
    {
        return g_log_mode;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::SetLogMode( CsLogMode m )
    {
        g_log_mode = m;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //static T_Lock g_lLogMessage;
    void UnitServer::OutputMessage( CsLogMode m, const tchar_t* format, ... )
    {
        //tchar_t string[ _MAX_LONG_MSG ] = { "" };
        //FILE str;
        //register FILE *outfile = &str;
        va_list arglist;
        //register int retval;
        //const tchar_t* i = format;
        va_start( arglist, format );
        //assert( string != NULL );
        //assert( format != NULL );
        //vsprintf( string, format, arglist );
        cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, m, format, arglist );
        va_end( arglist );
        //cyberspace::LogSerivce::OutputMessage( cyberspace::Arbitration::g_log_index, m, format, arglist );
        return;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SetLogMode( CsLogMode mode )
    //{
    //    v_coreserver_->SetLogMode( mode );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //CsLogMode UnitServer::GetLogMode( void )
    //{
    //    return v_coreserver_->GetLogMode();
    //}

    //    //=============================================================================
    //  //
    //  //
    //    //=============================================================================
    //  int UnitServer::SendCsMessage(cs_session_uid_t dest, uniqueid_t from, channel_uid_t cid, const void* bp, cs_size_t len)
    //  {
    //      int ret = v_coreserver_->SendClientMessage(dest, from, cid, bp, len);
    // //       pfstd::MsSleep(0);
    //      return ret;
    //  }

    //    //=============================================================================
    //  //
    //  //
    //    //=============================================================================
    //  void UnitServer::BroadcastMessage(uniqueid_t uid, channel_uid_t cid, void *bp, cs_size_t len)
    //  {
    //      for(uint_t i=0; i<GetLocalConnectedSize(); i++)
    //      {
    //          SendCsMessage(GetSessionByIndex(i), uid, cid, bp, len);
    //      }
    //      return;
    //  }



    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::SetBanClient( CsSessionDescriptor sdp, bool bBlock )
    {
        _csbinbuf buf;
        cs_size_t len = CsBaseCommand::BanClient::Server::SetRequest( buf, bBlock );
        SendCsMessage( sdp, CsINVALID_ID, CsS_C_BAN_CLIENT, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
        return;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::SetKeepAliveMstime( mstimen_t mst )
    {
        v_coreserver_->SetKeepAliveMstime( mst );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SetRiskAddress( int b )
    //{
    //    v_coreserver_->SetRiskAddress( b );
    //}


    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::InsertAllowRemotePort( ushort_t port )
    {
        v_coreserver_->InsertAllowRemotePort( port );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //bool UnitServer::BindSession( CsSessionDescriptor sdp, cs_session_uid_t cid )
    //{
    //    //要先將cid 加密**********
    //    //CscombinPacket hChMsg;
    //    //cs_size_t len = CsBaseCommand::SetConnectId::Server::SetRequest(&hChMsg, cid);
    //    //SEND_BASE_MESSAGE(s, &hChMsg);
    //    v_coreserver_->BindSession( sdp, cid );
    //    _csbinbuf buf;
    //    CsBaseCommand::BindSession::Server::SetRequest( buf, cid, 0, 0 );
    //    SendCsMessage( sdp, CsINVALID_ID, CsS_C_BINDSESSION_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
    //    return true;
    //}
    /*
    T_Lock g_CreateSessionLock;
    bool UnitServer::CreateSession(CsSessionDescriptor sdp, cs_session_uid_t cid, cs_authority_t pid, long32_t flag, const tchar_t* username)
    {
        CsSSession* scn = v_coreserver_->GetCsSSession(sdp);
        if(csc==NULL)
            return false;

        // 從資料庫中取出這個帳號的ID
        //cs_session_uid_t cid=GetMemberSessionUid(username);
        //cs_authority_t   pid = -1;
        if(cid!=CsINVALID_ID)
        {
            TS_Lock lock(&g_CreateSessionLock);

            bool wait=false;
            CsSessionDescriptor srced=GetSessionDescriptor(cid);
            if(srced!=NULL)
                wait=true;

            macn_t mac = GetSessionMac(sdp);

            // 填入這個連線的
            // ID, MAC, username, IP, server_uid, 建立時間, 權限編號
            v_coreserver_->SetSession(csc, cid, mac, pid, flag, GetSyncSec(), CsINVALID_ID);

            cs_db_function::member::dbcsmember::SetLogin(GetCommonDB(cid), cid, mac , ACCSTATE_ONLINE);

         // 更新這次記錄的資料
            DBHistory::dbcsmember::SetLogin(GetCommonDB(cid), sdp->index, cid, net::Socket::IPN4ToA4(GetSessionIpn4(sdp)).c_str(), mac, 0);

            v_coreserver_->SCSCreateSession(sdp);

            pid = sdp->power;
        }

        //要先將cid 加密**********
        //CscombinPacket hChMsg;
        //cs_size_t len = CsBaseCommand::SetConnectId::Server::SetRequest(&hChMsg, cid);
     //SEND_BASE_MESSAGE(s, &hChMsg);

        _csbinbuf buf;
        CsBaseCommand::SetConnectId::Server::SetRequest(buf, cid, pid);
        SendCsMessage(sdp, CsINVALID_ID, CsS_C_SETCONNECTID_REQUEST, _CsGetResBufPtr(buf), _CsGetResBufLen(buf));

    // #ifdef _DEBUG
    //       //CsSSession* scn = v_coreserver_->GetCsSSession(s);
    //
    //       char msgbuf[255];
    //       _stprintf(msgbuf, _T("Set socket %d as ID:%lld "), (csc->GetFD()), cid);
    //       PfOutputDebugString(msgbuf);
    // #endif

        return true;
    }
    */

    /*
      //=============================================================================
    //
    //
    //
      //=============================================================================
    void UnitServer::OnRemoveSession(CsSessionDescriptor sdp)
    {
    // 改成斷線的那個人去做
    //       unsigned int duration_sec  = 0; // 連線的時間
    //       unsigned int frequency_count = 0; // 連線的次數
    //
    //       if(DBHistory::dbGetMemberHistory(GetCommonDB(), s->index, duration_sec, frequency_count))
    //       {
    //           //stimen_t timep = GetSyncSec();
    //           //duration_sec+= (timep-GetConnectCreateTime(s));
    //           stimen_t timep = GetSyncSec();
    //           duration_sec=(timep-GetConnectCreateTime(s));
    //           DBHistory::dbcsmember::SetLogout(GetCommonDB(), s->index, duration_sec);
    //       }
    //       else
    //       {
    //           //s->index = DBHistory::dbBeginMemberHistory(GetCommonDB(), cid);
    //       }

        if(v_coreserver_->m_bDBMember)
        {
            stimen_t logouttimee = 0;
            stimen_t logintimee  = 0;
            stimen_t currtimee   = 0;
            cs_session_uid_t cid=GetSessionSessionUid(sdp);
            if(cid!=CsINVALID_ID)
            {
                CseMEMBER_STATE state;
                cs_db_function::member::dbcsmember::GetState(GetCommonDB(cid), cid, state, logouttimee, logintimee, currtimee);
                // 如果是正常上線的狀態 才會被自動清為下線
                if(state==ACCSTATE_ONLINE)
                {
                    stimen_t timep=0;
                    if(m_bSaveLogoutSec)
                    {
                        //time(&timep);
                        timep=-1;
                    }
                    else
                    {
                        timep=0;
                    }

                    cs_db_function::member::dbcsmember::SetLogout(GetCommonDB(cid), cid, timep);
                }
            }
        }
    }
    */


    /*
            //===============================================================================//
            //
            //
            //! 這邊進來的一定要是一個Socket
            //===============================================================================//
            bool UnitServer::SetSession(CsSessionDescriptor sdp, cs_session_uid_t cid, macn_t mac)
            {
                CsSSession* scn = v_coreserver_->GetCsSSession(s);
                if(csc==NULL)
                    return false;

                s->cid    = cid;
                s->mac    = mac;
                s->sid    = GetLocalServerUid(); // the connected server
                s->ipn    = csc->GetInternetAddress();
                s->createsec = GetSyncSec();


                CsSessionDescriptor ced=GetSessionDescriptor(cid);
                //if(cid != CsINVALID_ID)
                // 別台已經建立過了
                if(ced)
                {
                    csc->SetPower(GetSessionPId(ced));
                    //          cs_string  name;
                    //          int    power;
                    //          cs_db_function::member::dbcsmember::GetIdentity(GetCommonDB(), cid, name, power);
                    //          csc->SetUsername(name.c_str());
                    //          csc->SetPower(power);
                    return true;
                }
                //沒有人建立過
                else
                {
                    //csc->SetUsername("");
                    csc->SetPower(-1);

                    // 如果他已經被分派到CID 代表 別台伺服器應該已經記錄過他的登入資料了
                    // 不管成功或失敗登入都應該有記錄 所以 在這邊分配給他ID
                    if(s->index==CsINVALID_ID)
                        s->index  = DBHistory::dbBeginMemberHistory(GetCommonDB(), cid, net::Socket::IPN4ToA4(s->ipn).c_str(), mac);
                    return false;
                }
            }*/




            /*
              //=============================================================================
            // NAME :
            // DESC : 做連線的驗證，並且回報給 arbit-server 有一個連線加入
              //=============================================================================
            bool UnitServer::DoVerifyJoinSession(CsSessionDescriptor sdp)
            {
                cs_session_uid_t cid = GetSessionSessionUid(sdp);
                macn_t   mac = GetSessionMac(sdp);

                if(IsCsInvalidId(cid))
                {
                    return false;
                }

                // 從有沒有Clone做驗證
            //       CsSessionDescriptor ced=GetSessionDescriptor(cid);
            //       if(ced)
            //       {
            //           if(GetSessionMac(ced)==mac)
            //           {
            //               if(!v_coreserver_->SCSCreateSession(sdp))
            //                   return false;
            //               return true;
            //           }
            //       }
            //       return false;

                int state;
                macn_t macadd;
                int mtype;
                if(cs_db_function::member::dbcsmember::GetCheckQualification(GetCommonDB(cid), cid, state, macadd, mtype))
                {
                    if(((state==ACCSTATE_ONLINE)&&(macadd==GetSessionMac(sdp)))||  // 要檢查MAC
                        ((state==ACCSTATE_ONLINE)&&(mtype==2)))        // 不要檢查MAC
                    {
                        if(!v_coreserver_->SCSCreateSession(sdp))
                            return false;
                        return true;
                    }
                    else
                    {
                        if(state!=ACCSTATE_ONLINE)
                        {
                            printf("Member %lld state not online\n", cid);
                        }
                        if(!(macadd==mac))
                        {
                            printf("Member %lld MId not the same\n", cid);
                        }
                        return false;
                    }
                }

                return false;
            }
            */

            //=============================================================================
            //
            //
            //=============================================================================
            /*
                void UnitServer::DoVerifyExitSession(CsSessionDescriptor sdp)
                {
                    // TODO: Pin
                    // 其實應該 如果是 Clone Kickout 裡面只有呼叫 SCSDelectSession所以應該不會跑到這邊 (但如果是被呼叫的狀況 就會)
                    //if(GetSessionIsMaster(s)) 如果是交換連線的時候 傳進來的也還是一個Master可是此時不應該去清除資料庫

                    cs_session_uid_t cid = GetSessionSessionUid(sdp);
                    if(v_coreserver_->SCSDelectSession(sdp))
                    {
                        stimen_t    timee  = 0;
                        unsigned int duration_sec  = 0; // 連線的時間
                        unsigned int frequency_count = 0; // 連線的次數

                        if(DBHistory::dbGetMemberHistory(GetCommonDB(cid), sdp->index, duration_sec, frequency_count))
                        {
                            //stimen_t timep = GetSyncSec();
                            //duration_sec+= (timep-GetConnectCreateTime(s));
                            stimen_t timep = GetSyncSec();
                            duration_sec=(timep-GetConnectCreateTime(sdp));
                            DBHistory::dbcsmember::SetLogout(GetCommonDB(cid), sdp->index, duration_sec);
                        }
                        else
                        {
                            //s->index = DBHistory::dbBeginMemberHistory(GetCommonDB(), cid);
                        }

            //
            //          cs_db_function::member::dbcsmember::GetState(GetCommonDB(), cid, state, timee);
            //          // 如果是正常上線的狀態 才會被自動清為下線
            //          if(state==ACCSTATE_ONLINE)
            //          {
            //              stimen_t timep=-1;
            //              if(m_bSaveLogoutSec)
            //              {
            //                  time(&timep);
            //              }
            //
            //              cs_db_function::member::dbcsmember::SetLogout(GetCommonDB(), cid, timep);
            //          }
                    }



                    // 應該要判斷他是被 server 呼叫 還是被 arbit
                    //if(server 呼叫)
                    // 在裡面會判斷 有沒有在 list裡面，如果有 才會回報 沒有表示被拿掉了 就不回報

                    //else
                    //  不呼叫


            //      // 如被ARBIT的 ONRECEIVE喚起 會死結
            //      cs_session_uid_t cid=GetSessionSessionUid(s);
            //
            //      // 如果找部到，表示他是被 Arbitration移除的
            //      if(GetSession(cid)==NULL)
            //          return;
            //
            //      // True表示是自己機器移除自己的 FALSE被ARBIT給踢除的
            //      if(scn->m_Kick.GetValue())
            //      {
            //          unsigned int duration_sec = 0;
            //          unsigned int frequency_count = 0;
            //          if(DBHistory::dbGetMemberHistory(GetCommonDB(), cid, duration_sec, frequency_count))
            //          {
            //              stimen_t timep;
            //              time(&timep);
            //              duration_sec+= timep-GetLastCreateTime(s);
            //          }
            //          else
            //          {
            //              DBHistory::dbBeginMemberHistory(GetCommonDB(), cid);
            //          }
            //
            //          DBHistory::dbcsmember::SetLogout(GetCommonDB(), cid, duration_sec);
            //          cs_db_function::member::dbcsmember::SetLogout(GetCommonDB(), cid);
            //          //OutputMessage(GetLogMode(), " VerifyExit 2\n");
            //          v_coreserver_->SCSDelectSession(s);
            //      }
            //      scn->m_Kick.SetValue(true);

                    return;
                }
            */


            /*

                //=============================================================================
              //
              //
                //=============================================================================
                 // 請CLIENT 暫停送資料
                 void UnitServer::SetBanClient(cs_session_uid_t cid, bool bBlock)
              {
                     _csbinbuf buf;
                     cs_size_t len = CsBaseCommand::BanClient::Server::SetRequest(buf, bBlock);
                     SendCsMessage(cid, CsINVALID_ID, CsS_C_BAN_CLIENT, &(buf[0]), buf.size());
                     return;
                 }
                    //===============================================================================//
                    //
                    //
                    //===============================================================================//
                    void UnitServer::SetAllowConnect(cs_session_uid_t aid, bool allow)
                    {
                        unsigned int duration_sec = 0;
                        unsigned int frequency_count = 0;

                        CsSessionDescriptor sdp = GetSession(aid);
                        if(allow)
                        {
                            macn_t mac = GetSessionMac(socket);
                            cs_db_function::member::dbcsmember::SetLogin(GetCommonDB(), aid, mac);

                            if(DBHistory::dbGetMemberHistory(GetCommonDB(), aid, duration_sec, frequency_count))
                            {
                                frequency_count++;
                            }
                            else
                            {
                                frequency_count=1;
                                DBHistory::dbBeginMemberHistory(GetCommonDB(), aid);
                            }

                            DBHistory::dbcsmember::SetLogin(GetCommonDB(), aid, net::Socket::IPN4ToA4(socket->GetInternetAddress()), frequency_count);

                        }
                        else
                        {
                            if(DBHistory::dbGetMemberHistory(GetCommonDB(), aid, duration_sec, frequency_count))
                            {
                                //OutputMessage(GetLogMode(), " Member:%d on logged out 2\n", cid);
                                stimen_t timep;
                                time(&timep);
                                duration_sec+= timep-socket->GetLastCreateTime();
                                //OutputMessage(GetLogMode(), " Member:%d on logged out 3\n", cid);
                            }
                            else
                            {
                                DBHistory::dbBeginMemberHistory(GetCommonDB(), aid);
                                //OutputMessage(GetLogMode(), " Member:%d on logged out 4\n", cid);
                            }

                            DBHistory::dbcsmember::SetLogout(GetCommonDB(), aid, duration_sec);
                            //OutputMessage(GetLogMode(), " Member:%d logged out save successful\n", cid);
                            cs_db_function::member::dbcsmember::SetLogout(GetCommonDB(), aid);
                        }
                    }*/




                    //=============================================================================
                    //
                    //
                    //=============================================================================
                    //  bool UnitServer::WaitForSingleRequest(CsSessionDescriptor sdp, int block)
                    //  {
                    //      CsSSession* scn = v_coreserver_->GetCsSSession(s);
                    //      if(CsPSockConnobject!=NULL)
                    //       return CsPSockConnobject->WaitForSingleUsed(block);
                    //      return true;
                    //  }


                    //=============================================================================
                    //
                    //
                    //=============================================================================
    int UnitServer::GetSCSServerSize()
    {
        return v_coreserver_->GetSectionSize();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::SendCsMessage( CsSessionDescriptor sdp, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len )
    {
        if( sdp == 0 )
        {
            return -1;
        }

        if( ( bp == NULL ) || ( len == 0 ) )
        {
            return -1;
        }

        if( v_coreserver_->GetSessionIsMaster( sdp ) )
        {
            return v_coreserver_->SendPacket( sdp, uid, cid, bp, len );
            //return v_coreserver_->SendPacket(v_coreserver_->GetSocket(sdp), uid, cid, bp, len);
        }

        if( v_coreserver_->GetSessionIsClone( sdp ) )
        {
            if( v_coreserver_->is_base_sync_ )
            {
                // 同步接收模式 不處理
                return len;
            }
            else
            {
                // 非同步接收模式 轉送給 該連線 所在的機器
                // DESC: DSM send message
                _csbinbuf buff;
                CsSCSCommand::SCSSendRecvMessage::Client::SetRequest( buff, cyberspace::CoreServer::GetSessionSessionUid( sdp ), cid, bp, len );
                return v_coreserver_->SendScsMessage( sdp, uid, CsS_C_SCS_SEND_MESSAGE, _CsGetResBufPtr( buff ), _CsGetResBufLen( buff ) );
            }
        }

        return -1;
    }



    //=============================================================================
    //
    //
    //=============================================================================
    void UnitServer::BroadcastMessage( uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len )
    {
        if( ( bp == NULL ) || ( len == 0 ) )
        {
            return;
        }

        std::vector<CsSessionDescriptor> CsPSockConnlist;
        GetAllLocalSessionDescriptor( CsPSockConnlist );

        for( uint_t i = 0; i < CsPSockConnlist.size(); i++ )
        {
            SendCsMessage( CsPSockConnlist[ i ], uid, cid, bp, len );
        }

        return;
    }



    //=============================================================================
    //
    //
    //=============================================================================
    /*
    bool UnitServer::RegistercombinHandlingFunc(cs_section_uid_t sid,
                      combinHandlingFunc Authernticfunc,
             combinHandlingFunc Fictitiousfunc)
    {
        combinHandingMap::iterator cpmi;
        cpmi=m_CHFPort.find(sid);
        if(cpmi!=m_CHFPort.end())
        {
            return false;
        }

        chfs_t funcs;// = Authernticfunc;
        funcs.resize(2);
        funcs[0]=Authernticfunc;
        funcs[1]=Fictitiousfunc;

        m_CHFPort.insert(combinHandingMap::value_type(sid, funcs));
        return true;
    }
    */


    //=============================================================================
    //
    //
    //=============================================================================
    //int UnitServer::RegisterArbitData( const ubyte_t* bp, cs_size_t len )
    //{
    //    if ( ( bp == NULL ) || ( len == 0 ) )
    //    {
    //        return -1;
    //    }
    //
    //    return v_coreserver_->RegisterUserData( bp, len );
    //}


    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::DoDispatchMessage( const Unit::Evocator* evocator, channel_uid_t chid, const ubyte_t* combin, unsigned int comlen )
    {
        std::map<channel_uid_t, unit_member_func>::iterator it = v_coreserver_->channel_functions_.find( chid );
        if( it != v_coreserver_->channel_functions_.end() )
        {
            unit_member_func func = it->second;
            return ( this->*func )( evocator, combin, comlen );
        }
        return 0;

        //return v_coreserver_->m_UserDefMessageDispatcher.DispatchMessage( evocator, chid, combin, len );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::OnRecvDefineRequest( CsSessionDescriptor sdp, uniqueid_t who, channel_uid_t cid, const ubyte_t* combin, cs_size_t len )
    {
        if( sdp == 0 )
        {
            return -1;
        }

        if( ( combin == NULL ) || ( len == 0 ) )
        {
            return -1;
        }

        // 同步接收資料
        if( v_coreserver_->is_base_sync_ )
        {
            // 是不是別台同步給我的
            // 同步接收 所以 如果是我這台機器的連線 把它通知所有server
            if( GetSessionIsMaster( sdp ) )
            {
                // 同步到所有的server上
                _csbinbuf buff;
                CsSCSCommand::SCSSendRecvMessage::Client::SetRequest( buff, cyberspace::CoreServer::GetSessionSessionUid( sdp ), cid, combin, len );
                //v_coreserver_->SCSBroadcastMessageToOtherAndVisible(who, CsS_C_SCS_RECV_MESSAGE, _CsGetResBufPtr(buff), _CsGetResBufLen(buff)) ;
                v_coreserver_->BroadcastScsMessageToOtherWithObject( ObjectName(), who, CsS_C_SCS_RECV_MESSAGE, _CsGetResBufPtr( buff ), _CsGetResBufLen( buff ) );
            }
        }
        else
        {
            // 如果不同步接收 從別人機器上傳來的 同步資料 也不會 收到
            if( GetSessionIsClone( sdp ) )
            {
                return 0;
            }
        }

        Unit::ServerEvocator evocator;
        evocator.sessiondescriptor = sdp;
        evocator.uniqueid = who;
        evocator.pserver = this;
        return DoDispatchMessage( &evocator, cid, ( ubyte_t* )combin, len );
    }



    //=============================================================================
    //
    //
    //=============================================================================
    //bool UnitServer::SCSRegisterUserDefChannel( channel_uid_t session_uid, cyberspace::scs::ServerAbtpChannelFunction func )
    //{
    //    return v_coreserver_->RegisterScsUserDefChannel( session_uid, func );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //bool UnitServer::SCSUnRegisterUserDefChannel( channel_uid_t session_uid )
    //{
    //    return v_coreserver_->UnRegisterScsUserDefChannel( session_uid );
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::SCSSendMessage( cs_section_uid_t receiver, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len )
    {
        if( ( bp == NULL ) || ( len == 0 ) )
        {
            return -1;
        }

        return v_coreserver_->SendScsMessage( receiver, uid, cid, bp, len );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::SCSBroadcastMessage( uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len )
    {
        if( ( bp == NULL ) || ( len == 0 ) )
        {
            return -1;
        }

        return v_coreserver_->BroadcastScsMessage( uid, cid, bp, len );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int UnitServer::SCSBroadcastMessageToOther( uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len )
    {
        if( ( bp == NULL ) || ( len == 0 ) )
        {
            return -1;
        }

        return v_coreserver_->BroadcastScsMessageToOther( uid, cid, bp, len );
    }

    int UnitServer::SCSBroadcastMessageToOtherAndVisible( uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len )
    {
        if( ( bp == NULL ) || ( len == 0 ) )
        {
            return -1;
        }

        return v_coreserver_->BroadcastScsMessageToOtherAndVisible( uid, cid, bp, len );
    }

    int UnitServer::SCSBroadcastMessageToOtherWithObject( const tchar_t* objectname, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, cs_size_t len )
    {
        if( ( bp == NULL ) || ( len == 0 ) )
        {
            return -1;
        }

        return v_coreserver_->BroadcastScsMessageToOtherWithObject( objectname, uid, cid, bp, len );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    cs_section_uid_t UnitServer::SCSGetServerUid( const tchar_t* servername )
    {
        return v_coreserver_->GetSectionId( servername );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //void SectionEntityToServerInfo( _OUT cyberspace::ServerInfo& info, _IN const CsSectionEntity& symbol )
    //{
    //    info._sid = symbol.sectionid;
    //    strcpy( info._ip, net::Socket::IPN6ToA6(symbol.sym_addr[0]).c_str() );
    //    info._port = symbol.pub_port;
    //    strcpy( info._hostname, symbol.hostname );
    //    strcpy( info._aliasname, symbol.aliasname );
    //    //strcpy( sdata._functionname, hostnode->functionname );
    //    info._maxconnect = symbol.limitconnect;
    //    info._usedconnect = symbol.usedconnect;
    //}

    //=============================================================================
    //
    //
    //=============================================================================
    //void UnitServer::SCSGetAllServerInfo( std::vector<cyberspace::ServerInfo>& serverint_list )
    //{
    //    std::vector<CsSectionEntity> hlist;
    //    v_coreserver_->SCSGetAllSectionEntity( hlist );
    //    for ( int i = 0; i < hlist.size(); i++ )
    //    {
    //        ServerInfo sinfo;
    //        SectionEntityToServerInfo( sinfo , hlist[i] );
    //        serverint_list.push_back( sinfo );
    //    }
    //}

    /*
    //=============================================================================
    //
    //
    //=============================================================================
    cyberspace::scs::BufferDescriptor UnitServer::CreateShareBuffer(uniqueid_t uid, uint8_t gid, const ubyte_t* buff, cs_size_t len)
    {
       return v_coreserver_->SCSCreateBuffer(uid, gid, buff, len);
    }


    //=============================================================================
    //
    //
    //=============================================================================
    bool UnitServer::DeleteShareBuffer(const cyberspace::scs::BufferDescriptor* sbmm)
    {
       return v_coreserver_->SCSDeleteBuffer(sbmm);
    }
    */


    //=============================================================================
    //
    //
    //=============================================================================
    bool LoginIsIntranet( ipa_t ipa )
    {
        if( memcmp( ipa.c_str(), "192.168", _cssizeof( char ) * 7 ) == 0 )
        {
            return true;
        }

        if( strcmp( ipa.c_str(), "127.0.0.1" ) == 0 )
        {
            return true;
        }

        return false;
    }

    int g_Serverinfoflag = 0;

    void UnitServer::PublishServerList( CsSessionDescriptor sdp )
    {
        //CscombinPacket hSysMsg;
        //Arbitration::Client* aserver = ( Arbitration::Client* )v_coreserver_->v_aribt_agent_;
        ipn6_t ipn = GetSessionIpn6( sdp );
        ipa_t ipa = net::Socket::IPN6ToA4( ipn );
        // 連上的的CLIENT是不是內網
        //內網
        bool intragroup = LoginIsIntranet( ipa ); // 是內網
        std::vector<ServerInfo> hlist;
        v_coreserver_->GetAllSectionInfo( hlist );

        switch( g_Serverinfoflag )
        {
        case 0://client active
            {
                std::vector<ServerInfo> sendlist;
                for( uint_t i = 0; i < hlist.size(); i++ )
                {
                    ServerInfo* hostnode = &( hlist[ i ] );

                    if( ( ___IF_HOST_VISIBLE( hostnode->_state ) ) && ( hostnode->_limitconnect != 0 ) )
                    {
                        bool bsendgroup = false;
                        tchar_t ipname[ _MAX_ALIASNAME ];
                        {
                            // 我要找個一個最好的IP送下去啦
                            for( uint_t h = 0; h < hostnode->_hostlen; h++ )
                            {
                                printf( "List server information [%s] to client!!\n", hostnode->_hostname );
                                ipa_t ipah = net::Socket::IPA6ToA4( hostnode->GetIp( h ) );

                                // 這台SERVER有內網
                                // 內部IP
                                // 連上來的CLIENT是內網 而且 SERVER也有內網
                                if( LoginIsIntranet( ipah ) )
                                {
                                    // 連上來的CLIENT是內網 這個SERVERIP也室內網
                                    // 就直接送下去了
                                    if( intragroup )
                                    {
                                        strcpy( ipname, net::Socket::IPA6ToA4( hostnode->GetIp( h ) ).c_str() );
                                        bsendgroup = true;
                                        break;
                                    }
                                    else
                                    {
                                        bsendgroup = false;
                                    }
                                }
                                else
                                {
                                    // 記住外網的IP
                                    // 都找不到時 至少有外網可以送
                                    strcpy( ipname, net::Socket::IPA6ToA4( hostnode->_ip[ h ] ).c_str() );

                                    // 因為連上來的CLIENT不是內網 而且SERVER也不是內網
                                    // 所以必須送SERVER的外部IP下去
                                    // 查下一個SERVER的IP
                                    if( intragroup )
                                    {
                                        bsendgroup = false;
                                    }
                                    else
                                    {
                                        bsendgroup = true;
                                        break;
                                    }
                                }
                            }
                        }

                        if( bsendgroup )
                        {
                            //ServerInfo sinfo;
                            //SectionEntityToServerInfo( sinfo, *hostnode );
                            //sdata._sid = hostnode->sectionid;
                            memset( hostnode->_ip, sizeof( char ) * _MAX_IPNSIZE * _MAX_HOST_NAME, 0 );
                            strcpy( hostnode->GetIp( 0 ), ipname );
                            //sdata._port = hostnode->port;
                            //strcpy( sdata._hostname, hostnode->hostname );
                            //strcpy( sdata._objectname, hostnode->objectname );
                            //strcpy( sdata._functionname, hostnode->functionname );
                            //sdata._maxconnect = hostnode->limitconnect;
                            //sdata._usedconnect = hostnode->usedconnect;
                            sendlist.push_back( *hostnode );
                            OutputMessage( g_log_mode, "  find %s\n, list to connect", ipname );
                            //SEND_LOBBY_MESSAGE(sdp, &hSysMsg);
                        }
                        else
                        {
                            OutputMessage( g_log_mode, "  not find any server, stop send\n" );
                        }
                    }

                    //it++;
                }

                if( !sendlist.empty() )
                {
                    _csbinbuf buf;
                    CsBaseCommand::ServerInfo::Server::SetRequest( buf, &sendlist );
                    SendCsMessage( sdp, CsINVALID_ID, CsS_C_LISTSERVER_FEEDBACK, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                }
            }
            break;

        case 1://client passive
            {
                /*
                ServerInfo* mininode = NULL;

                // 找出目前連線人數最少的server
                for ( uint_t i = 0; i < hlist.size(); i++ )
                {
                    ServerInfo* hostnode = &( hlist[ i ] );

                    if ( mininode == NULL )
                    {
                        mininode = hostnode;
                        continue;
                    }

                    if ( ( mininode->_limitconnect / mininode->usedconnect ) > ( hostnode->limitconnect / hostnode->usedconnect ) )
                    {
                        mininode = hostnode;
                    }
                }

                if ( mininode )
                {
                    if ( ( ___IF_HOST_VISIBLE( mininode->_state ) ) && ( mininode->limitconnect != 0 ) )
                    {
                        bool bsendgroup = false;
                        // 我要找個一個最好的IP送去啦
                        tchar_t ipname[ _MAX_ALIASNAME ];

                        if ( strcmp( mininode->aliasname, "" ) != 0 )
                        {
                            printf( "List server information [%s] to client!!\n", mininode->hostname );
                            strcpy( ipname, mininode->aliasname );
                            bsendgroup = true;
                        }
                        else
                        {
                            for ( uint_t h = 0; h < mininode->sym_addrlen; h++ )
                            {
                                printf( "Assign server information [%s] to client!!\n", mininode->hostname );
                                ipa_t ipah = net::Socket::IPN6ToA4( mininode->sym_addr[ h ] );

                                // 這台SERVER有內網
                                // 內部IP
                                // 連上來的CLIENT是內網 而且 SERVER也有內網
                                if ( strstr( ipah.c_str(), "192.168" ) != NULL )
                                {
                                    // 連上來的CLIENT是內網 這個SERVERIP也室內網
                                    // 就直接送下去了
                                    if ( intragroup )
                                    {
                                        strcpy( ipname, net::Socket::IPN6ToA4(  mininode->sym_addr[ h ] ).c_str() );
                                        bsendgroup = true;
                                        break;
                                    }
                                    else
                                    {
                                        bsendgroup = false;
                                    }
                                }
                                else
                                {
                                    // 記住外網的IP
                                    // 都找不到時 至少有外網可以送
                                    strcpy( ipname, net::Socket::IPN6ToA4( mininode->sym_addr[ h ] ).c_str() );

                                    // 因為連上來的CLIENT不是內網 而且SERVER也不是內網
                                    // 所以必須送SERVER的外部IP下去
                                    // 查下一個SERVER的IP
                                    if ( intragroup )
                                    {
                                        bsendgroup = false;
                                    }
                                    else
                                    {
                                        bsendgroup = true;
                                        break;
                                    }
                                }
                            }
                        }

                        if ( bsendgroup )
                        {
                            ServerInfo sinfo;
                            SectionEntityToServerInfo( sinfo, *mininode );
                            //sdata._sid = mininode->sectionid;
                            strcpy( sinfo._ip, ipname );
                            //sdata._port = mininode->port;
                            //strcpy( sdata._hostname, mininode->hostname );
                            //strcpy( sdata._objectname, mininode->objectname );
                            //strcpy( sdata._functionname, mininode->functionname );
                            //sdata._maxconnect = mininode->limitconnect;
                            //sdata._usedconnect = mininode->usedconnect;
                            std::vector<ServerInfo>  sendlist;
                            sendlist.push_back( sinfo );
                            _csbinbuf buf;
                            CsBaseCommand::ServerInfo::Server::SetRequest( buf, &sendlist );
                            SendCsMessage( sdp, CsINVALID_ID, CsS_C_ASSIGNSERVER_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                            OutputMessage( g_log_mode, "  find %s, assign to connect\n", ipname );
                            //SEND_LOBBY_MESSAGE(sdp, &hSysMsg);
                        }
                        else
                        {
                            OutputMessage( g_log_mode, " not find any server, stop send\n" );
                        }
                    }
                }
                */
            }
            break;
        }
        OutputMessage( g_log_mode, "List server successful\n" );
        return;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    //int UnitServer::DoPing( const Unit::ServerEvocator* evocator, const void* combin, unsigned int len )
    int UnitServer::DoPing( const Unit::Evocator* evocator, const ubyte_t* combin, unsigned int len )
    {
        CsAbtpCnSocket* scn = CdToSc( evocator->sessiondescriptor );
        mstimen_t mst = 0;
        CsPingMessage::Server::GetRequest( ( const ubyte_t* )combin, mst );
        scn->SendPacket( CsINVALID_ID, CsS_C_PING_FEEDBACK, ( ubyte_t* )&mst, _cssizeof( mstimen_t ) );
        return 1;
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    int UnitServer::DoListServer( const Unit::Evocator* evocator, const ubyte_t* combin, unsigned int len )
    {
        //LoginServer* pServer = ( LoginServer* ) evocator->pserver;
        PublishServerList( evocator->sessiondescriptor );
        return 1;
    }
};
