
/******************************************************************************
 *  Class to handle MySql database connection and query.
 *
 *  Created Feb. 24 2004
 *  Karl Lee
 * apt-get install libmysqlclient-dev
 * yum info mysql-devel.
 *****************************************************************************/

 //01. == C 系統文件/C++ 系統文件
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <errmsg.h>
#include <mysql.h>

 //02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <t_lock.h>
#include <pf-debug.h>
#include <pf-stdlib.h>
//

 //03. == 其他 使用 函式庫的.h 文件

 //04. == 此專案內.h 文件
 //#include <p_mysql_++.h>
#include "xdb-mysqldb.h"


//typedef Connection DBConnection;
//typedef Query   DBQuery;
//typedef Result  DBResult;
//typedef Row  DBRow;
//typedef Date    DBDate;

//#pragma comment (lib, "libmysql.lib")

namespace xdb
{
    class DBResult
    {
    private:

        bool                     haveError;
        int                      ErrNo;
        _mstring                 ErrMsg;
        uint64_t                 AffectedRow; // 有改變的數量
        MYSQL_RES*               result;      //struct save query result

        std::map<_mstring, int>  field_name_; // 欄位的名稱的位置

    public:

        MySqlDB * m_pQueryDB;

        DBResult();
        DBResult( MYSQL* p_mysql_, char_t* query, int len );
        ~DBResult();

        void        init( MYSQL* p_mysql_, char_t* query, int len );

        int         get_errno();
        _mstring    get_errmsg();
        bool        get_successful( void );
        uint64_t    get_affectedrow();


        bool        empty( void );
        size64_t    size( void );

        XDBResBuf   GetResult( uint_t field, uint_t row );

        int         status();                       // Return query status

        longlong_t  nrTuples();                     // Number of fetched tuples
        int         nrFields();                     // Number of fields per tuple


        uint_t      fieldIdx( const char* name );   // index of name
        char_t*     fieldName( uint_t n );          // name of nth field
        size32_t    fieldSize( uint_t n );          // Size of nth field
        size32_t    fieldSize( char_t* name );      // Size of nth field

        void        seekTuple( int tuple );         // Sets internal cursor to tuple
        char_t**    getTuple();                     // Return the next tuple or NULL
        char_t**    getTuple( int tuple );          // Return tuple
    };

    // TODO: Sean
    //CRITICAL_SECTION  g_crtical;

    namespace
    {
        //T_Lock              s_query_result_lock;
        //std::set<XDBResult> s_query_result_pool;
    }

    MySqlDB::MySqlDB()
    {
        p_xmysql_lock_ = new T_Lock;
        strcpy( error_, "No connection" );
        p_mysql_ = new MYSQL;
        mysql_init( p_mysql_ );
        is_connected_ = false;   // No connection yet
    }

    MySqlDB::~MySqlDB( void )
    {
        Disconnect();
        //mysql_close( p_mysql_ );
        SAFE_DELETE( p_mysql_ );
        SAFE_DELETE( p_xmysql_lock_ );
        // TODO: Sean
        //DeleteCriticalSection(&g_crtical);
    }

    _mstring MySqlDB::OnEscapeString( _IN const char_t* str, _IN size32_t len )
    {
        char_t* query = new char_t[ len * 2 + 1 ];
        mysql_real_escape_string( p_mysql_, query, str, len );
        _mstring q = query;
        SAFE_DELETE_ARRAY( query );
        return q;
    }

    bool MySqlDB::Connect( _IN const char_t* db, _IN const char_t* host, _IN unsigned short port, _IN const char_t* username, _IN const char_t* password )
    {
        //TS_Lock lock(p_xmysql_lock_);
        Pf_INFO( "DB connect %s, %s, %s, %s\n", db, host, username, password );
        {
            // 這邊不要鎖算了 如果查詢失敗 會死結
            // 因為現在是用多連線 所以應該 沒差
            TS_Lock lock( p_xmysql_lock_ );
            _mstring ndb = db;
            _mstring nhost = host;
            _mstring nname = username;
            _mstring npswd = password;

            mysql_close( p_mysql_ );

            if ( is_connected_ )
            {
                is_connected_ = false;
                //mysql_init( p_mysql_ );
            }

            for ( uint_t i = 0; i < 5; i++ )
            {
                mysql_init( p_mysql_ );
                if ( mysql_real_connect( p_mysql_, nhost.c_str(), nname.c_str(), npswd.c_str(), ndb.c_str(), port, NULL, 0 ) != NULL )
                {
                    char value = 1;
                    mysql_options( p_mysql_, MYSQL_OPT_RECONNECT, (char *)&value );
                    int  time_out = 10;
                    mysql_options( p_mysql_, MYSQL_OPT_READ_TIMEOUT, (const char*)&time_out );
                    break;
                }
                printf( "Connect to database failed %s\n", mysql_error( p_mysql_ ) );
                if ( i == ( 5 - 1 ) )
                {
                    strcpy( error_, "Connect to database failed" );
                    return false;
                }
                mysql_close( p_mysql_ );
                pfstd::MSSleep( 1000 );
            }

            db_name_ = ndb;
            host_name_ = nhost;
            username_ = nname;
            password_ = npswd;
            host_port_ = port;
            is_connected_ = true;
        }

        SetCharSet( charset_.c_str() );

        //if(!SelectDatabase(db))
        //return false;
        //unused++;
        // Just to suppress a compiler warning
        return true;
    }

    bool MySqlDB::SetCharSet( _IN const char_t* CharSet )
    {
        TS_Lock lock( p_xmysql_lock_, 3 );
        charset_ = CharSet;
        int ret = false;
        if ( charset_ != "" )
        {
            char CharSetBuff[ 255 ];
            sprintf( CharSetBuff, "SET NAMES '%s'", charset_.c_str() );
            DBResult* result = new DBResult( p_mysql_, (char_t*)CharSetBuff, strlen( CharSetBuff ) );
            ret = result->get_successful();
            SAFE_DELETE( result );
        }
        return ret;
    }

    bool MySqlDB::SelectDatabase( _IN const char_t* db )
    {
        TS_Lock lock( p_xmysql_lock_, 4 );
        db_name_ = db;
        if ( mysql_select_db( p_mysql_, db ) )
        {
            mysql_close( p_mysql_ );
            strcpy( error_, "No such database" );
            return false;
        }
        return true;
    }

    /*
    bool MySqlDB::connect(bool te)
    {
        m_pConn = new DBConnection(te);

        return true;
    }
    */

    bool MySqlDB::Disconnect( void )
    {
        TS_Lock lock( p_xmysql_lock_, 5 );
        if ( is_connected_ )
        {
            mysql_close( p_mysql_ );
        }
        is_connected_ = false;
        db_name_ = "";
        host_name_ = "";
        username_ = "";
        password_ = "";

        std::vector<DBResult*> erase_result;

        //TS_Lock lock2( &s_query_result_lock );
        std::set<XDBResult>::iterator it = query_result_cntr_.begin();
        while ( it != query_result_cntr_.end() )
        {
            //XDBResult result = ( *it );
            DBResult* ret = (DBResult*)( *it );
            if ( ret->m_pQueryDB == this )
            {
                erase_result.push_back( ret );
                //SAFE_DELETE( ret );
            }
            it++;
        }

        for ( unsigned int i = 0; i < erase_result.size(); i++ )
        {
            query_result_cntr_.erase( erase_result[ i ] );
            SAFE_DELETE( erase_result[ i ] );
        }

        //m_QueryResult.clear();
        //for( unsigned int i = 0; i < removeresult.size(); i++ )
        //{
        //    s_query_result_pool.erase( removeresult[ i ] );
        //    SAFE_DELETE( removeresult[ i ] );
        //}
        //
        //removeresult.clear();
        return true;
    }

    bool MySqlDB::IsConnected( void )
    {
        // Sean:
        // 如果斷線 mysql_ping()會主動重連....
        TS_Lock lock( p_xmysql_lock_, 6 );
        // Checks whether the connection to the server is working. If the connection has gone down, an automatic reconnection is attempted.
        // This function can be used by clients that remain idle for a long32_t while, to check whether the server has closed the connection and
        // reconnect if necessary.

        return is_connected_;

        // 20220128 慢.....
        if ( mysql_ping( p_mysql_ ) != 0 )
        {
            return false;
        }
        return true;
    }

    _mstring MySqlDB::Query_error()
    {
        if ( !IsConnected() )
        {
            return _mstring( "DB not yet Connected!!" );
        }
        return _mstring( "" );
    }

    //static char_t errbuf[128];
    //XDBResult MySqlDB::ExecQuery( const char_t* query, ... )
    XDBResult MySqlDB::ExecQuery( _IN const char_t* query )
    {
        if ( query == NULL )
        {
            return NULL;
        }

        if ( strcmp( query, "" ) == 0 )
        {
            return NULL;
        }

        //char_t* buffer = NULL;

        /*
        va_list args;
        va_start( args, query );
    #ifdef _WINDOWS
        int len = _vscprintf( query, args ) + 1; //  // _vscprintf doesn't count   terminating '\0'
        buffer = new char[ len ];
        vsprintf_s( buffer, len, query, args );
    #else
        int len = 0;
        {
            va_list argcopy;
            va_copy( argcopy, args );
            len = vsnprintf( NULL, 0, query, argcopy ) + 1;
            va_end( argcopy );
        }
        buffer = new char[ len ];
        vsnprintf( buffer, len, query, args );
    #endif
        va_end( args );
        */

        uint_t    requercont = 0;
        int       myerrno = 0;
        DBResult* result = NULL;

        // 在等級低的電腦容易造成 讓資料庫當掉 ，所以試著 加 看會部會降低對資料庫的影響
        //pfstd::MSSleep( 1 );

_REQUERY:
        {
            //printf( _T( "Query in %s \n" ), query );
            // 就算失敗 也要有  result否則問不出 錯誤的message
            XDBResult xdb_result = (XDBResult)result;
            FreeResult( xdb_result );
            {
                TS_Lock lock( p_xmysql_lock_, 7 );
                if( !is_connected_ )
                {
                    return NULL;
                }
                //TS_Lock lock( p_xmysql_lock_ );
                result = new DBResult( p_mysql_, (char_t*)query, strlen( query ) );
                myerrno = result->get_errno();
                result->m_pQueryDB = this;
                query_result_cntr_.insert( std::set<XDBResult>::value_type( result ) );
            }

            // 成功
            if ( result->get_successful() )
            {
                //TS_Lock lock2( &s_query_result_lock );
                //if ( requercont > 0 )
                //{
                //    //Pf_INFO( "\nxdb::MySqlDB::ExecQuery: Database do query successful %d times\n", requercont );
                //}
                //SAFE_DELETE_ARRAY( buffer );
                //printf( _T( "Query out 00 %s \n" ), query );
                return result;
            }
            //printf( _T( "Query out 01 %s \n" ), query );
        }

        // 這邊不刪除 才能拿到錯誤訊息
        //SAFE_DELETE( result );
        //先判斷是否是連線造成 還是 詢問錯誤
        if ( myerrno >= 1924 )
        {
            static char_t buff[ 10240 ];
            ++requercont;
            if ( requercont >= 5 )
            {
                Pf_ERROR( "\nxdb::MySqlDB::ExecQuery: DB try connect failure over 10 times\n\n" );
                //SAFE_DELETE_ARRAY( buffer );
                //return result;
            }
            else
            {
                // Sean:
                // 修改
                for ( uint_t i = 0; i < 5; i++ )
                {
                    int pingret = 0;
                    {
                        pfstd::MSSleep( 100 );
                        TS_Lock lock( p_xmysql_lock_, 8 );
                        pingret = mysql_ping( p_mysql_ );
                    }
                    if ( ( pingret == 0 ) && ( requercont < 5 ) ) // 表示還有救
                    {
                        pfstd::MSSleep( 100 );
                        Pf_INFO( "\nxdb::MySqlDB::ExecQuery: Database ping successful\n" );
                        SetCharSet( charset_.c_str() );
                        goto _REQUERY;
                    }
                    else
                    {
                        // 如果資料庫斷線 等一秒再重連
                        pfstd::MSSleep( 1000 );
                        if ( Connect( db_name_.c_str(), host_name_.c_str(), host_port_, username_.c_str(), password_.c_str() ) )
                        {
                            Pf_INFO( buff, "\nxdb::MySqlDB::ExecQuery: Database connect successful\n" );
                            goto _REQUERY;
                        }
                    }
                }
            }
        }
        Pf_INFO( "\nxdb::MySqlDB::ExecQuery: error_ command %s\n", query );
        //SAFE_DELETE_ARRAY( buffer );
        //SAFE_DELETE( result );
        return result;
    }

    XDBResult MySqlDB::ExecQuery( _IN const wchar_t* query )
    {
        return XDatabase::ExecQuery( query );
    }

    void MySqlDB::FreeResult( _IN XDBResult& result )
    {
        XDBResult xret = result;
        if ( xret == NULL )
        {
            return;
        }

        //TS_Lock lock(p_xmysql_lock_);
        MySqlDB* p_mysqldb = ( (DBResult*)xret )->m_pQueryDB;

        {
            TS_Lock lock( p_mysqldb->p_xmysql_lock_, 9 );
            //TS_Lock lock( &s_query_result_lock );
            std::set<XDBResult>::iterator it = p_mysqldb->query_result_cntr_.find( xret );
            if ( it != p_mysqldb->query_result_cntr_.end() )
            {
                //XDBResult result = ( *it );
                //DBResult* ret = ( DBResult* ) result;
                p_mysqldb->query_result_cntr_.erase( it );
            }
        }

        {
            DBResult* ret = (DBResult*)xret;
            SAFE_DELETE( ret );
            result = NULL;
        }
    }

    size32_t MySqlDB::GetFieldsSize( _IN XDBResult result )
    {
        if ( result == NULL )
        {
            return 0;
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 10 );
        return ( (DBResult*)result )->nrFields();
    }

    _mstring MySqlDB::GetFieldsNameM( _IN XDBResult result, _IN uint_t n )
    {
        if ( result == NULL )
        {
            return "";
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 11 );
        return ( (DBResult*)result )->fieldName( n );
    }

    uint_t MySqlDB::GetFieldIndex( _IN XDBResult result, const char* col_name )
    {
        if( result == NULL )
        {
            return 0;
        }

        TS_Lock lock( ( ( DBResult* )result )->m_pQueryDB->p_xmysql_lock_, 15 );
        return ( ( DBResult* )result )->fieldIdx( col_name );
    }

    size64_t MySqlDB::GetResultSize( _IN XDBResult result )
    {
        if ( result == NULL )
        {
            return 0;
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 12 );
        if ( ( (DBResult*)result )->empty() )
        {
            return 0;
        }
        return ( (DBResult*)result )->size();
    }

    bool MySqlDB::GetResultEmpty( _IN XDBResult result )
    {
        if ( result == NULL )
        {
            return true;
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 13 );
        return ( (DBResult*)result )->empty();
    }

    int MySqlDB::GetQueryErrNo( _IN const XDBResult result )
    {
        if ( result == NULL )
        {
            return -1;
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 14 );
        return ( (DBResult*)result )->get_errno();
    }

    _mstring MySqlDB::GetQueryErrMsgM( _IN const XDBResult result )
    {
        if ( result == NULL )
        {
            return "";
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 15 );
        return ( (DBResult*)result )->get_errmsg();
    }

    bool MySqlDB::GetQuerySuccessful( _IN const XDBResult result )
    {
        if ( result == NULL )
        {
            return false;
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 16 );
        return ( (DBResult*)result )->get_successful();
    }

    size64_t MySqlDB::GetQueryAffectedRow( _IN const XDBResult result )
    {
        if ( result == NULL )
        {
            return 0;
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 17 );
        return ( (DBResult*)result )->get_affectedrow();
    }

    _mstring MySqlDB::GetQueryResultM( _IN const XDBResult result, _IN uint_t col, _IN uint_t row )
    {
        if ( result == NULL )
        {
            return "";
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 18 );
        //const char_t* result = ((MySqlDB*)m_Database)->GetQueryResultT(0, 0);
        XDBResBuf buff = ( (DBResult*)result )->GetResult( col, row );
        uint_t len = buff.size();
        _mstring str;
        if ( len != 0 )
        {
            str.resize( len + 1 );
            char_t* stc = (char_t*)_GetXDBResBufPtr( buff );
            str.assign( stc, len );
        }
        return str;
        //return _mstring("");
    }

    XDBResBuf MySqlDB::GetQueryResource( _IN const XDBResult result, _IN uint_t col, _IN uint_t row )
    {
        if ( result == NULL )
        {
            XDBResBuf emtpybuf;
            return emtpybuf;
        }
        TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 19 );
        return ( (DBResult*)result )->GetResult( col, row );
    }


    //-------------------------------------------------------------------------------------------------
    // ------------------- Database result implementation ------------------
    //-------------------------------------------------------------------------------------------------


    DBResult::DBResult()
    {
        result = NULL;
        haveError = false;
        ErrNo = 0;
    }

    //-------------------------------------------------------------------------------------------------

    DBResult::DBResult( MYSQL* p_mysql, char_t* query, int len )
    {
        result = NULL;
        init( p_mysql, query, len );
    }

    //-------------------------------------------------------------------------------------------------

    DBResult::~DBResult()
    {
        if ( result != NULL )       // Free memory resources
        {
            mysql_free_result( result );
        }
    }

    //-------------------------------------------------------------------------------------------------

    void DBResult::init( MYSQL* mysql, char_t* query, int len )
    {
        if ( result != NULL )
        {
            // clear the result struct at first
            mysql_free_result( result );
            result = NULL;
        }

        if ( ( query == NULL ) || ( len == 0 ) )
        {
            haveError = true;
            return;
        }

        haveError = false;
        ErrMsg = ( "" );
        /* query database */
        int state = mysql_real_query( mysql, query, len );
        ErrNo = mysql_errno( mysql );
        AffectedRow = mysql_affected_rows( mysql );
        if ( state != 0 )
        {
            ErrMsg = mysql_error( mysql );
            //printf( buff );
            Pf_ERROR( "MySqlDB::init: %s", ErrMsg.c_str() );
            haveError = true;
            return;
        }

        result = mysql_store_result( mysql );

        // 放入欄位名稱
        for( int i = 0; i < nrFields(); i++ )
        {
            field_name_[ fieldName( i ) ] = i;
        }
    }

    //-------------------------------------------------------------------------------------------------

    int DBResult::status()
    {
        if ( haveError )
        {
            return DB_ERROR;
        }

        if ( result == NULL )
        {
            return 1;
        }

        return DB_TUPLES_OK;
    }

    int DBResult::get_errno()
    {
        return ErrNo;
    }

    _mstring DBResult::get_errmsg()
    {
        return ErrMsg;
    }

    bool DBResult::get_successful( void )
    {
        return !haveError;
    }

    uint64_t DBResult::get_affectedrow( void )
    {
        return AffectedRow;
    }

    bool DBResult::empty( void )
    {
        if ( size() > 0 )
        {
            return false;
        }

        return true;
    }

    size64_t DBResult::size( void )
    {
        if ( result == NULL )
        {
            return 0;
        }

        return (size64_t)mysql_num_rows( result );
    }

    XDBResBuf DBResult::GetResult( uint_t col, uint_t row )
    {
        //MYSQL_FIELD fidel;
        XDBResBuf buff;

        if ( result == NULL )
        {
            return buff;
        }

        mysql_data_seek( result, row );
        MYSQL_ROW dbrow = mysql_fetch_row( result );
        unsigned long* len = mysql_fetch_lengths( result );

        if ( len == NULL )
        {
            return buff;
        }

        buff.resize( len[ col ] );

        if ( !buff.empty() )
        {
            memcpy( ( &( buff[ 0 ] ) ), dbrow[ col ], len[ col ] );
        }

        return buff;
    }

    //-------------------------------------------------------------------------------------------------

    longlong_t DBResult::nrTuples()
    {
        if ( result == NULL )
        {
            return 0;
        }

        return mysql_num_rows( result );
    }

    //-------------------------------------------------------------------------------------------------

    int DBResult::nrFields()
    {
        if ( result == NULL )
        {
            return 0;
        }

        return mysql_num_fields( result );
    }

    //-------------------------------------------------------------------------------------------------

    uint_t DBResult::fieldIdx( const char* name )
    {
        return field_name_[ name ];
    }

    //-------------------------------------------------------------------------------------------------

    char_t* DBResult::fieldName( uint_t n )
    {
        MYSQL_FIELD* field;

        if ( result == NULL )
        {
            return NULL;
        }

        mysql_field_seek( result, n );
        field = mysql_fetch_field( result );

        if ( field == NULL )
        {
            return NULL;
        }

        return field->name;
    }

    //-------------------------------------------------------------------------------------------------

    size32_t DBResult::fieldSize( uint_t n )
    {
        MYSQL_FIELD* field;

        if ( result == NULL )
        {
            return 0;
        }

        mysql_field_seek( result, n );
        field = mysql_fetch_field( result );

        if ( field == NULL )
        {
            return 0;
        }

        return (uint_t)field->length;
    }

    //-------------------------------------------------------------------------------------------------

    size32_t DBResult::fieldSize( char_t* name )
    {
        int i;

        if ( result == NULL )
        {
            return 0;
        }

        for ( i = 0; i < nrFields(); i++ )
        {
            if ( !strcmp( name, fieldName( i ) ) )
            {
                return fieldSize( i );
            }
        }

        return 0;
    }

    //-------------------------------------------------------------------------------------------------

    void DBResult::seekTuple( int tuple )
    {
        if ( result == NULL )
        {
            return;
        }

        mysql_data_seek( result, tuple );
    }

    //-------------------------------------------------------------------------------------------------

    char_t** DBResult::getTuple()
    {
        MYSQL_ROW row;

        if ( result == NULL )
        {
            return NULL;
        }

        row = mysql_fetch_row( result );
        return row;
    }

    //-------------------------------------------------------------------------------------------------

    char_t** DBResult::getTuple( int tuple )
    {
        seekTuple( tuple );
        return getTuple();
    }

}