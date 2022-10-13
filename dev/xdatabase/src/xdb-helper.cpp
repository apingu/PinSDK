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

//01. == C 系統文件/C++ 系統文件
#include <list>
#include <assert.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <t_lock.h>
#include <pf-thread.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xdb-helper.h"
#include "xdb-mysqldb.h"


namespace xdb
{
    namespace helper
    {
        ///////////////////////////////////////////////////////////////////////////////
        //
        //
        namespace
        {
            struct DBUpdateQuery
            {
                XDatabase* p_query_db;
                _mstring                        sql_string;
                UnPromptQuery::OnQueryListener* p_query_listener;
                static T_Lock                   s_l_uid;
                static int                      s_n_uid;
            };

            T_Lock DBUpdateQuery::s_l_uid;
            int    DBUpdateQuery::s_n_uid = 0;

            static int                                  s_max_update_size = 0;
            typedef std::list<DBUpdateQuery*>           db_update_query_list_t;
            typedef std::vector<db_update_query_list_t> db_update_query_vec_t;

            std::vector<T_Lock>* s_db_update_lock = NULL;
            db_update_query_vec_t* s_db_update_query = NULL;
            std::vector<pfkrl::Thread::Handle>          s_db_update_thread;
            bool                                        s_db_update_idle = true;

            //int  GetDBAgentUseIndex();
            void DBAgentUpdateQuery( void* argv )
            {
                int* idptr = ( int* )argv;
                int idx = *idptr;
                SAFE_DELETE( idptr );
                DBUpdateQuery* procquer = NULL;

                while( s_db_update_idle )
                {
                    {
                        TS_Lock lock( &( *s_db_update_lock )[ idx ] );

                        if( !( *s_db_update_query )[ idx ].empty() )
                        {
                            procquer = ( *s_db_update_query )[ idx ].front();
                            ( *s_db_update_query )[ idx ].pop_front();
                        }
                        else
                        {
                            procquer = NULL;
                        }
                    }

                    if( procquer )
                    {
                        if( procquer->p_query_db != NULL )
                        {
                            XDBResult result = procquer->p_query_db->ExecQueryT( procquer->sql_string.c_str() );

                            if( procquer->p_query_listener != NULL )
                            {
                                procquer->p_query_listener->OnResult( result );
                            }

                            procquer->p_query_db->FreeResult( result );
                        }

                        SAFE_DELETE( procquer );
                    }
                    else
                    {
                        //pfstd::MSSleep( 1 );
                    }
                }
            }
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        UnPromptQuery::UnPromptQuery()
        {
        }

        ///=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        UnPromptQuery::~UnPromptQuery()
        {
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void UnPromptQuery::Create( int process )
        {
            s_db_update_idle = true;
            s_max_update_size = process;
            s_db_update_lock = new std::vector<T_Lock>;
            s_db_update_query = new db_update_query_vec_t;
            s_db_update_lock->resize( s_max_update_size );
            s_db_update_query->resize( s_max_update_size );

            for( int i = 0; i < s_max_update_size; i++ )
            {
                int* idx = new int;
                ( *idx ) = i;
                s_db_update_thread.push_back( pfkrl::Thread::BeginProcess( DBAgentUpdateQuery, idx, "UnPromptQuery::Create" ) );
            }
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void UnPromptQuery::Destroy()
        {
            s_db_update_idle = false;

            for( int idx = 0; idx < s_max_update_size; idx++ )
            {
                TS_Lock lock( &( *s_db_update_lock )[ idx ] );
                std::list<DBUpdateQuery*>::iterator it = ( *s_db_update_query )[ idx ].begin();

                while( it != ( *s_db_update_query )[ idx ].end() )
                {
                    DBUpdateQuery* procquer = ( *it );

                    if( procquer->p_query_db != NULL )
                    {
                        XDBResult result = procquer->p_query_db->ExecQueryT( procquer->sql_string.c_str() );
                        procquer->p_query_db->FreeResult( result );
                    }

                    SAFE_DELETE( procquer );
                }

                ( *s_db_update_query )[ idx ].clear();
            }

            s_db_update_thread.clear();
            s_db_update_lock->clear();
            s_db_update_query->clear();
            SAFE_DELETE( s_db_update_lock );
            SAFE_DELETE( s_db_update_query );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void UnPromptQuery::Query( XDatabase* p_query_db, const char* sql_string, UnPromptQuery::OnQueryListener* p_listener )
        {
            if( s_max_update_size == 0 )
            {
                return;
            }

            if( p_query_db == NULL )
            {
                return;
            }

            DBUpdateQuery* updatequery = new DBUpdateQuery;
            updatequery->p_query_db = p_query_db;
            updatequery->sql_string = sql_string;
            updatequery->p_query_listener = p_listener;
            // TODO: Pin
            // 用記憶體當依據 不知道會不會出事
            //int uid = (int)updatequery->p_query_db;
            //int pos = (uid % s_max_update_size);
            int uid = 0;
            int pos = 0;
            {
                TS_Lock lock( &DBUpdateQuery::s_l_uid );
                uid = DBUpdateQuery::s_n_uid;
                DBUpdateQuery::s_n_uid++;
                pos = ( uid % s_max_update_size );
            }
            TS_Lock querylock( &( *s_db_update_lock )[ pos ] );
            ( *s_db_update_query )[ pos ].push_back( updatequery );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void UnPromptQuery::Query( XDatabase* p_query_db, const wchar_t* sql_string, UnPromptQuery::OnQueryListener* p_listener )
        {
            Query( p_query_db, pfstd::UnicodeToUTF8( sql_string ).c_str(), p_listener );
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //
        namespace
        {
            class DBObjBuffer
            {
            public:

                DBObjBuffer()
                {
                    srand( time( NULL ) );
                }

                ~DBObjBuffer()
                {
                    for( std::map<_mstring, std::vector<XDatabase*>>::iterator iter = xdb_buffer.begin(); iter != xdb_buffer.end(); iter++ )
                    {
                        for( uint_t iti = 0; iti < iter->second.size(); iti++ )
                        {
                            DBObjectPool::DestoryDB( iter->second[ iti ] );
                        }
                    }
                }

                std::map<_mstring, std::vector<XDatabase*>>  xdb_buffer;
            };


            T_Lock                     s_xdb_utlock;
            std::map<_mstring, T_Lock> s_xdb_lock;
            // 讓他可以自動被消滅
            DBObjBuffer                s_xdb_pool;

        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        DBObjectPool::DBObjectPool()
        {

        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        DBObjectPool::~DBObjectPool()
        {

        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        XDatabase* DBObjectPool::CreateDB( _IN XDB_Type xdb_type )
        {
            switch( xdb_type )
            {
                case XDB_Type::XDB_MYSQL:
                default:
                    {
                        return  new xdb::MySqlDB;
                    }
            }
            return NULL;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================        
        void DBObjectPool::DestoryDB( _IN XDatabase* xdb )
        {
            if( strcmp( xdb->ObjectName(), "XdbMySQL" ) == 0 )
            {
                xdb::MySqlDB* xmysql = ( xdb::MySqlDB* )xdb;
                SAFE_DELETE( xmysql );
            }
            else
            {
                assert( NULL || "no sql object" );
            }
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool DBObjectPool::InsertPool( _IN const char_t* pool_name, _IN uint_t pool_size, _IN XDB_Type xdb_type,
                                    _IN const char_t* db, _IN const char_t* host, _IN unsigned short port, _IN const char_t* username, _IN const char_t* password, _IN const char_t* char_set )
        {
            bool is_connected = true;
            std::vector<XDatabase*> db_buff;

            TS_Lock lock( &s_xdb_utlock );
            {
                TS_Lock lock( &s_xdb_lock[ pool_name ] );
                std::map<_mstring, std::vector<XDatabase*>>::iterator iter = s_xdb_pool.xdb_buffer.find( pool_name );
                if( iter != s_xdb_pool.xdb_buffer.end() )
                {
                    return false;
                }

                {
                    for( uint_t ti = 0; ti < pool_size; ti++ )
                    {
                        XDatabase* xdb = CreateDB( xdb_type );

                        if( !xdb->Connect( db, host, port, username, password ) )
                        {
                            is_connected = false;
                            break;
                        }
                        xdb->SetCharSet( char_set );
                        db_buff.push_back( xdb );
                    }
                }

                if( is_connected )
                {
                    s_xdb_pool.xdb_buffer.insert( std::map<_mstring, std::vector<XDatabase*>>::value_type( pool_name, db_buff ) );
                }
            }

            // 其中一個不成功 就當不成功
            if( !is_connected )
            {
                for( uint_t ti = 0; ti < db_buff.size(); ti++ )
                {
                    DestoryDB( db_buff[ ti ] );
                }
                s_xdb_lock.erase( pool_name );
            }

            return is_connected;
        }

        bool DBObjectPool::InsertPool( _IN const wchar_t* pool_name, _IN uint_t pool_size, _IN XDB_Type xdb_type,
                                    _IN const wchar_t* db, _IN const wchar_t* host, _IN unsigned short port, _IN const wchar_t* username, _IN const wchar_t* password, _IN const wchar_t* char_set )
        {
            return InsertPool(
                pfstd::UnicodeToUTF8( pool_name ).c_str(), pool_size, xdb_type,
                pfstd::UnicodeToUTF8( db ).c_str(),
                pfstd::UnicodeToUTF8( host ).c_str(),
                port,
                pfstd::UnicodeToUTF8( username ).c_str(),
                pfstd::UnicodeToUTF8( password ).c_str(),
                pfstd::UnicodeToUTF8( char_set ).c_str() );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool DBObjectPool::RemovePool( _IN const char_t* pool_name )
        {
            TS_Lock lock( &s_xdb_utlock );
            {
                TS_Lock lock( &s_xdb_lock[ pool_name ] );
                std::map<_mstring, std::vector<XDatabase*>>::iterator iter = s_xdb_pool.xdb_buffer.find( pool_name );
                if( iter == s_xdb_pool.xdb_buffer.end() )
                {
                    return false;
                }

                for( uint_t iti = 0; iti < iter->second.size(); iti++ )
                {
                    DestoryDB( iter->second[ iti ] );
                }
                s_xdb_pool.xdb_buffer.erase( pool_name );
            }

            s_xdb_lock.erase( pool_name );
            return true;
        }

        bool DBObjectPool::RemovePool( _IN const wchar_t* pool_name )
        {
            return RemovePool( pfstd::UnicodeToUTF8( pool_name ).c_str() );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void DBObjectPool::GetAllPoolName( _OUT std::vector<_mstring>& pool_name )
        {
            TS_Lock lock( &s_xdb_utlock );
            for( std::map<_mstring, std::vector<XDatabase*>>::iterator iter = s_xdb_pool.xdb_buffer.begin(); iter != s_xdb_pool.xdb_buffer.end(); iter++ )
            {
                pool_name.push_back( iter->first );
            }
        }

        void DBObjectPool::GetAllPoolName( _OUT std::vector<_wstring>& pool_name )
        {
            TS_Lock lock( &s_xdb_utlock );
            for( std::map<_mstring, std::vector<XDatabase*>>::iterator iter = s_xdb_pool.xdb_buffer.begin(); iter != s_xdb_pool.xdb_buffer.end(); iter++ )
            {
                pool_name.push_back( pfstd::UTF8ToUnicode( iter->first ) );
            }
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        XDatabase* DBObjectPool::GetDatabase( _IN const char_t* pool_name, _IN int_t idx )
        {
            TS_Lock lock( &s_xdb_lock[ pool_name ] );
            std::map<_mstring, std::vector<XDatabase*>>::iterator iter = s_xdb_pool.xdb_buffer.find( pool_name );
            if( iter == s_xdb_pool.xdb_buffer.end() )
            {
                return NULL;
            }

            int pool_size = iter->second.size();
            if( idx < 0 )
            {
                return iter->second[ rand() % pool_size ];
            }
            return iter->second[ idx % pool_size ];
        }

        XDatabase* DBObjectPool::GetDatabase( _IN const wchar_t* pool_name, _IN int_t idx )
        {
            return GetDatabase( pfstd::UnicodeToUTF8( pool_name ).c_str(), idx );
        }

        ////////////////
        //

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        DBS_Object::DBS_Object( _IN XDB_Type xdb_type )
        {
            db_obj_ = DBObjectPool::CreateDB( xdb_type );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        DBS_Object::~DBS_Object()
        {
            DBObjectPool::DestoryDB( db_obj_ );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        XDatabase* DBS_Object::operator->( void )
        {
            return db_obj_;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        DBS_Object::operator XDatabase* ( )
        {
            return db_obj_;
        }
    }
}