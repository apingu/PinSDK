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
#include <string.h>
#include <stdarg.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xdatabase.h"


//=============================================================================
// NAME:
// DESC:
//=============================================================================
XDatabase::XDatabase()
{
    charset_ = "utf8mb4";
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
XDatabase::~XDatabase()
{
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool XDatabase::Connect( _IN const wchar_t* db, _IN const wchar_t* host, _IN unsigned short port, _IN const wchar_t* username, _IN const wchar_t* password )
{
    return Connect( pfstd::UnicodeToUTF8( db ).c_str(), pfstd::UnicodeToUTF8( host ).c_str(), port, pfstd::UnicodeToUTF8( username ).c_str(), pfstd::UnicodeToUTF8( password ).c_str() );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool XDatabase::SelectDatabase( _IN const wchar_t* db )
{
    return SelectDatabase( pfstd::UnicodeToUTF8( db ).c_str() );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool XDatabase::SetCharSet( _IN const wchar_t* char_set )
{
    return SetCharSet( pfstd::UnicodeToUTF8( char_set ).c_str() );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_wstring XDatabase::OnEscapeString( _IN const wchar_t* str, _IN size32_t len )
{
    return pfstd::UTF8ToUnicode( OnEscapeString( pfstd::UnicodeToUTF8( str ).c_str(), len ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void XDatabase::SetDBCode( _IN _mstring code )
{
    db_code_ = code;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring XDatabase::GetDBCode( void )
{
    return db_code_;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring XDatabase::GetDBNameM( void )
{
    return db_name_;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_wstring XDatabase::GetDBNameW( void )
{
    return pfstd::UTF8ToUnicode( GetDBNameM() );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
XDBResult XDatabase::ExecQuery( _IN const wchar_t* query )
{
    return ExecQuery( pfstd::UnicodeToUTF8( query ).c_str() );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_wstring XDatabase::GetQueryErrMsgW( _IN const XDBResult result )
{
    return pfstd::UTF8ToUnicode( GetQueryErrMsgM( result ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring XDatabase::GetQueryResultM( _IN const XDBResult result, _IN _mstring col_name, _IN uint_t row )
{
    return GetQueryResultM( result, GetFieldIndex( result, col_name.c_str() ), row );
}

_wstring XDatabase::GetQueryResultW( _IN const XDBResult result, _IN uint_t col, _IN uint_t row )
{
    return pfstd::UTF8ToUnicode( GetQueryResultM( result, col, row ) );
}

_wstring XDatabase::GetQueryResultW( _IN const XDBResult result, _IN _wstring col_name, _IN uint_t row )
{
    return pfstd::UTF8ToUnicode( GetQueryResultM( result, pfstd::UnicodeToUTF8(col_name ).c_str(), row));
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_wstring XDatabase::GetFieldsNameW( XDBResult result, uint_t n )
{
    return pfstd::UTF8ToUnicode( GetFieldsNameM( result, n ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
uint_t XDatabase::GetFieldIndex( _IN XDBResult result, const wchar_t* col_name )
{
    return GetFieldIndex( result, pfstd::UnicodeToUTF8( col_name ).c_str() );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mbsmap XDatabase::GetResultRecordM( const XDBResult result, uint_t row )
{
    if( result == NULL )
    {
        return _mbsmap();
    }

    // 超過了
    size32_t row_size = GetResultSize( result );
    if( row >= row_size )
    {
        return _mbsmap();
    }

    _mbsmap record;

    for( uint_t ci = 0; ci < GetFieldsSize( result ); ci++ )
    {
        record[ GetFieldsNameM( result, ci ) ] = GetQueryResultM( result, ci, row );
    }
    return record;
}

_wcsmap XDatabase::GetResultRecordW( const XDBResult result, uint_t row )
{
    if( result == NULL )
    {
        return _wcsmap();
    }

    size32_t row_size = GetResultSize( result );
    if( row >= row_size )
    {
        return _wcsmap();
    }

    _wcsmap record;

    for( uint_t ci = 0; ci < GetFieldsSize( result ); ci++ )
    {
        record[ GetFieldsNameW( result, ci ) ] = GetQueryResultW( result, ci, row );
    }
    return record;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
//std::vector<_mbsmap> XDatabase::GetResultMMap( const XDBResult result )
_mbstablesi XDatabase::GetResultTableM( const XDBResult result )
{
    if( result == NULL )
    {
        return std::map< _mstring, _mbsvector >();
    }

    std::map< _mstring, _mbsvector > table;

    size32_t rows = GetResultSize( result );
    size32_t cols = GetFieldsSize( result );

    for( uint_t ci = 0; ci < cols; ci++ )
    {
        _mbsvector record;
        for( uint_t ri = 0; ri < rows; ri++ )
        {
            _mstring value = GetQueryResultM( result, ci, ri );
            record.push_back( value );
        }
        table[ GetFieldsNameM( result, ci ) ] = record;
    }
    return table;

    //if ( result == NULL )
    //{
    //    return std::vector<_mbsmap>();
    //}
    //
    //std::vector<_mbsmap> table;
    //
    ////TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 20 );
    //int rows = GetResultSize( result );
    //int cols = GetFieldsSize( result );
    //
    //for ( uint_t ri = 0; ri < rows; ri++ )
    //{
    //    _mbsmap record;
    //    for ( uint_t ci = 0; ci < cols; ci++ )
    //    {
    //        record[ GetFieldsName( result, ci ) ] = GetQueryResultM( result, ci, ri );
    //    }
    //    table.push_back( record );
    //}
    //
    //return table;
}

//std::vector<_wcsmap> XDatabase::GetResultWMap( const XDBResult result )
_wcstablesi XDatabase::GetResultTableW( const XDBResult result )
{
    if( result == NULL )
    {
        return std::map< _wstring, _wcsvector >();
    }

    std::map< _wstring, _wcsvector > table;

    size32_t rows = GetResultSize( result );
    size32_t cols = GetFieldsSize( result );

    for( uint_t ci = 0; ci < cols; ci++ )
    {
        _wcsvector record;
        for( uint_t ri = 0; ri < rows; ri++ )
        {
            _wstring value = GetQueryResultW( result, ci, ri );
            record.push_back( value );
        }
        table[ GetFieldsNameW( result, ci ) ] = record;
    }
    return table;

    //if ( result == NULL )
    //{
    //    return std::vector<_wcsmap>();
    //}
    //
    //std::vector<_wcsmap> table;
    //
    ////TS_Lock lock( ( (DBResult*)result )->m_pQueryDB->p_xmysql_lock_, 21 );
    //int rows = GetResultSize( result );
    //int cols = GetFieldsSize( result );
    //
    //for ( uint_t ri = 0; ri < rows; ri++ )
    //{
    //    _wcsmap record;
    //    for ( uint_t ci = 0; ci < cols; ci++ )
    //    {
    //        record[ pfstd::UTF8ToUnicode( GetFieldsName( result, ci ) ) ] = GetQueryResultW( result, ci, ri );
    //    }
    //    table.push_back( record );
    //}
    //
    //return table;

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring XDatabase::SafeEscapeString( const char_t* s )
{
    _mstring str;
    size_t sl = ( strlen( s ) * 4 ) + 1;
    char_t* out = new char_t[ sl ];
    memset( out, 0, sl * sizeof( char_t ) );


    //char_t seps[]   = "\0\'\"\b\n\r\tz\\%_";
    /*
    \0     An ASCII NUL( 0x00 ) character.
    \'     A single quote (“'”) character.
    \"     A double quote (“"”) character.
    \b     A backspace character.
    \n     A newline( linefeed ) character.
    \r     A carriage return character.
    \t     A tab character.
    \Z     ASCII 26 ( Control - Z ).See note following the table.
    \\     A backslash( “\” ) character.
    \%     A “%” character.See note following the table.
    \_     A “_” character.See note following the table.
    */

    int j = 0;
    for( size32_t i = 0; i < strlen( s ); i++ )
    {
        switch( s[ i ] )
        {
            case '\0':
            case '\'':
            case '\"':
            case '\b':
            case '\n':
            case '\r':
            case '\t':
                //case '\Z':
            case '\\':
                //case '\%':
                //case '\_':
                out[ j ] = '\\';
                j++;
            default:
                out[ j ] = s[ i ];
                j++;
                break;
        }
    }
    out[ j ] = 0;
    str = out;
    delete[] out;
    return str;
}

_wstring XDatabase::SafeEscapeString( const wchar_t* s )
{
    _wstring str;
    size_t sl = ( wcslen( s ) * 4 ) + 1;
    wchar_t* out = new wchar_t[ sl ];
    memset( out, 0, sl * sizeof( wchar_t ) );

    //char_t seps[]   = "\0\'\"\b\n\r\tz\\%_";
    int j = 0;
    for( size32_t i = 0; i < wcslen( s ); i++ )
    {
        switch( s[ i ] )
        {
            case L'\0':
            case L'\'':
            case L'\"':
            case L'\b':
            case L'\n':
            case L'\r':
            case L'\t':
                //case L'\Z':
            case L'\\':
                //case L'\%':
                //case L'\_':
                out[ j ] = L'\\';
                j++;
            default:
                out[ j ] = s[ i ];
                j++;
                break;
        }
    }
    str = out;
    delete[] out;
    return str;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void XDatabase::AppendStringTable( _OUT _mbstableii& retult_table, _IN XDBResult result )
{
    int rows = GetResultSize( result );
    int cols = GetFieldsSize( result );
    for( uint_t ri = 0; ri < rows; ri++ )
    {
        _mbsvector row_string;
        row_string.resize( rows );
        for( uint_t ci = 0; ci < cols; ci++ )
        {
            row_string.push_back( GetQueryResultM( result, ci, ri ) );
        }
        retult_table.push_back( row_string );
    }
}

void XDatabase::AppendStringTable( _OUT _wcstableii& retult_table, _IN XDBResult result )
{
    int rows = GetResultSize( result );
    int cols = GetFieldsSize( result );

    for( uint_t ri = 0; ri < rows; ri++ )
    {
        _wcsvector row_string;
        for( uint_t ci = 0; ci < cols; ci++ )
        {
            row_string.push_back( GetQueryResultW( result, ci, ri ) );
        }
        retult_table.push_back( row_string );
    }
}