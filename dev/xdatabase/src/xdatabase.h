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
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <string>
#include <vector>
#include <map>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


//========================================================
// Define
//========================================================

#ifdef MYDLL_EXPORTS
    #define MYDLL_API __declspec(dllexport)
#else
    #define MYDLL_API __declspec(dllimport)
#endif


#define ERROR_MSG_SIZE  100
#define TRY_CONNECTION  10


//typedef _tagResBuf   XDBResBuf;
typedef std::vector<ubyte_t>  XDBResBuf;
typedef void*                 XDBResult;

#define _GetXDBResBufPtr(b) ((b.empty())? NULL: (&(b[0])))
#define _GetXDBResBufLen(b) (b.size())


typedef std::string          _xdb_sql_mstring;
typedef std::wstring         _xdb_sql_wstring;

#ifdef _UNICODE
    #define _xdb_sql_tstring    _xdb_sql_wstring
#else
    #define _xdb_sql_tstring    _xdb_sql_mstring
#endif


//========================================================
// Enumerable
//========================================================
enum DB_CONNECT_RESULT {    
                            DB_CONNECTION_FAILS     = -1,
                            DB_CONNECTION_NONE,
                            DB_SERVER_INFO_ERROR,
                            DB_CONNECTION_LINKING,
                            DB_CONNECTION_SUCCESS,
                            DB_CONNECTION_MISS,
                            DB_SERVER_QUIT
                       };

enum DB_SEARCH_DATA    {   
                            DB_TABLE_NONE           = -10,
                            DB_COLUMN_NONE,
                            DB_ROW_NONE,
                            DB_DATA_NONE
                       };

#define DBRET_SUCCESSFUL  0

//========================================================
// Data Store
//========================================================
//struct sRow{
//  int                         size;
//  std::vector<_mstring>       data;
//};
//struct sTable{
//  int                         size;
//  std::vector<sRow>           data;
//};

//========================================================
// Class Object
//========================================================
class XDatabase
{
protected:
    //存放從database下載下來的資料(字串表單)
    //std::vector<sTable>           m_table;
    
    //存放資料庫連結的狀態
    //DB_CONNECT_RESULT             m_state;
    
    //儲存資料庫連結時所需的資訊
    //std::vector<_mstring>         m_db_info;
    
    //存放資料庫連結狀態的說明
    //char_t                        m_error[ERROR_MSG_SIZE];
    
    _mstring       db_code_;
    _mstring       db_name_;
    _mstring       host_name_;
    unsigned short host_port_;
    _mstring       username_;
    _mstring       password_;
    _mstring       charset_;
    
public:

    //==========================================================================
    //給其他物件使用的interface
    //--------------------------------------------------------------------------
    //建構,解構子
    XDatabase();
    virtual ~XDatabase();

    virtual const char_t*    ObjectName() const  = 0;
    
    //==========================================================================
    //給實體的database來重新定義
    //--------------------------------------------------------------------------
    //讀取XML檔案中,database連線所需的資訊
    //virtual bool LoadConfigure(const char_t* filename);
    
    //連接至database
    virtual bool             Connect( _IN const char_t* db, _IN const char_t* host, _IN unsigned short port, _IN const char_t* username, _IN const char_t* password ) = 0;
    bool                     Connect( _IN const wchar_t* db, _IN const wchar_t* host, _IN unsigned short port, _IN const wchar_t* username, _IN const wchar_t* password );
                            
    //結束與資料庫的連線       
    virtual bool             Disconnect( _IN void ) = 0;

    virtual bool             SelectDatabase( _IN const char_t* db ) = 0;
    bool                     SelectDatabase( _IN const wchar_t* db );

    virtual bool             SetCharSet( _IN const char_t* char_set ) = 0;
    bool                     SetCharSet( _IN const wchar_t* char_set );
                                
    
    //讀取資料庫內,一個table的資料,並且將資料存放至本物件中,傳回該筆資料的索引
    //virtual int LoadDatabaseTable(const char_t *table_name)  {   return DB_TABLE_NONE;   }
    
    //讀取資料庫內,一個table的資料,並且將資料存放至本物件中,傳回該筆資料的索引
    //virtual int UpdateDatabaseTable(const char_t *table_name)    {   return DB_TABLE_NONE;   }
    //==========================================================================
    
    virtual _mstring         OnEscapeString      ( _IN const char_t* str, _IN size32_t len ) = 0;
     _wstring                OnEscapeString      ( _IN const wchar_t* str, _IN size32_t len );
    
    //判定目前是否與資料庫有良好的連接狀態
    bool                     IsConnected         ( void ) { return false; }
                             
    // 該物件編號             
    virtual void             SetDBCode           ( _IN _mstring code ) final;
    virtual _mstring         GetDBCode           ( void ) final;
                             
    _mstring                 GetDBNameM          ( void );
    _wstring                 GetDBNameW          ( void );
                             
    virtual XDBResult        ExecQuery           ( _IN const char_t* query ) = 0;
    XDBResult                ExecQuery           ( _IN const wchar_t* query );
                             
    //DBRET                  
    virtual int              GetQueryErrNo       ( _IN const XDBResult result ) = 0;
    virtual _mstring         GetQueryErrMsgM     ( _IN const XDBResult result ) = 0;
    virtual _wstring         GetQueryErrMsgW     ( _IN const XDBResult result );

    virtual bool             GetQuerySuccessful  ( _IN const XDBResult result ) = 0;
    // 有多少筆資料被改變
    virtual size64_t         GetQueryAffectedRow ( _IN const XDBResult result ) = 0;
                             
    virtual _mstring         GetQueryResultM     ( _IN const XDBResult result, _IN uint_t col, _IN uint_t row ) = 0;
    _mstring                 GetQueryResultM     ( _IN const XDBResult result, _IN _mstring col_name, _IN uint_t row );
    _wstring                 GetQueryResultW     ( _IN const XDBResult result, _IN uint_t col, _IN uint_t row );
    _wstring                 GetQueryResultW     ( _IN const XDBResult result, _IN _wstring col_name, _IN uint_t row );
                             
    virtual XDBResBuf        GetQueryResource    ( _IN const XDBResult result, _IN uint_t col, _IN uint_t row ) { XDBResBuf res; return res; }

    virtual void             FreeResult          ( _IN XDBResult& result ) = 0;
                             
    virtual size32_t         GetFieldsSize       ( _IN XDBResult result ) = 0;
    virtual _mstring         GetFieldsNameM      ( _IN XDBResult result, uint_t n ) = 0;
    _wstring                 GetFieldsNameW      ( _IN XDBResult result, uint_t n );
    virtual uint_t           GetFieldIndex       ( _IN XDBResult result, const char* col_name ) = 0;
    uint_t                   GetFieldIndex       ( _IN XDBResult result, const wchar_t* col_name );
                             
    virtual size64_t         GetResultSize       ( _IN XDBResult result ) = 0;
    virtual bool             GetResultEmpty      ( _IN XDBResult result ) = 0;

    // map< string, string >
    // ret[ field_name ] => value
    _mbsmap                  GetResultRecordM    ( _IN const XDBResult result, _IN uint_t row );
    _wcsmap                  GetResultRecordW    ( _IN const XDBResult result, _IN uint_t row );
    // vector( xxxxx[] ) map[ "field_name "] 
    // ret[ field_name ][ row ]  => value
    _mbstablesi              GetResultTableM     ( _IN const XDBResult result );
    _wcstablesi              GetResultTableW     ( _IN const XDBResult result );
    //std::vector<_mbsmap>    GetResultMMap               ( const XDBResult result );
    //std::vector<_wcsmap>    GetResultWMap               ( const XDBResult result );

    static _mstring          SafeEscapeString    ( _IN const char_t* s );
    static _wstring          SafeEscapeString    ( _IN const wchar_t* s );
                             
    void                     AppendStringTable   ( _OUT _mbstableii& retult_table, _IN XDBResult result );
    void                     AppendStringTable   ( _OUT _wcstableii& retult_table, _IN XDBResult result );
    //取出目前資料庫連接的狀態說明
    //char_t * GetErrorMessage(void){}     // Return current error_ message
    //==========================================================================
};

#ifdef _UNICODE
    #define GetDBNameT           GetDBNameW
    #define ExecQueryT           ExecQuery
    #define GetQueryResultT      GetQueryResultW
    #define GetFieldsNameT       GetFieldsNameW
    #define GetResultRecordT     GetResultRecordW
    #define GetResultTableT      GetResultTableW
    #define GetQueryErrMsgT      GetQueryErrMsgW
#else
    #define GetDBNameT           GetDBNameM
    #define ExecQueryT           ExecQuery
    #define GetQueryResultT      GetQueryResultM
    #define GetFieldsNameT       GetFieldsNameM
    #define GetResultRecordT     GetResultRecordM
    #define GetResultTableT      GetResultTableM
    #define GetQueryErrMsgT      GetQueryErrMsgM
#endif

//#define SafeFreeResult( r )  FreeResult( r ); r = NULL