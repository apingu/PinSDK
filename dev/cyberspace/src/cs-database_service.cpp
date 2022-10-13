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
#ifdef _WINDOWS
#include <tchar.h>
#endif
#include <stdarg.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-thread.h>
#include <gadget/sentence_pattern.h>
#include <database/xdb-helper.h>
//#include <database/xdb-mysqldb.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-database_service.h"
#include "cs-db_function_base.h"
#include "cs-uid_generator.h"
#include "cs-db_cfg_reader.h"


namespace cyberspace
{
    namespace
    {
        struct LogData
        {
            cs_session_uid_t    session_uid;
            int                 classify;
            pftml::Time         logtime;
            int                 level;
            _cs_string          event;
            _cs_string          data;
            _cs_string          content;
        };

        const int               _MAX_QUEUE = 10;
        T_Lock                  logdata_lock[ _MAX_QUEUE ];
        std::vector<LogData*>   logdata_queue[ _MAX_QUEUE ];
        T_Lock                  quene_lock;
        int                     quene_index = 0;

        const uint_t            max_identity_div = 100;
        T_Lock                  service_identity_lock;
        uint_t                  max_service_identity = 0;  // 分配給買一個  service 一個獨特的iid


        //const uint_t            max_db_pool_div = 100;
        //T_Lock                  max_db_pool_lock[ max_db_pool_div ];
        //// identity.0,   
        //std::map< _cs_string, cs_db_pool_index_t >   database_pool_seq_[ max_identity_div ][ CsDBType_MAX ]; // pool 在該 類型的順序 
        //
        //cs_db_pool_index_t GetDBPoolPos( _IN uint_t service_identity, _IN CsDBType dbc_type, _IN int seq )
        //{
        //    uint_t pool_div = service_identity % max_db_pool_div;
        //    TS_Lock utlock( &max_db_pool_lock[ pool_div ] );
        //    _cs_string db_poo_seq;
        //    pfstd::strprintf( db_poo_seq, "%u.%u", service_identity, seq );
        //    return database_pool_seq_[ pool_div ][ dbc_type ][ db_poo_seq ];
        //}
    }

    //===============================================================================
    //
    //
    //===============================================================================
    DatabaseService::DatabaseService()//: UnitServer( name )
    {
        is_alive_ = false;
        {
            TS_Lock lock( &service_identity_lock );
            service_identity_ = max_service_identity;
            max_service_identity++;
        }
    }

    //===============================================================================
    //
    //
    //===============================================================================
    DatabaseService::~DatabaseService()
    {
    }

    //===============================================================================
    //
    //
    //===============================================================================
    int DatabaseService::Shutdown()
    {
        is_alive_ = false;
        pfkrl::Thread::WaitAllExit( 30000, ( "DatabaseService::DoUpdate" ) );


        //int ret = UnitServer::Shutdown();
        for( uint_t dbt = 0; dbt < CsDBType_MAX; dbt++ )
        {
            int seq = 0;
            for( std::map< cs_db_pool_index_t, XDBPool >::iterator dbit = database_pool_[ dbt ].begin(); dbit != database_pool_[ dbt ].end(); dbit++ )
            {
                for( uint_t i = 0; i < dbit->second.size(); i++ )
                {
                    xdb::helper::DBObjectPool::DestoryDB( dbit->second[ i ] );
                }
                dbit->second.clear();


                //{
                //    uint_t pool_div = service_identity_ % max_db_pool_div;
                //    TS_Lock utlock( &max_db_pool_lock[ pool_div ] );
                //    _cs_string db_poo_seq;
                //    pfstd::strprintf( db_poo_seq, "%u.%u", service_identity_, seq );
                //    database_pool_seq_[ pool_div ]->erase( db_poo_seq );
                //    seq++;
                //}


            }
            database_pool_[ dbt ].clear();
            database_pool_seq_[ dbt ].clear();
        }

        return 1;//UnitServer::Shutdown();
    }

    //===============================================================================
    //
    //
    //===============================================================================
    //XDatabase* DatabaseService::OnCreateDB( CsDBType db_type )
    //{
    //    if ( db_type >= CsDBType_MAX )
    //    {
    //        return NULL;
    //    }
    //    //xdb::MySqlDB* db = new xdb::MySqlDB;
    //    return xdb::helper::DBObjectPool::CreateDB( xdb::helper::XDB_Type::XDB_MYSQL );
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    //void DatabaseService::OnDeleteDB( XDatabase* db )
    //{
    //    //xdb::MySqlDB* xdb = ( xdb::MySqlDB* )db;
    //    //SAFE_DELETE( xdb );
    //    xdb::helper::DBObjectPool::DestoryDB( db );
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    //XDatabase* DatabaseService::OnCreateDatalogDB( void )
    //{
    //    return ( XDatabase* )new xdb::MySqlDB;
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    //void DatabaseService::OnDeleteDatalogDB( _IN XDatabase* db )
    //{
    //    xdb::MySqlDB* xdb = ( xdb::MySqlDB* )db;
    //    SAFE_DELETE( xdb );
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    XDatabase* DatabaseService::GetDB( _IN CsDBType db_type, _IN uniqueid_t db_index )
    {
        return GetDatabase( db_type, "", db_index );
        //if( db_type >= CsDBType_MAX )
        //{
        //    return NULL;
        //}
        //if( ( database_pool_[ db_type ].size() == 0 ) || ( db_index < 0 ) )
        //{
        //    return NULL;
        //}
        //
        //uint_t db_index = db_index % database_pool_[ db_type ].size();
        //std::map< cs_db_pool_index_t, XDBPool >::iterator db_it = database_pool_[ db_type ].find( pfstd::uitostr( db_index ) );
        //if ( db_it == database_pool_[ db_type ].end() )
        //{
        //    return NULL;
        //}
        //
        ////std::map< cs_db_pool_index_t, XDBPool >::iterator db_it = database_pool_[ db_type ].begin();
        //
        //XDatabase* xdb = db_it->second[ db_index % db_it->second.size() ];
        ////return database_[ 0 ][ session_uid % database_[ 0 ].size() ];
        //return xdb;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    //size32_t DatabaseService::GetDBSize( _IN CsDBType db_type, _IN cs_session_uid_t session_uid )
    //{
    //    if ( db_type >= CsDBType_MAX )
    //    {
    //        return 0;
    //    }
    //
    //    uint_t db_index = session_uid % database_pool_[ db_type ].size();
    //    std::map< uint_t, XDBPool >::iterator db_it = database_pool_[ db_type ].find( db_index );
    //    if ( db_it == database_pool_[ db_type ].end() )
    //    {
    //        return 0;
    //    }
    //
    //    return (size32_t)db_it->second.size();
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    XDatabase* DatabaseService::GetDatabase( _IN CsDBType db_type, _IN cs_db_pool_index_t db_pool_idx, _IN cs_db_obj_index_t db_obj_idx )
    {
        if( db_type >= CsDBType_MAX )
        {
            return NULL;
        }

        if( database_pool_[ db_type ].size() == 0 )
        {
            return NULL;
        }

        std::map< cs_db_pool_index_t, XDBPool >::iterator db_it;
        if( db_pool_idx == "" )
        {
            db_it = database_pool_[ db_type ].find( database_pool_seq_[ db_type ][ db_obj_idx % database_pool_[ db_type ].size() ] );
        }
        else if( db_pool_idx == "@first" )
        {
            db_it = database_pool_[ db_type ].begin();
        }
        else
        {
            db_it = database_pool_[ db_type ].find( db_pool_idx );
            if( db_it == database_pool_[ db_type ].end() )
            {
                return NULL;
            }
        }

        //cs_db_pool_index_t db_index = db_pool_idx % database_pool_[ db_type ].size();
        if( db_it->second.empty() )
        {
            return NULL;
        }

        if( db_obj_idx < 0 )
        {
            db_obj_idx = rand() % database_pool_[ db_type ].size();
        }

        XDatabase* xdb = db_it->second[ db_obj_idx % db_it->second.size() ];
        //return database_[ 0 ][ session_uid % database_[ 0 ].size() ];
        return xdb;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    //XDatabase* DatabaseService::GetFirstDatabase( _IN CsDBType db_type, _IN cs_db_obj_index_t db_obj_idx )
    //{
    //    if( db_type >= CsDBType_MAX )
    //    {
    //        return NULL;
    //    }
    //
    //    if( database_pool_[ db_type ].size() == 0 )
    //    {
    //        return NULL;
    //    }
    //    std::map< cs_db_pool_index_t, XDBPool >::iterator db_it = database_pool_[ db_type ].begin();
    //
    //    //cs_db_pool_index_t db_index = db_pool_idx % database_pool_[ db_type ].size();
    //    if( db_it->second.empty() )
    //    {
    //        return NULL;
    //    }
    //
    //    if( db_obj_idx < 0 )
    //    {
    //        db_obj_idx = rand() % database_pool_[ db_type ].size();
    //    }
    //
    //    XDatabase* xdb = db_it->second[ db_obj_idx % db_it->second.size() ];
    //    //return database_[ 0 ][ session_uid % database_[ 0 ].size() ];
    //    return xdb;
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    bool DatabaseService::OnLoadConfigure( std::map<_cs_string, _cs_string> setting )
    {
        SET_BIT_FLAG( cyberspace::DBCfgReader::g_dbcfg_logmode, LOGMODE_PRINTOUT );

        // log
        //{
        //    int max_db = cyberspace::DBCfgReader::GetMaxConnect( setting, ( "DatalogDB" ) );
        //    for( _cs_size_t i = 0; i < database_[ 2 ].size(); i++ )
        //    {
        //        OnDeleteDatalogDB( database_[ 2 ][ i ] );
        //    }
        //    database_[ 2 ].clear();
        //    for( int_t i = 0; i < max_db; i++ )
        //    {
        //        database_[ 2 ].push_back( OnCreateDatalogDB() );
        //    }
        //
        //    int max_tb = 0;
        //    if( !cyberspace::DBCfgReader::LoadCfg( setting, ( "DatalogDB" ), max_tb, database_[ 2 ] ) )
        //        return false;
        //
        //    // 為了避免會找不到 所以 不給再設定檔中調整
        //    //cyberspace::MAX_CsDatalogDBTable = max_tb;
        //}

        // Extern
        for( int dbci = 0; dbci < 3; dbci++ )
        {
            _cs_string dbc_name;
            CsDBType  dbc_type;
            if( dbci == 0 )
            {
                dbc_name = ( "ExternDB>>" );
                dbc_type = CsDBType_Extern;
            }
            if( dbci == 1 )
            {
                dbc_name = ( "ExpandDB>>" );
                dbc_type = CsDBType_Expand;
            }
            if( dbci == 2 )
            {
                dbc_name = ( "CommonDB>>" );
                dbc_type = CsDBType_Common;
            }

            {
                // 清除
                for( std::map< cs_db_pool_index_t, XDBPool >::iterator it = database_pool_[ dbc_type ].begin(); it != database_pool_[ dbc_type ].end(); it++ )
                {
                    for( _cs_size_t i = 0; i < it->second.size(); i++ )
                    {
                        it->second[ i ]->Disconnect();
                        xdb::helper::DBObjectPool::DestoryDB( it->second[ i ] );
                    }
                    database_pool_[ dbc_type ].clear();
                }

                // 取得有哪些名子
                std::set< _cs_string > db_node;
                for( std::map<_cs_string, _cs_string>::iterator it = setting.begin(); it != setting.end(); it++ )
                {
                    int out_size = it->first.size() + 1;
                    _cs_char_t* out_buff = new _cs_char_t[ out_size ];
                    if( strbtwnstr( out_buff, out_size, it->first.c_str(), dbc_name.c_str(), ( ">>" ) ) )
                    {
                        db_node.insert( std::set< _cs_string >::value_type( dbc_name + _cs_string( out_buff ) ) );
                    }
                    SAFE_DELETE_ARRAY( out_buff );
                }

                for( std::set< _cs_string >::iterator it = db_node.begin(); it != db_node.end(); it++ )
                {
                    // 找出 資料庫名子
                    cs_db_pool_index_t db_pool_idx;
                    {
                        int out_size = ( *it ).size() + 1;
                        _cs_char_t* out_buff = new _cs_char_t[ out_size ];
                        if( strbtwnstr( out_buff, out_size, ( *it ).c_str(), ( "index=\"" ), ( "\"" ) ) )
                        {
                            db_pool_idx = out_buff;
                        }
                        SAFE_DELETE_ARRAY( out_buff );
                    }

                    xdb::helper::XDB_Type xdb_type = xdb::helper::XDB_Type::XDB_MYSQL;
                    {
                        int out_size = ( *it ).size() + 1;
                        _cs_char_t* out_buff = new _cs_char_t[ out_size ];
                        if( strbtwnstr( out_buff, out_size, ( *it ).c_str(), ( "type=\"" ), ( "\"" ) ) )
                        {
                            if( strcmp( out_buff, "mysql" ) == 0 )
                            {
                                xdb_type = xdb::helper::XDB_Type::XDB_MYSQL;
                            }
                            // 其他待支援
                            else
                            {
                                xdb_type = xdb::helper::XDB_Type::XDB_MYSQL;
                            }
                        }
                        SAFE_DELETE_ARRAY( out_buff );
                    }

                    // 建立資料庫
                    XDBPool xdb_pool;
                    {
                        int max_db = cyberspace::DBCfgReader::GetMaxConnect( setting, ( *it ).c_str() );
                        //int max_db = cyberspace::DBCfgReader::GetDatabaseName( setting, *it );
                        for( int_t i = 0; i < max_db; i++ )
                        {
                            xdb_pool.push_back( xdb::helper::DBObjectPool::CreateDB( xdb_type ) );
                        }
                    }

                    // 紀錄資料庫的順序 給底成 不管 資料庫名稱使用
                    //_database_pool_seq.insert( std::map< cs_db_pool_index_t, int >::value_type( db_pool_idx, database_pool_[ dbc_type ].size() ) );
                    database_pool_seq_[ dbc_type ][ database_pool_[ dbc_type ].size() ] = db_pool_idx;
                    database_pool_[ dbc_type ].insert( std::map< cs_db_pool_index_t, XDBPool >::value_type( db_pool_idx, xdb_pool ) );

                    int max_tb = 0;
                    if( !cyberspace::DBCfgReader::LoadCfg( setting, ( *it ).c_str(), max_tb, xdb_pool ) )
                    {
                        return false;
                    }
                }
            }
            // 為了避免會找不到 所以 不給再設定檔中調整
            //cyberspace::kMOD_CsExpondDBTable = max_tb;
        }

        // unique session_uid, member        
        CreateCommonDBTable();

        if( !is_alive_ )
        {
            is_alive_ = true;
            quene_index = 0;
            //logdata_queue.resize( _MAX_QUEUE );
            for( int i = 0; i < _MAX_QUEUE; i++ )
            {
                pfkrl::Thread::BeginProcess( DoUpdate, this, ( "DatabaseService::DoUpdate" ) );
            }
        }

        return true;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    cs_session_uid_t DatabaseService::CreateMemberSessionUID( const _cs_char_t* username )
    {
        cs_session_uid_t session_uid = cyberspace::uid_generator::GetSessionUID( username );

        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return CsINVALID_SESSIONID;
        }

        // 建立前再去創 table
        if( !cs_db_function::member::dbCreateTable( xdb, session_uid ) )
        {
            return CsINVALID_SESSIONID;
        }

        // 判斷有沒有存在
        if( cs_db_function::member::dbGetSessionUID( xdb, username ) != CsINVALID_ID )
        {
            return CsINVALID_SESSIONID;
        }

        if( !cs_db_function::member::dbSetSessionUID( xdb, session_uid, username ) )
        {
            return CsINVALID_SESSIONID;
        }
        return session_uid;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    bool DatabaseService::SetMemberPassword( _IN cs_session_uid_t session_uid, const _cs_char_t* password )
    {
        //cs_session_uid_t cid = session_uid;

        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return false;
        }

        if( !cs_db_function::member::dbSetPassword( xdb, session_uid, password ) )
        {
            return false;
        }
        return true;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    bool DatabaseService::SetMemberGroupUID( cs_session_uid_t session_uid, cs_group_uid_t groupd_uid )
    {
        //cs_session_uid_t cid = session_uid;
        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return false;
        }

        if( !cs_db_function::member::dbSetGroupUID( xdb, session_uid, groupd_uid.c_str() ) )
        {
            return false;
        }
        return true;
    }

    //===============================================================================
   //
   //
   //===============================================================================
    cs_session_uid_t DatabaseService::CheckMemberSessionUID( const _cs_char_t* username, const _cs_char_t* password )
    {
        cs_session_uid_t session_uid = cyberspace::uid_generator::GetSessionUID( username );

        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return CsINVALID_SESSIONID;
        }

        std::string user_string = xdb->OnEscapeString( username, strlen( username ) );
        std::string pawd_string = xdb->OnEscapeString( password, strlen( password ) );

        if( !cs_db_function::member::dbCheckMember( xdb, user_string.c_str(), pawd_string.c_str() ) )
        {
            return CsINVALID_SESSIONID;
        }
        return session_uid;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    cs_session_uid_t DatabaseService::GetMemberSessionUID( const _cs_char_t* username )
    {
        /*
        {
        TS_Lock lock(&v_coreserver_->m_lCIdMemberNameCache);
        std::map<_cs_string, cs_session_uid_t>::iterator it=v_coreserver_->m_tCIdMemberNameCache.find(name);
        if(it!=v_coreserver_->m_tCIdMemberNameCache.end())
        return it->second;
        }
        */
        //if ( v_coreserver_->m_bDBMember )
        {
            cs_session_uid_t session_uid = cyberspace::uid_generator::GetSessionUID( username );
            //cs_session_uid_t cid = cs_db_function::member::dbGetSessionUID( GetCommonDB( 0 ), name );
            /*
            {
            TS_Lock lock(&v_coreserver_->m_lCIdMemberNameCache);
            v_coreserver_->m_tCIdMemberNameCache.insert(std::map<_cs_string, cs_session_uid_t>::value_type(name, cid));
            }
            */
            return session_uid;
        }
        return CsINVALID_SESSIONID;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    _cs_string DatabaseService::GetMemberName( cs_session_uid_t session_uid )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return "";
        }

        //if ( v_coreserver_->m_bDBMember )
        {
            cs_db_function::member::Data data;
            cs_db_function::member::dbGetData( data, xdb, session_uid );
            return data[ cs_db_function::member::_member_data_username ];
        }
        return  "";
    }

    //===============================================================================
   //
   //
   //===============================================================================
    CseMEMBER_STATE DatabaseService::GetMemberState( cs_session_uid_t session_uid )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return ACCSTATE_MAX;
        }

        //if ( v_coreserver_->m_bDBMember )
        {
            cs_db_function::member::Data data;
            cs_db_function::member::dbGetData( data, xdb, session_uid );
            return ( CseMEMBER_STATE )pfstd::strtoi( data[ cs_db_function::member::_member_data_state ] );
        }
        return ACCSTATE_MAX;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    void DatabaseService::SetMemberState( cs_session_uid_t session_uid, CseMEMBER_STATE state )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return;
        }

        //if ( v_coreserver_->m_bDBMember )
        {
            cs_db_function::member::dbSetState( xdb, session_uid, state );
        }
        return;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    bool DatabaseService::GetMemberPermission( cs_flag_t& flag, cs_group_uid_t& group_uid,
                                               cs_permissionll_t& pri_permissionl, cs_permissionll_t& pub_permissionl,
                                               cs_permissionss_t& pri_permissions, cs_permissionss_t& pub_permissions,
                                               cs_session_uid_t session_uid )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return false;
        }

        cs_db_function::member::Data data;
        if( !cs_db_function::member::dbGetData( data, xdb, session_uid ) )
        {
            flag = -1;
            group_uid = "";
            pri_permissionl = -1;
            pub_permissionl = -1;
            pri_permissions = "";
            pub_permissions = "";
            return false;
        }
        flag = pfstd::strtoi( data[ cs_db_function::member::_member_data_flag ] );
        group_uid = data[ cs_db_function::member::_member_data_group_uid ];
        pri_permissionl = pfstd::strtoll( data[ cs_db_function::member::_member_data_pri_permissionl ] );
        pub_permissionl = pfstd::strtoll( data[ cs_db_function::member::_member_data_pub_permissionl ] );
        pri_permissions = data[ cs_db_function::member::_member_data_pri_permissions ];
        pub_permissions = data[ cs_db_function::member::_member_data_pub_permissions ];
        return true;
    }

    //===============================================================================
    //
    // 使用string map 儲存的權限值
    //===============================================================================
    bool DatabaseService::SetMemberPermission( cs_flag_t flag, cs_group_uid_t group_uid,
                                               cs_permissionll_t pri_permissionl, cs_permissionll_t pub_permissionl,
                                               cs_permissionss_t pri_permissions, cs_permissionss_t pub_permissions,
                                               cs_session_uid_t session_uid )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return false;
        }

        cs_db_function::member::Data data;

        data[ cs_db_function::member::_member_data_flag ] = pfstd::itostr( flag );
        data[ cs_db_function::member::_member_data_group_uid ] = group_uid;
        data[ cs_db_function::member::_member_data_pri_permissionl ] = pfstd::lltostr( pri_permissionl );
        data[ cs_db_function::member::_member_data_pub_permissionl ] = pfstd::lltostr( pub_permissionl );
        data[ cs_db_function::member::_member_data_pri_permissions ] = pri_permissions;
        data[ cs_db_function::member::_member_data_pub_permissions ] = pub_permissions;

        if( !cs_db_function::member::dbSetPermission( xdb, session_uid, data ) )
        {
            return false;
        }
        return false;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    longlong_t DatabaseService::GetMemberAccountBalance( cs_session_uid_t session_uid )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Common, "", session_uid );
        if( xdb == NULL )
        {
            return 0;
        }

        //if ( v_coreserver_->m_bDBMember )
        {
            cs_db_function::member::Data data;
            cs_db_function::member::dbGetData( data, xdb, session_uid );
            return pfstd::strtoll( data[ cs_db_function::member::_member_data_acc_quota ] ) + pfstd::strtoll( data[ cs_db_function::member::_member_data_acc_point00 ] );
        }
        return 0;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    //cs_baccount_t DatabaseService::GetMemberAccount( _IN cs_session_uid_t session_uid )
    //{
    //    return cs_baccount_t();
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    //void DatabaseService::SetMemberAccount( cs_session_uid_t session_uid, cs_baccount_t account )
    //{
    //    //if ( v_coreserver_->m_bDBMember )
    //    {
    //        //cs_db_function::member::dbSetPoint( GetDatabase( CsDBType_Common, session_uid ), session_uid, point );
    //    }
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    //longlong_t DatabaseService::AddMemberAccount( cs_session_uid_t session_uid, cs_baccount_t account )
    //{
    //    //if ( v_coreserver_->m_bDBMember )
    //    {
    //        //return cs_db_function::member::dbAddPoint( GetDatabase( CsDBType_Common, session_uid ), session_uid, point );
    //    }
    //    return 0;
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    void DatabaseService::SetLogMessage( int classify, cs_session_uid_t session_uid, _IN int level, const _cs_char_t* event, const _cs_char_t* data, const _cs_char_t* content )
    {
        ulonglong_t tbidx = session_uid % _MAX_QUEUE;
        LogData* combin = new LogData;
        combin->session_uid = session_uid;
        combin->classify = classify;
        combin->event = event;
        combin->level = level;
        combin->data = data;
        combin->content = content;
        {
            TS_Lock lock( &logdata_lock[ tbidx ] );
            logdata_queue[ tbidx ].push_back( combin );
        }
    }

    //===============================================================================
    //
    // 某一個區段時間內
    //===============================================================================
    size32_t DatabaseService::GetLogCount( int classify, cs_session_uid_t session_uid, const pftml::Time& bdate, const pftml::Time& edate )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Extern, "", session_uid );
        return cs_db_function::log::dbGetCount( xdb, classify, session_uid, bdate, bdate );
    }

    //===============================================================================
    //
    // 某一個區段時間內  開始的資料 取的筆數
    // log_uid, create_time, insert_time, member_uid, event, event, data, content
    //===============================================================================
    bool DatabaseService::GetLogMessage( _cs_string_tableii& result_table, int classify, cs_session_uid_t session_uid, const pftml::Time& bdate, const pftml::Time& edate, int limit_offset, int limit_rowcount )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Extern, "", session_uid );
        return cs_db_function::log::dbGetRecord( result_table, xdb, classify, session_uid, bdate, bdate, limit_offset, limit_rowcount );
    }

    // 留言表  用 留言的類型定位
    //===============================================================================
    //
    //
    //===============================================================================
    void DatabaseService::PostBulletinMessage( int classify, uniqueid_t bulletin_gid, cs_session_uid_t author_uid, const _cs_char_t* title, const _cs_char_t* content )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Extern, "", bulletin_gid );
        pftml::Time today;
        cs_db_function::bulletin::dbSetRecord( xdb, classify, today, bulletin_gid, author_uid, 0, title, content );
        return;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    void DatabaseService::ReplyBulletinMessage( int classify, uniqueid_t bulletin_gid, cs_session_uid_t author_uid, uint64_t reply_bulletin_uid, const _cs_char_t* title, const _cs_char_t* content )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Extern, "", bulletin_gid );
        pftml::Time today;
        cs_db_function::bulletin::dbSetRecord( xdb, classify, today, bulletin_gid, author_uid, reply_bulletin_uid, title, content );
        return;
    }

    //===============================================================================
    //
    //
    //===============================================================================
    size32_t DatabaseService::GetBulletinCount( int classify, uniqueid_t bulletin_gid )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Extern, "", bulletin_gid );
        return cs_db_function::bulletin::dbGetCount( xdb, classify, bulletin_gid );
    }

    //===============================================================================
    //
    //
    //===============================================================================
    bool DatabaseService::GetBulletinMessage( _cs_string_tableii& result_table, int classify, uniqueid_t bulletin_gid, int limit_offset, int limit_rowcount )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Extern, "", bulletin_gid );
        return cs_db_function::bulletin::dbGetRecord( result_table, xdb, classify, bulletin_gid, limit_offset, limit_rowcount );
    }

    //===============================================================================
    //
    //
    //===============================================================================
    bool DatabaseService::DelBulletinMessage( const _cs_char_t* bulletin_adr, uniqueid_t bulletin_gid )
    {
        XDatabase* xdb = GetDatabase( CsDBType_Extern, "", bulletin_gid );
        return cs_db_function::bulletin::dbDelRecord( xdb, bulletin_adr );
    }

    //===============================================================================
    //
    //
    //===============================================================================
    void DatabaseService::CreateCommonDBTable()
    {
        // 只有一張的共用表 所以建立在第一台
        cs_db_function::utility::dbCreateCsCommonTable( GetDatabase( CsDBType_Common, "@first", 0));
    }

    //===============================================================================
    //
    //
    //===============================================================================
    //void DatabaseService::CreateExpondDBTable()
    //{
    //    // player history
    //    cyberspace::utility::dbCreateCsExpondTable( database_[ 1 ][ 0 ] );
    //}

    //===============================================================================
    //
    //
    //===============================================================================
    void DatabaseService::DoUpdate( void* data )
    {
        DatabaseService* service = ( DatabaseService* )data;

        // 取出要處理的序號
        int thread_idx = 0;
        {
            TS_Lock lock( &quene_lock );
            thread_idx = quene_index;
            quene_index++;
        }

        while( service->is_alive_ )
        {
            std::vector<LogData*> logdata;
            {
                TS_Lock lock( &logdata_lock[ thread_idx ] );
                logdata = logdata_queue[ thread_idx ];
                logdata_queue[ thread_idx ].clear();
            }

            for( size_t qi = 0; qi < logdata.size(); qi++ )
            {
                cs_session_uid_t session_uid = logdata[ qi ]->session_uid;
                XDatabase* pXDB = service->GetDatabase( CsDBType_Extern, "", session_uid );
                cs_db_function::log::dbSetRecord( pXDB, logdata[ qi ]->classify, logdata[ qi ]->logtime, logdata[ qi ]->level, logdata[ qi ]->session_uid, logdata[ qi ]->event.c_str(), logdata[ qi ]->data.c_str(), logdata[ qi ]->content.c_str() );
                SAFE_DELETE( logdata[ qi ] );
            }
            pfstd::MSSleep( 1000 );
        }
    }
}
