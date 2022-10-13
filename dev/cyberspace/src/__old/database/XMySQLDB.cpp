/******************************************************************************
 *  Class to handle MySql database connection and query.
 *
 *  Created Feb. 24 2004
 *  Karl Lee
 * apt-get install libmysqlclient-dev
 * yum info mysql-devel.
 *****************************************************************************/
//#include "stdafx.h"   //?
//#include <iostream>

#include <T_Lock.h>
//#include <m_mysql++.h>

#include "XMySQLDB.h"
#include <Debug.h>
#include <assert.h>
#include <string.h>
#include <PfStdlib.h>

//typedef Connection DBConnection;
//typedef Query   DBQuery;
//typedef Result  DBResult;
//typedef Row  DBRow;
//typedef Date    DBDate;

#pragma comment (lib, "libmysql.lib")
//#pragma comment (lib, "mysqlclient.lib")


class DBResult
{
private:
    bool       haveError;
    MYSQL_RES* result;     //struct save query result
public:

    XMySqlDB*  m_pQueryDB;
    
    DBResult();
    DBResult( MYSQL* m_mysql, tchar_t* query, int len );
    ~DBResult();
    
    void      init( MYSQL* m_mysql, tchar_t* query, int len );
    
    bool      successful( void );
    bool      empty( void );
	size64_t  size( void );
    
    DBResBuf  GetResult( int field, int row );
    
    int       status();                     // Return query status
    
    longlong_t  nrTuples();                   // Number of fetched tuples
    int       nrFields();                   // Number of fields per tuple
    
    tchar_t*  fieldName( int n );           // name of nth fiel
    int       fieldSize( int n );           // Size of nth field
    int       fieldSize( tchar_t* name );   // Size of nth field
    
    void      seekTuple( int tuple );       // Sets internal cursor to tuple
    tchar_t** getTuple();                   // Return the next tuple or NULL
    tchar_t** getTuple( int tuple );        // Return tuple
};


// TODO: Sean
//CRITICAL_SECTION  g_crtical;

T_Lock              XMySqlDB::s_QueryResultLock;
std::set<XDBResult> XMySqlDB::s_QueryResult;


XMySqlDB::XMySqlDB()
{
    is_connected = false;   // No connection yet
    strcpy( error, "No connection" );
}

XMySqlDB::~XMySqlDB( void )
{
    Disconnect();
    // TODO: Sean
    //DeleteCriticalSection(&g_crtical);
}

_tstring XMySqlDB::OnEscapeString( const tchar_t* str, size32_t len )
{
    tchar_t* query = new tchar_t[ len * 2 + 1 ];
    mysql_real_escape_string( &m_mysql, query, str, len );
    _tstring q = query;
    SAFE_DELETE_ARRAY( query );
    return q;
}


bool XMySqlDB::Connect( const tchar_t* db, const tchar_t* host, unsigned short port, const tchar_t* name, const tchar_t* pswd )
{
    //TS_Lock lock(&m_XMySqlDBUtLock);
    Pf_DEBUG( "DB connect %s, %s, %s, %s\n", db, host, name, pswd );
    // 這邊不要鎖算了 如果查詢失敗 會死結
    // 因為現在是用多連線 所以應該 沒差
    //TS_Lock lock(&m_XMySqlDBUtLock);
    _tstring ndb = db;
    _tstring nhost = host;
    _tstring nname = name;
    _tstring npswd = pswd;
    
    if ( is_connected )
    {
        mysql_close( &m_mysql );
        is_connected = false;
        //m_Dbname="";
        //m_Hostname="";
        //m_Username="";
        //m_Password="";
        // 這裡不清除 不然會有多序的問題 如果有一個序再 ExecQuery 會把原本可能已經有結果的 result也給刪除掉
        //      std::set<XDBResult>::iterator it = m_QueryResult.begin();
        //      while(it != m_QueryResult.end())
        //      {
        //          XDBResult result = (*it);
        //          DBResult* ret= (DBResult*)result;
        //          SAFE_DELETE(ret);
        //          it++;
        //      }
        //      m_QueryResult.clear();
    }
    
    mysql_init( &m_mysql );
    
    for ( uint_t i = 0; i <= 50; i++ )
    {
        if ( mysql_real_connect( &m_mysql, nhost.c_str(), nname.c_str(), npswd.c_str(), ndb.c_str(), port, NULL, 0 ) != NULL )
        {
            break;
        }
        
        if ( i >= 20 )
        {
            strcpy( error, "Connect to database failed" );
            return false;
        }
        
        PfStd::MsSleep( 1000 );
    }
    
    m_Dbname = ndb;
    m_Hostname = nhost;
    m_Username = nname;
    m_Password = npswd;
    m_Port = port;
    is_connected = true;
    
    if ( m_CharSet != "" )
    {
        char CharSetBuff[ 255 ];
        _stprintf( CharSetBuff, "SET NAMES '%s'", m_CharSet.c_str() );
        DBResult* result = new DBResult( &m_mysql, ( tchar_t* ) CharSetBuff, strlen( CharSetBuff ) );
        SAFE_DELETE( result );
    }
    
    //if(!SelectDatabase(db))
    //return false;
    //unused++;
    // Just to suppress a compiler warning
    return true;
}

bool XMySqlDB::SelectDatabase( const tchar_t* db )
{
    TS_Lock lock( &m_XMySqlDBUtLock );
    m_Dbname = db;
    
    if ( mysql_select_db( &m_mysql, db ) )
    {
        mysql_close( &m_mysql );
        strcpy( error, "No such database" );
        return false;
    }
    
    return true;
}

/*
bool XMySqlDB::connect(bool te)
{
	m_pConn = new DBConnection(te);

	return true;
}
*/

bool XMySqlDB::Disconnect( void )
{
    if ( is_connected == false )
    {
        return true;
    }
    
    TS_Lock lock( &m_XMySqlDBUtLock );
    mysql_close( &m_mysql );
    is_connected = false;
    m_Dbname = "";
    m_Hostname = "";
    m_Username = "";
    m_Password = "";
    std::vector<DBResult*> removeresult;
    TS_Lock lock2( &s_QueryResultLock );
    std::set<XDBResult>::iterator it = s_QueryResult.begin();
    
    while ( it != s_QueryResult.end() )
    {
        XDBResult result = ( *it );
        DBResult* ret = ( DBResult* ) result;
        
        if ( ret->m_pQueryDB == this )
        {
            removeresult.push_back( ret );
        }
        
        it++;
    }
    
    //m_QueryResult.clear();
    for ( unsigned int i = 0; i < removeresult.size(); i++ )
    {
        s_QueryResult.erase( removeresult[ i ] );
        SAFE_DELETE( removeresult[ i ] );
    }
    
    removeresult.clear();
    return true;
}


bool XMySqlDB::IsConnected( void )
{
    // Sean:
    // 如果斷線 mysql_ping()會主動重連....
    TS_Lock lock( &m_XMySqlDBUtLock );
    
    // Checks whether the connection to the server is working. If the connection has gone down, an automatic reconnection is attempted.
    // This function can be used by clients that remain idle for a long32_t while, to check whether the server has closed the connection and
    // reconnect if necessary.
    if ( mysql_ping( &m_mysql ) != 0 )
    {
        return false;
    }
    
    return true;
}


_tstring XMySqlDB::Query_error()
{
    if ( !IsConnected() )
    {
        return _tstring( "DB not yet Connected!!" );
    }
    
    return _tstring( "" );
}

//static tchar_t errbuf[128];
XDBResult XMySqlDB::ExecQuery( const tchar_t* query, ... )
{
    TS_Lock lock( &m_XMySqlDBUtLock );
    
    //
    //  va_list ap;
    //  tchar_t sqlCommand[5000];
    //
    //  va_start(ap, query);6
    //  vsprintf(sqlCommand, query, ap);6
    //  va_end(ap);
    if ( query == NULL )
    {
        return NULL;
    }
    
    if ( strcmp( query, "" ) == 0 )
    {
        return NULL;
    }
    
    uint_t requercont = 0;
_REQUERY:
    DBResult* result = NULL;
    bool   bsuccess = true;
    {
        //TS_Lock lock(&m_XMySqlDBUtLock);
        result = new DBResult( &m_mysql, ( tchar_t* ) query, strlen( query ) );
        
        if ( result == NULL )
        {
            bsuccess = false;
        }
        else
        {
            bsuccess = result->successful();
            result->m_pQueryDB = this;
        }
    }
    
    if ( bsuccess )
    {
        TS_Lock lock2( &s_QueryResultLock );
        s_QueryResult.insert( std::set<XDBResult>::value_type( result ) );
        
        if ( requercont > 0 )
        {
            Pf_DEBUG( "\nXMySqlDB::ExecQuery: Database do query successful %d times\n", requercont );
        }
        
        return result;
    }
    else
    {
        static tchar_t buff[ 10240 ];
        ++requercont;
        SAFE_DELETE( result );
        
        if ( requercont > 10 )
        {
            Pf_ERROR( "\nXMySqlDB::ExecQuery: DB try connect failure over 10 times\n\n" );
            return NULL;
        }
        
        if ( strlen( query ) < 9500 )
        {
            Pf_ERROR( buff, "\nXMySqlDB::ExecQuery: Database do query %s failure %d times\n", query, requercont );
            printf( buff );
        }
        else
        {
            Pf_ERROR( buff, "\nXMySqlDB::ExecQuery: Database do query failure %d times, query string to long32_t %d\n", requercont, strlen( query ) );
            printf( buff );
        }
        
        //PfStd::MsSleep(100);
        // Sean:
        // 修改
        for ( uint_t i = 0; i < 20; i++ )
        {
            PfStd::MsSleep( 50 );
            int pingret = 0;
            {
                //TS_Lock lock(&m_XMySqlDBUtLock);
                pingret = mysql_ping( &m_mysql );
            }
            
            if ( ( pingret == 0 ) && ( requercont < 5 ) ) // 表示還有救
            {
                Pf_DEBUG( "\nXMySqlDB::ExecQuery: Database ping successful\n" );
                goto _REQUERY;
            }
            else
            {
                // 如果資料庫斷線 等一秒再重連
                PfStd::MsSleep( 1000 );
                
                if ( Connect( m_Dbname.c_str(), m_Hostname.c_str(), m_Port, m_Username.c_str(), m_Password.c_str() ) )
                {
                    Pf_DEBUG( buff, "\nXMySqlDB::ExecQuery: Database connect successful\n" );
                    goto _REQUERY;
                }
            }
        }
    }
    
    return NULL;
}

void XMySqlDB::FreeResult( XDBResult result )
{
    if ( result == NULL )
    {
        return;
    }
    
    //TS_Lock lock(&m_XMySqlDBUtLock);
    TS_Lock lock2( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    TS_Lock lock( &s_QueryResultLock );
    std::set<XDBResult>::iterator it = s_QueryResult.find( result );
    
    if ( it != s_QueryResult.end() )
    {
        XDBResult result = ( *it );
        DBResult* ret = ( DBResult* ) result;
        s_QueryResult.erase( it );
    }
    
    {
        DBResult* ret = ( DBResult* ) result;
        SAFE_DELETE( ret );
    }
}

size32_t XMySqlDB::GetFieldsSize( XDBResult result )
{
    if ( result == NULL )
    {
        return 0;
    }
    
    TS_Lock lock( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    return ( ( DBResult* ) result )->nrFields();
}


const tchar_t* XMySqlDB::GetFieldsName( XDBResult result, int n )
{
    if ( result == NULL )
    {
        return "";
    }
    
    TS_Lock lock( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    return ( ( DBResult* ) result )->fieldName( n );
}


size64_t XMySqlDB::GetResultSize( XDBResult result )
{
    if ( result == NULL )
    {
        return 0;
    }
    
    TS_Lock lock( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    
    if ( ( ( DBResult* ) result )->empty() )
    {
        return 0;
    }
    
    return ( ( DBResult* ) result )->size();
}

bool XMySqlDB::GetResultEmpty( XDBResult result )
{
    if ( result == NULL )
    {
        return true;
    }
    
    TS_Lock lock( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    return ( ( DBResult* ) result )->empty();
}

std::string XMySqlDB::GetQueryResult( const XDBResult result, unsigned int col, unsigned row )
{
    if ( result == NULL )
    {
        return 0;
    }
    
    TS_Lock lock( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    //const tchar_t* result = ((XMySqlDB*)m_Database)->GetQueryResult(0, 0);
    DBResBuf buff = ( ( DBResult* ) result )->GetResult( col, row );
    uint_t len = buff.size();
    std::string str;
    
    if ( len != 0 )
    {
        str.resize( len + 1 );
        char_t* stc = ( char_t*)_GetDBResBufPtr( buff );
        str.assign( stc, len );
    }
    
    return str;
    //return _tstring("");
}

std::wstring XMySqlDB::GetQueryResultW( const XDBResult result, unsigned int col, unsigned row )
{
    if ( result == NULL )
    {
        return 0;
    }
    
    TS_Lock lock( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    //const tchar_t* result = ((XMySqlDB*)m_Database)->GetQueryResult(0, 0);
    DBResBuf buff = ( ( DBResult* ) result )->GetResult( col, row );
    uint_t len = buff.size();
    std::wstring str;
    
    if ( len != 0 )
    {
        str.resize( ( len / 2 ) + 1 );
        wchar_t* stc = ( wchar_t* ) _GetDBResBufPtr( buff );
        str.assign( stc, len );
    }
    
    return str;
}

DBResBuf XMySqlDB::GetQueryResource( const XDBResult result, unsigned int col, unsigned row )
{
    if ( result == NULL )
    {
        DBResBuf emtpybuf;
        return emtpybuf;
    }
    
    TS_Lock lock( &( ( DBResult* ) result )->m_pQueryDB->m_XMySqlDBUtLock );
    return ( ( DBResult* ) result )->GetResult( col, row );
}




//-------------------------------------------------------------------------------------------------
// ------------------- Database result implementation ------------------
//-------------------------------------------------------------------------------------------------


DBResult::DBResult()
{
    result = NULL;
    haveError = false;
}

//-------------------------------------------------------------------------------------------------

DBResult::DBResult( MYSQL* m_mysql, tchar_t* query, int len )
{
    result = NULL;
    haveError = false;
    init( m_mysql, query, len );
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

void DBResult::init( MYSQL* mysql, tchar_t* query, int len )
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
    /* query database */
    int state = mysql_real_query( mysql, query, len );
    
    if ( state != 0 )
    {
        char buff[ 512 ];
        _stprintf( buff, "%s", mysql_error( mysql ) );
        printf( buff );
        Pf_ERROR( "XMySqlDB::init: %s", buff );
        haveError = true;
        return;
    }
    
    result = mysql_store_result( mysql );
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

bool DBResult::successful( void )
{
    return !haveError;
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
    
    return mysql_num_rows( result );
}

DBResBuf DBResult::GetResult( int col, int row )
{
    //MYSQL_FIELD fidel;
    DBResBuf buff;
    
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

tchar_t* DBResult::fieldName( int n )
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

int DBResult::fieldSize( int n )
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
    
    return field->length;
}

//-------------------------------------------------------------------------------------------------

int DBResult::fieldSize( tchar_t* name )
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

tchar_t** DBResult::getTuple()
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

tchar_t** DBResult::getTuple( int tuple )
{
    seekTuple( tuple );
    return getTuple();
}
