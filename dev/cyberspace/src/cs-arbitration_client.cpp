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
#include <string>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-timer.h>
#include <pf-system.h>
#include <pf-stdlib.h>
#include <net/net-server.h>
//
#include <memio.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-arbitration_client.h"
#include "cs-scs-system.h"
#include "cs-packet_factory.h"
#include "cs-base_command.h"
#include "cs-arbit_command.h"
#include "cs-log_file_service.h"
#include "cs-pri_def.h"


namespace cyberspace
{
    extern int g_log_index;

    namespace Arbitration
    {
#define WAIT_TRY_TIME 10000

        //pfkrl::Thread::Handle g_ArbitReconnect = 0;

#define _GetAbtpCSocketClient( s )    ((CsAbtpCnSocket*)((net::TCPSocket*)s))
    };

    //=============================================================================
    //
    //
    //=============================================================================
    ArbitrationClient::ArbitrationClient( ArbitrationDelegate* s ): net::TCPClient( new CsAbtpCnSocket )
    {
        //sectionid = id;
        delegate_ = s;
        //v_packetfactory_ = NULL;
        log_mode_ = 0;
        SET_BIT_FLAG( log_mode_, LOGMODE_OUTDEBUG );
        SET_BIT_FLAG( log_mode_, LOGMODE_SAVEFILE );
        //v_scssystem_ = NULL;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ArbitrationClient::~ArbitrationClient()
    {
        delegate_ = NULL;
        //PacketClient::Close();
        Shutdown();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int ArbitrationClient::StartUp( _cs_char_t* hname, unsigned short hport )
    {
        TS_Lock lock( &arbitrationclient_ut_lock_ );
        //if ( v_packetfactory_ == NULL )
        //{
        //    v_packetfactory_ = new PacketFactory;
        //}
        //v_packetfactory_->Initate();
        host_name_ = hname;
        host_port_ = hport;

        if ( Ping() )
        {
            return 1;
        }

        threads_handle_.push_back( pfkrl::Thread::BeginProcess( ArbitrationClient::TryToConnect, this, ( "ArbitrationClient::StartUp" ) ) );
        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int ArbitrationClient::Shutdown( void )
    {
        m_RetryConnectToArbit.SetValue( false );
        while ( 1 )
        {
            if ( pfkrl::Thread::TestProcessExit( threads_handle_ ) )
            {
                break;
            }

            pfstd::MSSleep( 100 );
        }
        return Close();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int ArbitrationClient::SendMessage( channel_uid_t cid, const ubyte_t* buff, _cs_size_t len )
    {
        if ( delegate_ == NULL )
        {
            return -1;
        }

        return _GetAbtpCSocketClient(this)->SendPacket( CsINVALID_ID, cid, buff, len );
    }

    //=============================================================================
    //
    // send host information to arbitserver
    //=============================================================================
    int ArbitrationClient::RecvMessage( channel_uid_t& cid, ubyte_t** buff, _cs_size_t& len )
    {
        uniqueid_t uid = CsINVALID_ID;
        return _GetAbtpCSocketClient( this )->RecvPacket( uid, cid, buff, len );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int ArbitrationClient::Close()
    {
        //m_RetryConnectToArbit.SetValue( false );
        TS_Lock lock( &arbitrationclient_ut_lock_ );
        //{
        //    TS_Lock lock( &m_lCommVec );
        //    
        //    for ( uint_t i = 0; i < m_tCommVec.size(); i++ )
        //    {
        //        SAFE_DELETE_ARRAY( m_tCommVec[ i ]->m_buf );
        //        SAFE_DELETE( m_tCommVec[ i ] );
        //    }
        //    
        //    m_tCommVec.clear();
        //}

        if ( m_RetryConnectToArbit.GetValue() )
        {
            threads_handle_.push_back( pfkrl::Thread::BeginProcess( TryToConnect, this, ( "ArbitrationClient::Closer" ) ) );
        }

        return net::TCPClient::Close();
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void ArbitrationClient::OnReceive()
    {
        channel_uid_t command_uid = 0;
        ubyte_t*      recv_buff = NULL;
        _cs_size_t     command_len = 0;
        
        //_cs_size_t recvlen = 0;
        if ( RecvMessage( command_uid, &recv_buff, command_len ) > -1 )
        {
            switch ( command_uid )
            {
            case CsS_C_REQUEST_REPORT_HOSTINFO:
                {
                    TS_Lock lock( &arbitrationclient_ut_lock_ );
#ifdef _DEBUG
                    cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, " Arbitration receive request report host information!!\n" );
#endif
                    CsArbitCommand::ReportHostInfo::Client::GetRequest( recv_buff, server_id_, arbit_entity_ );

                    //ArbitSyncHost();
                    if ( delegate_ == NULL )
                    {
                        return;
                    }

                    //TS_Lock utlock(&arbitrationclient_ut_lock_);
                    // 請告訴我本端的資訊
                    CsSectionEntity hnode;
                    delegate_->OnSyncSection( hnode );
                    _csbinbuf buf;
                    CsArbitCommand::ReportHostInfo::Client::SetFeedback( buf, &hnode );
                    SendMessage( CsC_S_FEEDBACK_REPORT_HOSTINFO, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                }
                break;

            case CsS_C_JOIN_HOSTINFO_POST:
                {
#ifdef _DEBUG
                    cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, " Arbitration receive host join information!!\n" );
#endif
                    CsSectionEntity node;
                    //memset(&node, 0, _cssizeof(CsSectionEntity));
                    CsArbitCommand::PostJoinHostInfo::Client::GetFeedback( recv_buff, node );
                    delegate_->OnJoinSection( &node );
                }
                break;

            case CsS_C_EXIT_HOSTINFO_POST:
                {
#ifdef _DEBUG
                    cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, " Arbitration receive host exit information!!\n" );
#endif
                    cs_section_uid_t sid;
                    CsArbitCommand::PostExitHostInfo::Client::GetFeedback( recv_buff, sid );
                    delegate_->OnExitSection( sid );
                }
                break;
            }
        }
        return;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool ArbitrationClient::Ping( void )
    {
        //v_scssystem_->Close();
        net::TCPClient::Close();

        if ( net::TCPClient::Connect( (_cs_char_t*)host_name_.c_str(), host_port_ ) )
        {
            // 2016/09/21 由 server 要求
            //UpdateHost();
            //ArbitGetAllHostInfo();
            return true;
        }

        return false;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void ArbitrationClient::TryToConnect( void* data )
    {
        ArbitrationClient* client = (ArbitrationClient*)data;

        if ( client->delegate_ == NULL )
        {
            return;
        }

        //client->m_RetryConnectToArbit.SetValue(true);
        cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, client->log_mode_, " Start search arbit-server....\n" );
        uint_t timesi = 0;

        while ( client->m_RetryConnectToArbit.GetValue() )
        {
            timesi++;
            char buff[512];
            _stprintf( buff, " Test search arbit-server %d times\n", timesi );
            cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, client->log_mode_, buff );

            if ( client->Ping() )
            {
                _stprintf( buff, " Reconnect arbit-server on %d times successful\n", timesi );
                cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, client->log_mode_, buff );
                //client->m_RetryConnectToArbit.SetValue(false);
                return;
            }

            pfstd::MSSleep( WAIT_TRY_TIME );
        }

        //client->m_RetryConnectToArbit.SetValue(false);
    }

};
