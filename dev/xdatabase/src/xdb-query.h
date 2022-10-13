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
#include <vector>
#include <map>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "xdatabase.h"


namespace xdb
{
    namespace query
    {
        //////////////////////////////////////////////////////////////////////////
        // 
        namespace inquire
        {
            // 檢查這個資料表存不存在
            bool                 IfTableExist( _IN XDatabase* p_xdb, _IN const char_t* table_name );
            bool                 IfTableExist( _IN XDatabase* p_xdb, _IN const wchar_t* table_name );
                                
            void                 DropTableIfExistt( _IN XDatabase* p_xdb, _IN const char_t* table_name );
            void                 DropTableIfExistt( _IN XDatabase* p_xdb, _IN const wchar_t* table_name );

            // 將不存在的資料表 從vector 移除掉
            void                 FilterRmoveNotExistTable( _INOUT _mbsvector& table_names, _IN XDatabase* p_xdb );
            void                 FilterRmoveNotExistTable( _INOUT _wcsvector& table_names, _IN XDatabase* p_xdb );

            // Select 如果存在的畫
            XDBResult            SelectIfTableExist( _IN XDatabase* p_xdb, _IN const char_t* colums, _IN const char_t* table_name, _IN const char_t* condition );
            XDBResult            SelectIfTableExist( _IN XDatabase* p_xdb, _IN const wchar_t* colums, _IN const wchar_t* table_name, _IN const wchar_t* condition );

            // 選擇所有 名稱 Like 條件的table 傳回table 數量
            size64_t             SelectAllTableNames( _OUT _mbsvector& table_names, _IN XDatabase* p_xdb, _IN const char_t* name_keyword );
            size64_t             SelectAllTableNames( _OUT _wcsvector& table_names, _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword );

            // 取得表 建立的時間
            // return time
            _mstring             SelectTableCreateTime( _IN XDatabase* p_xdb, _IN const char_t* table_name );
            _wstring             SelectTableCreateTime( _IN XDatabase* p_xdb, _IN const wchar_t* table_name );

            // 取得表 資訊
            // `TABLE_CATALOG` `TABLE_SCHEMA` `TABLE_NAME` `TABLE_TYPE` `ENGINE` `VERSION` `ROW_FORMAT` `TABLE_ROWS` `AVG_ROW_LENGTH` 
            // `DATA_LENGTH` `MAX_DATA_LENGTH` `INDEX_LENGTH` `DATA_FREE` `AUTO_INCREMENT` `CREATE_TIME` `UPDATE_TIME` `CHECK_TIME` `TABLE_COLLATION` 
            // `CHECKSUM` `CREATE_OPTIONS` `TABLE_COMMENT` `MAX_INDEX_LENGTH` `TEMPORARY`
            // `COLLATION_NAME` `CHARACTER_NAME`
            _mbsmap              SelectTableInformation( _IN XDatabase* p_xdb, _IN const char_t* table_name );
            _wcsmap              SelectTableInformation( _IN XDatabase* p_xdb, _IN const wchar_t* table_name );

            // 取得表 欄位資訊 schema
            // `TABLE_CATALOG TABLE_SCHEMA` `TABLE_NAME COLUMN_NAME` `ORDINAL_POSITION` `COLUMN_DEFAULT` `IS_NULLABLE DATA_TYPE` `CHARACTER_MAXIMUM_LENGTH
            // `CHARACTER_OCTET_LENGTH` `NUMERIC_PRECISION` `NUMERIC_SCALE` `DATETIME_PRECISION` `CHARACTER_SET_NAME` `COLLATION_NAME` `COLUMN_TYPE
            // `COLUMN_KEY` `EXTRA` `PRIVILEGES` `COLUMN_COMMENT` `IS_GENERATED` `GENERATION_EXPRESSION
            _mbstablesi          SelectTableColumn( _IN XDatabase* p_xdb, _IN const char_t* table_name );
            _wcstablesi          SelectTableColumn( _IN XDatabase* p_xdb, _IN const wchar_t* table_name );

            // 取得表 最後更新的時間
            // return time
            _mstring             SelectTableUpdateTime( _IN XDatabase* p_xdb, _IN const char_t* table_name );
            _wstring             SelectTableUpdateTime( _IN XDatabase* p_xdb, _IN const wchar_t* table_name );

            // 最早建立的表
            // return table_name
            _mstring             SelectFirstTableNameByCreate( _IN XDatabase* p_xdb, _IN const char_t* name_keyword );
            _wstring             SelectFirstTableNameByCreate( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword );

            // 最晚建立的表
            // return table_name
            _mstring             SelectLastTableNameByCreate( _IN XDatabase* p_xdb, _IN const char_t* name_keyword );
            _wstring             SelectLastTableNameByCreate( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword );

            // 名字排序建立第一的表
            // return table_name
            _mstring             SelectFirstTableNameByName( _IN XDatabase* p_xdb, _IN const char_t* name_keyword );
            _wstring             SelectFirstTableNameByName( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword );

            // 名字排序建立最後的表
            // return table_name
            _mstring             SelectLastTableNameByName( _IN XDatabase* p_xdb, _IN const char_t* name_keyword );
            _wstring             SelectLastTableNameByName( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword );

            // 取得建立 table 的 SQL 語法
            // get create table sql string
            _xdb_sql_mstring     ShowCreateTableSQL( _IN XDatabase* p_xdb, _IN const char_t* table_name );
            _xdb_sql_wstring     ShowCreateTableSQL( _IN XDatabase* p_xdb, _IN const wchar_t* table_name );

            // 顯示索引 
            // primary key => "PRI"
            // UNIQUE      => "UNI"
            // INDEX       => 
            // include auto_increment
            _mstring_vector      SelectTableIndexes( _IN XDatabase* p_xdb, _IN const char_t* table_name, _IN const char_t* key = "PRI", _IN bool include_auto = true );
            _wstring_vector      SelectTableIndexes( _IN XDatabase* p_xdb, _IN const wchar_t* table_name, _IN const wchar_t* key = L"PRI", _IN bool include_auto = true );

        }

        //////////////////////////////////////////////////////////////////////////
        // 句法
        namespace syntax
        {
            // 建立跨資料表 聯集的字串
            // SELECT XXXX FROM AAAA WHERE
            // UNION ALL
            // SELECT XXXX FROM BBBB WHERE
            //.....
            // 加入 record_from_table_name 放在最後一個欄位
            _mstring             SelectFromWhereUnionAll( _IN const char_t* expression, _IN const _mbsvector& reference_tables, _IN const char_t* condition = NULL );
            _wstring             SelectFromWhereUnionAll( _IN const wchar_t* expression, _IN const _wcsvector& reference_tables, _IN const wchar_t* condition = NULL );

            // 用map 產出 insert 的語法
            _mstring             INSERT_INTO( _IN char_t* table_name, _IN const _mbsmap* fv );
            _wstring             INSERT_INTO( _IN wchar_t* table_name, _IN const _wcsmap* fv );

            // 用map 產出 update 的語法
            _mstring             UPDATE_SET( _IN char_t* table_name, _IN const _mbsmap* fv, _IN char_t* where_clause );
            _wstring             UPDATE_SET( _IN wchar_t* table_name, _IN const _wcsmap* fv, _IN wchar_t* where_clause );
        }
    }
}