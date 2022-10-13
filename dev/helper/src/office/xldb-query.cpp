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
//01. == C 系統文件/C++ 系統文件
#ifdef _WINDOWS
#include <windows.h>
#endif

#include <algorithm>
//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <gadget/sentence_pattern.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xl_file.h"
#include "xldb-query.h"


namespace xldb
{
    enum field_define
    {
        FIELD_NAME = 0,           // 本欄位的名稱
        FIELD_TYPE = 1,           // 本欄位的資料類型
        FIELD_CHARACTER_SET = 2,  // 本欄位的編碼
        FIELD_DEFAULT = 3,        // 本欄位的預設值
        FIELD_DESCRIPTION = 4,    // 本欄位的欄位說明(中文名)
        FIELD_INDEX = 5,          // 本欄位的索引直 (空白)
        FIELD_FUNCTION = 6,       // 本欄位的函數 (空白)
        FIELD_COMMENT = 7,        // 本欄位的註解
        FIELD_RESERIVC01 = 8,     // 本欄位解保留
        FIELD_RESERIVC02 = 9,     // 本欄位的保留
        FIELD_PREV_NAME = 10,     // 上一個欄位的名稱
        MAX_FIELD_SIZE = 11,
    };

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Query::Query()
    {
        first_row_ = 2;
        first_col_ = 1;
        character_set_ = _T( "utf8mb4" );
        collation_ = _T( "utf8mb4_unicode_ci" );
        row_format_ = _T( "Dynamic" );
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Query::~Query()
    {
        fields_.clear();
        desc_index_.clear();
        //m_query_string.clear();
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Query::Load( _IN const tchar_t* file_name, const _IN tchar_t* sheet_name )
    {
        office::XlFile* xlsf = new office::XlFile;
        if( !xlsf->Load( file_name ) )
        {
            file_name_ = _T( "" );
            SAFE_DELETE( xlsf );
            return false;
        }

        file_name_ = file_name;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            sheet_name_ = xlsf->SelectSheet( 1 )->GetSheetName();
        }
        else
        {
            if( xlsf->FindSheetIndex( sheet_name ) == -1 )
            {
                return false;
            }
            xlsf->SelectSheet( sheet_name );
            sheet_name_ = sheet_name;
        }

        // 檢查格式
        {
            //_tstring sa1 = xlsf->SelectCell( _T( "A1" ) )->GetCell2CStr();
            //_tstring sb1 = xlsf->SelectCell( _T( "B1" ) )->GetCell2CStr();
            //_tstring sc1 = xlsf->SelectCell( _T( "C1" ) )->GetCell2CStr();
            //_tstring sd1 = xlsf->SelectCell( _T( "D1" ) )->GetCell2CStr();
            //_tstring se1 = xlsf->SelectCell( _T( "E1" ) )->GetCell2CStr();

            //if( ( sa1 != _T( "欄位名稱" ) ) || ( sb1 != _T( "資料類型" ) ) || ( sc1 != _T( "編碼" ) ) || ( sd1 != _T( "預設值" ) ) || ( se1 != _T( "欄位說明" ) ) )
            //{
            //    if( ( sa1 != _T( "Name" ) ) || ( sb1 != _T( "Type" ) ) || ( sc1 != _T( "Character Set" ) ) || ( sd1 != _T( "Default" ) ) || ( se1 != _T( "Description" ) ) )
            //    {
            //        return false;
            //    }
            //}
            _tstring check_code = xlsf->SelectCell( _T( "O1" ) )->GetCell2CStr();
            if( check_code != _T( "xdb_query_file_format" ) )
            {
                return false;
            }
        }

        // 將fiels與對映的col index填入
        //int b_row = first_row_;
        //int b_col = first_col_;
        //int max_col = xlsf->GetColTotalCell();
        int max_col = (int)field_define::MAX_FIELD_SIZE;
        int max_row = xlsf->GetRowTotalCell();
        {
            mstimen_t currt = pftml::Time::GetTickMSTime();

            //_TCHAR keybuff[_MAX_STRING_LEN];
            // 從第2列開始
            for( int trow = first_row_; trow <= max_row; trow++ )
            {
                _tcsvector row_data;

                {
                    _tstring cell_data = xlsf->SelectCell( field_define::FIELD_NAME + 1, trow )->GetCell2CStr();
                    if( cell_data == _T( "" ) )
                    {
                        continue;
                    }

                    // 欄位數值
                    row_data.resize( field_define::MAX_FIELD_SIZE );
                    {
                        row_data[ field_define::FIELD_NAME] = cell_data;
                        for( int tcol = 2; tcol < max_col; tcol++ )
                        {
                            cell_data = xlsf->SelectCell( tcol, trow )->GetCell2CStr();
                            row_data[ tcol - 1 ] = cell_data;
                        }
                    }
                }

                // 檢查 type
                {
                    _tstring cell_data = row_data[ field_define::FIELD_TYPE ];
                    if( _tcscasestr( cell_data.c_str(), _T( "json" ) ) != 0 )
                    {
                        json_value_.push_back( row_data[ field_define::FIELD_NAME ] );
                        // json 不能 設定編碼?
                        row_data[ field_define::FIELD_CHARACTER_SET ] = _T( "" );
                    }
                }

                // 檢查 3 預設值
                {
                    //_tstring celldata = xlsf->GetCellRange2CStr( 3, trow );
                    // primary key
                    //if( row_data[ 2 ] == _T( "PRIMARY KEY" ) )
                    // 要放到最後
                    // CONSTRAINT XXXX_PRIMARY_KEY PRIMARY KEY ( XXXX, XXXX ) 不然 double key 時會建立不了 
                    if( row_data[ field_define::FIELD_DEFAULT ].find( _T( "PRIMARY KEY" ) ) != std::string::npos )
                    {
                        //row_data[ 2 ] = _T( "PRIMARY KEY" );
                        // 清掉原本有 primary key的地方
                        pfstd::strreplaceall( row_data[ field_define::FIELD_DEFAULT ], _T( "PRIMARY KEY" ), _T( "" ) );
                        primary_nkey_.push_back( row_data[ field_define::FIELD_NAME ] );
                        primary_ikey_.push_back( trow - first_row_ );
                    }
                    else if( row_data[ field_define::FIELD_DEFAULT ] == _T( "AUTO_INCREMENT" ) )
                    {
                        row_data[ field_define::FIELD_DEFAULT ] = _T( "PRIMARY KEY AUTO_INCREMENT" );
                        primary_nkey_.insert( primary_nkey_.begin(), row_data[ field_define::FIELD_NAME ] );
                        primary_ikey_.insert( primary_ikey_.begin(), ( trow - 1 ) );
                    }
                    else if( row_data[ field_define::FIELD_DEFAULT ] == _T( "" ) )
                    {
                        _pf_tstrprintf( row_data[ field_define::FIELD_DEFAULT ], _T( "DEFAULT NULL" ) );
                    }
                    else if( row_data[ field_define::FIELD_DEFAULT ] == _T( "NOT NULL" ) )
                    {
                    }
                    else
                    {
                        if( ( row_data[ field_define::FIELD_DEFAULT ].front() == _T( '\"' ) ) && ( row_data[ 3 ].back() == _T( '\"' ) ) )
                        {
                            row_data[ field_define::FIELD_DEFAULT ].front() = _T( '\'' );
                            row_data[ field_define::FIELD_DEFAULT ].back() = _T( '\'' );
                            _pf_tstrprintf( row_data[ field_define::FIELD_DEFAULT ], _T( "DEFAULT %s" ), row_data[ field_define::FIELD_DEFAULT ].c_str() );
                        }
                        else
                        {
                            _pf_tstrprintf( row_data[ field_define::FIELD_DEFAULT ], _T( "DEFAULT %s" ), row_data[ field_define::FIELD_DEFAULT ].c_str() );
                        }
                    }
                }

                // 索引值欄位
                {
                    _tstring celldata = xlsf->SelectCell( 6, trow )->GetCell2CStr();
                    tchar_t* buff = new tchar_t[ celldata.size() + 1 ];
                    _tcscpy( buff, celldata.c_str() );

                    tchar_t seps[] = _T( "," );
                    tchar_t* token = NULL;
                    tchar_t* next_token = NULL;

                    token = _pf_tcstok_rc( buff, seps, &next_token );
                    while( token )
                    {
                        // 外部連結的key
                        if( _tcsstr( token, _T( "relational_key" ) ) != 0 )
                        {
                            relational_key_.push_back( row_data[ field_define::FIELD_NAME ] );
                        }
                        // json 格式檢查
                        else if( _tcsstr( token, _T( "json" ) ) != 0 )
                        {
                            json_value_.push_back( row_data[ field_define::FIELD_NAME ] );
                        }
                        //else if ( _tcsstr( token, _T( " getsync key" ) ) != 0 )
                        //{
                        //    m_setsync_keyid.push_back( trow );
                        //}
                        //else if ( _tcsstr( token, _T( " getsync value" ) ) != 0 )
                        //{
                        //    m_getsync_value.push_back( row_data[0] );
                        //}
                        //else if ( _tcsstr( token, _T( " settsync key" ) ) != 0 )
                        //{
                        //    m_setsync_keyid.push_back( trow );
                        //}
                        //else if ( _tcsstr( token, _T( " setsync value" ) ) != 0 )
                        //{
                        //    m_setsync_value.push_back( row_data[0] );
                        //}
                        token = _pf_tcstok_rc( NULL, seps, &next_token );
                    }
                    SAFE_DELETE_ARRAY( buff );
                }

                // 呼叫的函式 G
                {
                    _tstring celldata = xlsf->SelectCell( 7, trow )->GetCell2CStr();
                    if( celldata != _T( "" ) )
                    {
                        function_.push_back( celldata );
                    }
                }
                fields_.push_back( row_data );
                desc_index_.insert( _tcsidx::value_type( row_data[ field_define::FIELD_DESCRIPTION ], trow - 1 ) );
                columes_index_.insert( _tcsidx::value_type( row_data[ field_define::FIELD_NAME ], trow - 1 ) );
            }

            // 編碼
            _tstring character_set = xlsf->SelectCell( _T( "M2" ) )->GetCell2CStr();
            if( !character_set.empty() )
            {
                character_set_ = pfstd::strtolower( character_set );
            }
            _tstring collate = xlsf->SelectCell( _T( "M3" ) )->GetCell2CStr();
            if( !collate.empty() )
            {
                collation_ = pfstd::strtolower( collate );
            }
            _tstring row_format = xlsf->SelectCell( _T( "M4" ) )->GetCell2CStr();
            if( !row_format.empty() )
            {
                row_format_ = row_format;
            }

            partition_.push_back( xlsf->SelectCell( _T( "M10" ) )->GetCell2CStr() );
            partition_.push_back( xlsf->SelectCell( _T( "M11" ) )->GetCell2CStr() );
            partition_.push_back( xlsf->SelectCell( _T( "M12" ) )->GetCell2CStr() );

            mstimen_t currf = pftml::Time::GetTickMSTime() - currt;
#ifdef _WINDOWS
            {
                tchar_t buff[ 1024 ];
                _stprintf_s( buff, _T( "xlsdb::Query load table %s:%s row = %d spend time: %lld\n" ), file_name_.c_str(), sheet_name_.c_str(), max_row, currf );
                OutputDebugString( buff );
            }
#endif
        }

        if( fields_.empty() )
        {
            SAFE_DELETE( xlsf );
            return false;
        }


        // 校正資料
        // 01  如果type是 json
        //     a. type 會被改為 longtext
        //     b. COLLATE 會被改為 longtext XXXX_bin
        // 02 如果 character set 是空 且 type 是文字類型 
        //     a. 文字類型 就填 預設
        //     b. json 就變成 _bin
        // 03  補充前一個欄位的名稱
        {
            for( uint_t fi = 0; fi < fields_.size(); fi++ )
            {
                // 因為 有兼容的空白字元 乾脆拿掉 char(20)  char( 20 )
                // 資料類型
                _tstring field_type = pfstd::strtolower( fields_[ fi ][ field_define::FIELD_TYPE ] );

                // 因為 如果是 項 josn 或 有其他類型再傳入資料庫時 會被轉換 所以 先記錄
                // json 會在資料庫被改寫為 longtext
                if( field_type == _T( "json" ) )
                {
                    fields_[ fi ][ field_define::FIELD_TYPE ] = _T( "longtext" );
                }

                _tstring character_set = pfstd::strtolower( fields_[ fi ][ field_define::FIELD_CHARACTER_SET ] );
                // 沒有寫character set 用 default
                if( character_set.empty() )
                {
                    // 字串 或 字元 類型
                    // CHAR VARCHAR TEXT LONGTEXT TINYTEXT........
                    if( ( pfstd::strcasefind( fields_[ fi ][ field_define::FIELD_TYPE ].c_str(), _T( "CHAR" ) ) != -1 ) || ( pfstd::strcasefind( fields_[ fi ][ field_define::FIELD_TYPE ].c_str(), _T( "TEXT" ) ) != -1 ) )
                    {
                        // json 會被強制轉為 _bin
                        if( field_type == _T( "json" ) )
                        {
                            _pf_tstrprintf( character_set, _T( "CHARACTER SET %s COLLATE %s_bin" ), character_set_.c_str(), character_set_.c_str() );
                        }
                        else
                        {
                            _pf_tstrprintf( character_set, _T( "CHARACTER SET %s COLLATE %s" ), character_set_.c_str(), collation_.c_str() );
                        }
                        fields_[ fi ][ field_define::FIELD_CHARACTER_SET ] = character_set;
                    }
                }

                // 預留清空
                {
                    fields_[ fi ][ field_define::FIELD_RESERIVC01 ].clear();
                    fields_[ fi ][ field_define::FIELD_RESERIVC02 ].clear();
                }

                // 前一個欄位名稱
                {
                    if( fi == 0 )
                    {
                        fields_[ fi ][ field_define::FIELD_PREV_NAME ] = _T( "FIRST" );
                    }
                    else
                    {
                        fields_[ fi ][ field_define::FIELD_PREV_NAME ] = fields_[ fi - 1 ][ field_define::FIELD_NAME ];
                    }
                }
            }
        }


        // field name 0
        //{
        //
        //}

        // select form order by 1
        //{
        //    _tstring sql_string;
        //    _pf_tstrprintf( sql_string, _T( "SELECT %s FROM "), m_query_string[1].c_str() );
        //    sql_string += _T( "%s ORDER BY %s %s" );
        //    m_query_string.push_back( sql_string );
        //}
        //
        //
        //// select form order by 2
        //{
        //    _tstring sql_string;
        //    _pf_tstrprintf( sql_string, _T( "SELECT %s FROM " ), m_query_string[1].c_str() );
        //    sql_string += _T( "%s WHERE %s ORDER BY %s %s" );
        //    m_query_string.push_back( sql_string );
        //}
        //
        //
        //// replace into 3
        //{
        //    _tstring sql_string = _T( "REPLACE INTO %s( ");
        //    sql_string += m_query_string[1];
        //    sql_string += _T( " ) VALUES( %s )" );
        //    m_query_string.push_back( sql_string );
        //}

        SAFE_DELETE( xlsf );
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Query::Close()
    {
        file_name_.clear();
        sheet_name_.clear();

        fields_.clear();
        primary_nkey_.clear();
        primary_ikey_.clear();
        function_.clear();
        columes_index_.clear();
        desc_index_.clear();

        relational_key_.clear();
        json_value_.clear();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetPrimaryKey( _IN uint_t i )
    {
        if( i >= primary_nkey_.size() )
            return _T( "" );
        return primary_nkey_[ i ];
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetSheetName()
    {
        return sheet_name_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Query::GetFieldSize()
    {
        return fields_.size();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetFieldName( _IN uint_t idx )
    {
        return fields_[ idx ][ 0 ];
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int_t Query::GetFieldIndexByName( _IN _tstring name )
    {
        _tcsidx::iterator it = columes_index_.find( name );
        if( it != columes_index_.end() )
        {
            return it->second;
        }
        return -1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int_t Query::GetFieldIndexByDesc( _IN _tstring fields_desc )
    {
        _tcsidx::iterator it = desc_index_.find( fields_desc );
        if( it != columes_index_.end() )
        {
            return it->second;
        }
        return -1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetFieldNameByDesc( _IN _tstring fields_desc )
    {
        _tcsidx::iterator it = desc_index_.find( fields_desc );
        if( it != desc_index_.end() )
        {
            return fields_[ it->second ][ 0 ];
        }
        return _T( "" );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Query::GetAllFieldName( _OUT _tcsvector& fields_name )
    {
        for( uint_t i = 0; i < fields_.size(); i++ )
        {
            fields_name.push_back( fields_[ i ][ 0 ] );
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Query::GetAllFieldDesc( _OUT _tcsvector& fields_desc )
    {
        for( uint_t i = 0; i < fields_.size(); i++ )
        {
            fields_desc.push_back( fields_[ i ][ 4 ] );
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetQueryFieldsName()
    {
        _tstring sql_string = fields_[ 0 ][ 0 ];
        for( uint_t i = 1; i < fields_.size(); i++ )
        {
            _pf_tstrprintfa( sql_string, _T( ", %s" ), fields_[ i ][ 0 ].c_str() );
        }
        //m_query_string.push_back( sql_string );
        return sql_string;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetQueryFieldsName( _IN const _tcsvector& fields_desc )
    {
        _tstring sql_string = GetFieldNameByDesc( fields_desc[ 0 ] );
        for( uint_t i = 1; i < fields_desc.size(); i++ )
        {
            _tstring queryfield;
            _pf_tstrprintf( queryfield, _T( ", %s" ), GetFieldNameByDesc( fields_desc[ i ] ).c_str() );
            sql_string += queryfield;
        }
        return sql_string;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //_xl_sql_string Query::SQL_Drop_Table( const tchar_t* tb_name, const tchar_t* charset )
    //{
    //    _tstring sql_string;
    //    _pf_tstrprintfa( sql_string, _T( "DROP TABLE IF EXISTS `%s`" ), tb_name );
    //    return sql_string;
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _xl_sql_string Query::SQL_Create_Table_If_Not_Exist( _IN const tchar_t* tb_name )
    {
        _tstring sql_string;

        _pf_tstrprintfa( sql_string, _T( "CREATE TABLE IF NOT EXISTS `%s` (\r\n" ), tb_name );
        for( uint_t i = 0; i < fields_.size(); i++ )
        {
            _pf_tstrprintfa( sql_string, _T( "`%s` %s %s NULL %s COMMENT '[ %s ] %s',\r\n" ), fields_[ i ][ field_define::FIELD_NAME ].c_str(), fields_[ i ][ field_define::FIELD_TYPE ].c_str(), fields_[ i ][ field_define::FIELD_CHARACTER_SET ].c_str(), fields_[ i ][ field_define::FIELD_DEFAULT ].c_str(), fields_[ i ][ field_define::FIELD_DESCRIPTION ].c_str(), fields_[ i ][ field_define::FIELD_COMMENT ].c_str() );
        }

        if( !primary_nkey_.empty() )
        {
            //_tstring queryfield;
            _pf_tstrprintfa( sql_string, _T( "CONSTRAINT %s_PRIMARY_KEY PRIMARY KEY( `%s`" ), tb_name, primary_nkey_[ 0 ].c_str() );
            for( int i = 1; i < primary_nkey_.size(); i++ )
            {
                _pf_tstrprintfa( sql_string, _T( ", `%s`" ), primary_nkey_[ i ].c_str() );
            }

            _pf_tstrprintfa( sql_string, _T( "),  " ) );
        }

        if( !json_value_.empty() )
        {
            //_tstring queryfield;
            for( uint_t i = 0; i < json_value_.size(); i++ )
            {
                _pf_tstrprintfa( sql_string, _T( "CONSTRAINT`CONSTRAINT_%d` CHECK( `%s` is null or json_valid( `%s` ) ),\r\n" ), i, json_value_[ i ].c_str(), json_value_[ i ].c_str() );
            }
        }
        sql_string.erase( sql_string.end() - _tcslen( _T( ",\r\n" ) ), sql_string.end() );

        {
            _pf_tstrprintfa( sql_string, _T( "\r\n) ENGINE = InnoDB CHARACTER SET = %s COLLATE = %s ROW_FORMAT = %s\r\n" ), character_set_.c_str(), collation_.c_str(), row_format_.c_str() );
        }

        if( !partition_.empty() )
        {
            // HASH(XXXX)
            if( _tcscasestr( partition_[ 0 ].c_str(), _T( "HASH" ) ) != NULL )
            {
                if( _istnumber( partition_[ 1 ].c_str() ) != 0 )
                {
                    _pf_tstrprintfa( sql_string, _T( "PARTITION BY %s\r\nPARTITIONS %d\r\n" ), partition_[ 0 ].c_str(), _pf_ttoi( partition_[ 1 ].c_str() ) );
                }
            }
            else if( _tcscasestr( partition_[ 0 ].c_str(), _T( "KEY" ) ) != NULL )
            {
                if( _istnumber( partition_[ 1 ].c_str() ) != 0 )
                {
                    _pf_tstrprintfa( sql_string, _T( "PARTITION BY %s\r\nPARTITIONS %d\r\n" ), partition_[ 0 ].c_str(), _pf_ttoi( partition_[ 1 ].c_str() ) );
                }
            }
            else if( _tcscasestr( partition_[ 0 ].c_str(), _T( "LIST" ) ) != NULL )
            {
                if( !partition_[ 2 ].empty() )
                {
                    _pf_tstrprintfa( sql_string, _T( "PARTITION BY %s\r\n(\r\n%s\r\n)" ), partition_[ 0 ].c_str(), _pf_ttoi( partition_[ 2 ].c_str() ) );
                }
            }
            else if( _tcscasestr( partition_[ 0 ].c_str(), _T( "RANGE" ) ) != NULL )
            {
                if( !partition_[ 2 ].empty() )
                {
                    _pf_tstrprintfa( sql_string, _T( "PARTITION BY %s\r\n(\r\n%s\r\n)" ), partition_[ 0 ].c_str(), _pf_ttoi( partition_[ 2 ].c_str() ) );
                }
            }
        }

        _pf_tstrprintfa( sql_string, _T( ";" ) );

        return sql_string;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _xl_sql_string Query::SQL_Alter_Table( _IN const tchar_t* tb_name, _IN const tchar_t* old_file_name, _IN const tchar_t* old_sheet_name )
    {

        xldb::Query old_xquery;
        if( !old_xquery.Load( old_file_name, old_sheet_name ) )
        {
            return _T( "" );
        }

        std::vector<_tcsvector> new_field = fields_;
        std::vector<_tcsvector> old_field = old_xquery.fields_;

        std::map<_tstring, int> new_field_cache;  // 比對的 快取
        std::map<_tstring, int> old_field_cache;  // 比對的 快取

        // 找新的有 舊的沒有的 => 可能是 新加入 或 修改
        // 
        // 欄位 類型 名稱 說明 都一樣的話 就是視為 相同
        // 放入比對快取中
        {
            // 前一筆的欄位名稱
            for( uint_t ni = 0; ni < new_field.size(); ni++ )
            {
                _tstring key;
                int index = ni + 1;

                _tstring pre_field = new_field[ ni ][ field_define::FIELD_PREV_NAME ];
                //if( ni == 0 )
                //{
                //    pre_field = _T( "FIRST" );
                //}
                //else
                //{
                //    pre_field = new_field[ ni - 1 ][ 0 ];
                //}

                // 因為 有兼容的空白字元 乾脆拿掉 char(20)  char( 20 )
                // 資料類型
                _tstring field_type = pfstd::strtolower( new_field[ ni ][ field_define::FIELD_TYPE ] );
                //pfstd::strreplaceall( field_type, _T( " " ), _T( "" ) );

                _tstring character_set = pfstd::strtolower( new_field[ ni ][ field_define::FIELD_CHARACTER_SET ] );

                //  比對  0:欄位名稱, 1:資料類型, 2:編碼,  4:欄位說明, 7:註解
                _pf_tstrprintf( key, _T( "%s.%s->%s_%s_%s_%s_%s" ), new_field[ ni ][ field_define::FIELD_NAME ].c_str(), pre_field.c_str(), new_field[ ni ][ field_define::FIELD_NAME ].c_str(), field_type.c_str(), character_set.c_str(), new_field[ ni ][ field_define::FIELD_DESCRIPTION ].c_str(), new_field[ ni ][ field_define::FIELD_COMMENT ].c_str() );
                pfstd::strreplaceall( key, _T( " " ), _T( "" ) );
                new_field_cache.insert( std::map<_tstring, int>::value_type( key, ni ) );
            }

            // 從資料庫來的
            for( uint_t oi = 0; oi < old_field.size(); oi++ )
            {
                _tstring key;
                int index = oi + 1;

                _tstring pre_field = old_field[ oi ][ field_define::FIELD_PREV_NAME ];
                //if( oi == 0 )
                //{
                //    pre_field = _T( "FIRST" );
                //}
                //else
                //{
                //    pre_field = old_field[ oi - 1 ][ 0 ];
                //}

                // 因為 有兼容的空白字元 乾脆拿掉 char(20)  char( 20 )
                _tstring field_type = pfstd::strtolower( old_field[ oi ][ field_define::FIELD_TYPE ] );
                //// 因為 如果是 項 josn 或 有其他類型再傳入資料庫時 會被轉換 所以 先記錄
                //_tstring source_field = field_type;
                //if( source_field == _T( "json" ) )
                //{
                //    field_type = _T( "longtext" );
                //}

                _tstring character_set = pfstd::strtolower( old_field[ oi ][ field_define::FIELD_CHARACTER_SET ] );
                // 沒有寫character set 用 default
                //if( character_set.empty() )
                //{
                //    // 字串 或 字元 類型
                //    // CHAR VARCHAR TEXT LONGTEXT TINYTEXT........
                //    if( ( pfstd::strcasefind( field_type.c_str(), _T( "CHAR" ) ) != -1 ) || ( pfstd::strcasefind( field_type.c_str(), _T( "TEXT" ) ) != -1 ) )
                //    {
                //        // json 會被強制轉為 _bin
                //        if( source_field == _T( "json" ) )
                //        {
                //            _pf_tstrprintf( character_set, _T( "CHARACTER SET %s COLLATE %s_bin" ), old_xquery.character_set_.c_str(), old_xquery.character_set_.c_str() );
                //        }
                //        else
                //        {
                //            _pf_tstrprintf( character_set, _T( "CHARACTER SET %s COLLATE %s" ), old_xquery.character_set_.c_str(), old_xquery.collation_.c_str() );
                //        }
                //        character_set = pfstd::strtolower( character_set );
                //    }
                //}
                //  比對  0:欄位名稱, 1:資料類型, 2:編碼,  4:欄位說明, 7:註解
                _pf_tstrprintf( key, _T( "%s.%s->%s_%s_%s_%s_%s" ), old_field[ oi ][ field_define::FIELD_NAME ].c_str(), pre_field.c_str(), old_field[ oi ][ field_define::FIELD_NAME ].c_str(), field_type.c_str(), character_set.c_str(), old_field[ oi ][ field_define::FIELD_DESCRIPTION ].c_str(), old_field[ oi ][ field_define::FIELD_COMMENT ].c_str() );
                pfstd::strreplaceall( key, _T( " " ), _T( "" ) );
                old_field_cache.insert( std::map<_tstring, int>::value_type( key, oi ) );
            }
        }


        // 找到 有新增 修改 或刪除的
        {
            std::vector<_tstring> keep_key;
            std::vector<_tstring> change_key; //debug

            for( std::map<_tstring, int>::iterator oit = old_field_cache.begin(); oit != old_field_cache.end(); oit++ )
            {
                std::map<_tstring, int>::iterator nit = new_field_cache.find( oit->first );
                // 舊的 schema 在新的裡面也有 沒變動
                if( nit != new_field_cache.end() )
                {
                    keep_key.push_back( oit->first );
                }
                else
                {
                    change_key.push_back( oit->first );
                }
            }

            // 移除 沒變化的
            // new -> 有變化 或新增的
            // old -> 有變化 或刪除的
            for( uint_t ki = 0; ki < keep_key.size(); ki++ )
            {
                old_field_cache.erase( keep_key[ ki ] );
                new_field_cache.erase( keep_key[ ki ] );
            }
        }


        // 判斷 是要 新增? 修改? 還是刪除
        // ALTER TABLE `%s` 
        // 刪除:   DROP i;// 
        // 新增:   ADD COLUMN `expiration_date` datetime COMMENT '商品期限日期[]' AFTER `period_end_date`
        //        ADD COLUMN `first_record` varchar(255) NULL COMMENT '加入第一筆' FIRST;
        // 改名稱: CHANGE COLUMN `pay_method` `pay_method` varchar(32) DEFAULT NULL COMMENT '支付方式[ 1:cash, 2:credit,3:後付款; 4:payal; 5:np ]' AFTER `pay_type`// 
        // 改定義: MODIFY COLUMN `product_set_cost_ramount` decimal(20, 4) DEFAULT '0' COMMENT '[ 原價實際金額 ] 商品給供貨商的總額 ( 流通高 原始 )'

        struct field_record
        {
            int         method;     // 1:modify 2:change 3:Add 4:drop  依照做的先後順序 優先處理 最大 drop
            int         index;      // new_field old_field index=>drop 
            _tstring    old_key;
            _tstring    new_key;
            _tcsvector  old_field_data; // 欄位的資料
            _tcsvector  new_field_data; // 欄位的資料
        };

        std::vector<field_record>  update_field;  // add modify change

        // 比對 新舊 欄位 
        // 要用優先權 所以 要先跑完 優先的才能比對 次要的
        {
            // 如果被比對到了 就放這邊 最後拿去拔掉 old_field_cache 就是 要移除的
            std::set<_tstring> old_find_field;
            // 判斷 是 新增 還是修改呢?
            for( std::map<_tstring, int>::iterator nit = new_field_cache.begin(); nit != new_field_cache.end(); nit++ )
            {
                bool is_find = false;
                // 取出位置
                int nid = nit->second;
                // 要判斷的欄位資料
                _tcsvector new_field_record = new_field[ nid ];

                // name 是第一優先順序
                for( std::map<_tstring, int>::iterator oit = old_field_cache.begin(); oit != old_field_cache.end(); oit++ )
                {
                    // 取出位置
                    int oid = oit->second;
                    // 要判斷的欄位資料
                    _tcsvector old_field_record = old_field[ oid ];

                    // 欄位相同
                    // modify? 
                    if( new_field_record[ field_define::FIELD_NAME ] == old_field_record[ field_define::FIELD_NAME ] )
                    {
                        is_find = true;
                        old_find_field.insert( std::set<_tstring>::value_type( oit->first ) );

                        field_record rec;
                        rec.method = 1;
                        rec.index = nit->second;
                        rec.old_field_data = old_field_record;
                        rec.new_field_data = new_field_record;
                        rec.old_key = oit->first;
                        rec.new_key = nit->first;
                        update_field.push_back( rec );

                        break;
                    }

                }

                if( is_find )
                {
                    continue;
                }

                // 中文欄位名第2
                for( std::map<_tstring, int>::iterator oit = old_field_cache.begin(); oit != old_field_cache.end(); oit++ )
                {
                    // 取出位置
                    int oid = oit->second;
                    // 要判斷的欄位資料
                    _tcsvector old_field_record = old_field[ oid ];

                    // 中文名稱 或註解相同
                    // change?
                    if( new_field_record[ field_define::FIELD_DESCRIPTION ] == old_field_record[ field_define::FIELD_DESCRIPTION ] ) 
                    {
                        is_find = true;
                        old_find_field.insert( std::set<_tstring>::value_type( oit->first ) );

                        field_record rec;
                        rec.method = 2;
                        rec.index = nit->second;
                        rec.old_field_data = old_field_record;
                        rec.new_field_data = new_field_record;
                        rec.old_key = oit->first;
                        rec.new_key = nit->first;
                        update_field.push_back( rec );

                        break;
                    }
                }

                if( is_find )
                {
                    continue;
                }

                // 註解+前一個欄位名稱+類型
                for( std::map<_tstring, int>::iterator oit = old_field_cache.begin(); oit != old_field_cache.end(); oit++ )
                {
                    // 取出位置
                    int oid = oit->second;
                    // 要判斷的欄位資料
                    _tcsvector old_field_record = old_field[ oid ];

                    // 中文名稱 或註解相同
                    // change?
                    if( ( ( new_field_record[ field_define::FIELD_COMMENT ] == old_field_record[ field_define::FIELD_COMMENT ] ) && ( new_field_record[ field_define::FIELD_COMMENT ] != _T( "" ) ) ) &&
                        ( new_field_record[ field_define::FIELD_PREV_NAME ] == old_field_record[ field_define::FIELD_PREV_NAME ] ) &&
                        ( new_field_record[ field_define::FIELD_TYPE ] == old_field_record[ field_define::FIELD_TYPE ] ) )
                    {
                        is_find = true;
                        old_find_field.insert( std::set<_tstring>::value_type( oit->first ) );

                        field_record rec;
                        rec.method = 2;
                        rec.index = nit->second;
                        rec.old_field_data = old_field_record;
                        rec.new_field_data = new_field_record;
                        rec.old_key = oit->first;
                        rec.new_key = nit->first;
                        update_field.push_back( rec );

                        break;
                    }
                }

                // 啥都沒找到 那應該是一個新增
                // add
                if( !is_find )
                {
                    field_record rec;
                    rec.method = 3;
                    rec.index = nit->second;
                    rec.new_field_data = new_field_record;
                    rec.new_key = nit->first;
                    update_field.push_back( rec );
                }
            }

            // 把有比對到的拿掉 剩下的 就是要移除的
            for( std::set<_tstring>::iterator of = old_find_field.begin(); of != old_find_field.end(); of++ )
            {
                old_field_cache.erase( *of );
            }

            // 要移除的
            for( std::map<_tstring, int>::iterator oit = old_field_cache.begin(); oit != old_field_cache.end(); oit++ )
            {
                field_record rec;
                rec.method = 4;
                rec.index = oit->second;
                rec.old_field_data = old_field[ rec.index ];
                rec.old_key = oit->first;
                update_field.push_back( rec );
            }
        }

        {
            struct sort_field
            {
                bool operator() ( field_record i, field_record j )
                {
                    if( i.method == j.method )
                    {
                        return ( i.index < j.index );
                    }
                    return i.method > j.method;
                }
            } mysort_field;

            std::sort( update_field.begin(), update_field.end(), mysort_field );
        }

        // 寫SQL
        _tstring sql_query;
        {
            //FILE* f = fopen( _T( "debug_file.txt" ), "wb+" );
            //for( uint_t ui = 0; ui < update_field.size(); ui++ )
            //{
            //    _tstring line_text;
            //    _pf_tstrprintf( line_text, _T( "method:%d    index:%d\nold: %s\nnew: %s\n\n" ), update_field[ ui ].method, update_field[ ui ].index, update_field[ ui ].old_key.c_str(), update_field[ ui ].new_key.c_str() );
            //    fwrite( line_text.c_str(), line_text.size() * sizeof( tchar_t ), 1, f );
            //}

            if( !update_field.empty() )
            {
                _pf_tstrprintf( sql_query, _T( "ALTER TABLE `%s`\r\n" ), tb_name );

                for( uint_t ui = 0; ui < update_field.size(); ui++ )
                {
                    switch( update_field[ ui ].method )
                    {
                        // drop 不真正移除 只是搬到最後面
                        case 4:
                            {
                                // 最後一個欄位的名子
                                _tstring last_field_name = new_field[ new_field.size() - 1 ][ 0 ];

                                _pf_tstrprintfa( sql_query, _T( "CHANGE COLUMN `%s` `drop_%s` %s %s %s COMMENT '[ %s ] %s' AFTER `%s`,\r\n" ),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_NAME ].c_str(),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_NAME ].c_str(),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_TYPE ].c_str(),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_CHARACTER_SET ].c_str(),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_DEFAULT ].c_str(),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_DESCRIPTION ].c_str(),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_COMMENT ].c_str(),
                                                 last_field_name.c_str() );
                            }
                            break;
                            // add
                        case 3:
                            {
                                // 前一個欄位的名子
                                _tstring after_field_name;
                                after_field_name = ( update_field[ ui ].index == 0 ) ? after_field_name = _T( "FIRST" ) : _tstring( _T( "`" ) ) + new_field[ update_field[ ui ].index - 1 ][ 0 ] + _tstring( _T( "`" ) );

                                _pf_tstrprintfa( sql_query, _T( "ADD COLUMN `%s` %s %s %s COMMENT '[ %s ] %s' AFTER %s,\r\n" ),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_NAME ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_TYPE ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_CHARACTER_SET ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_DEFAULT ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_DESCRIPTION ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_COMMENT ].c_str(),
                                                 after_field_name.c_str() );
                            }
                            break;
                        case 2:
                            {
                                // 前一個欄位的名子
                                _tstring after_field_name;
                                after_field_name = ( update_field[ ui ].index == 0 ) ? after_field_name = _T( "FIRST" ) : _tstring( _T( "`" ) ) + new_field[ update_field[ ui ].index - 1 ][ 0 ] + _tstring( _T( "`" ) );

                                _pf_tstrprintfa( sql_query, _T( "CHANGE COLUMN `%s` `%s` %s %s %s COMMENT '[ %s ] %s' AFTER %s,\r\n" ),
                                                 update_field[ ui ].old_field_data[ field_define::FIELD_NAME ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_NAME ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_TYPE ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_CHARACTER_SET ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_DEFAULT ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_DESCRIPTION ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_COMMENT ].c_str(),
                                                 after_field_name.c_str() );
                            }
                            break;
                        case 1:
                            {
                                // 前一個欄位的名子
                                _tstring after_field_name;
                                after_field_name = ( update_field[ ui ].index == 0 ) ? after_field_name = _T( "FIRST" ) : _tstring( _T( "`" ) ) + new_field[ update_field[ ui ].index - 1 ][ 0 ] + _tstring( _T( "`" ) );

                                _pf_tstrprintfa( sql_query, _T( "MODIFY COLUMN `%s` %s %s %s COMMENT '[ %s ] %s' AFTER %s,\r\n" ),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_NAME ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_TYPE ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_CHARACTER_SET ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_DEFAULT ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_DESCRIPTION ].c_str(),
                                                 update_field[ ui ].new_field_data[ field_define::FIELD_COMMENT ].c_str(),
                                                 after_field_name.c_str() );
                            }
                            break;
                    }
                }

                // 2022/10/09
                if( ( old_xquery.character_set_ != character_set_ ) ||
                    ( old_xquery.collation_ != collation_ ) ||
                    ( old_xquery.row_format_ != row_format_ ) )
                {
                    // 會同時替換目前欄位裡面的 編碼
                    //_pf_tstrprintfa( sql_query, _T( "CONVERT TO CHARACTER SET %s COLLATE %s, ROW_FORMAT = %s,\r\n" ), character_set_.c_str(), collation_.c_str(), row_format_.c_str() );
                    _pf_tstrprintfa( sql_query, _T( "DEFAULT CHARACTER SET %s COLLATE %s, ROW_FORMAT = %s,\r\n" ), character_set_.c_str(), collation_.c_str(), row_format_.c_str() );
                }

                // 最後一個是 \n
                if( sql_query[ sql_query.size() - 3 ] == _T( ',' ) )
                {
                    sql_query[ sql_query.size() - 3 ] = _T( ';' );
                }
            }


            //fwrite( sql_query.c_str(), sql_query.size() * sizeof( tchar_t ), 1, f );
            //fclose( f );
        }
        return sql_query;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _xl_sql_string Query::SQL_Alter_Tabel_Relational( _IN const tchar_t* ftb_name, _IN const tchar_t* ldb_name, _IN const tchar_t* ltb_name )
    {
        _tstring sql_string;
        if( !relational_key_.empty() )
        {
            _pf_tstrprintfa( sql_string, _T( "ALTER TABLE `%s`.`%s` ADD INDEX(`%s`)" ), ldb_name, ltb_name, relational_key_[ 0 ].c_str() );
            for( uint_t i = 1; i < relational_key_.size(); i++ )
            {
                _pf_tstrprintfa( sql_string, _T( " ,ADD INDEX(`%s`)" ), relational_key_[ i ].c_str() );
            }
            _pf_tstrprintfa( sql_string, _T( " ;" ) );

            _pf_tstrprintfa( sql_string, _T( "ALTER TABLE `%s` ADD INDEX(`%s`)" ), ftb_name, relational_key_[ 0 ].c_str() );
            for( uint_t i = 1; i < relational_key_.size(); i++ )
            {
                _pf_tstrprintfa( sql_string, _T( " ,ADD INDEX(`%s`)" ), relational_key_[ i ].c_str() );
            }
            _pf_tstrprintfa( sql_string, _T( " ;" ) );

            for( uint_t i = 0; i < relational_key_.size(); i++ )
            {
                _pf_tstrprintfa( sql_string, _T( "ALTER TABLE `%s` ADD FOREIGN KEY (`%s`) REFERENCES `%s` (`%s`) ON DELETE CASCADE ON UPDATE CASCADE;" ),
                                 ftb_name, relational_key_[ i ].c_str(), ltb_name, relational_key_[ i ].c_str() );
            }
        }
        return sql_string;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _xl_sql_string Query::SQL_Alter_Table_Key_Auto_Increment( _IN const tchar_t* ftb_name )
    {
        _tstring sql_string;
        // 因為只有 int 能auto 所以直接寫進去吧
        for( uint_t i = 0; i < primary_ikey_.size(); i++ )
        {
            int key_pos = primary_ikey_[ i ];
            _pf_tstrprintfa( sql_string, _T( "ALTER TABLE `%s` MODIFY COLUMN `%s` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '%s';" ),
                             ftb_name, fields_[ key_pos ][ 0 ].c_str(), fields_[ key_pos ][ 4 ].c_str() );
        }
        //sql_string.erase( sql_string.end() - _tcslen( _T( "," ) ), sql_string.end() );
        return sql_string;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _xl_sql_string Query::SQL_Alter_Table_Drop_Primary_Key( _IN const tchar_t* ftb_name )
    {
        _tstring sql_string;

        // 20220609
        // 要把 欄未設為 default NULL 以免沒有資料十 還是一樣寫不進去
        _pf_tstrprintfa( sql_string, _T( "ALTER TABLE `%s` DROP PRIMARY KEY" ), ftb_name );
        for( uint_t i = 0; i < primary_ikey_.size(); i++ )
        {
            int key_pos = primary_ikey_[ i ];
            _pf_tstrprintfa( sql_string, _T( ", MODIFY COLUMN `%s` %s DEFAULT NULL COMMENT '%s'" ),
                             ftb_name, fields_[ key_pos ][ 0 ].c_str(), fields_[ key_pos ][ 1 ].c_str(), fields_[ key_pos ][ 4 ].c_str() );
        }
        //_pf_tstrprintfa( sql_string, _T( "ALTER TABLE `%s` DROP PRIMARY KEY" ), ftb_name );
        return sql_string;
    }

    /*
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Query::GetSetSyncCondition( std::vector<UINT>& values )
    {
        values = m_setsync_keyid;
        return;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetSetSyncFieldname()
    {
        _tstring sql_string = m_setsync_value[0];
        for ( int i = 1; i < m_setsync_value.size(); i++ )
        {
            _pf_tstrprintfa( sql_string, _T( ", %s" ), m_setsync_value[i].c_str() );
        }
        return sql_string;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Query::GetGetSyncCondition( std::vector<UINT>& values )
    {
        values = m_setsync_keyid;
        return;

    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Query::GetGetSyncFieldname()
    {
        _tstring sql_string = m_getsync_value[0];
        for ( int i = 1; i < m_getsync_value.size(); i++ )
        {
            _pf_tstrprintfa( sql_string, _T( ", %s" ), m_getsync_value[i].c_str() );
        }
        return sql_string;
    }

    _tstring Query::Select_Table_From_Order_By( const tchar_t* tname, const tchar_t* order_field_name, const tchar_t* order_type )
    {
        _tstring sql_string;
        _pf_tstrprintf( sql_string, m_query_string[2].c_str(), tname, order_field_name, order_type );
        return sql_string;
    }

    _tstring Query::Select_Table_From_Where_Order_By( const tchar_t* tname, const _tcsvector& condition, const tchar_t* order_field_name, const tchar_t* order_type )
    {

        _tstring sql_string;
        _tstring conditionstring = condition[0];
        {
            for ( int i = 1; i < condition.size(); i++ )
            {
                conditionstring += _T(" AND ") + condition[i];
            }
        }

        _pf_tstrprintf( sql_string, m_query_string[2].c_str(), tname, conditionstring.c_str(),  order_field_name, order_type );
        return sql_string;

    }

    _tstring Query::Replace_Into( const tchar_t* tname, const _tcsvector& values )
    {
        // 欄位數 不相同
        if ( values.size() != fields_.size() )
        {
            return _T( "" );
        }

        _tcsvector safe_values;
        for ( int i = 0; i < values.size(); i++ )
        {
            safe_values.push_back( XDatabase::SafeEscapeString( values[i].c_str() ) );
        }

        _tstring sql_string;
        _tstring valusstring = sentence_pattern::SeriesString02( safe_values );
        _pf_tstrprintf( sql_string, m_query_string[4].c_str(), tname, valusstring.c_str() );
        return sql_string;
    }
    */
}
