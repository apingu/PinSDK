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
#include <assert.h>

//#include <xmlnode.h>
#include <algorithmic/Crc32.h>

//#include "CsLogService.h"
//#include "MessageDispatcher.h"
#include "CsDBFunction.h"
#include "CsPaperCommand.h"
#include "CsPaperServer.h"
//#include "CsPSockPeer.h"
//#include "CsPSockConn.h"
//#include "CsHttpServer.h"

// 有 T_Lock 放最後

#include <Debug.h>
#include <PfSysinfo.h>
#include <PfThread.h>
#include <PfStdlib.h>
//#include <database/XMySQLDB.h>
//#include <NetServer.h>
#include "CsDBCfgReader.h"
#include <XMySQLDB.h>

#include <algorithmic/Crc64.h>


namespace Cyberspace
{
    //typedef CsPSockConn AppSocket;

    namespace Paper
    {
        int       g_MaxTb = 100;
        CsLogMode g_LogMode;
        int       g_LogIndex;


        //////////////////////////////////////////////////////////////////////////
        //
        struct CallUpdate
        {
            Server*     pServer;
            int         Idx;
        };

        //////////////////////////////////////////////////////////////////////////
        //
        class SRecord : public CsRecord
        {
        public:
            _cs_string_list ties;
            mstimen_t       create_time;
            mstimen_t       update_time;
            mstimen_t       live_sec;
            int             ctl;

            void operator = ( CsRecord& record )
            {
                key = record.key;
                //table = record.table;
                value = record.value;
            }

            void operator = ( SRecord& srecord )
            {
                key = srecord.key;
                //table = srecord.table;
                value = srecord.value;
                ties = srecord.ties;
                create_time = srecord.create_time;
                update_time = srecord.update_time;
                live_sec = srecord.live_sec;
                ctl = srecord.ctl;
            }
        };


        //////////////////////////////////////////////////////////////////////////
        //
        qword_t GetKeyIndex( _tstring key )
        {
            return Crc64::Encode( key.c_str(), key.size() );
        }

        //===============================================================================//
        // NAME :
        // DESC :
        //===============================================================================//
        Server::Server( const tchar_t* name ) : AppServer( name )
        {
            SET_BIT_FLAG( Paper::g_LogMode, LOGMODE_PRINTOUT );
            //m_pAppServer  = new CPTPAppServer( this );
            //m_pWebServer  = new HttpAppServer( this );
            //m_pPSockPeer  = new PSockPeer;
            //m_fLogMode = 0;
            RegisterCptpFunction( CsC_S_REQUEST_SET_VALUE, ( Cyberspace::cptp_member_func )&Cyberspace::Paper::Server::DoSetValue );
            RegisterCptpFunction( CsC_S_REQUEST_SETNX_VALUE, ( Cyberspace::cptp_member_func )&Cyberspace::Paper::Server::DoSetnxValue );
            RegisterCptpFunction( CsC_S_REQUEST_GET_VALUE, ( Cyberspace::cptp_member_func )&Cyberspace::Paper::Server::DoGetValue );
            RegisterCptpFunction( CsC_S_REQUEST_DEL_VALUE, ( Cyberspace::cptp_member_func )&Cyberspace::Paper::Server::DoDelValue );
        }

        //===============================================================================//
        // NAME :
        // DESC :
        //===============================================================================//
        Server::~Server()
        {
            //ClearAllCsPSockConn();
            UnRegisterCptpFunction( CsC_S_REQUEST_SET_VALUE );
            UnRegisterCptpFunction( CsC_S_REQUEST_SETNX_VALUE );
            UnRegisterCptpFunction( CsC_S_REQUEST_GET_VALUE );
            UnRegisterCptpFunction( CsC_S_REQUEST_DEL_VALUE );

            Shutdown();
            //SAFE_DELETE( m_pAppServer );
            //SAFE_DELETE( m_pWebServer );
            //SAFE_DELETE( m_pPSockPeer );
        }

        //===============================================================================//
        // NAME :
        // DESC :
        //===============================================================================//
        int Server::StartUp( unsigned int allowplayers, _IN ushort_t ctptport, _IN ushort_t httpport )
        {
            srand( ( int )PfTime::GetSTime() );
            return AppServer::StartUp( allowplayers, ctptport, httpport );
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        int Server::Shutdown( void )
        {
            //m_pAppServer->Shutdown();
            //m_pWebServer->Shutdown();
            //m_StartUpSec = 0;
            SAFE_DELETE_ARRAY( m_DBQueue );
            {
                for ( unsigned i = 0; i < m_RecStorage.vecsize(); i++ )
                {
                    TS_Lock lock( &m_RecStorage.lock( i ) );
                    _vmapl_iterator( SRecord* ) it = m_RecStorage.begin( i );

                    while ( !m_RecStorage.miss( i, it ) )
                    {
                        SAFE_DELETE( it->second );
                        it++;
                    }
                    m_RecStorage.clear( i );
                }
                m_RecStorage.destroy();
            }

            /*
            {
                for ( unsigned i = 0; i < m_TabStorage.vecsize(); i++ )
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
            //for ( uint_t i = 0; i < m_Database.size(); i++ )
            //{
            //    OnDeleteDataBase( m_Database[ i ] );
            //}
            //m_Database.clear();
            //Cyberspace::LogSerivce::Close( Cyberspace::Paper::g_LogMode );
            return 0;
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        XDataBase* Server::GetPaperDB( _IN _cs_sessionid_t id )
        {
            if ( ( m_Database.size() == 0 ) || ( id < 0 ) )
            {
                return NULL;
            }
            return m_Database[ id % m_Database.size() ];
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        bool Server::OnLoadConfigure( _IN std::map<_tstring, _tstring> setting )
        {
            int max_storage = 0;
            {
                _cs_string log_msg;

                std::map<_cs_string, _cs_string>::iterator it;
                it = setting.find( _T( "MaxKeyStorage" ) );
                if ( it != setting.end() )
                {
                    log_msg += _T( "||  Max key strage = %d\n" );
                    OutputMessage( Paper::g_LogMode, log_msg.c_str(), max_storage );
                    m_RecStorage.init( max_storage );
                    //m_TabStorage.init( max_storage );
                }
                else
                {
                    OutputMessage( Paper::g_LogMode, _T( "||  Can't find < MaxKeyStorage >\n" ) );
                    return false;
                }
            }

            {
                SET_BIT_FLAG( Cyberspace::DBCfgReader::dbcfg_logmode, LOGMODE_PRINTOUT );
                int max_db = Cyberspace::DBCfgReader::GetMaxConnect( setting, _T( "PaperDB" ) );
                for ( _cs_size_t i = 0; i < m_Database.size(); i++ )
                {
                    OnDeletePaperDB( m_Database[ i ] );
                }
                m_Database.clear();
                for ( int_t i = 0; i < max_db; i++ )
                {
                    m_Database.push_back( OnCreatePaperDB() );
                }

                int max_tb = 0;
                if ( !Cyberspace::DBCfgReader::LoadCfg( setting, _T( "PaperDB" ), max_tb, m_Database ) )
                    return false;
            }

            if ( m_Database.size() > 0 )
            {
                Cyberspace::DBInit::dbCreateCpTable( m_Database[ 0 ] );
                // crate update
                m_dbLock.resize( m_MaxDB );
                m_DBQueue = new std::queue<SRecord*>[ m_MaxDB ];

                for ( int i = 0; i < m_MaxDB; i++ )
                {
                    CallUpdate* cp = new CallUpdate;
                    cp->pServer = this;
                    cp->Idx = i;
                    PfSystem::Thread::BeginProcess( Execute, cp, _T( "PaperServer::LoadConfigure" ) );
                }
            }
            return true;
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        XDataBase* Server::OnCreatePaperDB( void )
        {
            return ( XDataBase* )new XMySqlDB;
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        void Server::OnDeletePaperDB( _IN XDataBase* db )
        {
            XMySqlDB* xdb = ( XMySqlDB* )db;
            SAFE_DELETE( xdb );
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        //void Server::OutputMessage( CsLogMode m, const tchar_t* format, ... )
        //{
        //    //tchar_t string[ _MAX_LONG_MSG ] = { "" };
        //    //FILE str;
        //    //register FILE *outfile = &str;
        //    va_list arglist;
        //    //register int retval;
        //    //const tchar_t* i = format;
        //    va_start( arglist, format );
        //    //assert( string != NULL );
        //    //assert( format != NULL );
        //    //vsprintf( string, format, arglist );
        //    Cyberspace::LogSerivce::OutputMessage( Cyberspace::Paper::g_LogMode, m, format, arglist );
        //    va_end( arglist );
        //    
        //    //Cyberspace::LogSerivce::OutputMessage( Cyberspace::Arbitration::g_LogModeUnit, m, format, arglist );
        //    return;
        //}

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        //void Server::SetLogMode( CsLogMode m )
        //{
        //    m_fLogMode = m;
        //}

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        //CsLogMode Server::GetLogMode( void )
        //{
        //    return m_fLogMode;
        //}

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        void Server::PrintOut()
        {
            {
                for ( unsigned i = 0; i < m_RecStorage.vecsize(); i++ )
                {
                    TS_Lock lock( &m_RecStorage.lock( i ) );
                    _vmapl_iterator( SRecord* ) it = m_RecStorage.begin( i );

                    while ( !m_RecStorage.miss( i, it ) )
                    {
                        printf( "%s : %s\n", it->second->key.c_str(), it->second->value.c_str() );
                        it++;
                    }

                    m_RecStorage.clear( i );
                }

                m_RecStorage.destroy();
            }
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        //XDataBase*  Server::OnCreateCommonDB( void )
        //{
        //    return ( XDataBase* )new XMySqlDB;
        //}

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        //void Server::OnDeleteCommonDB( XDataBase* db )
        //{
        //    XMySqlDB* xdb = ( XMySqlDB* ) db;
        //    SAFE_DELETE( xdb );
        //}

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        int Server::DoSetValue( AppSocket* s, channelid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = ( Server* )this;
            AppSocket*  pclient = s;
            mstimen_t currmsce = PfTime::GetTickMSTime();
            mstimen_t create_msce = currmsce;
            //mstimen_t updaet_msce = currmsce;
            std::vector<CsRecord> records;
            std::vector<int>      live_secs;
            {
                Databuf     recvdata;
                recvdata.buf = ( ubyte_t* )combin;
                recvdata.len = comlen;
                CsPaperCommand::SetValue::Server::GetRequest( &recvdata, records, live_secs );
            }
            std::vector<int>      rets;
            std::vector<SRecord*> db_records;
            {
                for ( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    CsResBuf  old_value;
                    int     ret = PAPERRESULT_UNKNOW;

                    qword_t keyindex = GetKeyIndex( records[ i ].key );
                    TS_Lock lock( &pserver->m_RecStorage.lock( keyindex ) );
                    _vmapl_iterator( SRecord* ) it = pserver->m_RecStorage.find( keyindex );
                    SRecord* vb = NULL;

                    if ( !pserver->m_RecStorage.miss( keyindex, it ) )
                    {
                        old_value = it->second->value;
                        vb = it->second;
                        create_msce = vb->create_time;
                        ret = PAPERRESULT_EXIST_SUCCESS;
                    }
                    else
                    {
                        vb = new SRecord;
                        pserver->m_RecStorage.insert( keyindex, vb );
                        vb->create_time = currmsce;
                        ret = PAPERRESULT_SUCCESS;
                    }

                    ( *vb ) = records[ i ];
                    vb->update_time = currmsce;
                    vb->live_sec = live_secs[ i ];
                    vb->ctl = 1;
#ifdef _DEBUG
                    tchar_t debugstring[ _MAX_MSG ];
                    _stprintf( debugstring, _T( "set value : key = [ %s ], new value = [ %s ], old value = [ %s ], live_sec =  [ %d ]\n" ),
                               records[ i ].key.c_str(), records[ i ].value.c_str(), old_value.c_str(), live_secs[ i ] );
                    Pf_DEBUG( debugstring );
#endif
                    // put old value to return record
                    {
                        db_records.push_back( vb );
                        records[ i ].value = old_value;
                        rets.push_back( ret );
                    }
                }
            }
            // send feedback
            {
                CsResBuf retbuf;
                CsPaperCommand::SetValue::Server::SetFeedback( retbuf, records, rets );
                pserver->SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_SET_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }

            // put into db query list
            for ( int i = 0; i < db_records.size(); i++ )
            {
                SRecord* vb = new SRecord;
                ( *vb ) = ( *db_records[ i ] );
                vb->ctl = 1;
                int idx = rand() % pserver->m_MaxDB;
                TS_Lock lock( &pserver->m_dbLock[ idx ] );
                pserver->m_DBQueue[ idx ].push( vb );
            }

            return 1;
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        int Server::DoSetnxValue( AppSocket* s, channelid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = ( Server* )this;
            AppSocket*  pclient = s;
            mstimen_t currmsce = PfTime::GetTickMSTime();
            std::vector<CsRecord> records;
            std::vector<int>      live_secs;
            {
                Databuf     recvdata;
                recvdata.buf = ( ubyte_t* )combin;
                recvdata.len = comlen;
                CsPaperCommand::SetValue::Server::GetRequest( &recvdata, records, live_secs );
            }
            std::vector<int>      rets;
            std::vector<SRecord*> db_records;
            {
                for ( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    CsResBuf  old_value;
                    int     ret = PAPERRESULT_UNKNOW;

                    qword_t keyindex = GetKeyIndex( records[ i ].key );
                    TS_Lock lock( &pserver->m_RecStorage.lock( keyindex ) );
                    _vmapl_iterator( SRecord* ) it = pserver->m_RecStorage.find( keyindex );
                    SRecord* vb = NULL;

                    if ( pserver->m_RecStorage.miss( keyindex, it ) )
                    {
                        SRecord* vb = new SRecord;
                        pserver->m_RecStorage.insert( keyindex, vb );
                        ( *vb ) = records[ i ];
                        vb->create_time = currmsce;
                        vb->update_time = currmsce;
                        vb->live_sec = live_secs[ i ];
                        vb->ctl = 2;
                        ret = PAPERRESULT_SUCCESS;
                    }
                    else
                    {
                        old_value = it->second->value;
                        ret = PAPERRESULT_EXIST_FAILURE;
                    }

#ifdef _DEBUG
                    tchar_t debugstring[ _MAX_MSG ];
                    _stprintf( debugstring, _T( "set value : key = [ %s ], new value = [ %s ], old value = [ %s ], live_sec =  [ %d ]\n" ),
                               records[ i ].key.c_str(), records[ i ].value.c_str(), old_value.c_str(), live_secs[ i ] );
                    Pf_DEBUG( debugstring );
#endif
                    // put old value to return record
                    {
                        if ( vb != NULL )
                        {
                            db_records.push_back( vb );
                        }

                        records[ i ].value = old_value;
                        rets.push_back( ret );
                    }
                }
            }
            // send feedback
            {
                CsResBuf retbuf;
                CsPaperCommand::SetValue::Server::SetFeedback( retbuf, records, rets );
                pserver->SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_SETNX_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }

            // put into db query list
            for ( int i = 0; i < db_records.size(); i++ )
            {
                SRecord* vb = new SRecord;
                ( *vb ) = ( *db_records[ i ] );
                vb->ctl = 2;
                int idx = rand() % pserver->m_MaxDB;
                TS_Lock lock( &pserver->m_dbLock[ idx ] );
                pserver->m_DBQueue[ idx ].push( vb );
            }

            return 1;
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        int Server::DoGetValue( AppSocket* s, channelid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = ( Server* )this;
            AppSocket*  pclient = s;
            mstimen_t currmsce = PfTime::GetTickMSTime();
            _cs_string_list keys;
            {
                Databuf     recvdata;
                recvdata.buf = ( ubyte_t* )combin;
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
                    record.key = ( *it_keys );
                    int    ret = PAPERRESULT_UNKNOW;
                    {
                        qword_t keyindex = GetKeyIndex( record.key );
                        TS_Lock lock( &pserver->m_RecStorage.lock( keyindex ) );
                        _vmapl_iterator( SRecord* ) it_srecord = pserver->m_RecStorage.find( keyindex );

                        if ( !pserver->m_RecStorage.miss( keyindex, it_srecord ) )
                        {
                            SRecord* vb = it_srecord->second;

                            // 過期了?
                            if ( ( ( vb->create_time + vb->live_sec ) <= currmsce ) && ( vb->live_sec != 0 ) )
                            {
                                // 過期了
                                pserver->m_RecStorage.erase( it_srecord );
                                SAFE_DELETE( vb );
                                ret = PAPERRESULT_OVERDUE;
                            }
                            else
                            {
                                record.value = vb->value;
                                ret = PAPERRESULT_SUCCESS;
                            }
                        }
                        else
                        {
                            ret = PAPERRESULT_NOT_EXIST;
                        }
                    }

                    // find in database
                    if ( ret == PAPERRESULT_NOT_EXIST )
                    {
                        // put into db query list
                        _cs_string val;
                        int create_time = 0;
                        int update_time = 0;
                        int live_sec = 0;
                        int idx = rand() % pserver->m_MaxDB;
                        //int tbidx = Crc32::Encode( record.key.c_str(), record.key.size() ) % g_MaxTb;
                        DBResBuf dbvalue;

                        if ( DBPaper::dbGetRecord( pserver->GetPaperDB( idx ), record.key.c_str(), dbvalue, create_time, update_time, live_sec ) != -1 )
                        {
                            // 過期了?
                            if ( ( ( create_time + live_sec ) <= currmsce ) && ( live_sec != 0 ) )
                            {
                                ret = PAPERRESULT_OVERDUE;
                            }
                            else
                            {
                                SRecord* vb = new SRecord;
                                vb->key = record.key;
                                vb->value.copy( _GetDBResBufPtr( dbvalue ), _GetDBResBufLen( dbvalue ) );
                                vb->create_time = create_time;
                                vb->update_time = update_time;
                                vb->live_sec = live_sec;
                                vb->ctl = 3;
                                pserver->m_RecStorage.insert( GetKeyIndex( vb->key ), vb );
                                record.value = vb->value;
                                ret = PAPERRESULT_SUCCESS;
                            }
                        }
                    }

#ifdef _DEBUG
                    tchar_t debugstring[ _MAX_MSG ];
                    _stprintf( debugstring, _T( "get value : key = [ %s ], value = [ %s ], ret = [ %d ]\n" ), record.key.c_str(), record.value.c_str(), ret );
                    Pf_DEBUG( debugstring );
#endif
                    {
                        records.push_back( record );
                        rets.push_back( ret );
                    }
                }
            }
            {
                CsResBuf retbuf;
                CsPaperCommand::GetValue::Server::SetFeedback( retbuf, records, rets );
                pserver->SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_GET_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }
            return 1;
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        int Server::DoDelValue( AppSocket* s, channelid_t channelid, const ubyte_t* combin, unsigned int comlen )
        {
            Server*     pserver = ( Server* )this;
            AppSocket*  pclient = s;
            _cs_string_list  keys;
            {
                Databuf     recvdata;
                recvdata.buf = ( ubyte_t* )combin;
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
                    record.key = ( *it_keys );

                    qword_t keyindex = GetKeyIndex( record.key );
                    TS_Lock lock( &pserver->m_RecStorage.lock( keyindex ) );
                    _vmapl_iterator( SRecord* ) it = pserver->m_RecStorage.find( keyindex );

                    if ( !pserver->m_RecStorage.miss( keyindex, it ) )
                    {
                        SRecord* vb = it->second;
                        SAFE_DELETE( vb );
                        pserver->m_RecStorage.erase( keyindex );
                        ret = PAPERRESULT_SUCCESS;
                    }
                    else
                    {
                        ret = PAPERRESULT_NOT_EXIST_SUCCESS;
                    }

#ifdef _DEBUG
                    tchar_t debugstring[ _MAX_MSG ];
                    _stprintf( debugstring, _T( "del value : key = [ %s ], value = [ %s ], ret = [ %d ]\n" ), record.key.c_str(), record.value.c_str(), ret );
                    Pf_DEBUG( debugstring );
#endif
                    {
                        dbkeys.push_back( record.key );
                        rets.push_back( ret );
                    }
                }
            }
            {
                CsResBuf retbuf;
                CsPaperCommand::DelValue::Server::SetFeedback( retbuf, rets );
                pserver->SendMessage( pclient, CsINVALID_ID, CsS_C_FEEDBACK_DEL_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
            }

            // put into db query list
            for ( _cs_string_list::iterator it_keys = dbkeys.begin(); it_keys != dbkeys.end(); it_keys++ )
            {
                SRecord* vb = new SRecord;
                vb->key = ( *it_keys );
                vb->create_time = 0;
                vb->update_time = 0;
                vb->live_sec = 0;
                vb->ctl = 4;
                int idx = rand() % pserver->m_MaxDB;
                TS_Lock lock( &pserver->m_dbLock[ idx ] );
                pserver->m_DBQueue[ idx ].push( vb );
            }

            return 1;
        }

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        void __cdecl Server::Execute( void* data )
        {
            CallUpdate* cp = ( CallUpdate* )data;
            Server* pserver = cp->pServer;
            int     cindex = cp->Idx;
            XDataBase* pdb = pserver->GetPaperDB( cindex );

            while ( pserver->IsAlive() )
            {
                SRecord* vb = NULL;
                {
                    TS_Lock lock( &pserver->m_dbLock[ cindex ] );

                    if ( !pserver->m_DBQueue[ cindex ].empty() )
                    {
                        vb = pserver->m_DBQueue[ cindex ].front();
                        pserver->m_DBQueue[ cindex ].pop();
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
                        //int tbidx = Crc32::Encode( vb->key.c_str(), vb->key.size() ) % g_MaxTb;
                        //DBPaper::dbSetRecord( pdb, tbidx, vb->key.c_str(), vb->value.c_str(), vb->update_time, vb->live_sec );
                        DBPaper::dbSetRecord( pdb, vb->key.c_str(), vb->value.c_str(), vb->update_time, vb->live_sec );
                    }
                    break;

                case 2:
                    {
                        //int tbidx = Crc32::Encode( vb->key.c_str(), vb->key.size() ) % g_MaxTb;
                        //DBPaper::dbSetnxRecord( pdb, tbidx, vb->key.c_str(), vb->value.c_str(), vb->create_time, vb->live_sec );
                        DBPaper::dbSetnxRecord( pdb, vb->key.c_str(), vb->value.c_str(), vb->create_time, vb->live_sec );
                    }
                    break;

                case 4:
                    {
                        //int tbidx = Crc32::Encode( vb->key.c_str(), vb->key.size() ) % g_MaxTb;
                        //DBPaper::dbDelRecord( pdb, tbidx, vb->key.c_str() );
                        DBPaper::dbDelRecord( pdb, vb->key.c_str() );
                    }
                    break;
                }

                SAFE_DELETE( vb );
            _EXECUTE_EMPTY:
                PfStd::MsSleep( 100 );
            }

            //pserver->OnDelete( pdb );
        }

    }
}
;

