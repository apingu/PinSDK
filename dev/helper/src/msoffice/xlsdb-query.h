//
//
// output sql string by execl file sample ==> xlsdbquery_sample
// form 0
#pragma once

#include <pf-def.h>
#include <vector>
#include <string>
#include <map>

namespace xlsdb
{
    typedef _tstring _SQLstring;

    class Query
    {
        _tstring                 file_name_;                                               // open file name
        _tstring                 sheet_name_;                                              // open sheet name
        bool                     is_visible_;
        int                      m_b_row;
        int                      m_b_col;

        std::vector<_tcsvector>  fields_name_;
        _tcsvector               primary_nkey_;
        std::vector<int>         primary_ikey_;
        _tcsvector               function_;      // 放在建立後面的 函指令
        _tcsidx                  columes_index_;
        _tcsidx                  command_index_;   // D欄位附註 (中文名稱)


        _tcsvector               index_key_;      // 外部索引
        _tcsvector               json_value_;     // json 檢查
        //std::vector<UINT> m_setsync_keyid;  // 外部同步
        //_tcsvector   m_setsync_value;  // 外部同步
        //std::vector<UINT> m_getsync_keyid;  // 外部同步
        //_tcsvector   m_getsync_value;  // 外部同步


        //_tcsvector               m_query_string;

    public:

        Query();
        ~Query();

        bool          Load( _IN const TCHAR* fname, _IN const TCHAR* sname );
        void          Close();


        _tstring      GetPrimaryKey( _IN uint_t i );

        _tstring      GetSheetname();

        int           GetFieldSize();
        // get field form index
        _tstring      GetFieldname( _IN uint_t idx );
        // 資本上資料庫不能有兩個重複欄位 所以 不會有重疊的
        int_t         GetFieldindexByName( _IN _tstring name );
        int_t         GetFieldindexByComm( _IN _tstring comm );
        // get fieldname by command
        _tstring      GetFieldnameByComm( _IN _tstring command );

        void          GetAllFieldname( _OUT _tcsvector& fieldsname );
        void          GetAllCommends( _OUT _tcsvector& commends );

        // return a, b, c, d, e......
        _tstring      GetQueryFieldsname();

        // 依照command 對照 傳回欄位字串
        _tstring      GetQueryFieldsname( _IN const _tcsvector& commands );


        //////////////////////////////////////////////////////////////////////////
        // drop if exist
        _SQLstring    SQL_Drop_Table( _IN const TCHAR* tbname, _IN const TCHAR* charset = _T( "utf8" ) );

        // 建立資料表
        _SQLstring    SQL_Create_Table_If_Not_Exist( _IN const TCHAR* tbname, _IN const TCHAR* charset = _T( "utf8" ) );

        // 原本的table 要連結的到 資料庫 該資料庫的table
        _SQLstring    SQL_Alter_Tabel_Index( _IN const TCHAR* ftbname, _IN const TCHAR* ldbname, _IN const TCHAR* ltbname );
        _SQLstring    SQL_Alter_Table_KeyAutoIncrement( _IN const TCHAR* ftbname );
        _SQLstring    SQL_Alter_Table_DropPrimaryKey( _IN const TCHAR* ftbname );


        //void     GetSetSyncCondition( _OUT std::vector<UINT>& values );
        //_tstring GetSetSyncFieldname();
        //void     GetGetSyncCondition( _OUT std::vector<UINT>& values );
        //_tstring GetGetSyncFieldname();

        //_tstring Select_Table_From_Order_By( _IN const TCHAR* tname, _IN const TCHAR* order_field_name, _IN const TCHAR* order_type = _T( "ASC" ) );
        //_tstring Select_Table_From_Where_Order_By( _IN const TCHAR* tname, _IN const _tcsvector& condition,  _IN const TCHAR* order_field_name , _IN const TCHAR* order_type = _T( "ASC" ) );
        //_tstring Replace_Into( _IN const TCHAR* tname, _IN const _tcsvector& values );

    };
}
