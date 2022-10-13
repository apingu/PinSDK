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

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xdb-query.h"


namespace xdb
{
    namespace query
    {
        namespace inquire
        {
            //=========================================================================
            // NAME:
            // DESC:
            //=========================================================================
            bool IfTableExist( _IN XDatabase* p_xdb, _IN const char_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return false;
                }

                _mstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, ' ' );

                _mstring sql_string;

                // 先檢查該table 存不存在
                bool bexist = false;
                pfstd::strprintf( sql_string, ( "SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = '%s' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_table_name.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetQueryResultM( ret1, 0, 0 ) != "0" )
                {
                    bexist = true;
                }
                p_xdb->FreeResult( ret1 );
                return bexist;
            }

            bool IfTableExist( _IN XDatabase* p_xdb, _IN const wchar_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return false;
                }

                _wstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, L' ' );

                _wstring sql_string;

                // 先檢查該table 存不存在
                bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = '%s' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_table_name.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetQueryResultW( ret1, 0, 0 ) != L"0" )
                {
                    bexist = true;
                }
                p_xdb->FreeResult( ret1 );
                return bexist;
            }

            //=========================================================================
            // NAME:
            // DESC:
            //=========================================================================
            void DropTableIfExistt( _IN XDatabase* p_xdb, _IN const char_t* table_name )
            {
                _mstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, ' ' );

                _mstring sql_string;
                pfstd::strprintf( sql_string, "DROP TABLE IF EXISTS `%s`", pure_table_name.c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                p_xdb->FreeResult( ret1 );
                return;
            }

            void DropTableIfExistt( _IN XDatabase* p_xdb, _IN const wchar_t* table_name )
            {
                _wstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, L' ' );

                _wstring sql_string;
                pfstd::strprintf( sql_string, L"DROP TABLE IF EXISTS `%s`", pure_table_name.c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                p_xdb->FreeResult( ret1 );
                return;
            }

            //=========================================================================
            //
            // 將不存在的資料表 從vector 移除掉
            //=========================================================================
            void FilterRmoveNotExistTable( _INOUT _mbsvector& table_names, _IN XDatabase* p_xdb )
            {
                _mbsvector tmp_tables = table_names;
                table_names.clear();

                for( uint_t tbi = 0; tbi < tmp_tables.size(); tbi++ )
                {
                    _mstring pure_table_name = tmp_tables[ tbi ];
                    pfstd::strextractstring( pure_table_name, ' ' );

                    if( IfTableExist( p_xdb, pure_table_name.c_str() ) )
                    {
                        table_names.push_back( pure_table_name );
                    }
                }
                return;
            }

            void FilterRmoveNotExistTable( _INOUT _wcsvector& table_names, _IN XDatabase* p_xdb )
            {
                _wcsvector tmp_tables = table_names;
                table_names.clear();

                for( uint_t tbi = 0; tbi < tmp_tables.size(); tbi++ )
                {
                    _wstring pure_table_name = tmp_tables[ tbi ];
                    pfstd::strextractstring( pure_table_name, L' ' );

                    if( IfTableExist( p_xdb, pure_table_name.c_str() ) )
                    {
                        table_names.push_back( pure_table_name );
                    }
                }
                return;
            }

            //=========================================================================
            //
            // Select 如果存在的畫
            //=========================================================================
            XDBResult SelectIfTableExist( _IN XDatabase* p_xdb, _IN const _IN char_t* colums, _IN const char_t* table_name, _IN const char_t* condition )
            {
                if( p_xdb == NULL )
                {
                    return NULL;
                }

                _mstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, ' ' );

                _mstring sql_string;

                // 先檢查該table 存不存在
                XDBResult ret = NULL;
                if( IfTableExist( p_xdb, table_name ) )
                {
                    // 檢查table 有沒有這筆資料
                    pfstd::strprintf( sql_string, "SELECT %s FROM %s WHERE %s", colums, pure_table_name.c_str(), condition );
                    ret = p_xdb->ExecQuery( sql_string.c_str() );
                }
                return ret;
            }

            XDBResult SelectIfTableExist( _IN XDatabase* p_xdb, _IN const wchar_t* colums, _IN const wchar_t* table_name, _IN const wchar_t* condition )
            {
                if( p_xdb == NULL )
                {
                    return NULL;
                }

                _wstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, L' ' );

                _wstring sql_string;

                // 先檢查該table 存不存在
                XDBResult ret = NULL;
                if( IfTableExist( p_xdb, table_name ) )
                {
                    // 檢查table 有沒有這筆資料
                    pfstd::strprintf( sql_string, L"SELECT %s FROM %s WHERE %s", colums, pure_table_name.c_str(), condition );
                    ret = p_xdb->ExecQuery( sql_string.c_str() );
                }
                return ret;
            }

            //=========================================================================
            //
            // 選擇所有 名稱 Like 條件的table 傳回table 數量
            //=========================================================================
            size64_t SelectAllTableNames( _OUT _mbsvector& table_names, _IN XDatabase* p_xdb, _IN const char_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return 0;
                }

                _mstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, ' ' );

                _mstring sql_string;
                size64_t count = 0;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_name_keyword.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                count = p_xdb->GetResultSize( ret1 );
                for( size64_t i = 0; i < count; i++ )
                {
                    table_names.push_back( p_xdb->GetQueryResultM( ret1, 0, i ) );
                }
                p_xdb->FreeResult( ret1 );
                return count;
            }

            size64_t SelectAllTableNames( _OUT _wcsvector& table_names, _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return 0;
                }

                _wstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, L' ' );

                _wstring sql_string;
                size64_t count = 0;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_name_keyword.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                count = p_xdb->GetResultSize( ret1 );
                for( size64_t i = 0; i < count; i++ )
                {
                    table_names.push_back( p_xdb->GetQueryResultW( ret1, 0, i ) );
                }
                p_xdb->FreeResult( ret1 );
                return count;
            }

            //=========================================================================
            //
            // 取得資料表最後建立的時間
            //=========================================================================
            _mstring SelectTableCreateTime( _IN XDatabase* p_xdb, _IN const char_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return "";
                }

                _mstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, ' ' );

                _mstring sql_string;
                _mstring time_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( "SELECT CREATE_TIME FROM information_schema.TABLES WHERE TABLE_NAME = '%s' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_table_name.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    time_string = p_xdb->GetQueryResultM( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return time_string;
            }

            _wstring SelectTableCreateTime( _IN XDatabase* p_xdb, _IN const wchar_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return L"";
                }

                _wstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, L' ' );

                _wstring sql_string;
                _wstring time_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SELECT CREATE_TIME FROM information_schema.TABLES WHERE TABLE_NAME = '%s' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_table_name.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    time_string = p_xdb->GetQueryResultW( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return time_string;
            }

            //=========================================================================
            //
            // 取得表 編碼方式 
            //=========================================================================
            _mbsmap SelectTableInformation( _IN XDatabase* p_xdb, _IN const char_t* table_name )
            {
                _mbsmap retmap;

                if( p_xdb == NULL )
                {
                    return retmap;
                }

                _mstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, ' ' );

                _mstring sql_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string,
                                  ( "SELECT * \
                                         FROM information_schema.TABLES AS ISTB, \
                                              information_schema.COLLATION_CHARACTER_SET_APPLICABILITY AS CCSA\
                                         WHERE TABLE_NAME = '%s' AND \
                                               TABLE_SCHEMA = '%s' AND \
                                               TABLE_TYPE = 'BASE TABLE' AND\
                                               CCSA.COLLATION_NAME = ISTB.TABLE_COLLATION" ), pure_table_name.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    retmap = p_xdb->GetResultRecordM( ret1, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return retmap;
            }

            _wcsmap SelectTableInformation( _IN XDatabase* p_xdb, _IN const wchar_t* table_name )
            {
                _wcsmap retmap;

                if( p_xdb == NULL )
                {
                    return retmap;
                }

                _wstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, L' ' );

                _wstring sql_string;
                _wstring time_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string,
                                  ( L"SELECT * \
                                         FROM information_schema.TABLES AS ISTB, \
                                              information_schema.COLLATION_CHARACTER_SET_APPLICABILITY AS CCSA\
                                         WHERE TABLE_NAME = '%s' AND \
                                               TABLE_SCHEMA = '%s' AND \
                                               TABLE_TYPE = 'BASE TABLE' AND\
                                               CCSA.COLLATION_NAME = ISTB.TABLE_COLLATION" ), pure_table_name.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    retmap = p_xdb->GetResultRecordW( ret1, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return retmap;
            }

            //=========================================================================
            //
            // 取得資料表 欄位資訊
            //=========================================================================
            _mbstablesi SelectTableColumn( _IN XDatabase* p_xdb, _IN const char_t* table_name )
            {
                _mbstablesi retmap;

                if( p_xdb == NULL )
                {
                    return retmap;
                }

                _mstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, ' ' );

                _mstring sql_string;
                _mstring time_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string,
                                  ( "SELECT * \
                                         FROM information_schema.COLUMNS \
                                         WHERE TABLE_NAME = '%s' AND \
                                               TABLE_SCHEMA = '%s'" ), pure_table_name.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetQuerySuccessful( ret1 ) > 0 )
                {
                    retmap = p_xdb->GetResultTableM( ret1 );
                }
                p_xdb->FreeResult( ret1 );
                return retmap;
            }

            _wcstablesi SelectTableColumn( _IN XDatabase* p_xdb, _IN const wchar_t* table_name )
            {
                _wcstablesi retmap;

                if( p_xdb == NULL )
                {
                    return retmap;
                }

                _wstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, L' ' );

                _wstring sql_string;
                _wstring time_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string,
                                  ( L"SELECT * \
                                         FROM information_schema.COLUMNS \
                                         WHERE TABLE_NAME = '%s' AND \
                                               TABLE_SCHEMA = '%s'" ), pure_table_name.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetQuerySuccessful( ret1 ) > 0 )
                {
                    retmap = p_xdb->GetResultTableW( ret1 );
                }
                p_xdb->FreeResult( ret1 );
                return retmap;
            }

            //=========================================================================
            //
            // 取得資料表最後更新的時間
            //=========================================================================
            _mstring SelectTableUpdateTime( _IN XDatabase* p_xdb, _IN const char_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return "";
                }

                _mstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, ' ' );

                _mstring sql_string;
                _mstring time_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( "SELECT UPDATE_TIME FROM information_schema.TABLES WHERE TABLE_NAME = '%s' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_table_name.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    time_string = p_xdb->GetQueryResultM( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return time_string;
            }

            _wstring SelectTableUpdateTime( _IN XDatabase* p_xdb, _IN const wchar_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return L"";
                }

                _wstring pure_table_name = table_name;
                pfstd::strextractstring( pure_table_name, L' ' );

                _wstring sql_string;
                _wstring time_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SELECT UPDATE_TIME FROM information_schema.TABLES WHERE TABLE_NAME = '%s' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE'" ), pure_table_name.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    time_string = p_xdb->GetQueryResultW( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return time_string;
            }

            //=========================================================================
            //
            // 最早建立的表
            //=========================================================================
            _mstring SelectFirstTableNameByCreate( _IN XDatabase* p_xdb, _IN const char_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return "";
                }

                _mstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, ' ' );

                _mstring table_name;
                _mstring sql_string;

                // 先檢查該table 存不存在
                pfstd::strprintf( sql_string, ( "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY CREATE_TIME ASC LIMIT 0, 1" ), pure_name_keyword.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultM( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            _wstring SelectFirstTableNameByCreate( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return L"";
                }

                _wstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, L' ' );

                _wstring table_name;
                _wstring sql_string;

                // 先檢查該table 存不存在
                pfstd::strprintf( sql_string, L"SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY CREATE_TIME ASC LIMIT 0, 1", pure_name_keyword.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultW( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            //=========================================================================
            //
            // 最晚建立的表
            //=========================================================================
            _mstring SelectLastTableNameByCreate( _IN XDatabase* p_xdb, _IN const char_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return "";
                }

                _mstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, ' ' );

                _mstring table_name;
                _mstring sql_string;

                // 先檢查該table 存不存在
                pfstd::strprintf( sql_string, ( "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY CREATE_TIME DESC LIMIT 0, 1" ), pure_name_keyword.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultM( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            _wstring SelectLastTableNameByCreate( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return L"";
                }

                _wstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, L' ' );

                _wstring table_name;
                _wstring sql_string;

                // 先檢查該table 存不存在
                pfstd::strprintf( sql_string, ( L"SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY CREATE_TIME DESC LIMIT 0, 1" ), pure_name_keyword.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultW( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            //=========================================================================
            //
            // 名字排序建立第一的表
            //=========================================================================
            _mstring SelectFirstTableNameByName( _IN XDatabase* p_xdb, _IN const char_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return "";
                }

                _mstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, ' ' );

                _mstring table_name;
                _mstring sql_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY TABLE_NAME ASC LIMIT 0, 1" ), pure_name_keyword.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultM( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            _wstring SelectFirstTableNameByName( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return L"";
                }

                _wstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, L' ' );

                _wstring table_name;
                _wstring sql_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY TABLE_NAME ASC LIMIT 0, 1" ), pure_name_keyword.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultW( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            //=========================================================================
            //
            // 名字排序建立最後的表
            //=========================================================================
            _mstring SelectLastTableNameByName( _IN XDatabase* p_xdb, _IN const char_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return "";
                }

                _mstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, ' ' );

                _mstring table_name;
                _mstring sql_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY TABLE_NAME DESC LIMIT 0, 1" ), pure_name_keyword.c_str(), p_xdb->GetDBNameM().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultM( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            _wstring SelectLastTableNameByName( _IN XDatabase* p_xdb, _IN const wchar_t* name_keyword )
            {
                if( p_xdb == NULL )
                {
                    return L"";
                }

                _wstring pure_name_keyword = name_keyword;
                pfstd::strextractstring( pure_name_keyword, L' ' );

                _wstring table_name;
                _wstring sql_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s' AND TABLE_TYPE = 'BASE TABLE' ORDER BY TABLE_NAME DESC LIMIT 0, 1" ), pure_name_keyword.c_str(), p_xdb->GetDBNameW().c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    table_name = p_xdb->GetQueryResultW( ret1, 0, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return table_name;
            }

            //=========================================================================
            //
            // 取得建立 table 的 SQL
            //=========================================================================
            _xdb_sql_mstring ShowCreateTableSQL( _IN XDatabase* p_xdb, _IN const char_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return "";
                }

                _mstring sql_string;
                _xdb_sql_mstring create_table_sql_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( "SHOW CREATE TABLE %s" ), table_name );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    create_table_sql_string = p_xdb->GetQueryResultM( ret1, 1, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return create_table_sql_string;
            }

            _xdb_sql_wstring ShowCreateTableSQL( _IN XDatabase* p_xdb, _IN const wchar_t* table_name )
            {
                if( p_xdb == NULL )
                {
                    return L"";
                }

                _wstring sql_string;
                _xdb_sql_wstring create_table_sql_string;

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SHOW CREATE TABLE %s" ), table_name );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                if( p_xdb->GetResultSize( ret1 ) > 0 )
                {
                    create_table_sql_string = p_xdb->GetQueryResultW( ret1, 1, 0 );
                }
                p_xdb->FreeResult( ret1 );
                return create_table_sql_string;
            }

            //=========================================================================
            //
            // 取得 table 的 primary key
            //=========================================================================
            _mstring_vector SelectTableIndexes( _IN XDatabase* p_xdb, _IN const char_t* table_name, _IN const char_t* key, _IN bool include_auto )
            {
                _mstring_vector ret;
                if( p_xdb == NULL )
                {
                    return ret;
                }

                _mstring sql_string;
                _mstring condition;
                if( !include_auto )
                {
                    pfstd::strprintf( condition, ( "AND EXTRA <> 'auto_increment'" ) );
                }
                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( "SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS \
                                                        WHERE TABLE_SCHEMA = '%s' AND TABLE_NAME = '%s' AND COLUMN_KEY = '%s' %s\
                                                        ORDER BY ORDINAL_POSITION" ), p_xdb->GetDBNameM().c_str(), table_name, key, condition.c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                for( uint_t ri = 0; ri < p_xdb->GetResultSize( ret1 ); ri++ )
                {
                    ret.push_back( p_xdb->GetQueryResultM( ret1, 0, 1 ) );
                }
                p_xdb->FreeResult( ret1 );
                return ret;
            }

            _wstring_vector SelectTableIndexes( _IN XDatabase* p_xdb, _IN const wchar_t* table_name, _IN const wchar_t* key, _IN bool include_auto )
            {
                _wstring_vector ret;
                if( p_xdb == NULL )
                {
                    return ret;
                }

                _wstring sql_string;
                _mstring condition;
                if( !include_auto )
                {
                    pfstd::strprintf( condition, ( "AND EXTRA <> 'auto_increment'" ) );
                }

                // 先檢查該table 存不存在
                //bool bexist = false;
                pfstd::strprintf( sql_string, ( L"SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS \
                                                  WHERE TABLE_SCHEMA = '%s' AND TABLE_NAME = '%s' AND COLUMN_KEY = '%s' %s\
                                                  ORDER BY ORDINAL_POSITION" ), p_xdb->GetDBNameM().c_str(), table_name, key, condition.c_str() );
                XDBResult ret1 = p_xdb->ExecQuery( sql_string.c_str() );
                for( uint_t ri = 0; ri < p_xdb->GetResultSize( ret1 ); ri++ )
                {
                    ret.push_back( p_xdb->GetQueryResultW( ret1, 0, 1 ) );
                }
                p_xdb->FreeResult( ret1 );
                return ret;
            }
        }

        namespace syntax
        {
            //=========================================================================
            //
            //
            //=========================================================================
            _mstring SelectFromWhereUnionAll( const char_t* expression, const _mbsvector& reference_tables, const char_t* condition )
            {
                if( reference_tables.empty() )
                    return _T( "" );

                _mstring sql_string;

                if( condition == NULL )
                {
                    for( uint_t ti = 0; ti < reference_tables.size(); ti++ )
                    {
                        _mstring expression00;
                        pfstd::strprintf( expression00, ( "%s, '%s' AS record_from_table_name" ), expression, reference_tables[ ti ].c_str() );

                        pfstd::strprintfa( sql_string, ( "( SELECT %s FROM %s ) UNION ALL " ), expression00.c_str(), reference_tables[ ti ].c_str() );
                    }
                }
                else
                {
                    for( uint_t ti = 0; ti < reference_tables.size(); ti++ )
                    {
                        _mstring expression00;
                        pfstd::strprintf( expression00, ( "%s, '%s' AS record_from_table_name" ), expression, reference_tables[ ti ].c_str() );

                        pfstd::strprintfa( sql_string, ( "( SELECT %s FROM %s WHERE %s ) UNION ALL " ), expression00.c_str(), reference_tables[ ti ].c_str(), condition );
                    }
                }
                sql_string.erase( sql_string.end() - strlen( ( " UNION ALL " ) ), sql_string.end() );
                return sql_string;
            }

            _wstring  SelectFromWhereUnionAll( const wchar_t* expression, const _wcsvector& reference_tables, const wchar_t* condition )
            {
                if( reference_tables.empty() )
                    return L"";

                _wstring sql_string;

                if( condition == NULL )
                {
                    for( uint_t ti = 0; ti < reference_tables.size(); ti++ )
                    {
                        _wstring expression00;
                        pfstd::strprintf( expression00, L"%s, '%s' AS record_from_table_name", expression, reference_tables[ ti ].c_str() );

                        pfstd::strprintfa( sql_string, ( L"( SELECT %s FROM %s ) UNION ALL " ), expression00.c_str(), reference_tables[ ti ].c_str() );
                    }
                }
                else
                {
                    for( uint_t ti = 0; ti < reference_tables.size(); ti++ )
                    {
                        _wstring expression00;
                        pfstd::strprintf( expression00, L"%s, '%s' AS record_from_table_name", expression, reference_tables[ ti ].c_str() );

                        pfstd::strprintfa( sql_string, ( L"( SELECT %s FROM %s WHERE %s ) UNION ALL " ), expression00.c_str(), reference_tables[ ti ].c_str(), condition );
                    }
                }
                sql_string.erase( sql_string.end() - wcslen( ( L" UNION ALL " ) ), sql_string.end() );
                return sql_string;
            }

            //=========================================================================
            //
            //
            //=========================================================================
            _mstring INSERT_INTO( char_t* table_name, const _mbsmap* fv )
            {
                _mstring sql_string;

                _mstring fields;
                _mstring values;

                {
                    _mbsmap::const_iterator iter = fv->begin();
                    while( iter != fv->end() )
                    {
                        fields = fields + iter->first + ", ";
                        values = values + "'" + iter->second + "', ";
                        iter++;
                    }
                    fields.erase( fields.end() - strlen( ( ", " ) ), fields.end() );
                    values.erase( values.end() - strlen( ( ", " ) ), values.end() );
                }

                pfstd::strprintf( sql_string, "INSERT INTO %s( %s ) VALUES( %s ) ", table_name, fields.c_str(), values.c_str() );
                return sql_string;
            }

            _wstring INSERT_INTO( wchar_t* table_name, const _wcsmap* fv )
            {
                _wstring sql_string;

                _wstring fields;
                _wstring values;

                {
                    _wcsmap::const_iterator iter = fv->begin();
                    while( iter != fv->end() )
                    {
                        fields = fields + iter->first + L", ";
                        values = values + L"'" + iter->second + L"', ";
                        iter++;
                    }
                    fields.erase( fields.end() - wcslen( ( L", " ) ), fields.end() );
                    values.erase( values.end() - wcslen( ( L", " ) ), values.end() );
                }

                pfstd::strprintf( sql_string, L"INSERT INTO %s( %s ) VALUES( %s ) ", table_name, fields.c_str(), values.c_str() );
                return sql_string;
            }

            //=========================================================================
            //
            //
            //=========================================================================
            _mstring UPDATE_SET( char_t* table_name, const _mbsmap* fv, char_t* where_clause )
            {
                _mstring sql_string;

                _mstring set_string;

                {
                    _mbsmap::const_iterator iter = fv->begin();
                    while( iter != fv->end() )
                    {
                        set_string = set_string + iter->first + " = '" + iter->second + "', ";
                        iter++;
                    }
                    set_string.erase( set_string.end() - strlen( ( ", " ) ), set_string.end() );
                }

                if( ( where_clause == NULL ) || ( strcmp( where_clause, "" ) == 0 ) )
                {
                    pfstd::strprintf( sql_string, "UPDATE %s SET %s", table_name, set_string.c_str() );
                }
                else
                {
                    pfstd::strprintf( sql_string, "UPDATE %s SET %s WHERE %s", table_name, set_string.c_str(), where_clause );
                }
                return sql_string;
            }

            _wstring UPDATE_SET( _IN wchar_t* table_name, _IN const _wcsmap* fv, wchar_t* where_clause )
            {
                _wstring sql_string;

                _wstring set_string;

                {
                    _wcsmap::const_iterator iter = fv->begin();
                    while( iter != fv->end() )
                    {
                        set_string = set_string + iter->first + L" = '" + iter->second + L"', ";
                        iter++;
                    }
                    set_string.erase( set_string.end() - wcslen( ( L", " ) ), set_string.end() );
                }

                if( ( where_clause == NULL ) || ( wcscmp( where_clause, L"" ) == 0 ) )
                {
                    pfstd::strprintf( sql_string, L"UPDATE %s SET %s", table_name, set_string.c_str() );
                }
                else
                {
                    pfstd::strprintf( sql_string, L"UPDATE %s SET %s WHERE %s", table_name, set_string.c_str(), where_clause );
                }
                return sql_string;
            }
        }
    }
}