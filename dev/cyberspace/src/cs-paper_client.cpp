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

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <string>
#include <pf-timer.h>
//
#include <memio.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-paper_client.h"
#include "cs-packet_factory.h"
#include "cs-paper_command.h"
#include "cs-pri_def.h"



namespace cyberspace
{
    namespace Paper
    {
#define _GetAbtpCSocketClient( s )    ((CsAbtpCnSocket*)((net::TCPSocket*)s))

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Client::Client(): net::TCPClient( new CsAbtpCnSocket )
        {
            //v_packetfactory_ = NULL;
            //v_scssystem_ = NULL;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Client::~Client()
        {
            Close();
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Client::StartUp( const _cs_char_t* hname, unsigned short port )
        {
            TS_Lock lock( &paperclient_ut_lock_ );
            host_name_ = hname;              // server address
            host_port_ = port;               // port number

            if( Ping() )
            {
                return 1;
            }

            return 0;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Client::Shutdown( void )
        {
            return Close();
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        _cs_string Client::Set( _cs_string key, _cs_string value, int live_sec )
        {
            {
                _csbinbuf buf;
                CsRecord set_record;
                set_record.key_ = key;
                //set_record.table = ("");
                set_record.value_ = value.c_str();
                CsRecordList records;
                records.push_back( set_record );
                std::vector<int>      live_secs;
                live_secs.push_back( live_sec );
                CsPaperCommand::SetValue::Client::SetRequest( buf, records, live_secs );
                _GetAbtpCSocketClient( this )->SendPacket( CsINVALID_ID, CsC_S_REQUEST_SET_VALUE, ( buf ), _CsGetResBufLen( buf ) );
            }
            {
                uniqueid_t  uid = CsINVALID_ID;
                channel_uid_t commandId;
                ubyte_t*    recvbuf = NULL;
                _cs_size_t    recvlen = 0;

                if( _GetAbtpCSocketClient( this )->RecvPacket( uid, commandId, &recvbuf, recvlen ) > 0 )
                {
                    if( commandId != CsS_C_FEEDBACK_SET_VALUE )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    //_cs_string retkey;
                    //ubyte_t* valbuf = NULL;
                    //int     vallen = 0;
                    CsRecordList ret_records;
                    std::vector<int>      rets;
                    CsRecord              ret_record;
                    //int                   ret = -1;
                    Databuf               recvdata;
                    recvdata.buf = recvbuf;
                    recvdata.len = recvlen;

                    if( CsPaperCommand::SetValue::Client::GetFeedback( &recvdata, ret_records, rets ) != 1 )
                    {
                        return ( "" );
                    }

                    if( ret_record.key_ != key )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    //_csbinbuf retbuf;
                    //retbuf.copy( valbuf, vallen );
                    return ret_records[ 0 ].value_.c_str();
                }
            }
            return ( "" );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        _cs_string Client::Setnx( _IN _cs_string key, _IN _cs_string value, _IN int live_sec )
        {
            {
                _csbinbuf buf;
                CsRecord set_record;
                set_record.key_ = key;
                //set_record.table = ("");
                set_record.value_ = value.c_str();
                CsRecordList records;
                records.push_back( set_record );
                std::vector<int>      live_secs;
                live_secs.push_back( live_sec );
                CsPaperCommand::SetValue::Client::SetRequest( buf, records, live_secs );
                _GetAbtpCSocketClient( this )->SendPacket( CsINVALID_ID, CsC_S_REQUEST_SETNX_VALUE, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            }
            {
                uniqueid_t    uid = CsINVALID_ID;
                channel_uid_t commandId;
                ubyte_t*      recvbuf = NULL;
                _cs_size_t     recvlen = 0;

                if( _GetAbtpCSocketClient( this )->RecvPacket( uid, commandId, &recvbuf, recvlen ) > 0 )
                {
                    if( commandId != CsS_C_FEEDBACK_SETNX_VALUE )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    CsRecordList ret_records;
                    std::vector<int>      rets;
                    Databuf  recvdata;
                    recvdata.buf = recvbuf;
                    recvdata.len = recvlen;

                    if( CsPaperCommand::SetValue::Client::GetFeedback( &recvdata, ret_records, rets ) != 1 )
                    {
                        return ( "" );
                    }

                    if( ret_records[ 0 ].key_ != key )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    //_csbinbuf retbuf;
                    //retbuf.copy( valbuf, vallen );
                    return ret_records[ 0 ].value_.c_str();
                }
            }
            return ( "" );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        _cs_string Client::Get( _cs_string key )
        {
            {
                _cs_string_list keys;
                keys.push_back( key );
                _csbinbuf buf;
                CsPaperCommand::GetValue::Client::SetRequest( buf, keys );
                _GetAbtpCSocketClient( this )->SendPacket( CsINVALID_ID, CsC_S_REQUEST_GET_VALUE, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            }
            {
                uniqueid_t  uid = CsINVALID_ID;
                channel_uid_t commandId;
                ubyte_t*    recvbuf = NULL;
                _cs_size_t    recvlen = 0;

                if( _GetAbtpCSocketClient( this )->RecvPacket( uid, commandId, &recvbuf, recvlen ) > 0 )
                {
                    if( commandId != CsS_C_FEEDBACK_GET_VALUE )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    CsRecordList ret_records;
                    std::vector<int>      rets;
                    Databuf recvdata;
                    recvdata.buf = recvbuf;
                    recvdata.len = recvlen;
                    CsPaperCommand::GetValue::Client::GetFeedback( &recvdata, ret_records, rets );
                    //SAFE_DELETE_ARRAY( recvbuf );

                    if( ret_records[ 0 ].key_ != key )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    return ret_records[ 0 ].value_.c_str();
                }
            }
            return ( "" );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        _cs_string Client::Del( _cs_string key )
        {
            {
                _cs_string_list keys;
                keys.push_back( key );
                _csbinbuf buf;
                CsPaperCommand::DelValue::Client::SetRequest( buf, keys );
                _GetAbtpCSocketClient( this )->SendPacket( CsINVALID_ID, CsC_S_REQUEST_DEL_VALUE, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            }
            {
                uniqueid_t  uid = CsINVALID_ID;
                channel_uid_t commandId;
                ubyte_t*    recvbuf = NULL;
                _cs_size_t    recvlen = 0;

                if( _GetAbtpCSocketClient( this )->RecvPacket( uid, commandId, &recvbuf, recvlen ) > 0 )
                {
                    if( commandId != CsS_C_FEEDBACK_DEL_VALUE )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    std::vector<int> rets;
                    Databuf recvdata;
                    recvdata.buf = recvbuf;
                    recvdata.len = recvlen;
                    CsPaperCommand::DelValue::Client::GetFeedback( &recvdata, rets );
                    //SAFE_DELETE_ARRAY( recvbuf );

                    if( rets[ 0 ] != PAPERRESULT_SUCCESS )
                    {
                        // TODO: Pin
                        return ( "" );
                    }

                    return key;
                }
            }
            return ( "" );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Client::Join( _cs_string_list key, _cs_string tables )
        {
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Client::Part( _cs_string_list key, _cs_string tables )
        {
            return;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        _cs_string_map Client::GetTb( _cs_string table )
        {
            _cs_string_map string_map;
            return string_map;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Client::OnReceive()
        {
            return;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Client::Close( void )
        {
            TS_Lock lock( &paperclient_ut_lock_ );
            return net::TCPClient::Close();
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool Client::Ping( void )
        {
            //v_scssystem_->Close();
            net::TCPClient::Close();
            if( net::TCPClient::Connect( ( _cs_char_t* )host_name_.c_str(), host_port_ ) )
            {
                net::TCPClient::SetBlock( false );
                return true;
            }

            return false;
        }
    };

};
