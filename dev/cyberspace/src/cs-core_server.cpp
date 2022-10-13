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
#include <algorithmic/crypt-crc64.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-core_server.h"
#include "cs-log_file_service.h"
#include "cs-http_server.h"
#include "cs-utility.h"
#include "cs-if_session.h"
//#include "cs-abtp_cn_socket.h"
//#include "cs-http_cn_socket.h"


namespace cyberspace
{

#define _CsCORE_JUNCTION( s )     ( ( CoreJunction* )s->v_object_[ 0 ] )
#define _MAX_CHANNEL_POOL  100
#define _MAX_SESSION_TABLE 100

    //////////////////////////////////////////////////////////////////////////
    //
    //#define _CsAPP_RECDICTTBL( s )    ( ( Recdict* )s->v_object_[ 0 ][ 0 ] )
    //#define _CsAPP_FUNC_CHANNEL( s )  ( ( function_channel_t* )s->v_object_[ 0 ][ 1 ] )

//#define _CsAPP_HTTPSERVER( s )    ( ( AppHttpJunction* )s->v_object_[ 1 ][ 1 ] )

    namespace
    {
        class member_function
        {
        public:

            member_function( bool is_auth, CoreServer::OnMemberFunction ch_uid )
            {
                is_auth_ = is_auth;
                ch_fid_ = ch_uid;
            };

            ~member_function() {};

            bool is_auth_;  // 要不要經過 驗證 OnAuth
            CoreServer::OnMemberFunction ch_fid_;
        };

        typedef std::map<channel_uid_t, member_function> function_channel_t;

        typedef CoreConnector* ( _CALLBACK CoreServer::* OnCreateSession )( );
        typedef void           ( _CALLBACK CoreServer::* OnDestroySession )( _IN CoreConnector* session );
        typedef void           ( _CALLBACK CoreServer::* OnAcceptSession )( _IN CoreConnector* session );
        typedef void           ( _CALLBACK CoreServer::* OnKickoutSession )( _IN CoreConnector* session );
        typedef _csbinbuf( _CALLBACK CoreServer::* OnAuthenication )( _IN CoreConnector* session );
        typedef int            ( _CALLBACK CoreServer::* OnReceiveMessage )( _IN CoreConnector* s );
        typedef void           ( _CALLBACK CoreServer::* OnSetSessionObject )( _IN CoreConnector* session, _IN vclass_t* obj );

        //CsLogMode                                  g_log_mode = 0;
        int                                            g_log_index = 0;;  // 全區域共用的 除錯log
        bool                                           g_is_init = false;
        pfstl::vmapl< std::map< _cs_string, _cs_string > > g_session_table;
        const _cs_string SessionName = ( "SessionUId" );
    }

    //typedef bool         ( CoreServer::*func_authenication_request )( _IN CoreConnector* s, _IN uniqueid_t uid, _IN CsEncType enc_type, _IN channel_uid_t ch_ui, _IN const ubyte_t* com_buf, _IN uint_t& com_len );
    //typedef void         ( CoreServer::*func_feedback_request )( _IN CoreConnector* s, _IN uniqueid_t uid, _IN CsEncType enc_type, _IN channel_uid_t ch_ui, _IN const _csbinbuf combin );

    //////////////////////////////////////////////////////////////////////////
    // socket 接口
    // 
    class CoreJunction: public net::TCPServer
    {
        friend CoreServer;

        cyberspace::CoreServer* p_server_;
        cyberspace::SocketSupervise                       socket_supervis_;
        //std::vector<pfkrl::Thread::Handle>                vec_thread_handle_;

        OnCreateSession                                   p_on_create_session_;
        OnDestroySession                                  p_on_destroy_session_;
        OnAcceptSession                                   p_on_accept_session_;
        OnKickoutSession                                  p_on_kickout_session_;
        OnAuthenication                                   p_on_authenication_session_;
        OnReceiveMessage                                  p_on_receive_message_;
        OnSetSessionObject                                p_on_set_session_object;
        //func_authenication_request                        p_func_authenication_request_;
        //func_feedback_request                             p_func_feedback_request_;

        SafeServerState                                   state_;
        bool                                              is_basesync_;
        bool                                              is_active_;

        Recdict                                           moudle_;
        function_channel_t                                channel_functions_[ _MAX_CHANNEL_POOL ];
        mstimen_t                                         update_sleep_mstime_;

        //class CoreSocket: public net::TCPSocket
        //{
        //public:
        //    CoreSocket() {};
        //    ~CoreSocket() {};

        //    //uniqueid_t   session_uid;
        //    CoreConnector* p_core_session;
        //};

    public:

        //stimen_t                                  stime_startup_; // 啟動的時間
        //void        ( AppServer::*OnAcceptSession )( _IN CoreConnector* session ) = &AppServer::OnAccept;
        CoreJunction( CoreServer* p_server )
        {
            p_server_ = p_server;
            is_basesync_ = false;
            update_sleep_mstime_ = 100;
            //p_on_create_session_ = p_server->OnCreateSession;
            //p_on_destroy_session_ = p_server->OnDestorySession;
            //p_on_accept_session_ = p_server->OnAccept;
            //p_on_kickout_session_ = p_server->OnKickout;
            //p_func_receive_request_ = p_server->OnReceiveRequest;
            //p_func_authenication_request_ = p_server->OnAuthenicationRequest;
            //p_func_feedback_request_ = p_server->OnFeedbackRequest;
            is_active_ = true;
        }

        ~CoreJunction() {};

        const _cs_char_t* ObjectName() const { return ( "CoreJunction" ); }

        int StartUp( _IN unsigned int allowplayers, _IN ushort_t port )
        {
            time_t startt = pftml::Time::GetSTime();
            state_.Set_Startup_Stime( startt );
            //stime_startup_ = startt;
            srand( startt );
            //time( &startt );
            // open a log history file

            // log file
            {
                mkdir( "log" );
                struct tm* p = localtime( &startt );
                _cs_char_t filename[ _MAX_PATH ];
                _stprintf( filename, "log/%s_history.log", ObjectName() );
                cyberspace::g_log_index = cyberspace::LogFileSerivce::Open( filename );
            }

            moudle_.Init( _MAX_SESSION_TABLE );
            socket_supervis_.DoStartUp( p_server_->ObjectName(), state_.Get_Startup_Stime() );

            if( g_is_init == false )
            {
                g_session_table.init( _MAX_SESSION_TABLE );
                pfkrl::Thread::BeginProcess( Execute, this, ( "CoreJunction::Execute" ) );
                g_is_init = true;
            }
            return net::TCPServer::StartUp( allowplayers, port );
        }

        int Shutdown( void )
        {
            is_active_ = false;
            CsLogMode log_mode = 0;
            SET_BIT_FLAG( log_mode, LOGMODE_PRINTOUT );
            cyberspace::LogFileSerivce::Close( log_mode );
            socket_supervis_.DoClose();
            return net::TCPServer::Shutdown();
        }

        void SetKeepAliveMStime( _IN mstimen_t mst )
        {
            socket_supervis_.SetKeepAliveMStime( mst );
        }

        void SetUpdateIntervalMSTime( _IN mstimen_t mst )
        {
            update_sleep_mstime_ = mst;
        }

        int  GetInspectedSize()
        {
            return socket_supervis_.GeInsecureSize();
        }

        //void SetSessionUID( _IN net::TCPSocket* skt, _IN cs_session_uid_t sid )
        //{
        //    ( (CoreSocket*)skt )->session_uid = sid;
        //}
        //
        //cs_session_uid_t GetSessionUID( _IN  net::TCPSocket* skt )
        //{
        //    return ( (CoreSocket*)skt )->session_uid;
        //}

        bool RegisterFunction( _IN channel_uid_t ch_uid, _IN bool is_auth, _IN CoreServer::OnMemberFunction func )
        {
            //_CsAPP_HTTPSERVER->channel_functions_.insert( std::map<_cs_string, OnMemberFunction>::value_type( actstr, func ) );
            uint_t pool_idx = ch_uid % _MAX_CHANNEL_POOL;

            function_channel_t::iterator iter = channel_functions_[ pool_idx ].find( ch_uid );
            if( iter != channel_functions_[ pool_idx ].end() )
            {
                return false;
            }

            channel_functions_[ pool_idx ].insert( function_channel_t::value_type( ch_uid, member_function( is_auth, func ) ) );
            return true;
        }

        void UnRegisterFunction( _IN channel_uid_t ch_uid )
        {
            uint_t pool_idx = ch_uid % _MAX_CHANNEL_POOL;

            channel_functions_[ pool_idx ].erase( ch_uid );
        }

        _csbinbuf DoFunction( _IN net::TCPSocket* skt, _IN channel_uid_t ch_uid, _IN const ubyte_t* com_buf, _IN uint_t com_len )
        {
            uint_t pool_idx = ch_uid % _MAX_CHANNEL_POOL;

            _csbinbuf result_buf;
            function_channel_t::iterator it = channel_functions_[ pool_idx ].find( ch_uid );
            if( it != channel_functions_[ pool_idx ].end() )
            {
                CoreConnector* connector = ( CoreConnector* )skt->GetKey( ObjectName() );

                CoreServer::OnMemberFunction func = it->second.ch_fid_;
                // 需要驗證
                if( it->second.is_auth_ )
                {
                    result_buf = ( p_server_->*p_on_authenication_session_ )( connector );
                }
                if( result_buf.empty() )
                {
                    result_buf = ( p_server_->*func )( connector, com_buf, com_len );
                }
            }
            return result_buf;
        }

        static void Execute( _IN void* data )
        {
            CoreJunction* p_junction_server = ( CoreJunction* )data;
            stimen_t starttime = pftml::Time::GetSTime();
            while( p_junction_server->is_active_ )
            {
                // 安全性的檢查 鎖IP 等等之類的
                p_junction_server->socket_supervis_.Update( p_junction_server );
                // key value
                p_junction_server->moudle_.Update();
                pfstd::MSSleep( 100 );
            }
        }

    private:

        void OnAccept( _IN net::TCPSocket* skt )
        {
            // it allow connect until server initial finish
            //_cs_char_t host[ 255 ];
            //CoreSocket* core_socket = ( CoreSocket* )( skt );
            CoreConnector* connector = ( CoreConnector* )skt->GetKey( ObjectName() );

            time_t timep;
            time( &timep );
            sockaddr_storage addr_storage;
            sockaddr* addr = skt->GetPeerAddr( addr_storage );
            if( addr == NULL )
            {
                net::TCPServer::Kickout( skt );
                return;
            }
            ipa_t ipa = net::Socket::GetSockAddrIPA( addr );
            char hostname[ NI_MAXHOST ];
            char servinfo[ NI_MAXSERV ];
            net::Socket::GetAddrNameInfo( ( struct sockaddr* )addr, hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
            _cs_string log_msg;
            log_msg = ( " ||  accept >>   HostName:%s    Service:%s    Time:%s    Ip:%s\n" );
            //log_msg = ( " \n ||================================================\n" );
            //log_msg += ( " ||  Join Client" );
            //log_msg += ( " ||------------------------------------------------\n" );
            //log_msg += ( " ||  HostName : %s\n" );
            //log_msg += ( " ||  Service  : %s\n" );
            //log_msg += ( " ||  Time     : %s" );
            //log_msg += ( " ||  Ip       : %s\n" );
            //log_msg += ( " ||================================================\n" );
            p_server_->OutputMessage( g_log_index, log_msg.c_str(), hostname, servinfo, asctime( localtime( &timep ) ), ipa.c_str() );
            state_.Set_Max_Connected_Client( this );
            if( !socket_supervis_.DoAccept( skt ) )
            {
                net::TCPServer::Kickout( skt );
                return;
            }
            ( p_server_->*p_on_accept_session_ )( connector );
        }

        void OnKickout( _IN net::TCPSocket* skt )
        {
            //CoreSocket* core_socket = ( CoreSocket* )( skt );
            CoreConnector* connector = ( CoreConnector* )skt->GetKey( ObjectName() );
            //if ( skt->GetFD() == 0 )
            //{
            //    return 0;
            //}
            //_cs_char_t host[ 255 ];
            socket_supervis_.DoKickout( skt );
            time_t timep = pftml::Time::GetSTime();
            //time( &timep );
            sockaddr_storage addr_storage;
            sockaddr* addr = skt->GetPeerAddr( addr_storage );
            if( addr == NULL )
            {
                net::TCPServer::Kickout( skt );
                return;
            }
            ipa_t ipa = net::Socket::GetSockAddrIPA( addr );
            char hostname[ NI_MAXHOST ];
            char servinfo[ NI_MAXSERV ];
            net::Socket::GetAddrNameInfo( addr, hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
            _cs_string log_msg;
            log_msg = ( " ||  kickout >>   HostName:%s    Service:%s    Time:%s    Ip:%s\n" );
            //log_msg = ( " \n ||================================================\n" );
            //log_msg += ( " ||  Exit Client\n" );
            //log_msg += ( " ||------------------------------------------------\n" );
            //log_msg += ( " ||  HostName : %s\n" );
            //log_msg += ( " ||  Service  : %s\n" );
            //log_msg += ( " ||  Time     : %s" );
            //log_msg += ( " ||  Ip       : %s\n" );
            //log_msg += ( " ||================================================\n" );
            p_server_->OutputMessage( g_log_index, log_msg.c_str(), hostname, servinfo, asctime( localtime( &timep ) ), ipa.c_str() );

            ( p_server_->*p_on_kickout_session_ )( connector );
            //p_server_->OnKnockOut( ( CoreConnector* )skt );
        }

        void OnReceive( _IN net::TCPSocket* skt )
        {

            //CoreSocket* core_socket = ( CoreSocket* )( skt );
            CoreConnector* connector = ( CoreConnector* )skt->GetKey( ObjectName() );
            //uniqueid_t    eid = CsINVALID_ID;
            //channel_uid_t ch_uid = CsINVALID_ID;
            //CsEncType     enc_type = 0;
            //ubyte_t*      recvbuf = NULL;
            //unsigned int  recvlne = 0;
            //_csbinbuf     result_buf;

            ( p_server_->*p_on_receive_message_ )( connector );

            //( p_server_->*p_func_receive_request_ )( core_socket->p_core_session, eid, enc_type, ch_uid, &recvbuf, recvlne );
            //
            //std::map<channel_uid_t, OnMemberFunction>::iterator it = channel_functions_.find( ch_uid );
            //if ( it != channel_functions_.end() )
            //{
            //    if( ( p_server_->*p_func_authenication_request_ )( core_socket->p_core_session, eid, enc_type, ch_uid, recvbuf, recvlne ) )
            //    {
            //        OnMemberFunction func = it->second;
            //        result_buf = ( p_server_->*func )( core_socket->p_core_session, eid, enc_type, recvbuf, recvlne );
            //        ( p_server_->*p_func_feedback_request_ )( core_socket->p_core_session, eid, enc_type, ch_uid, result_buf );
            //    }
            //}
            return;
        }

        //net::TCPSocket* OnNewSocket( void )
        void OnCreateSocket( _IN net::TCPSocket* sk )
        {
            //CoreSocket* core_socket = new CoreSocket;
            CoreConnector* connector = ( p_server_->*p_on_create_session_ )( );
            ( p_server_->*p_on_set_session_object )( connector, ( vclass_t* )sk );
            sk->SetKey( ObjectName(), connector );
            //return core_socket;
        }

        //void OnDelSocket( _IN net::TCPSocket* skt )
        void OnDestroySocket( _IN net::TCPSocket* sk )
        {
            //CoreSocket* core_socket = (CoreSocket*)( skt );
            CoreConnector* connector = ( CoreConnector* )sk->GetKey( ObjectName() );
            ( p_server_->*p_on_set_session_object )( connector, NULL );
            ( p_server_->*p_on_destroy_session_ )( connector );
            sk->SetKey( ObjectName(), NULL );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    //
    //
    //=============================================================================
    CoreServer::CoreServer( _IN const _cs_char_t* name )
    {
        {
            CoreJunction* p_moudle = new CoreJunction( this );
            p_moudle->p_on_create_session_ = &CoreServer::OnCreateConnector;
            p_moudle->p_on_destroy_session_ = &CoreServer::OnDestoryConnector;
            p_moudle->p_on_accept_session_ = &CoreServer::OnAccept;
            p_moudle->p_on_kickout_session_ = &CoreServer::OnKickout;
            p_moudle->p_on_authenication_session_ = &CoreServer::OnAuthenication;
            p_moudle->p_on_receive_message_ = &CoreServer::OnRecvMessage;
            p_moudle->p_on_set_session_object = &CoreServer::SetConnectorObject;
            //p_moudle->p_func_authenication_request_ = &CoreServer::OnAuthenicationRequest;
            //p_moudle->p_func_feedback_request_ = &CoreServer::OnFeedbackRequest;
            v_object_[ 0 ] = p_moudle;
            p_moudle->Create();
        }
        srand( ( int )pftml::Time::GetSTime() );

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
    CoreServer::~CoreServer()
    {
        Shutdown();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int CoreServer::Shutdown( void )
    {
        _CsCORE_JUNCTION( this )->Shutdown();
        return 0;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool CoreServer::IsAlive()
    {
        if( _CsCORE_JUNCTION( this )->GetFD() == 0 )
            return false;
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void CoreServer::SetKeepAliveMStime( _IN mstimen_t mst )      // 設定最短連線安全檢查時間
    {
        _CsCORE_JUNCTION( this )->SetKeepAliveMStime( mst );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void CoreServer::SetUpdateIntervalMSTime( _IN mstimen_t mst )   // OnUpdate 觸發的時間間隔
    {
        _CsCORE_JUNCTION( this )->SetUpdateIntervalMSTime( mst );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    stimen_t CoreServer::GetStartSec( void )               // 取得啟動的時間
    {
        return _CsCORE_JUNCTION( this )->state_.Get_Startup_Stime();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    const _cs_char_t* CoreServer::GetLocalHostname( _IN _cs_char_t* buf, _IN _cs_size_t len )
    {
        return _CsCORE_JUNCTION( this )->GetLocalHostname( buf, len );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    ushort_t CoreServer::GetLocalServicePort( void )  // 開啟接收的Port編號 0 cptp 1 http
    {
        return _CsCORE_JUNCTION( this )->GetServicePort();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    const _cs_char_t* CoreServer::GetLocalAliasName( void )         // 取得本機的別名
    {
        return name_alias_.c_str();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    const _cs_char_t* CoreServer::GetLocalServiceName( void )       //  設定檔中設定的Function名稱
    {
        return name_service_.c_str();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t CoreServer::GetLocalPollingCount( void )
    {
        return _CsCORE_JUNCTION( this )->GetPollingCount();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t CoreServer::GetLocalLimitConnectedSize( void )  // maximum for allow connect client size
    {
        return _CsCORE_JUNCTION( this )->GetMaxConnectSize();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t CoreServer::GetLocalConnectedSize( void )     // size of current connected client
    {
        return _CsCORE_JUNCTION( this )->GetConnectedSize();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t CoreServer::GetLocalMaxConnectedSize( void )     // size of current connected client
    {
        return _CsCORE_JUNCTION( this )->state_.Get_Max_Connected_Client();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    long32_t CoreServer::GetLivingThreadSize( void )
    {
        return _CsCORE_JUNCTION( this )->GetLivingThreadSize();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    bool CoreServer::LoadConfigure( _IN const _cs_char_t* filename )
    {
        XMLNode xml;
        CsLogMode log_mode = 0;
        SET_BIT_FLAG( log_mode, LOGMODE_PRINTOUT );

        if( xml.LoadFromFile( filename ) )
        {
            XMLNode* GroupCfg = xml.FindFirstNode( GroupName() );
            if( GroupCfg != NULL )
            {
                unsigned int   AllowedConnection = 0;
                _cs_char_t     host[ 255 ];
                unsigned short port = 0;
                _cs_string     aliasname;
                _cs_string     servicename;
                bool           baasesync = false;
                mstimen_t      keepalivemstime = 3000;
                std::map<_cs_string, _cs_string> setting_node;
                //////////////////////////////////////////////////////////////////////////
                // Read configure file
                {
                    XMLNode* ServerCfg = GroupCfg->FindFirstNode( ( ObjectName() ) );

                    while( ServerCfg )
                    {
                        XMLAttribute* pa = ServerCfg->FindAttribute( ( "name" ) );

                        // 如果有name 必須要跟 name_class_ 一樣
                        if( ( name_class_ == "" ) || ( pa == NULL ) )
                        {
                            break;
                        }
                        else if( pa->value_ == name_class_.c_str() )
                        {
                            break;
                        }

                        ServerCfg = xml.FindNextNode( ( ObjectName() ) );
                    }

                    //if(ServerCfg.text_.c_str()!=NULL)
                    if( ServerCfg )
                    {
                        for( uint_t ni = 0; ni < ServerCfg->GetNodeSize(); ni++ )
                        {
                            XMLNode* SettingNode = ServerCfg->FindNode( ni );

                            if( SettingNode != NULL )
                            {
                                if( SettingNode->name_ == ( "MaxNodes" ) )
                                {
                                    SettingNode->text_ >> AllowedConnection;
                                }
                                else if( SettingNode->name_ == ( "Port" ) )
                                {
                                    SettingNode->text_ >> port;
                                }
                                else if( SettingNode->name_ == ( "Alias" ) )
                                {
                                    aliasname = SettingNode->text_;
                                }
                                else if( SettingNode->name_ == ( "Service" ) )
                                {
                                    servicename = SettingNode->text_;
                                }
                                else if( SettingNode->name_ == ( "SyncMsg" ) )
                                {
                                    XMLNode* SyncMsgCfg = SettingNode;
                                    baasesync = ( SyncMsgCfg->text_ == ( "1" ) ) ? true : false;
                                }
                                else if( SettingNode->name_ == ( "RiskMSTime" ) )
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
                                            _cs_string nodename;

                                            for( uint_t pi = 1; pi < ParNodes.size(); pi++ )
                                            {
                                                // commondb>>database::name=00>>maxconnect
                                                _cs_string node_title = ParNodes[ pi ]->name_;

                                                XMLAttribute* xml_attr = ParNodes[ pi ]->FindFirstAttribute( NULL );
                                                while( xml_attr )
                                                {
                                                    node_title += _cs_string( ( "::" ) ) + xml_attr->name_ + _cs_string( ( "=\"" ) + xml_attr->value_ + _cs_string( ( "\"" ) ) );
                                                    xml_attr = ParNodes[ pi ]->FindNextAttribute( NULL );
                                                }
                                                nodename += node_title + _cs_string( ( ">>" ) );
                                                //nodename += ParNodes[ pi ]->name_ + _cs_string( ( ">>" ) );
                                            }

                                            nodename += SubNode->name_;
                                            setting_node.insert( std::map<_cs_string, _cs_string>::value_type( nodename, SubNode->text_ ) );
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
                        OutputMessage( log_mode, ( ">> Can't find server configure node!!!\n" ) );
                        goto _LOADCONFIGURE_EXIT;
                    }
                }
                _cs_string log_msg;
                log_msg =  ( "||===========================================================================\n" );
                log_msg += ( "||  cyberspace server [ %s ] bootstrap........\n||\n" );
                log_msg += ( "||              version  : %s\n" );
                log_msg += ( "||              build on : %s\n" );
                log_msg += ( "||              start on : %s||\n" );

                time_t starttime = 0;
                time( &starttime );
                log_msg += ( "||---------------------------------------------------------------------------\n" );
                OutputMessage( log_mode, log_msg.c_str(), ObjectName(), OnVersion(), __TIMESTAMP__, ctime( &starttime ) );

                if( _CsCORE_JUNCTION( this )->StartUp( AllowedConnection, port ) != 1 )
                {
                    OutputMessage( log_mode, ("StartUp failure !\n" ) );
                    goto _LOADCONFIGURE_EXIT;
                }

                //uint_t  ipcount = 0;
                ipn4_t   ipn;
                memset( &ipn, 0, _cssizeof( ipn4_t ) );

                //OutputMessage( log_mode, ( "||  server_uid : %d\n" ), GetLocalServerUID() );
                OutputMessage( log_mode, ( "||  HostName : %s\n" ), net::Socket::GetLocalHostname( host, 255 ) );
                // 自己乖乖防錯
                name_service_ = servicename;
                OutputMessage( log_mode, ( "||  Service  : %s\n" ), name_service_.c_str() );
                //
                name_alias_ = aliasname;
                OutputMessage( log_mode, ( "||  Alias    : %s\n" ), name_alias_.c_str() );
                //std::vector<ipa_t> ip4list;
                //std::vector<ipa_t> ip6list;
                //GetLocalIPAddress( ip4list, ip6list );
                std::vector<EthIF> ethif_list;
                net::Socket::GetEthIFInfo( ethif_list );

                if( ethif_list.empty() )
                {
                    log_msg = ( "||      .... can't find any ip!!\n" );
                    OutputMessage( log_mode, log_msg.c_str() );
                    return false;
                }

                OutputMessage( log_mode, ( "||  Enternet interface....\n" ) );

                for( int ifi = 0; ifi < ethif_list.size(); ifi++ )
                {
                    if( ethif_list[ ifi ].iftype == _IF_TYPE_ETHERNET )
                    {
                        if( !ethif_list[ ifi ].unicast_addrs.empty() )
                        {
                            OutputMessage( log_mode, ( "||  %-10s : " ), ethif_list[ ifi ].name.c_str() );

                            for( int ip4i = 0; ip4i < ethif_list[ ifi ].unicast_addrs.size(); ip4i++ )
                            {
                                sockaddr* skaddr = ( sockaddr* )&ethif_list[ ifi ].unicast_addrs[ ip4i ];

                                if( skaddr->sa_family == AF_INET )
                                {
                                    ipn4_t ipn4 = net::Socket::GetSockAddrIPN4( skaddr );
                                    OutputMessage( log_mode, ( "IPv4 = %-15s " ), net::Socket::IPN4ToA4( ipn4 ).c_str() );
                                    //ifidex++;
                                }

                                if( skaddr->sa_family == AF_INET6 )
                                {
                                    ipn6_t ipn6 = net::Socket::GetSockAddrIPN6( skaddr );
                                    OutputMessage( log_mode, ( "IPv6 = %s " ), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                                    //OutputMessage( log_mode, ( "||  Ip %-12s     : %-15s = %s\n" ), ethif_list[ ifi ].name.c_str(), net::Socket::IPN4ToA4( ipn4 ).c_str(), net::Socket::IPN6ToA6( ipn6 ).c_str() );
                                    //ifidex++;
                                }
                            }

                            OutputMessage( log_mode, ( "\n" ) );
                        }
                    }
                }

                log_msg = ( "||  Port : %d\n" );
                log_msg += ( "||  Max connect = %d\n" );
                _CsCORE_JUNCTION( this )->is_basesync_ = baasesync;
                log_msg += ( "||  Synchronize message : %s\n" );
                SetKeepAliveMStime( keepalivemstime );
                log_msg += ( "||  Keep-Alive mstime : %d\n" );
                OutputMessage( log_mode, log_msg.c_str(), port, AllowedConnection, ( baasesync == true ) ? "Y" : "N", keepalivemstime );

                //////////////////////////////////////////////////////////////////////////
                // load extern config setting
                if( !OnLoadConfigure( setting_node ) )
                {
                    OutputMessage( log_mode, ( "||      .... load configure setting failure\n" ) );
                    //loadret = false;
                    goto _LOADCONFIGURE_EXIT;
                }

                //
            }
        }
        else
        {
            OutputMessage( log_mode, ( "||    .... can't load configure file!!\n" ) );
            goto _LOADCONFIGURE_EXIT;
        }

        if( OnServerInitialing() != 1 )
        {
            OutputMessage( log_mode, ( "||    .... server initialize failure\n" ) );
            //loadret = false;
            goto _LOADCONFIGURE_EXIT;
        }

        pfkrl::Thread::BeginProcess( Execute, _CsCORE_JUNCTION( this ), ( "CoreServer::Execute" ) );
        OutputMessage( log_mode, ( "||\n||  >> successful to activate.\n" ) );
        OutputMessage( log_mode, ( "||===========================================================================\n\n" ) );
        return true;

_LOADCONFIGURE_EXIT:
        OutputMessage( log_mode, ( "||\n||  >> failed to activate!\n" ) );
        OutputMessage( log_mode, ( "||===========================================================================\n\n" ) );
        Shutdown();
        return false;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void CoreServer::OutputMessage( _IN CsLogMode m, _IN const _cs_char_t* format, ... )
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
        cyberspace::LogFileSerivce::OutputMessageV( 0, m, format, arglist );
        va_end( arglist );

        //cyberspace::LogSerivce::OutputMessage( cyberspace::Arbitration::g_LogModeUnit, m, format, arglist );
        return;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //CoreConnector* CoreServer::GetSessionSocket( cs_session_uid_t sid )
    //{
    //
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void CoreServer::SetSessionTimoutSec( cs_session_uid_t sessionid, stimen_t sec )
    //{
    //    sec_session_timeout_ = sec;
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool CoreServer::InsertSession( _IN cs_session_uid_t sessionid, _IN stimen_t timeoutsec )
    {
        _cs_string tseesionuid = _pf_ulltot( sessionid );
        // 直接加入
        _CsCORE_JUNCTION( this )->moudle_.Set( tseesionuid.c_str(), tseesionuid.c_str() );
        _CsCORE_JUNCTION( this )->moudle_.Setlt( tseesionuid.c_str(), timeoutsec );
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void CoreServer::DeleteSession( _IN cs_session_uid_t sessionid )
    {
        _cs_string tseesionuid = _pf_ulltot( sessionid );
        _CsCORE_JUNCTION( this )->moudle_.Del( tseesionuid.c_str() );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool CoreServer::SetSessionKey( _IN cs_session_uid_t sessionid, _IN _cs_string key, _IN _cs_string val )
    {
        _cs_string tseesionuid = _pf_ulltot( sessionid );
        // 這個seesion 必須先存在
        return _CsCORE_JUNCTION( this )->moudle_.HSetex( tseesionuid.c_str(), key.c_str(), val.c_str() );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _cs_string CoreServer::GetSessionKey( _IN cs_session_uid_t sessionid, _IN _cs_string key )
    {
        _cs_string tseesionuid = _pf_ulltot( sessionid );
        return _CsCORE_JUNCTION( this )->moudle_.HGet( tseesionuid.c_str(), key.c_str() ).c_str();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void CoreServer::DelSessionKey( _IN cs_session_uid_t sessionid, _IN _cs_string key )
    {
        _cs_string tseesionuid = _pf_ulltot( sessionid );
        _CsCORE_JUNCTION( this )->moudle_.HDel( tseesionuid.c_str(), key.c_str() );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void  CoreServer::SetSessionUID( CoreConnector* session, cs_session_uid_t sid )
    //{
    //    _CsCORE_JUNCTION( this )->SetSessionUID( ( net::TCPSocket* )session->v_object_, sid );
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //cs_session_uid_t CoreServer::GetSessionUID( CoreConnector* session )
    //{
    //    return _CsCORE_JUNCTION( this )->GetSessionUID( ( net::TCPSocket* )session->v_object_ );
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void CoreServer::Kickout( _IN CoreConnector* session )
    {
        _CsCORE_JUNCTION( this )->Kickout( ( net::TCPSocket* )session->v_object_ );
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    bool CoreServer::RegisterFunction( _IN channel_uid_t ch_uid, _IN bool is_auth, _IN OnMemberFunction func )
    {
        return _CsCORE_JUNCTION( this )->RegisterFunction( ch_uid, is_auth, func );
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    void CoreServer::UnRegisterFunction( _IN channel_uid_t ch_uid )
    {
        _CsCORE_JUNCTION( this )->UnRegisterFunction( ch_uid );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _csbinbuf CoreServer::DoFunction( _IN CoreConnector* session, _IN channel_uid_t ch_uid, _IN const ubyte_t* com_buf, _IN uint_t com_len )
    {
        return _CsCORE_JUNCTION( this )->DoFunction( ( net::TCPSocket* )session->v_object_, ch_uid, com_buf, com_len );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int CoreServer::SendMessage( _IN CoreConnector* session, _IN const ubyte_t* bp, _IN uint_t len )
    {
        return ( ( net::TCPSocket* )session->v_object_ )->Sendn( bp, len );
        //return _CsCORE_JUNCTION( this )->SendMessage( ( net::TCPSocket* )session->v_object_, bp, len );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    size32_t CoreServer::GetRecvLength( _IN CoreConnector* session )
    {
        return ( ( net::TCPSocket* )session->v_object_ )->GetReadLength();
        //return _CsCORE_JUNCTION( this )->GetRecvLength( ( net::TCPSocket* )session->v_object_ );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int CoreServer::RecvMessage( _IN CoreConnector* session, _IN ubyte_t* bp, _IN uint_t len )
    {
        return ( ( net::TCPSocket* )session->v_object_ )->Recvn( bp, len );
        //return _CsCORE_JUNCTION( this )->RecvMessage( ( net::TCPSocket* )session->v_object_, bp, len );
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    void CoreServer::SetConnectorObject( _IN CoreConnector* session, _IN vclass_t* obj )
    {
        session->v_object_ = obj;
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    void CoreServer::Execute( _IN void* data )
    {
        CoreJunction* p_junction_server = ( CoreJunction* )data;
        stimen_t start_time = pftml::Time::GetSTime();
        stimen_t curr_time = start_time;
        while( p_junction_server->p_server_->IsAlive() )
        {
            // 更新
            p_junction_server->p_server_->OnUpdate( curr_time - start_time );
            pfstd::MSSleep( p_junction_server->update_sleep_mstime_ );
            stimen_t curr_time = pftml::Time::GetSTime();
        }
    }
}

