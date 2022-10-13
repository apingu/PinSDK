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
#include "stdafx.h"

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <gadget/sentence_pattern.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "ExcelFile.h"
#include "xlsdb-query.h"


namespace xlsdb
{
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Query::Query()
    {
        m_b_row = 2;
        m_b_col = 1;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Query::~Query()
    {
        fields_name_.clear();
        command_index_.clear();
        //m_query_string.clear();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool Query::Load( const TCHAR* fname, const TCHAR* sname )
    {
        Close();
        TCHAR buff[ 1024 ];

        ExcelFile* xlsf = new ExcelFile;
        xlsf->SetAlert( FALSE );
        if ( xlsf->Open( fname ) == NULL )
        {
            file_name_ = _T( "" );
            SAFE_DELETE( xlsf );
            return false;
        }

        xlsf->SetVisible( is_visible_ );

        if ( xlsf->SelectSheet( sname ) == NULL )
        {
            SAFE_DELETE( xlsf );
            return false;
        }

        file_name_ = fname;
        sheet_name_ = sname;

        // 將fiels與對映的col index填入
        int b_row = m_b_row;
        int b_col = m_b_col;

        int max_col = xlsf->GetHorztlTotalCell();
        int max_row = xlsf->GetVrticlTotalCell();
        {
            mstimen_t currt = pftml::Time::GetTickMSTime();

            xlsf->SelectCell( b_col, b_row, max_col - 1, max_row - 1 );

            //_TCHAR keybuff[_MAX_STRING_LEN];
            for ( int trow = 1; trow <= max_row; trow++ )
            {
                _tcsvector row_data;

                CString celldata = xlsf->GetCellRange2CStr( 1, trow );
                if ( celldata == _T( "" ) )
                {
                    continue;
                }

                // 欄位數值
                {
                    row_data.push_back( (LPCTSTR)celldata );
                    for ( int tcol = 2; tcol < max_col; tcol++ )
                    {
                        celldata = xlsf->GetCellRange2CStr( tcol, trow );
                        row_data.push_back( (LPCTSTR)celldata );
                    }
                }

                // 檢查資料型別
                {
                    if ( _tcsicmp( row_data[ 1 ].c_str(), _T( "json" ) ) == 0 )
                    {
                        json_value_.push_back( row_data[ 0 ] );
                    }
                }

                // 檢查 3 預設值
                {
                    //CString celldata = xlsf->GetCellRange2CStr( 3, trow );
                    // primary key
                    if ( _tcsicmp( row_data[ 2 ].c_str(), _T( "PRIMARY KEY" ) ) == 0 )
                    {
                        row_data[ 2 ] = _T( "PRIMARY KEY" );
                        primary_nkey_.push_back( row_data[ 0 ] );
                        primary_ikey_.push_back( trow - 1 );
                    }
                    else if ( _tcsicmp( row_data[ 2 ].c_str(), _T( "AUTO_INCREMENT" ) ) == 0 )
                    {
                        row_data[ 2 ] = _T( "PRIMARY KEY AUTO_INCREMENT" );
                        primary_nkey_.insert( primary_nkey_.begin(), row_data[ 0 ] );
                        primary_ikey_.insert( primary_ikey_.begin(), ( trow - 1 ) );
                    }
                    else if ( row_data[ 2 ] == _T( "" ) )
                    {
                        _pf_tstrprintf( row_data[ 2 ], _T( "DEFAULT NULL" ) );
                    }
                    else if ( _tcsicmp( row_data[ 2 ].c_str(), _T( "NOT NULL" ) ) == 0 )
                    {

                    }
                    else
                    {
                        if ( ( row_data[ 2 ].front() == _T( '\"' ) ) && ( row_data[ 2 ].back() == _T( '\"' ) ) )
                        {
                            row_data[ 2 ].front() = _T( '\'' );
                            row_data[ 2 ].back() = _T( '\'' );
                            _pf_tstrprintf( row_data[ 2 ], _T( "DEFAULT %s" ), row_data[ 2 ].c_str() );
                        }
                        else
                        {
                            _pf_tstrprintf( row_data[ 2 ], _T( "DEFAULT %s" ), row_data[ 2 ].c_str() );
                        }
                    }
                }

                // 索引值欄位
                {
                    CString celldata = xlsf->GetCellRange2CStr( 5, trow );
                    TCHAR* buff = new TCHAR[ celldata.GetLength() + 1 ];
                    _tcscpy( buff, (LPCTSTR)celldata );

                    TCHAR seps[] = _T( "," );
                    TCHAR *token = NULL;
                    TCHAR *next_token = NULL;

                    token = _tcstok_s( buff, seps, &next_token );
                    while ( token )
                    {
                        // 外部連結的key
                        if ( _tcsstr( token, _T( "index key" ) ) != 0 )
                        {
                            index_key_.push_back( row_data[ 0 ] );
                        }
                        // json 格式檢查
                        //else if( _tcsstr( token, _T( "json" ) ) != 0 )
                        //{
                        json_value_.push_back( row_data[ 0 ] );
                        //}
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
                        token = _tcstok_s( NULL, seps, &next_token );
                    }
                    SAFE_DELETE_ARRAY( buff );
                }

                // 呼叫的函式
                {
                    CString celldata = xlsf->GetCellRange2CStr( 5, trow );
                    if ( celldata != _T( "" ) )
                    {
                        function_.push_back( (LPCTSTR)celldata );
                    }
                }

                fields_name_.push_back( row_data );
                command_index_.insert( _tcsidx::value_type( row_data[ 3 ], trow - 1 ) );
                columes_index_.insert( _tcsidx::value_type( row_data[ 0 ], trow - 1 ) );
                //_stprintf_s( buff, _T( "xlsdb::Query read row: %d/%d\n" ), trow, max_row - 1 );
                //OutputDebugString( buff );
            }

            mstimen_t currf = pftml::Time::GetTickMSTime() - currt;
            _stprintf_s( buff, _T( "xlsdb::Query load table %s:%s row = %d spend time: %lld\n" ), file_name_.c_str(), sheet_name_.c_str(), max_row, currf );
            OutputDebugString( buff );
        }


        if ( fields_name_.empty() )
        {
            SAFE_DELETE( xlsf );
            return false;
        }


        // field name 0
        //{
        //
        //}

        // select form order by 1
        //{
        //    _tstring querystring;
        //    _pf_tstrprintf( querystring, _T( "SELECT %s FROM "), m_query_string[1].c_str() );
        //    querystring += _T( "%s ORDER BY %s %s" );
        //    m_query_string.push_back( querystring );
        //}
        //
        //
        //// select form order by 2
        //{
        //    _tstring querystring;
        //    _pf_tstrprintf( querystring, _T( "SELECT %s FROM " ), m_query_string[1].c_str() );
        //    querystring += _T( "%s WHERE %s ORDER BY %s %s" );
        //    m_query_string.push_back( querystring );
        //}
        //
        //
        //// replace into 3
        //{
        //    _tstring querystring = _T( "REPLACE INTO %s( ");
        //    querystring += m_query_string[1];
        //    querystring += _T( " ) VALUES( %s )" );
        //    m_query_string.push_back( querystring );
        //}

        SAFE_DELETE( xlsf );
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Query::Close()
    {
        file_name_.clear();
        sheet_name_.clear();

        fields_name_.clear();
        primary_nkey_.clear();
        primary_ikey_.clear();
        function_.clear();
        columes_index_.clear();
        command_index_.clear();

        index_key_.clear();
        json_value_.clear();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetPrimaryKey( uint_t i )
    {
        if ( i >= primary_nkey_.size() )
            return _T( "" );
        return primary_nkey_[ i ];
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetSheetname()
    {
        return sheet_name_;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int Query::GetFieldSize()
    {
        return fields_name_.size();
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetFieldname( uint_t idx )
    {
        return fields_name_[ idx ][ 0 ];
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int_t Query::GetFieldindexByName( _tstring name )
    {
        _tcsidx::iterator it = columes_index_.find( name );
        if ( it != columes_index_.end() )
        {
            return it->second;
        }
        return -1;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================

    int_t Query::GetFieldindexByComm( _tstring comm )
    {
        _tcsidx::iterator it = command_index_.find( comm );
        if ( it != columes_index_.end() )
        {
            return it->second;
        }
        return -1;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetFieldnameByComm( _tstring comm )
    {
        _tcsidx::iterator it = command_index_.find( comm );
        if ( it != command_index_.end() )
        {
            return fields_name_[ it->second ][ 0 ];
        }
        return _T( "" );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Query::GetAllFieldname( _tcsvector& fieldsname )
    {
        for ( uint_t i = 0; i < fields_name_.size(); i++ )
        {
            fieldsname.push_back( fields_name_[ i ][ 0 ] );
        }
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Query::GetAllCommends( _tcsvector& commends )
    {
        for ( uint_t i = 0; i < fields_name_.size(); i++ )
        {
            commends.push_back( fields_name_[ i ][ 3 ] );
        }
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetQueryFieldsname()
    {
        _tstring querystring = fields_name_[ 0 ][ 0 ];
        for ( uint_t i = 1; i < fields_name_.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( ", %s" ), fields_name_[ i ][ 0 ].c_str() );
        }
        //m_query_string.push_back( querystring );
        return querystring;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetQueryFieldsname( const _tcsvector& commands )
    {
        _tstring querystring = GetFieldnameByComm( commands[ 0 ] );
        for ( uint_t i = 1; i < commands.size(); i++ )
        {
            _tstring queryfield;
            _pf_tstrprintf( queryfield, _T( ", %s" ), GetFieldnameByComm( commands[ i ] ).c_str() );
            querystring += queryfield;
        }
        return querystring;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _SQLstring Query::SQL_Drop_Table( const TCHAR* tbname, const TCHAR* charset )
    {
        _tstring querystring;
        _pf_tstrprintfa( querystring, _T( "DROP TABLE IF EXISTS `%s`" ), tbname );
        return querystring;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _SQLstring Query::SQL_Create_Table_If_Not_Exist( const TCHAR* tbname, const TCHAR* charset )
    {
        _tstring querystring;
        _pf_tstrprintfa( querystring, _T( "CREATE TABLE IF NOT EXISTS `%s` ( " ), tbname );
        for ( uint_t i = 0; i < fields_name_.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( "`%s` %s %s COMMENT '%s { %s }', " ), fields_name_[ i ][ 0 ].c_str(), fields_name_[ i ][ 1 ].c_str(), fields_name_[ i ][ 2 ].c_str(), fields_name_[ i ][ 3 ].c_str(), fields_name_[ i ][ 6 ].c_str() );
        }

        //if ( !primary_nkey_.empty() )
        //{
        //    //_tstring queryfield;
        //    _pf_tstrprintfa( querystring, _T( "PRIMARY KEY(`%s`" ), primary_nkey_[0].c_str() );
        //    for ( int i = 1; i < primary_nkey_.size(); i++ )
        //    {
        //        _pf_tstrprintfa( querystring, _T( ", `%s`" ), primary_nkey_[i].c_str() );
        //    }
        //    _pf_tstrprintfa( querystring, _T( ")" ) );
        //}

        if ( !json_value_.empty() )
        {
            //_tstring queryfield;
            for ( uint_t i = 0; i < json_value_.size(); i++ )
            {
                _pf_tstrprintfa( querystring, _T( "CONSTRAINT`CONSTRAINT_%d` CHECK( `%s` is null or json_valid( `%s` ) ), " ), i, json_value_[ i ].c_str(), json_value_[ i ].c_str() );
            }
        }

        querystring.erase( querystring.end() - _tcslen( _T( ", " ) ), querystring.end() );

        {
            _pf_tstrprintfa( querystring, _T( ") ENGINE = InnoDB DEFAULT CHARSET = %s" ), charset );
        }

        return querystring;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _SQLstring Query::SQL_Alter_Tabel_Index( const TCHAR* ftbname, const TCHAR* ldbname, const TCHAR* ltbname )
    {
        _tstring querystring;
        if ( !index_key_.empty() )
        {
            _pf_tstrprintfa( querystring, _T( "ALTER TABLE `%s`.`%s` ADD INDEX(`%s`)" ), ldbname, ltbname, index_key_[ 0 ].c_str() );
            for ( uint_t i = 1; i < index_key_.size(); i++ )
            {
                _pf_tstrprintfa( querystring, _T( " ,ADD INDEX(`%s`)" ), index_key_[ i ].c_str() );
            }
            _pf_tstrprintfa( querystring, _T( " ;" ) );

            _pf_tstrprintfa( querystring, _T( "ALTER TABLE `%s` ADD INDEX(`%s`)" ), ftbname, index_key_[ 0 ].c_str() );
            for ( uint_t i = 1; i < index_key_.size(); i++ )
            {
                _pf_tstrprintfa( querystring, _T( " ,ADD INDEX(`%s`)" ), index_key_[ i ].c_str() );
            }
            _pf_tstrprintfa( querystring, _T( " ;" ) );

            for ( uint_t i = 0; i < index_key_.size(); i++ )
            {
                _pf_tstrprintfa( querystring, _T( "ALTER TABLE `%s` ADD FOREIGN KEY (`%s`) REFERENCES `%s` (`%s`) ON DELETE CASCADE ON UPDATE CASCADE;" ),
                    ftbname, index_key_[ i ].c_str(), ltbname, index_key_[ i ].c_str() );
            }
        }
        return querystring;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _SQLstring Query::SQL_Alter_Table_KeyAutoIncrement( const TCHAR* ftbname )
    {
        _tstring querystring;
        // 因為只有 int 能auto 所以直接寫進去吧
        for ( uint_t i = 0; i < primary_ikey_.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( "ALTER TABLE `%s` MODIFY COLUMN `%s` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '%s' FIRST" ),
                ftbname, fields_name_[ i ][ 0 ].c_str(), fields_name_[ i ][ 3 ].c_str() );
        }
        return querystring;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _SQLstring Query::SQL_Alter_Table_DropPrimaryKey( _IN const TCHAR* ftbname )
    {
        _tstring querystring;
        for ( uint_t i = 0; i < primary_ikey_.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( "ALTER TABLE `%s` MODIFY COLUMN `%s` %s DEFAULT NULL COMMENT '%s' FIRST, " ),
                ftbname, fields_name_[ i ][ 0 ].c_str(), fields_name_[ i ][ 1 ].c_str(), fields_name_[ i ][ 3 ].c_str() );
        }
        //_pf_tstrprintfa( querystring, _T( "ALTER TABLE `%s` DROP PRIMARY KEY" ), ftbname );
        _pf_tstrprintfa( querystring, _T( "DROP PRIMARY KEY" ), ftbname );
        return querystring;
    }


    /*
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Query::GetSetSyncCondition( std::vector<UINT>& values )
    {
        values = m_setsync_keyid;
        return;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetSetSyncFieldname()
    {
        _tstring querystring = m_setsync_value[0];
        for ( int i = 1; i < m_setsync_value.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( ", %s" ), m_setsync_value[i].c_str() );
        }
        return querystring;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Query::GetGetSyncCondition( std::vector<UINT>& values )
    {
        values = m_setsync_keyid;
        return;

    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Query::GetGetSyncFieldname()
    {
        _tstring querystring = m_getsync_value[0];
        for ( int i = 1; i < m_getsync_value.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( ", %s" ), m_getsync_value[i].c_str() );
        }
        return querystring;
    }

    _tstring Query::Select_Table_From_Order_By( const TCHAR* tname, const TCHAR* order_field_name, const TCHAR* order_type )
    {
        _tstring querystring;
        _pf_tstrprintf( querystring, m_query_string[2].c_str(), tname, order_field_name, order_type );
        return querystring;
    }

    _tstring Query::Select_Table_From_Where_Order_By( const TCHAR* tname, const _tcsvector& condition, const TCHAR* order_field_name, const TCHAR* order_type )
    {

        _tstring querystring;
        _tstring conditionstring = condition[0];
        {
            for ( int i = 1; i < condition.size(); i++ )
            {
                conditionstring += _T(" AND ") + condition[i];
            }
        }

        _pf_tstrprintf( querystring, m_query_string[2].c_str(), tname, conditionstring.c_str(),  order_field_name, order_type );
        return querystring;

    }

    _tstring Query::Replace_Into( const TCHAR* tname, const _tcsvector& values )
    {
        // 欄位數 不相同
        if ( values.size() != fields_name_.size() )
        {
            return _T( "" );
        }

        _tcsvector safe_values;
        for ( int i = 0; i < values.size(); i++ )
        {
            safe_values.push_back( XDatabase::SafeEscapeString( values[i].c_str() ) );
        }

        _tstring querystring;
        _tstring valusstring = sentence_pattern::SeriesString02( safe_values );
        _pf_tstrprintf( querystring, m_query_string[4].c_str(), tname, valusstring.c_str() );
        return querystring;
    }
    */
}
