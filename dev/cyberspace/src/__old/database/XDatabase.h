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
#ifndef XDATABASE_H
#define XDATABASE_H

#include <vector>
#include <string>
#include <PfDef.h>


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


//typedef _tagResBuf   DBResBuf;
typedef std::vector<ubyte_t>  DBResBuf;
typedef void*                 XDBResult;

#define _GetDBResBufPtr(b) ((b.empty())? NULL: (&(b[0])))
#define _GetDBResBufLen(b) (b.size())


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

enum DB_SEARCH_DATA     {   
                            DB_TABLE_NONE           = -10,
                            DB_COLUMN_NONE,
                            DB_ROW_NONE,
                            DB_DATA_NONE
                        };

//========================================================
// Data Store
//========================================================
//struct sRow{
//  int                         size;
//  std::vector<_tstring>    data;
//};
//struct sTable{
//  int                         size;
//  std::vector<sRow>           data;
//};

//========================================================
// Class Object
//========================================================
class __SLAPI XDataBase
{
protected:
    //存放從database下載下來的資料(字串表單)
    //std::vector<sTable>           m_table;
    
    //存放資料庫連結的狀態
    //DB_CONNECT_RESULT         m_state;
    
    //儲存資料庫連結時所需的資訊
    //std::vector<_tstring>  m_db_info;
    
    //存放資料庫連結狀態的說明
    //tchar_t                       m_error[ERROR_MSG_SIZE];
    
    _tstring    m_Dbname;
    _tstring    m_Hostname;
    _tstring    m_Username;
    _tstring    m_Password;
    _tstring    m_CharSet;
    unsigned short m_Port;
    
public:

    //==========================================================================
    //給其他物件使用的interface
    //--------------------------------------------------------------------------
    //建構,解構子
    XDataBase() {};
    virtual ~XDataBase() {};
    
    //==========================================================================
    //給實體的database來重新定義
    //--------------------------------------------------------------------------
    //讀取XML檔案中,database連線所需的資訊
    //virtual bool LoadConfigure(const tchar_t* filename);
    
    //連接至database
    virtual bool            Connect             ( const tchar_t* db, const tchar_t* host, unsigned short port, const tchar_t* login, const tchar_t* pswd ) = 0;

    virtual bool            SelectDatabase      ( const tchar_t* db ) = 0;

    //結束與資料庫的連線     
    virtual bool            Disconnect          ( void ) = 0;

    void                    SetCharSet          ( const char* CharSet );
    
    //讀取資料庫內,一個table的資料,並且將資料存放至本物件中,傳回該筆資料的索引
    //virtual int LoadDatabaseTable(const tchar_t *table_name)  {   return DB_TABLE_NONE;   }
    
    //讀取資料庫內,一個table的資料,並且將資料存放至本物件中,傳回該筆資料的索引
    //virtual int UpdateDatabaseTable(const tchar_t *table_name)    {   return DB_TABLE_NONE;   }
    //==========================================================================
    
    virtual _tstring         OnEscapeString      ( const tchar_t* str, size32_t len ) = 0;
    
    //判定目前是否與資料庫有良好的連接狀態
    bool                    IsConnected         ( void ) { return false; }
    
    virtual XDBResult       ExecQuery           ( const tchar_t* query, ... ) = 0;
    
    virtual _tstring         GetQueryResult      ( const XDBResult result, unsigned int col, unsigned row ) = 0;
    virtual std::wstring    GetQueryResultW     ( const XDBResult result, unsigned int col, unsigned row ) = 0;
    virtual DBResBuf        GetQueryResource    ( const XDBResult result, unsigned int col, unsigned row ) { DBResBuf res; return res; }
    virtual void            FreeResult          ( XDBResult result ) = 0;
    
    virtual size32_t        GetFieldsSize       ( XDBResult result ) = 0;
    virtual const tchar_t*  GetFieldsName       ( XDBResult result, int n ) = 0;
    
    virtual size64_t        GetResultSize       ( XDBResult result ) = 0;
    virtual bool            GetResultEmpty      ( XDBResult result ) = 0;

    static _tstring          SafeEscapeString    ( const tchar_t* s );
    //取出目前資料庫連接的狀態說明
    //tchar_t * GetErrorMessage(void){}     // Return current error message
    //==========================================================================
};



#endif // XDATABASE_H