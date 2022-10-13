//
//
//
//
//
//
//
//        linux : Include   /usr/include/mysql
//                Link with -lmysqlclient   (mysqlclient)
//
#ifndef XMYSQLDB_H
#define XMYSQLDB_H

/******************************************************************************
 *  Class to handle MySql database connection and query.
 *
 *  Created Feb. 24 2004
 *  Karl Lee
 *****************************************************************************/
//#define __LCC__
#pragma warning(disable:4800)
#pragma warning(disable:4273)
#include <set>

//#define USE_OLD_FUNCTIONS

#include <mysql.h>
#include "XDatabase.h"
#include "T_Lock.h"


//class  Connection;
//class  Query;
//class  Result;
//class  Row;
//struct Date;
//
//
//#define BADQUERY "BadQuery Excetion"
//#define BADCONVERSION "BadConversion Exception"
//
//
////#pragma comment (lib, "libmysql.lib")


#define DB_CONNECTION_NONE      0
#define DB_CONNECTION_SUCCESS   1
#define DB_CONNECTION_FAILS     2

#define DB_COMMANDEXEC_OK       0   // OK - command executed
#define DB_EMPTY_QUERY          1   // Query didn't return tuples
#define DB_TUPLES_OK            2   // Query returned tuples
#define DB_ERROR                5
#define DB_FAILE_RESPONSE       6
#define DB_UNEXPECTED           7   // This shouldn't ever happen


class XMySqlDB : public XDataBase
{
private:
    MYSQL   m_mysql;
    bool    is_connected;
    tchar_t error[100];     // Error description
    
public:

    XMySqlDB();
    ~XMySqlDB();
    
    // Database connection
    //
    // connect: Connect to a database
    // disconnect: Close a connnection
    // connected: determine if connection is successfully built
    bool           Connect( const tchar_t* db, const tchar_t* host,  unsigned short port, const tchar_t* login, const tchar_t* pswd );
    
    bool           SelectDatabase( const tchar_t* db );
    //bool Connect(bool te);
    bool           Disconnect( void ); // delete the DBConnection obj and DBQuery obj
    bool           IsConnected( void );  // Query if connected by a database
    _tstring       OnEscapeString( const tchar_t* str, size32_t len );
    //bool Shutdown();  // Shut down the MySql server
    
    // Database query
    //
    // query:           query a database
    // quert_error:     return error messages after quering
    // queru_result:    return query results
    // GetQueryResult:  get a data from specific row and column of the query result
    // GetResource:     load full table
public:

    XDBResult      ExecQuery( const tchar_t* query, ... );
    
    _tstring        Query_error();
    
    // col = 幾個欄位   row = 第幾筆資料
    std::string    GetQueryResult( const XDBResult result, unsigned int col, unsigned row );
    std::wstring   GetQueryResultW( const XDBResult result, unsigned int col, unsigned row );
    
    size32_t       GetFieldsSize( XDBResult result );
    const tchar_t* GetFieldsName( XDBResult result, int n );
    
    size64_t       GetResultSize( XDBResult result );
    bool           GetResultEmpty( XDBResult result );
    
    
    // 取得row裡欄位的內容；不連接資料庫
    DBResBuf       GetQueryResource( const XDBResult result, unsigned int col, unsigned row );
    
    void           FreeResult( XDBResult result );
    
    
private:

    T_Lock                     m_XMySqlDBUtLock;
    
    static T_Lock              s_QueryResultLock;
    static std::set<XDBResult> s_QueryResult;
};

// 獨立的編解碼器, 可以正確的將任意資料編碼存入讀出資料庫.
// 實作方面, 嗯嗯...
// outbuff 必須至少有足夠的容納空間(請自行裁量). 皆傳回輸出長度
//int db_encode_data(const tchar_t *data,int dlen,tchar_t *encdata);
//int db_decode_data(const tchar_t *encdata,tchar_t *decbuff);




#endif