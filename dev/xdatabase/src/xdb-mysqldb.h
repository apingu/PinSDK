//
//
//
//
//
//
//
//        linux : Include   /usr/include/mysql
//                locate     mysqlclient   => 將資料夾位置加入
//                Link with -lmysqlclient   (mysqlclient)
//                crypto;ssl;dl;mariadbclient     (會因為順序 產生undefined reference 多嘗試改變幾種順序)
//                              mariadb           (centos8) 再執行檔中link 
//                要安裝 lib  sudo yum install mysql-devel
//                           sudo yum install openssl-devel
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//#define __LCC__
#pragma warning(disable:4800)
#pragma warning(disable:4273)

//01.== C 系統文件/C++ 系統文件
#include <set>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
//#define USE_OLD_FUNCTIONS
#include "xdatabase.h"


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
#ifdef _WINDOWS
//#pragma comment ( lib, "libmysql.lib" )
#endif

#if defined( _LINUX )
    #define __MYSQL st_mysql 
#else
    #define __MYSQL MYSQL
#endif

class  T_Lock;
struct __MYSQL;

#define DB_CONNECTION_NONE      0
#define DB_CONNECTION_SUCCESS   1
#define DB_CONNECTION_FAILS     2

#define DB_COMMANDEXEC_OK       0   // OK - command executed
#define DB_EMPTY_QUERY          1   // Query didn't return tuples
#define DB_TUPLES_OK            2   // Query returned tuples
#define DB_ERROR                5
#define DB_FAILE_RESPONSE       6
#define DB_UNEXPECTED           7   // This shouldn't ever happen

namespace xdb
{
    class MySqlDB : public XDatabase
    {
    private:

        __MYSQL*            p_mysql_;
        T_Lock*             p_xmysql_lock_;
        bool                is_connected_;
        char_t              error_[ 100 ];     // Error description
        std::set<XDBResult> query_result_cntr_;

    public:

        MySqlDB();
        ~MySqlDB();

        const char_t*   ObjectName() const { return "XdbMySQL"; }

        // Database connection
        //
        // connect: Connect to a database
        // disconnect: Close a connnection
        // connected: determine if connection is successfully built
        bool            Connect( _IN const char_t* db, _IN const char_t* host, _IN unsigned short port, _IN const char_t* username, _IN const char_t* password );

        bool            SetCharSet( _IN const char_t* CharSet );

        bool            SelectDatabase( _IN const char_t* db );
        //bool Connect(bool te);
        bool            Disconnect( void ); // delete the DBConnection obj and DBQuery obj
        bool            IsConnected( void );  // Query if connected by a database
        _mstring        OnEscapeString( _IN const char_t* str, _IN size32_t len );
        //bool Shutdown();  // Shut down the MySql server

        // Database query
        //
        // query:           query a database
        // quert_error:     return error_ messages after quering
        // queru_result:    return query results
        // GetQueryResultT:  get a data from specific row and column of the query result
        // GetResource:     load full table
    public:

        //XDBResult      ExecQuery( const char_t* query, ... );
        XDBResult       ExecQuery( _IN const char_t* query );
        XDBResult       ExecQuery( _IN const wchar_t* query );

        _mstring        Query_error();

        int             GetQueryErrNo( _IN const XDBResult result );  // 0 sucessful
        _mstring        GetQueryErrMsgM( _IN const XDBResult result );  // 0 sucessful
        bool            GetQuerySuccessful( _IN const XDBResult result );
        size64_t        GetQueryAffectedRow( _IN const XDBResult result );

        // col = 幾個欄位   row = 第幾筆資料
        _mstring        GetQueryResultM( _IN const XDBResult result, _IN uint_t col, _IN uint_t row );
        //_wstring        GetQueryResultW( const XDBResult result, unsigned int col, unsigned row );

        size32_t        GetFieldsSize( _IN XDBResult result );
        _mstring        GetFieldsNameM( _IN XDBResult result, _IN uint_t n );
        uint_t          GetFieldIndex( _IN XDBResult result, const char* col_name );

        size64_t        GetResultSize( _IN XDBResult result );
        bool            GetResultEmpty( _IN XDBResult result );


        // 取得row裡欄位的內容；不連接資料庫
        XDBResBuf       GetQueryResource( _IN const XDBResult result, _IN uint_t col, _IN uint_t row );
        void            FreeResult( _IN XDBResult& result );
    };

    // 獨立的編解碼器, 可以正確的將任意資料編碼存入讀出資料庫.
    // 實作方面, 嗯嗯...
    // outbuff 必須至少有足夠的容納空間(請自行裁量). 皆傳回輸出長度
    //int db_encode_data(const char_t *data,int dlen,char_t *encdata);
    //int db_decode_data(const char_t *encdata,char_t *decbuff);

}