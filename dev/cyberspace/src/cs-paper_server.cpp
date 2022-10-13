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

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-system.h>
#include <pf-thread.h>
#include <pf-stdlib.h>
#include <gadget/recdict.h>
#include <algorithmic/crypt-crc32.h>
#include <algorithmic/crypt-crc64.h>
#include <database/xdb-helper.h>
//#include <database/xdb-mysqldb.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-db_function_base.h"
#include "cs-paper_command.h"
#include "cs-paper_server.h"
#include "cs-db_cfg_reader.h"

// 有 T_Lock 放最後




namespace cyberspace
{
    //typedef CsAbtpCnSocket AppSocket;

    namespace Paper
    {
        int                g_max_tables = 100;
        CsLogMode          g_log_mode;
        int                g_log_index;

        //////////////////////////////////////////////////////////////////////////
        //
        struct CallUpdate
        {
            Server*        p_server;
            int            idx;
        };

        //////////////////////////////////////////////////////////////////////////
        //
        class SRecord : public CsRecord
        {
        public:
            //_cs_string_list ties;
            mstimen_t       create_time;
            mstimen_t       update_time;
            mstimen_t       lifetm_time;
            int             ctl;

            //void operator = ( CsRecord& record )
            //{
            //    key = record.key_;
            //    //table = record.table;
            //    value = record.value_;
            //}
            //
            //void operator = ( SRecord& srecord )
            //{
            //    key = srecord.key_;
            //    //table = srecord.table;
            //    value = srecord.value_;
            //    ties = srecord.ties;
            //    create_time = srecord.create_time;
            //    update_time = srecord.update_time;
            //    lifetm_time = srecord.lifetm_time;
            //    ctl = srecord.ctl;
            //}
        };

        //Recdict s_pRecdict;

#define _PaperRecdict(s)  ( ( (Recdict*)s->rec_storage_ ) )
        //#define _PaperRecdict( s )  (&(s->rec_storage_))

        //////////////////////////////////////////////////////////////////////////
        //
        qword_t GetKeyIndex( _cs_string key )
        {
            return crc64::Encode( key.c_str(), key.size() );
        }

        //===============================================================================
        // NAME :
        // DESC :
        //===============================================================================
        Server::Server( const _cs_char_t* name ) : CoreServer( name )
        {
            SET_BIT_FLAG( Paper::g_log_mode, LOGMODE_PRINTOUT );
            rec_storage_ = new Recdict;
            //m_pAppServer  = new CPTPAppServer( this );
            //m_pWebServer  = new HttpAppServer( this );
            //m_pPSockPeer  = new AbtpPrSocket;
            //m_fLogMode = 0;
            RegisterFunction( CsC_S_REQUEST_SET_VALUE, true, ( CoreServer::OnMemberFunction )&cyberspace::Paper::Server::DoSetValue );
            RegisterFunction( CsC_S_REQUEST_SETNX_VALUE, true, ( CoreServer::OnMemberFunction )&cyberspace::Paper::Server::DoSetnxValue );
            RegisterFunction( CsC_S_REQUEST_GET_VALUE, true, ( CoreServer::OnMemberFunction )&cyberspace::Paper::Server::DoGetValue );
            RegisterFunction( CsC_S_REQUEST_DEL_VALUE, true, ( CoreServer::OnMemberFunction )&cyberspace::Paper::Server::DoDelValue );
        }

        //===============================================================================
        // NAME :
        // DESC :
        //===============================================================================
        Server::~Server()
        {
            //ClearAllCsPSockConn();
            UnRegisterFunction( CsC_S_REQUEST_SET_VALUE );
            UnRegisterFunction( CsC_S_REQUEST_SETNX_VALUE );
            UnRegisterFunction( CsC_S_REQUEST_GET_VALUE );
            UnRegisterFunction( CsC_S_REQUEST_DEL_VALUE );
            Recdict* storage = (Recdict*)rec_storage_;
            SAFE_DELETE( storage );
            rec_storage_ = NULL;

            Shutdown();
            //SAFE_DELETE( m_pAppServer );
            //SAFE_DELETE( m_pWebServer );
            //SAFE_DELETE( m_pPSockPeer );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Server::Shutdown( void )
        {
            //m_pAppServer->Shutdown();
            //m_pWebServer->Shutdown();
            //stime_startup_ = 0;
            SAFE_DELETE_ARRAY( db_queue_ );
            _PaperRecdict( this )->Clear();

            /*
            {
                for (unsigned i = 0; i < rec_storage_.vsize(); i++)
                {
                    TS_Lock lock( &rec_storage_.lock( i ) );
                    _vmapl_iterator( SRecord* ) it = rec_storage_.begin( i );

                    while (!rec_storage_.miss( i, it ))
                    {
                        SAFE_DELETE( it->second );
                        it++;
                    }
                    rec_storage_.clear( i );
                }
                rec_storage_.destroy();
            }
            */

            /*
            {
                for ( unsigned i = 0; i < m_TabStorage.vsize(); i++ )
                {
                    TS_Lock lock( &m_TabStorage.lock( i ) );
                    _vmapl_iterator( recmap_t* ) it = m_TabStorage.begin( i );

                    while ( !m_TabStorage.miss( i, it ) )
                    {
                        SAFE_DELETE( it->second );
                        it++;
                    }

                    m_TabStorage.clear( i );
                }

                m_TabStorage.destroy();
            }
            */
            //for ( uint_t i = 0; i < database_.size(); i++ )
            //{
            //    OnDeleteDataBase( database_[ i ] );
            //}
            //database_.clear();
            //cyberspace::LogSerivce::Close( cyberspace::Paper::g_log_mode );
            return 0;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        XDatabase* Server::GetPaperDB( _IN cs_session_uid_t session_uid )
        {
            if ( ( database_pool_.size() == 0 ) || ( session_uid < 0 ) )
            {
                return NULL;
            }

            uint_t db_index = session_uid % database_pool_.size();

            std::map< uint_t, XDBPool >::iterator iter = database_pool_.find( db_index );
            if ( iter == database_pool_.end() )
            {
                return NULL;
            }

            return iter->second[ session_uid % iter->second.size() ];
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Server::OnServerInitialing( void )
        {
            srand( (int)pftml::Time::GetSTime() );
            return 1;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool Server::OnLoadConfigure( _IN std::map<_cs_string, _cs_string> setting )
        {
            int max_storage = 0;
            {
                _cs_string log_msg;

                std::map<_cs_string, _cs_string>::iterator it;
                it = setting.find( ( "MaxKeyStorage" ) );
                if ( it != setting.end() )
                {
                    log_msg += ( "||  Max key strage = %d\n" );
                    OutputMessage( Paper::g_log_mode, log_msg.c_str(), max_storage );
                    _PaperRecdict( this )->Init( max_storage );
                    //m_TabStorage.init( max_storage );
                }
                else
                {
                    OutputMessage( Paper::g_log_mode, ( "||  Can't find < MaxKeyStorage >\n" ) );
                    return false;
                }
            }

            {
                SET_BIT_FLAG( cyberspace::DBCfgReader::g_dbcfg_logmode, LOGMODE_PRINTOUT );

                {
                    _cs_string dbc_name;
                    dbc_name = ( "PaperDB>>" );

                    // 清除
                    for ( std::map< uint_t, XDBPool >::iterator it = database_pool_.begin(); it != database_pool_.end(); it++ )
                    {
                        for ( _cs_size_t i = 0; i < it->second.size(); i++ )
                        {
                            it->second[ i ]->Disconnect();
                            OnDeletePaperDB( it->second[ i ] );
                        }
                        database_pool_.clear();
                    }

                    // 取得有哪些名子
                    std::set< _cs_string > db_node;
                    for ( std::map<_cs_string, _cs_string>::iterator it = setting.begin(); it != setting.end(); it++ )
                    {
                        int      out_size = it->first.size() + 1;
                        _cs_char_t* out_buff = new _cs_char_t[ out_size ];
                        if ( _tcsbtwnstr( out_buff, out_size, it->first.c_str(), dbc_name.c_str(), ( ">>" ) ) )
                        {
                            db_node.insert( std::set< _cs_string >::value_type( dbc_name + _cs_string( out_buff ) ) );
                        }
                        SAFE_DELETE_ARRAY( out_buff );
                    }

                    for ( std::set< _cs_string >::iterator it = db_node.begin(); it != db_node.end(); it++ )
                    {
                        // 找出 資料庫名子
                        uint_t db_index;
                        {
                            int       out_size = ( *it ).size() + 1;
                            _cs_char_t * out_buff = new _cs_char_t[ out_size ];
                            if ( _tcsbtwnstr( out_buff, out_size, ( *it ).c_str(), ( "index=\"" ), ( "\"" ) ) )
                            {
                                db_index = _pf_ttoui( out_buff );
                            }
                            SAFE_DELETE_ARRAY( out_buff );
                        }

                        // 建立資料庫
                        XDBPool xdb_pool;
                        {
                            int max_db = cyberspace::DBCfgReader::GetMaxConnect( setting, ( *it ).c_str() );
                            //int max_db = cyberspace::DBCfgReader::GetDatabaseName( setting, *it );
                            for ( int_t i = 0; i < max_db; i++ )
                            {
                                xdb_pool.push_back( OnCreatePaperDB() );
                            }
                        }

                        database_pool_.insert( std::map< uint_t, XDBPool >::value_type( db_index, xdb_pool ) );

                        int max_tb = 0;
                        if ( !cyberspace::DBCfgReader::LoadCfg( setting, ( *it ).c_str(), max_tb, xdb_pool ) )
                        {
                            return false;
                        }
                    }
                }
            }

            if ( database_pool_.size() > 0 )
            {
                // crate update
                db_lock_.resize( max_db_ );
                db_queue_ = new std::queue<SRecord*>[ max_db_ ];

                for ( int i = 0; i < max_db_; i++ )
                {
                    CallUpdate* cp = new CallUpdate;
                    cp->p_server = this;
                    cp->idx = i;
                    pfkrl::Thread::BeginProcess( Execute, cp, ( "PaperServer::LoadConfigure" ) );
                }
            }
            return true;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        XDatabase* Server::OnCreatePaperDB( void )
        {
            //return ( XDatabase* )new xdb::MySqlDB;
            return xdb::helper::DBObjectPool::CreateDB( xdb::helper::XDB_Type::XDB_MYSQL );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Server::OnDeletePaperDB( _IN XDatabase* db )
        {
            //xdb::MySqlDB* xdb = ( xdb::MySqlDB* )db;
            //SAFE_DELETE( xdb );
            xdb::helper::DBObjectPool::DestoryDB( db );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //void Server::OutputMessage( CsLogMode m, const _cs_char_t* format, ... )
        //{
        //    //_cs_char_t string[ _MAX_LONG_MSG ] = { "" };
        //    //FILE str;
        //    //register FILE *outfile = &str;
        //    va_list arglist;
        //    //register int retval;
        //    //const _cs_char_t* i = format;
        //    va_start( arglist, format );
        //    //assert( string != NULL );
        //    //assert( format != NULL );
        //    //vsprintf( string, format, arglist );
        //    cyberspace::LogSerivce::OutputMessage( cyberspace::Paper::g_log_mode, m, format, arglist );
        //    va_end( arglist );
        //    
        //    //cyberspace::LogSerivce::OutputMessage( cyberspace::Arbitration::g_LogModeUnit, m, format, arglist );
        //    return;
        //}

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //void Server::SetLogMode( CsLogMode m )
        //{
        //    m_fLogMode = m;
        //}

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //CsLogMode Server::GetLogMode( void )
        //{
        //    return m_fLogMode;
        //}

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //void fucn_prinf_all_value( RecHash value )
        //{
        //
        //}

        void Server::PrintOut()
        {
            /*
            _PaperRecdict( this )->ForAllValue( fucn_prinf_all_value );

            {
                for (unsigned i = 0; i < rec_storage_.vsize(); i++)
                {
                    TS_Lock lock( &rec_storage_.lock( i ) );
                    _vmapl_iterator( SRecord* ) it = rec_storage_.begin( i );

                    while (!rec_storage_.miss( i, it ))
                    {
                        printf( "%s : %s\n", it->second->key_.c_str(), it->second->value_.c_str() );
                        it++;
                    }

                    rec_storage_.clear( i );
                }
                rec_storage_.destroy();
            }
            */
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //XDatabase*  Server::OnCreateCommonDB( void )
        //{
        //    return ( XDatabase* )new xdb::MySqlDB;
        //}

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //void Server::OnDeleteCommonDB( XDatabase* db )
        //{
        //    xdb::MySqlDB* xdb = ( xdb::MySqlDB* ) db;
        //    SAFE_DELETE( xdb );
        //}

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Server::DoSetValue( AppSocket* s, channel_uid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = (Server*)this;
            AppSocket*  pclient = s;
            mstimen_t currmsce = pftml::Time::GetTickMSTime();
            mstimen_t create_msce = currmsce;
            //mstimen_t updaet_msce = currmsce;
            std::vector<CsRecord> records;
            std::vector<int>      live_secs;
            {
                Databuf     recvdata;
                recvdata.buf = (ubyte_t*)combin;
                recvdata.len = comlen;
                CsPaperCommand::SetValue::Server::GetRequest( &recvdata, records, live_secs );
            }
            std::vector<int>      rets;
            std::vector<SRecord*> db_records;
            {
                for ( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    //_csbinbuf  old_value;
                    const _csbinbuf old_value = _PaperRecdict( this )->GetSet( records[ i ].key_.c_str(), records[ i ].value_ );
                    int  ret = PAPERRESULT_UNKNOW;
                    if ( !old_value.empty() )
                    {
                        ret = PAPERRESULT_EXIST_SUCCESS;
                    }
                    else
                    {
                        ret = PAPERRESULT_SUCCESS;
                    }

                    /*
                    qword_t keyindex = GetKeyIndex( records[ i ].key_ );
                    TS_Lock lock( &pserver->rec_storage_.lock( keyindex ) );
                    _vmapl_iterator( SRecord* ) it = pserver->rec_storage_.find( keyindex );
                    SRecord* vb = NULL;

                    if ( !pserver->rec_storage_.miss( keyindex, it ) )
                    {
                        old_value = it->second->value_;
                        vb = it->second;
                        create_msce = vb->create_time;
                        ret = PAPERRESULT_EXIST_SUCCESS;
                    }
                    else
                    {
                        vb = new SRecord;
                        pserver->rec_storage_.insert( keyindex, vb );
                        vb->create_time = currmsce;
                        ret = PAPERRESULT_SUCCESS;
                    }


                    ( *vb ) = records[ i ];
                    vb->update_time = currmsce;
                    vb->lifetm_time = live_secs[ i ];
                    vb->ctl = 1;
                    */
#ifdef _DEBUG
                    _cs_string debugstring;
                    Pf_INFO( _pf_tstrprintf( debugstring, ( "set value : key = [ %s ], new value = [ %s ], old value = [ %s ], lifetm_time =  [ %d ]\n" ), records[ i ].key_.c_str(), records[ i ].value_.c_str(), old_value.c_str(), live_secs[ i ] ) );
#endif
                    // 把原先存在的資料再傳回去
                    {
                        records[ i ].value_ = old_value.c_str();
                        rets.push_back( ret );
                    }

                    // put into db query list
                    {
                        SRecord* vb = new SRecord;
                        {
                            vb->key_ = records[ i ].key_;
                            vb->value_ = db_records[ i ]->value_;
                            vb->create_time = db_records[ i ]->create_time;
                            vb->update_time;
                            vb->lifetm_time = live_secs[ i ];

                            vb->ctl = 1;
                            int idx = rand() % pserver->max_db_;
                            TS_Lock lock( &pserver->db_lock_[ idx ] );
                            pserver->db_queue_[ idx ].push( vb );
                        }

                        //(*vb) = (*db_records[i]);
                        //vb->ctl = 1;
                        //int idx = rand() % pserver->max_db_;
                        //TS_Lock lock( &pserver->db_lock_[idx] );
                        //pserver->db_queue_[idx].push( vb );

                        //db_records.push_back( vb );
                        //records[ i ].value_ = old_value;
                        //rets.push_back( ret );
                    }
                }
            }
            // send feedback
            {
                _csbinbuf retbuf;
                CsPaperCommand::SetValue::Server::SetFeedback( retbuf, records, rets );
                SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_SET_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }

            // put into db query list
            //for ( int i = 0; i < db_records.size(); i++ )
            //{
            //    SRecord* vb = new SRecord;
            //    ( *vb ) = ( *db_records[ i ] );
            //    vb->ctl = 1;
            //    int idx = rand() % pserver->max_db_;
            //    TS_Lock lock( &pserver->db_lock_[ idx ] );
            //    pserver->db_queue_[ idx ].push( vb );
            //}
            return 1;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Server::DoSetnxValue( AppSocket* s, channel_uid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = (Server*)this;
            AppSocket*  pclient = s;
            mstimen_t currmsce = pftml::Time::GetTickMSTime();
            std::vector<CsRecord> records;
            std::vector<int>      live_secs;
            {
                Databuf     recvdata;
                recvdata.buf = (ubyte_t*)combin;
                recvdata.len = comlen;
                CsPaperCommand::SetValue::Server::GetRequest( &recvdata, records, live_secs );
            }
            std::vector<int>      rets;
            std::vector<SRecord*> db_records;
            {
                for ( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    //_csbinbuf  old_value;
                    //int     ret = PAPERRESULT_UNKNOW;
                    bool old_value = _PaperRecdict( this )->Setnx( records[ i ].key_.c_str(), records[ i ].value_.c_str() );
                    int      ret = PAPERRESULT_UNKNOW;
                    if ( old_value )
                    {
                        ret = PAPERRESULT_SUCCESS;
                    }
                    else
                    {
                        ret = PAPERRESULT_EXIST_FAILURE;
                    }

                    //qword_t keyindex = GetKeyIndex( records[i].key_ );
                    //TS_Lock lock( &pserver->rec_storage_.lock( keyindex ) );
                    //_vmapl_iterator( SRecord* ) it = pserver->rec_storage_.find( keyindex );
                    //SRecord* vb = NULL;
                    //
                    //if (pserver->rec_storage_.miss( keyindex, it ))
                    //{
                    //    SRecord* vb = new SRecord;
                    //    pserver->rec_storage_.insert( keyindex, vb );
                    //    (*vb) = records[i];
                    //    vb->create_time = currmsce;
                    //    vb->update_time = currmsce;
                    //    vb->lifetm_time = live_secs[i];
                    //    vb->ctl = 2;
                    //    ret = PAPERRESULT_SUCCESS;
                    //}
                    //else
                    //{
                    //    old_value = it->second->value_;
                    //    ret = PAPERRESULT_EXIST_FAILURE;
                    //}

#ifdef _DEBUG
                    _cs_char_t debugstring[ _MAX_MSG ];
                    _stprintf( debugstring, ( "set value : key = [ %s ], new value = [ %s ], lifetm_time =  [ %d ]\n" ), records[ i ].key_.c_str(), records[ i ].value_.c_str(), live_secs[ i ] );
                    Pf_INFO( debugstring );
#endif
                    // 把原先存在的資料再傳回去
                    {
                        records[ i ].value_ = records[ i ].value_.c_str();
                        rets.push_back( ret );
                    }

                    {
                        SRecord* vb = new SRecord;
                        {
                            vb->key_ = db_records[ i ]->key_;
                            vb->value_ = db_records[ i ]->value_;
                            vb->ctl = 1;
                            int idx = rand() % pserver->max_db_;
                            TS_Lock lock( &pserver->db_lock_[ idx ] );
                            pserver->db_queue_[ idx ].push( vb );
                        }
                    }
                    // put old value to return record
                    //{
                    //    if (vb != NULL)
                    //    {
                    //        db_records.push_back( vb );
                    //    }
                    //
                    //    records[i].value_ = old_value;
                    //    rets.push_back( ret );
                    //}
                }
            }
            // send feedback
            {
                _csbinbuf retbuf;
                CsPaperCommand::SetValue::Server::SetFeedback( retbuf, records, rets );
                SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_SETNX_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }

            // put into db query list
            //for (int i = 0; i < db_records.size(); i++)
            //{
            //    SRecord* vb = new SRecord;
            //    (*vb) = (*db_records[i]);
            //    vb->ctl = 2;
            //    int idx = rand() % pserver->max_db_;
            //    TS_Lock lock( &pserver->db_lock_[idx] );
            //    pserver->db_queue_[idx].push( vb );
            //}

            return 1;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Server::DoGetValue( AppSocket* s, channel_uid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = (Server*)this;
            AppSocket*  pclient = s;
            mstimen_t currmsce = pftml::Time::GetTickMSTime();
            _cs_string_list keys;
            {
                Databuf     recvdata;
                recvdata.buf = (ubyte_t*)combin;
                recvdata.len = comlen;
                CsPaperCommand::GetValue::Server::GetRequest( &recvdata, keys );
            }
            std::vector<CsRecord> records;
            std::vector<int>    rets;
            {
                for ( _cs_string_list::iterator it_keys = keys.begin(); it_keys != keys.end(); it_keys++ )
                {
                    //long32_t idx = 0;
                    CsRecord record;
                    record.key_ = ( *it_keys );
                    int    ret = PAPERRESULT_UNKNOW;
                    {
                        record.value_ = _PaperRecdict( this )->Get( record.key_.c_str() );
                        if ( !record.value_.empty() )
                        {
                            ret = PAPERRESULT_SUCCESS;
                        }
                        else
                        {
                            ret = PAPERRESULT_NOT_EXIST;
                        }

                        /*
                        qword_t keyindex = GetKeyIndex( record.key_ );
                        TS_Lock lock( &pserver->rec_storage_.lock( keyindex ) );
                        _vmapl_iterator( SRecord* ) it_srecord = pserver->rec_storage_.find( keyindex );

                        if (!pserver->rec_storage_.miss( keyindex, it_srecord ))
                        {
                            SRecord* vb = it_srecord->second;

                            // 過期了?
                            if (((vb->create_time + vb->lifetm_time) <= currmsce) && (vb->lifetm_time != 0))
                            {
                                // 過期了
                                pserver->rec_storage_.erase( it_srecord );
                                SAFE_DELETE( vb );
                                ret = PAPERRESULT_OVERDUE;
                            }
                            else
                            {
                                record.value_ = vb->value_;
                                ret = PAPERRESULT_SUCCESS;
                            }
                        }
                        else
                        {
                            ret = PAPERRESULT_NOT_EXIST;
                        }
                        */
                    }

                    // find in database
                    if ( ret == PAPERRESULT_NOT_EXIST )
                    {
                        // put into db query list
                        _cs_string val;
                        int create_time = 0;
                        int update_time = 0;
                        int lifetm_time = 0;
                        int idx = rand() % pserver->max_db_;
                        //int tbidx =   crc32::Encode( record.key_.c_str(), record.key_.size() ) % g_max_tables;
                        XDBResBuf dbvalue;

                        if ( cs_db_function::paper::dbGetRecord( pserver->GetPaperDB( idx ), record.key_.c_str(), dbvalue, create_time, update_time, lifetm_time ) != -1 )
                        {
                            // 過期了?
                            if ( ( ( create_time + lifetm_time ) <= currmsce ) && ( lifetm_time != 0 ) )
                            {
                                ret = PAPERRESULT_OVERDUE;
                            }
                            else
                            {
                                SRecord* vb = new SRecord;
                                vb->key_ = record.key_;
                                vb->value_.copy( _GetXDBResBufPtr( dbvalue ), _GetXDBResBufLen( dbvalue ) );
                                vb->create_time = create_time;
                                vb->update_time = update_time;
                                vb->lifetm_time = lifetm_time;
                                vb->ctl = 3;
                                _PaperRecdict( this )->Set( vb->key_.c_str(), vb->value_ );
                                record.value_ = vb->value_;
                                ret = PAPERRESULT_SUCCESS;
                            }
                        }
                    }

#ifdef _DEBUG
                    _cs_char_t debugstring[ _MAX_MSG ];
                    _stprintf( debugstring, ( "get value : key = [ %s ], value = [ %s ], ret = [ %d ]\n" ), record.key_.c_str(), record.value_.c_str(), ret );
                    Pf_INFO( debugstring );
#endif
                    {
                        records.push_back( record );
                        rets.push_back( ret );
                    }
                }
            }
            {
                _csbinbuf retbuf;
                CsPaperCommand::GetValue::Server::SetFeedback( retbuf, records, rets );
                SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_GET_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }
            return 1;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Server::DoDelValue( AppSocket* s, channel_uid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = (Server*)this;
            AppSocket*  pclient = s;
            _cs_string_list  keys;
            {
                Databuf     recvdata;
                recvdata.buf = (ubyte_t*)combin;
                recvdata.len = comlen;
                CsPaperCommand::DelValue::Server::GetRequest( &recvdata, keys );
            }
            std::vector<int>      rets;
            _cs_string_list         dbkeys;
            {
                for ( _cs_string_list::iterator it_keys = keys.begin(); it_keys != keys.end(); it_keys++ )
                {
                    CsRecord record;
                    int      ret = PAPERRESULT_UNKNOW;
                    record.key_ = ( *it_keys );

                    {
                        _PaperRecdict( this )->Del( record.key_.c_str() );
                        ret = PAPERRESULT_NOT_EXIST_SUCCESS;
                    }

                    /*
                    {
                        qword_t keyindex = GetKeyIndex( record.key_ );
                        TS_Lock lock( &pserver->rec_storage_.lock( keyindex ) );
                        _vmapl_iterator( SRecord* ) it = pserver->rec_storage_.find( keyindex );

                        if (!pserver->rec_storage_.miss( keyindex, it ))
                        {
                            SRecord* vb = it->second;
                            SAFE_DELETE( vb );
                            pserver->rec_storage_.erase( keyindex );
                            ret = PAPERRESULT_SUCCESS;
                        }
                        else
                        {
                            ret = PAPERRESULT_NOT_EXIST_SUCCESS;
                        }
                    }
                    */
#ifdef _DEBUG
                    _cs_char_t debugstring[ _MAX_MSG ];
                    _stprintf( debugstring, ( "del value : key = [ %s ], value = [ %s ], ret = [ %d ]\n" ), record.key_.c_str(), record.value_.c_str(), ret );
                    Pf_INFO( debugstring );
#endif
                    {
                        dbkeys.push_back( record.key_ );
                        rets.push_back( ret );
                    }
                }
            }
            {
                _csbinbuf retbuf;
                CsPaperCommand::DelValue::Server::SetFeedback( retbuf, rets );
                SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_DEL_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }

            // put into db query list
            for ( _cs_string_list::iterator it_keys = dbkeys.begin(); it_keys != dbkeys.end(); it_keys++ )
            {
                SRecord* vb = new SRecord;
                vb->key_ = ( *it_keys );
                vb->create_time = 0;
                vb->update_time = 0;
                vb->lifetm_time = 0;
                vb->ctl = 4;
                int idx = rand() % pserver->max_db_;
                TS_Lock lock( &pserver->db_lock_[ idx ] );
                pserver->db_queue_[ idx ].push( vb );
            }

            return 1;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void __cdecl Server::Execute( void* data )
        {
            CallUpdate* cp = (CallUpdate*)data;
            Server* pserver = cp->p_server;
            int     cindex = cp->idx;
            XDatabase* pdb = pserver->GetPaperDB( cindex );

            while ( pserver->IsAlive() )
            {
                _PaperRecdict( pserver )->Update();

                SRecord* vb = NULL;
                {
                    TS_Lock lock( &pserver->db_lock_[ cindex ] );

                    if ( !pserver->db_queue_[ cindex ].empty() )
                    {
                        vb = pserver->db_queue_[ cindex ].front();
                        pserver->db_queue_[ cindex ].pop();
                    }
                    else
                    {
                        goto _EXECUTE_EMPTY;
                    }
                }

                switch ( vb->ctl )
                {
                case 1:
                    {
                        //int tbidx =   crc32::Encode( vb->key_.c_str(), vb->key_.size() ) % g_max_tables;
                        //paper::dbSetRecord( pdb, tbidx, vb->key_.c_str(), vb->value_.c_str(), vb->update_time, vb->lifetm_time );
                        cs_db_function::paper::dbSetRecord( pdb, vb->key_.c_str(), vb->value_.c_str(), vb->update_time, vb->lifetm_time );
                    }
                    break;

                case 2:
                    {
                        //int tbidx =   crc32::Encode( vb->key_.c_str(), vb->key_.size() ) % g_max_tables;
                        //paper::dbSetnxRecord( pdb, tbidx, vb->key_.c_str(), vb->value_.c_str(), vb->create_time, vb->lifetm_time );
                        cs_db_function::paper::dbSetnxRecord( pdb, vb->key_.c_str(), vb->value_.c_str(), vb->create_time, vb->lifetm_time );
                    }
                    break;

                case 4:
                    {
                        //int tbidx =   crc32::Encode( vb->key_.c_str(), vb->key_.size() ) % g_max_tables;
                        //paper::dbDeleteRecord( pdb, tbidx, vb->key_.c_str() );
                        cs_db_function::paper::dbDelRecord( pdb, vb->key_.c_str() );
                    }
                    break;
                }

                SAFE_DELETE( vb );
_EXECUTE_EMPTY:
                pfstd::MSSleep( 100 );
            }

            //pserver->OnDelete( pdb );
        }

    }
}
;

