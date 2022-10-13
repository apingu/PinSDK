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
#if  defined(_WINDOWS)
#include <direct.h>
#endif
#if  defined(_LINUX)
#include <stdarg.h>
#endif

#include <set>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <pf-thread.h>
#include <pf-system.h>
#include <net/net-server.h>
#include <gadget/recdict.h>
#include <parser/xmlnode.h>
#include <template/vmapl.inl>
#include <algorithmic/crc64.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-app_server.h"
#include "cs-log_file_service.h"
#include "cs-http_server.h"
#include "cs-utility.h"
#include "cs-if_session.h"
#include "cs-abtp_cn_socket.h"
#include "cs-http_cn_socket.h"


namespace cyberspace
{
    //////////////////////////////////////////////////////////////////////////
    //

typedef std::map<uniqueid_t, member_func> function_channel_t;

#define _CsAPP_RECDICTTBL( s )    ( ( Recdict* )s->v_object_[ 0 ][ 0 ] )
#define _CsAPP_FUNC_CHANNEL( s )  ( ( function_channel_t* )s->v_object_[ 0 ][ 1 ] )

#define _CsAPP_ABTPSERVER( s )    ( ( AppApBtJunction* )s->v_object_[ 1 ][ 0 ] )
#define _CsAPP_HTTPSERVER( s )    ( ( AppHttpJunction* )s->v_object_[ 1 ][ 1 ] )

    namespace
    {
        CsLogMode                                  g_log_mode;
        int                                        g_log_index;
        bool                                       g_is_init = false;
        pfstl::vmapl<std::map<_tstring, _tstring>> g_session_table;
        const _tstring SessionName = _T( "SessionUId" );
    }

#define _MAX_SESSION_TABLE 100

    typedef void      ( AppServer::*func_accept )( _IN AppSocket* socket );
    typedef void      ( AppServer::*func_kickout )( _IN AppSocket* socket );
    typedef _csbinbuf ( AppServer::*func_recvrequest )( _IN AppSocket* s, _IN CsAbtpEncType enctype, _IN const char_t* actstr, _IN const ubyte_t* combuf, _IN uint_t comlen );

    typedef int       ( AppServer::*func_recvabtprequest )( _IN AppSocket* s, _IN CsAbtpEncType enctype, _IN channel_uid_t channelid, _IN const ubyte_t* combuf, _IN uint_t comlen );
    typedef _csbinbuf ( AppServer::*func_recvhttprequest )( _IN AppSocket* s, _IN CsHttpEncType enctype, _IN const char_t* actstr, _IN const ubyte_t* combuf, _IN uint_t comlen );

    //////////////////////////////////////////////////////////////////////////
    // binary 接口
    // 
    class AppApBtJunction : public net::TCPServer
    {
        cyberspace::AppServer*                    p_server_;
        cyberspace::SocketSupervise               socket_supervis_;
        //std::vector<pfkrl::Thread::Handle>        vec_thread_handle_;

    public:

        SafeServerState                           state_;
        bool                                      is_basesync_;
        std::map<channel_uid_t, cptp_member_func> channel_functions_;
        func_accept                               p_function_accept_;
        func_kickout                              p_function_kickout_;
        func_recvabtprequest                      p_function_fequest_;

        //void        ( AppServer::*func_accept )( _IN AppSocket* socket ) = &AppServer::OnAccept;

        AppApBtJunction( AppServer* pServer )
        {
            p_server_ = pServer;
            is_basesync_ = false;
            p_function_accept_ = NULL;
            p_function_kickout_ = NULL;
            p_function_fequest_ = NULL;
        }
        ~AppApBtJunction() {};

        const tchar_t* ObjectName() const { return _T( "AppApBtJunction" ); }

        int StartUp( unsigned int allowplayers, ushort_t port )
        {
            socket_supervis_.DoStartUp( p_server_->ObjectName(), state_.Get_Startup_Stime() );
            return net::TCPServer::StartUp( allowplayers, port );
        }

        int Shutdown( void )
        {
            socket_supervis_.DoClose();
            return net::TCPServer::Shutdown();
        }

        void SetKeepAliveMstime( mstimen_t mst )
        {
            socket_supervis_.SetKeepAliveMstime( mst );
        }

        int  GetInspectedSize()
        {
            return socket_supervis_.GeInsecureSize();
        }

        void Update()
        {
            socket_supervis_.Update( this );
        }

    private:

        void OnAccept( net::TCPSocket* skt )
        {

            // it allow connect until server initial finish
            //tchar_t host[ 255 ];
            time_t timep;
            time( &timep );
            sockaddr_storage addr_storage;
            sockaddr* addr = skt->GetPeerAddr( addr_storage );
            ipa_t     ipa = net::Socket::GetSockaddrIPA( addr );
            char hostname[ NI_MAXHOST ];
            char servinfo[ NI_MAXSERV ];
            net::Socket::GetAddrNameinfo( ( struct sockaddr* )addr, hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
            tstring log_msg;
            log_msg = _T( " \n ||================================================\n" );
            log_msg += _T( " ||  Join Client\n" );
            log_msg += _T( " ||------------------------------------------------\n" );
            log_msg += _T( " ||  HostName : %s\n" );
            log_msg += _T( " ||  Service  : %s\n" );
            log_msg += _T( " ||  Time     : %s" );
            log_msg += _T( " ||  Ip       : %s\n" );
            log_msg += _T( " ||================================================\n" );
            p_server_->OutputMessage( g_log_mode, log_msg.c_str(), hostname, servinfo, asctime( localtime( &timep ) ), ipa.c_str() );
            state_.Set_Max_Connected_Client( this );
            if( !socket_supervis_.DoAccept( skt ) )
            {
                net::TCPServer::Kickout( skt );
                return;
            }

            if( p_function_accept_ )
                ( p_server_->*p_function_accept_ )( ( CsAbtpCnSocket* )skt );
        }

        void OnKickout( net::TCPSocket* skt )
        {
            //if ( skt->GetFD() == 0 )
            //{
            //	return 0;
            //}
            //tchar_t host[ 255 ];
            socket_supervis_.DoKickout( skt );
            time_t timep = pftml::Time::GetSTime();
            //time( &timep );
            sockaddr_storage addr_storage;
            sockaddr* addr = skt->GetPeerAddr( addr_storage );
            ipa_t     ipa = net::Socket::GetSockaddrIPA( addr );
            char hostname[ NI_MAXHOST ];
            char servinfo[ NI_MAXSERV ];
            net::Socket::GetAddrNameinfo( addr, hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
            tstring log_msg;
            log_msg = _T( " \n ||================================================\n" );
            log_msg += _T( " ||  Exit Client\n" );
            log_msg += _T( " ||------------------------------------------------\n" );
            log_msg += _T( " ||  HostName : %s\n" );
            log_msg += _T( " ||  Service  : %s\n" );
            log_msg += _T( " ||  Time     : %s" );
            log_msg += _T( " ||  Ip       : %s\n" );
            log_msg += _T( " ||================================================\n" );
            p_server_->OutputMessage( g_log_mode, log_msg.c_str(), hostname, servinfo, asctime( localtime( &timep ) ), ipa.c_str() );

            if( p_function_kickout_ )
                ( p_server_->*p_function_kickout_ )( ( CsAbtpCnSocket* )skt );
            //p_server_->OnKnockOut( ( AppSocket* )skt );
        }

        void OnReceive( net::TCPSocket* skt )
        {
            channel_uid_t  commandidx = 0;
            unsigned int recvlne = 0;
            ubyte_t*     recvbuf = NULL;
            uniqueid_t   eid = CsINVALID_ID;
            ( ( AbtpCnSocket* )skt )->RecvPacket( eid, commandidx, &recvbuf, recvlne );

            if( p_function_fequest_ )
                ( p_server_->*p_function_fequest_ )( ( CsAbtpCnSocket* )skt, CsAbtpEncType_Binary, commandidx, recvbuf, recvlne );
            //p_server_->OnRecvAbtpRequest( ( AppSocket* )skt, commandidx, recvbuf, recvlne );
            return;
        }

        net::TCPSocket* OnNewSocket( void )
        {
            CsAbtpCnSocket* psockconn = new CsAbtpCnSocket;
            return psockconn;
        }

        void OnDelSocket( net::TCPSocket* skt )
        {
            CsAbtpCnSocket* scn = SkToSc( skt );
            SAFE_DELETE( scn );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    //
    class AppHttpJunction : public cyberspace::HttpServer
    {
        AppServer*                               p_server_;

    public:

        SafeServerState                          state_;
        bool                                     is_basesync_;
        //std::map<_tstring, AppServer::member_func>  channel_functions_;
        std::map<_tstring, http_member_func>     channel_functions_;
        func_accept                              p_function_accept_;
        func_kickout                             p_function_kickout_;
        func_recvrequest                         p_function_fequest_;

        AppHttpJunction( AppServer* pServer )
        {
            p_server_ = pServer;
            is_basesync_ = false;
            p_function_accept_ = NULL;
            p_function_kickout_ = NULL;
            p_function_fequest_ = NULL;
        }

        ~AppHttpJunction() {}

        const tchar_t* ObjectName() const { return _T( "AppHttpJunction" ); }

        // 請掛載
        void           Update() { cyberspace::HttpServer::Update(); }

    private:

        void OnAcceptSocket( net::TCPSocket* skt )
        {
            state_.Set_Max_Connected_Client( this );
            //
            if( p_function_accept_ )
                ( p_server_->*p_function_accept_ )( ( CsHttpCnSocket* )skt );
        }

        void OnKickoutSocket( net::TCPSocket* skt )
        {
            if( p_function_kickout_ )
                ( p_server_->*p_function_kickout_ )( ( CsHttpCnSocket* )skt );
        }

        _csbinbuf OnReceiveSocket( net::TCPSocket* skt, CsHttpEncType enctype, const char_t* actstr, const ubyte_t* comstr, uint_t comlen )
        {
            if( p_function_fequest_ )
                return ( p_server_->*p_function_fequest_ )( ( CsHttpCnSocket* )skt, ( CsHttpEncType )enctype, actstr, comstr, comlen );
            return _csbinbuf();
        }
    };


    //////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    //
    //
    //=============================================================================
    AppServer::AppServer( const tchar_t* name )
    {
        SET_BIT_FLAG( g_log_mode, LOGMODE_PRINTOUT );


        {
            Recdict* pmoudle = new Recdict();
            pmoudle->Init( _MAX_SESSION_TABLE );
            v_object_[ 0 ][ 0 ] = pmoudle;
        }

        {
            function_channel_t* pmoudle = new function_channel_t;
            v_object_[ 0 ][ 1 ] = pmoudle;
        }

        {
            AppApBtJunction* pmoudle = new AppApBtJunction( this );
            pmoudle->p_function_accept_ = &AppServer::OnAccept;
            pmoudle->p_function_kickout_ = &AppServer::OnKickout;
            pmoudle->p_function_fequest_ = &AppServer::OnRecvAbtpRequest;
            v_object_[ 1 ][ 0 ] = pmoudle;
        }

        {
            AppHttpJunction* pmoudle = new AppHttpJunction( this );
            pmoudle->p_function_accept_ = &AppServer::OnAccept;
            pmoudle->p_function_kickout_ = &AppServer::OnKickout;
            pmoudle->p_function_fequest_ = &AppServer::OnRecvHttpRequest;
            v_object_[ 1 ][ 1 ] = pmoudle;
        }

        stime_startup_ = 0;
        //sec_session_timeout_ = 900;
        //m_ClearTempSec = 43200; // 預設半天清一次
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
    AppServer::~AppServer()
    {
        Shutdown();

        Recdict*              pmoudle_00_00 = _CsAPP_RECDICTTBL( this );
        function_channel_t*   pmoudle_00_01 = _CsAPP_FUNC_CHANNEL( this );
        AppApBtJunction*      pmoudle_01_00 = _CsAPP_ABTPSERVER( this );
        AppHttpJunction*      pmoudle_01_01 = _CsAPP_HTTPSERVER( this );
 
        SAFE_DELETE( pmoudle_00_00 );
        SAFE_DELETE( pmoudle_00_01 );
        SAFE_DELETE( pmoudle_01_00 );
        SAFE_DELETE( pmoudle_01_01 );
    }


    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void AppServer:: RegisterFunction( _tstring actstr, member_func func )
    {
        //_CsAPP_HTTPSERVER->channel_functions_.insert( std::map<_tstring, member_func>::value_type( actstr, func ) );
        _CsAPP_FUNC_CHANNEL( this )->insert( function_channel_t::value_type( crc64::Encode( actstr.c_str(), actstr.size() ), func ) );
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void AppServer::UnRegisterFunction( _tstring actstr )
    {
        _CsAPP_FUNC_CHANNEL( this )->erase( crc64::Encode( actstr.c_str(), actstr.size() ) );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::RegisterAbtpFunction( channel_uid_t id, cptp_member_func func )
    {
        _CsAPP_ABTPSERVER( this )->channel_functions_.insert( std::map<channel_uid_t, cptp_member_func>::value_type( id, func ) );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::UnRegisterAbtpFunction( channel_uid_t id )
    {
        _CsAPP_ABTPSERVER( this )->channel_functions_.erase( id );
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void AppServer::RegisterHttpFunction( _tstring actstr, http_member_func func )
    {
        //_CsAPP_HTTPSERVER->channel_functions_.insert( std::map<_tstring, member_func>::value_type( actstr, func ) );
        _CsAPP_HTTPSERVER( this )->channel_functions_.insert( std::map<_tstring, http_member_func>::value_type( actstr, func ) );
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void AppServer::UnRegisterHttpFunction( _tstring actstr )
    {
        _CsAPP_HTTPSERVER( this )->channel_functions_.erase( actstr );
    }

    ////===============================================================================//
    //// NAME :
    //// DESC :
    ////===============================================================================//
    //void AppServer::RegisterFunction( _tstring actstr, member_func func )
    //{
    //    _CsAPP_HTTPSERVER->channel_functions_.insert( std::map<_tstring, member_func>::value_type( actstr, func ) );
    //}
    //
    ////===============================================================================//
    //// NAME :
    //// DESC :
    ////===============================================================================//
    //void AppServer::UnRegisterFunction( _tstring actstr )
    //{
    //    _CsAPP_HTTPSERVER->channel_functions_.erase( actstr );
    //}

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    //_tstring AppServer::ExeFunction( AppSocket* s, CsHttpEncType enctype, const char* actstr, const char* comstr, int comlen )
    //{
    //    _tstring result_string;
    //    std::map<_tstring, member_func>::iterator it = _CsAPP_HTTPSERVER->channel_functions_.find( actstr );
    //    if ( it != _CsAPP_HTTPSERVER->channel_functions_.end() )
    //    {
    //        member_func func = it->second;
    //        result_string = ( this->*func )( s, enctype, actstr, comstr, comlen );
    //    }
    //    return result_string;
    //}

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    int AppServer::StartUp( unsigned int allowplayers, ushort_t ctptport, ushort_t httpport )
    {
        time_t startt = pftml::Time::GetSTime();
        stime_startup_ = startt;
        srand( startt );
        //time( &startt );
        // open a log history file
        mkdir( "log" );
        struct tm* p = localtime( &startt );
        tchar_t filename[ _MAX_PATH ];
        _stprintf( filename, "log/%s_history.log", ObjectName() );

        cyberspace::g_log_index = cyberspace::LogFileSerivce::Open( filename );

        _CsAPP_ABTPSERVER( this )->Create();
        _CsAPP_ABTPSERVER( this )->StartUp( allowplayers, ctptport );
        _CsAPP_ABTPSERVER( this )->state_.Set_Startup_Stime( startt );
        _CsAPP_HTTPSERVER( this )->Create();
        _CsAPP_HTTPSERVER( this )->StartUp( allowplayers, httpport );
        _CsAPP_HTTPSERVER( this )->state_.Set_Startup_Stime( startt );

        srand( ( int )pftml::Time::GetSTime() );

        if( g_is_init == false )
        {
            g_session_table.init( _MAX_SESSION_TABLE );
            pfkrl::Thread::BeginProcess( Execute, this, _T( "AppServer::Execute" ) );
            g_is_init = true;
        }
        return 1;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int AppServer::Shutdown( void )
    {
        _CsAPP_ABTPSERVER( this )->Shutdown();
        _CsAPP_HTTPSERVER( this )->Shutdown();
        stime_startup_ = 0;
        cyberspace::LogFileSerivce::Close( cyberspace::g_log_mode );
        return 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool AppServer::IsAlive()
    {
        if( _CsAPP_ABTPSERVER( this )->GetFD() == 0 )
            return false;
        if( _CsAPP_HTTPSERVER( this )->GetFD() == 0 )
            return false;
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::SetKeepAliveMstime( stimen_t t )      // 設定最短連線安全檢查時間
    {
        _CsAPP_ABTPSERVER( this )->SetKeepAliveMstime( t );
        _CsAPP_HTTPSERVER( this )->SetKeepAliveMstime( t );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    stimen_t AppServer::GetStartSec( void )               // 取得啟動的時間
    {
        return stime_startup_;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    const tchar_t* AppServer::GetLocalHostname( _IN tchar_t* buf, _IN cs_size_t len )
    {
        return _CsAPP_ABTPSERVER( this )->GetLocalHostname( buf, len );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    ushort_t AppServer::GetLocalServicePort( _IN int m )  // 開啟接收的Port編號 0 cptp 1 http
    {
        switch( m )
        {
        case 0:
        {
            return _CsAPP_ABTPSERVER( this )->GetServicePort();
        }
        break;
        case 1:
        {
            return _CsAPP_HTTPSERVER( this )->GetServicePort();
        }
        break;
        }
        return 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    const tchar_t* AppServer::GetLocalAliasName( void )         // 取得本機的別名
    {
        return name_alias_.c_str();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    const tchar_t* AppServer::GetLocalServiceName( void )       //  設定檔中設定的Function名稱
    {
        return name_service_.c_str();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    uint_t AppServer::GetLocalPollingCount( void )
    {
        return _CsAPP_ABTPSERVER( this )->GetPollingCount();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    uint_t AppServer::GetLocalLimitConnectedSize( void )  // maximum for allow connect client size
    {
        return _CsAPP_ABTPSERVER( this )->GetLimitConnectedSize();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    uint_t AppServer::GetLocalConnectedSize( int m )     // size of current connected client
    {
        switch( m )
        {
        case 0:
        {
            return _CsAPP_ABTPSERVER( this )->GetConnectedSize();
        }
        break;
        case 1:
        {
            return _CsAPP_HTTPSERVER( this )->GetConnectedSize();
        }
        break;
        }
        return 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    uint_t AppServer::GetLocalMaxConnectedSize( int m )     // size of current connected client
    {
        switch( m )
        {
        case 0:
        {
            return _CsAPP_ABTPSERVER( this )->state_.Get_Max_Connected_Client();
        }
        break;
        case 1:
        {
            return _CsAPP_HTTPSERVER( this )->state_.Get_Max_Connected_Client();
        }
        break;
        }
        return 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    long32_t AppServer::GetLivingThreadSize( void )
    {
        return _CsAPP_ABTPSERVER( this )->GetLivingThreadSize();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool AppServer::LoadConfigure( _IN const tchar_t* filename )
    {
        XMLNode xml;

        if( xml.LoadFromFile( filename ) )
        {
            XMLNode* GroupCfg = xml.FindFirstNode( GroupName() );
            if( GroupCfg != NULL )
            {
                unsigned int AllowedConnection = 0;
                tchar_t host[ 255 ];
                unsigned short cptp_port = 0;
                unsigned short http_port = 0;
                cs_string      aliasname;
                cs_string      servicename;
                bool           baasesync = false;
                mstimen_t      keepalivemstime = 3000;
                std::map<cs_string, cs_string> setting_node;
                //////////////////////////////////////////////////////////////////////////
                // Read configure file
                {
                    XMLNode* ServerCfg = GroupCfg->FindFirstNode( _T( ObjectName() ) );

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
                                if( SettingNode->name_ == _T( "MaxNodes" ) )
                                {
                                    SettingNode->text_ >> AllowedConnection;
                                }
                                else if( SettingNode->name_ == _T( "AbtpPort" ) )
                                {
                                    SettingNode->text_ >> cptp_port;
                                }
                                else if( SettingNode->name_ == _T( "HttpPort" ) )
                                {
                                    SettingNode->text_ >> http_port;
                                }
                                else if( SettingNode->name_ == _T( "Alias" ) )
                                {
                                    aliasname = SettingNode->text_;
                                }
                                else if( SettingNode->name_ == _T( "Service" ) )
                                {
                                    servicename = SettingNode->text_;
                                }
                                else if( SettingNode->name_ == _T( "SyncMsg" ) )
                                {
                                    XMLNode* SyncMsgCfg = SettingNode;
                                    baasesync = ( SyncMsgCfg->text_ == _T( "1" ) ) ? true : false;
                                }
                                else if( SettingNode->name_ == _T( "RiskMSTime" ) )
                                {
                                    SettingNode->text_ >> keepalivemstime;
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
                        OutputMessage( g_log_mode, ">> Can't find server configure node!!!\n" );
                        goto _LOADCONFIGURE_EXIT;
                    }
                }
                cs_string log_msg;
                log_msg = _T( "||===========================================================================\n" );
                log_msg += _T( "||  cyberspace server [ %s ] startup........\n" );
                log_msg += _T( "||      build:  %s %s\n||\n" );

                if( StartUp( AllowedConnection, cptp_port, http_port ) != 1 )
                {
                    log_msg += _T( "||>>  app server startup failure !\n" );
                    log_msg += _T( "||===========================================================================\n\n" );
                    OutputMessage( g_log_mode, log_msg.c_str(), ObjectName(), __DATE__, __TIME__ );
                    goto _LOADCONFIGURE_EXIT;
                }

                time_t starttime = 0;
                log_msg += _T( "||  Start on %s" );
                log_msg += _T( "||---------------------------------------------------------------------------\n" );
                OutputMessage( g_log_mode, log_msg.c_str(), ObjectName(), __DATE__, __TIME__, ctime( &starttime ) );
                //uint_t  ipcount = 0;
                ipn4_t   ipn;
                memset( &ipn, 0, _cssizeof( ipn4_t ) );

                //OutputMessage( g_log_mode, _T( "||  server_uid : %d\n" ), GetLocalServerUid() );
                OutputMessage( g_log_mode, _T( "||  HostName : %s\n" ), net::Socket::GetLocalHostname( host, 255 ) );
                // 自己乖乖防錯
                name_service_ = servicename;
                OutputMessage( g_log_mode, _T( "||  Service  : %s\n" ), name_service_.c_str() );
                //
                name_alias_ = aliasname;
                OutputMessage( g_log_mode, _T( "||  Alias    : %s\n" ), name_alias_.c_str() );
                //std::vector<ipa_t> ip4list;
                //std::vector<ipa_t> ip6list;
                //GetLocalIPAddress( ip4list, ip6list );
                std::vector<EthIf> ethif_list;
                net::Socket::GetEthIfinfo( ethif_list );

                if( ethif_list.empty() )
                {
                    log_msg = _T( "||>>  Can't find any ip!!...\n" );
                    log_msg += _T( "||===========================================================================\n\n" );
                    OutputMessage( g_log_mode, log_msg.c_str() );
                    return false;
                }

                OutputMessage( g_log_mode, _T( "||  Enternet interface....\n" ) );

                for( int ifi = 0; ifi < ethif_list.size(); ifi++ )
                {
                    if( ethif_list[ ifi ].iftype == _IF_TYPE_ETHERNET )
                    {
                        if( !ethif_list[ ifi ].unicast_addrs.empty() )
                        {
                            OutputMessage( g_log_mode, _T( "||  %-10s : " ), ethif_list[ ifi ].name.c_str() );

                            for( int ip4i = 0; ip4i < ethif_list[ ifi ].unicast_addrs.size(); ip4i++ )
                            {
                                sockaddr* skaddr = ( sockaddr* )&ethif_list[ ifi ].unicast_addrs[ ip4i ];

                                if( skaddr->sa_family == AF_INET )
                                {
                                    ipn4_t ipn4 = net::Socket::GetSockaddrIPN4( skaddr );
                                    OutputMessage( g_log_mode, _T( "IPv4 = %-15s " ), net::Socket::IPN4ToA4( ipn4 ).c_str() );
                                    //ifidex++;
                                }

                                if( skaddr->sa_family == AF_INET6 )
                                {
                                    ipn6_t ipn6 = net::Socket::GetSockaddrIPN6( skaddr );
                                    OutputMessage( g_log_mode, _T( "IPv6 = %s " ), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                                    //OutputMessage( g_log_mode, _T( "||  Ip %-12s     : %-15s = %s\n" ), ethif_list[ ifi ].name.c_str(), net::Socket::IPN4ToA4( ipn4 ).c_str(), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                                    //ifidex++;
                                }
                            }

                            OutputMessage( g_log_mode, _T( "\n" ) );
                        }
                    }
                }

                log_msg = _T( "||  AbtpPort : %d   HttpPort : %d\n" );
                log_msg += _T( "||  Max connect = %d\n" );
                _CsAPP_ABTPSERVER( this )->is_basesync_ = baasesync;
                _CsAPP_HTTPSERVER( this )->is_basesync_ = baasesync;
                log_msg += _T( "||  Synchronize message : %s\n" );
                SetKeepAliveMstime( keepalivemstime );
                log_msg += _T( "||  Keep-Alive mstime : %d\n" );
                OutputMessage( g_log_mode, log_msg.c_str(), cptp_port, http_port, AllowedConnection, ( baasesync == true ) ? "Y" : "N", keepalivemstime );

                //////////////////////////////////////////////////////////////////////////
                // load extern config setting
                if( !OnLoadConfigure( setting_node ) )
                {
                    OutputMessage( g_log_mode, _T( "||      ....failure\n" ) );
                    //loadret = false;
                    goto _LOADCONFIGURE_EXIT;
                }

                //
            }
        }
        else
        {
            OutputMessage( g_log_mode, _T( "||>>  Can't load configure file!!\n" ) );
            goto _LOADCONFIGURE_EXIT;
        }

        if( OnServerInitialing() != 1 )
        {
            OutputMessage( g_log_mode, _T( "||    ....failure\n" ) );
            //loadret = false;
            goto _LOADCONFIGURE_EXIT;
        }

        OutputMessage( g_log_mode, _T( "||  cyberspace [ %s ] server start successful.\n||===========================================================================\n" ), ObjectName() );
        return true;

    _LOADCONFIGURE_EXIT:
        OutputMessage( g_log_mode, _T( "||  cyberspace [ %s ] server start falure.\n||===========================================================================\n" ), ObjectName() );
        Shutdown();
        return false;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int AppServer::SendMessage( AppSocket* skt, uniqueid_t eid, channel_uid_t cid, const ubyte_t* bp, uint_t len )
    {
        return ( ( CsAbtpCnSocket* )skt )->SendPacket( eid, cid, bp, len );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::OutputMessage( CsLogMode m, const tchar_t* format, ... )
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
        cyberspace::LogFileSerivce::OutputMessage( 0, m, format, arglist );
        va_end( arglist );

        //cyberspace::LogSerivce::OutputMessage( cyberspace::Arbitration::g_LogModeUnit, m, format, arglist );
        return;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::BindSessionUid( AppSocket* skt, cs_session_uid_t sid )
    {
        //CsIfSession* s = const_cast< CsIfSession* >( skt );
        //CsIfSession* s = ( ( CsIfSession* )skt );
        skt->SetSessionUid( sid );
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::FreeSessionUid( AppSocket* skt )
    {
        skt->SetSessionUid( CsINVALID_ID );
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    cs_session_uid_t AppServer::GetSessionUid( AppSocket* skt )
    {
        CsHttpCnSocket* s = ( ( CsHttpCnSocket* )skt );
        return s->GetSessionUid();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //AppSocket* AppServer::GetSessionSocket( cs_session_uid_t sid )
    //{
    //
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //void AppServer::SetSessionTimoutSec( cs_session_uid_t sessionid, stimen_t sec )
    //{
    //    sec_session_timeout_ = sec;
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool AppServer::CreateSession( cs_session_uid_t sessionid, stimen_t timeoutsec )
    {
        _tstring tseesionuid = _pf_ulltot( sessionid );
        // 直接加入
        _CsAPP_RECDICTTBL( this )->Set( tseesionuid.c_str(), tseesionuid.c_str() );
        _CsAPP_RECDICTTBL( this )->Setlt( tseesionuid.c_str(), timeoutsec );
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::RemoveSession( cs_session_uid_t sessionid )
    {
        _tstring tseesionuid = _pf_ulltot( sessionid );
        _CsAPP_RECDICTTBL( this )->Del( tseesionuid.c_str() );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool AppServer::SetSessionKey( cs_session_uid_t sessionid, _tstring key, _tstring val )
    {
        _tstring tseesionuid = _pf_ulltot( sessionid );
        // 這個seesion 必須先存在
        return _CsAPP_RECDICTTBL( this )->HSetex( tseesionuid.c_str(), key.c_str(), val.c_str() );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring AppServer::GetSessionKey( cs_session_uid_t sessionid, _tstring key )
    {
        _tstring tseesionuid = _pf_ulltot( sessionid );
        return _CsAPP_RECDICTTBL( this )->HGet( tseesionuid.c_str(), key.c_str() ).c_str();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::DelSessionKey( cs_session_uid_t sessionid, _tstring key )
    {
        _tstring tseesionuid = _pf_ulltot( sessionid );
        _CsAPP_RECDICTTBL( this )->HDel( tseesionuid.c_str(), key.c_str() );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::SetHttpContentType( const char* buf )
    {
        _CsAPP_HTTPSERVER( this )->SetContentType( buf );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    channel_uid_t AppServer::OnCallAbtpRequest( AppSocket* s, channel_uid_t channelid )
    {
        return channelid;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    const char_t* AppServer::OnCallHttpRequest( AppSocket* s, const char_t* actstr )
    {
        return actstr;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int AppServer::OnRecvAbtpRequest( AppSocket* s, _IN CsAbtpEncType enctype, channel_uid_t channelid, const ubyte_t* combin, uint_t comlen )
    {
        std::map<channel_uid_t, cptp_member_func>::iterator it = _CsAPP_ABTPSERVER( this )->channel_functions_.find( OnCallAbtpRequest( s, channelid ) );
        if( it != _CsAPP_ABTPSERVER( this )->channel_functions_.end() )
        {
            //if( OnAuthenication( TT_ABTP, ( const ubyte_t* )combin, comlen ) != CsINVALID_ID )
            {
                cptp_member_func func = it->second;
                return ( this->*func )( s, enctype, combin, comlen );
            }
            //else
            {
                //    _CsAPP_ABTPSERVER(this)-> Kickout( ( net::TCPSocket*)s );
            }
        }
        return 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _csbinbuf AppServer::OnRecvHttpRequest( AppSocket* s, CsHttpEncType enctype, const char* actstr, const ubyte_t* combuf, uint_t comlen )
    {
        _csbinbuf result_string;
        std::map<_tstring, http_member_func>::iterator it = _CsAPP_HTTPSERVER( this )->channel_functions_.find( OnCallHttpRequest( s, actstr ) );
        if( it != _CsAPP_HTTPSERVER( this )->channel_functions_.end() )
        {
            //if( OnAuthenication( TT_HTTP, ( const ubyte_t* )comstr, comlen ) != CsINVALID_ID )
            {
                http_member_func func = it->second;
                result_string = ( this->*func )( s, enctype, combuf, comlen );
            }
            //else
            //{
                //    _CsAPP_HTTPSERVER( this )->Kickout( ( net::TCPSocket* )s );
            //}
        }
        return result_string;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void AppServer::Execute( void* data )
    {
        AppServer* pServer = ( AppServer* )data;
        stimen_t starttime = pftml::Time::GetSTime();
        while( pServer->IsAlive() )
        {
            _CsAPP_RECDICTTBL( pServer )->Update();
            _CsAPP_ABTPSERVER( pServer )->Update();
            _CsAPP_HTTPSERVER( pServer )->Update();
            pfstd::MsSleep( 100 );
        }
    }
}

