//
//
// 
// 
// 
// output sql string by execl file sample ==> xlsdbquery_sample
// form 0
//        將xlsx 格式的毒入後 建立資料表查詢SQL
//        xlsx 範例文件 xldb-query_sample.xlsx 在 source 中
//
//     xlsx file to SQL
//
// 
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <pf-def.h>
#include <vector>
#include <string>
#include <map>

namespace xldb
{
    typedef _tstring _xl_sql_string;

    class Query
    {
        _tstring                 file_name_;       // open file name
        _tstring                 sheet_name_;      // open sheet name
        //bool                     m_visible;
        int                      first_row_;
        int                      first_col_;

        std::vector<_tcsvector>  fields_;          // name, type, character_set, default, desc, X, X, comment
        _tcsvector               primary_nkey_;
        std::vector<int>         primary_ikey_;
        _tcsvector               function_;        // 放在建立後面的 函指令
        _tcsidx                  columes_index_;
        _tcsidx                  desc_index_;      // E 中文名稱
        _tstring                 character_set_;   // 編碼 預設 utf8mb4
        _tstring                 collation_;       // utf8_unicode_ci
        _tstring                 row_format_;
        _tcsvector               partition_;       // 0 by, 1 數量 other 條件

        _tcsvector               relational_key_;  // 外部索引
        _tcsvector               json_value_;      // json 檢查

    public:

        Query();
        ~Query();

        // 開發用 後面拿掉
        //std::vector<_tcsvector>  GetFieldsName() { return fields_; }

        // sheet_name NULL or "" for first sheet
        bool              Load( _IN const tchar_t* file_name, _IN const tchar_t* sheet_name );
        void              Close();
                          
        _tstring          GetPrimaryKey( _IN uint_t i );
                          
        _tstring          GetSheetName();
                          
        int               GetFieldSize();
        // get field form index
        _tstring          GetFieldName( _IN uint_t idx );
        // 資本上資料庫不能有兩個重複欄位 所以 不會有重疊的
        int_t             GetFieldIndexByName( _IN _tstring name );
        // 從 3 中文(當地)欄位取出
        int_t             GetFieldIndexByDesc( _IN _tstring fields_desc );
        // get field_name by command
        _tstring          GetFieldNameByDesc( _IN _tstring fields_desc );

        void              GetAllFieldName( _OUT _tcsvector& fields_name );
        void              GetAllFieldDesc( _OUT _tcsvector& fields_desc );

        // return a, b, c, d, e......
        _tstring          GetQueryFieldsName();

        // 依照command 對照 傳回欄位字串
        _tstring          GetQueryFieldsName( _IN const _tcsvector& fields_desc );


        //////////////////////////////////////////////////////////////////////////
        // drop if exist
        //_xl_sql_string    SQL_Drop_Table( _IN const tchar_t* tb_name, _IN const tchar_t* charset = _T( "utf8" ) );

        // 建立資料表
        _xl_sql_string    SQL_Create_Table_If_Not_Exist( _IN const tchar_t* tb_name );

        // 比對目前的 檔案 與另外讀入一個檔案，產生出修改的語法
        _xl_sql_string    SQL_Alter_Table( _IN const tchar_t* tb_name, _IN const tchar_t* old_file_name, _IN const tchar_t* old_sheet_name );

        // 原本的table 要連結的到 資料庫 該資料庫的table
        _xl_sql_string    SQL_Alter_Tabel_Relational( _IN const tchar_t* ftb_name, _IN const tchar_t* ldb_name, _IN const tchar_t* ltb_name );
        _xl_sql_string    SQL_Alter_Table_Key_Auto_Increment( _IN const tchar_t* ftb_name );
        _xl_sql_string    SQL_Alter_Table_Drop_Primary_Key( _IN const tchar_t* ftb_name );



        //void     GetSetSyncCondition( _OUT std::vector<UINT>& values );
        //_tstring GetSetSyncFieldname();
        //void     GetGetSyncCondition( _OUT std::vector<UINT>& values );
        //_tstring GetGetSyncFieldname();

        //_tstring Select_Table_From_Order_By( _IN const tchar_t* tname, _IN const tchar_t* order_field_name, _IN const tchar_t* order_type = _T( "ASC" ) );
        //_tstring Select_Table_From_Where_Order_By( _IN const tchar_t* tname, _IN const _tcsvector& condition,  _IN const tchar_t* order_field_name , _IN const tchar_t* order_type = _T( "ASC" ) );
        //_tstring Replace_Into( _IN const tchar_t* tname, _IN const _tcsvector& values );
    };
}
