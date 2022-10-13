#include <sstream>

#include <pf-stdlib.h>
#include <gadget/sentence_pattern.h>

#include "json_type.h"

namespace
{
    bool IsObjectString( const char* str )
    {
        int strlne = strlen( str );
        if( strlne == 0 )
            return false;

        for( int i = 0; i < strlne; i++ )
        {
            if( ( str[ i ] != ( ' ' ) ) && ( str[ i ] != ( '{' ) ) )
                return false;
            else
                break;
        }

        for( int i = strlne - 1; i >= 0; i-- )
        {
            if( ( str[ i ] != ( ' ' ) ) && ( str[ i ] != ( '}' ) ) )
                return false;
            else
                break;
        }
        return true;
    }

    bool IsArrayString( const char* str )
    {
        uint_t strlne = strlen( str );
        if( strlne == 0 )
            return false;

        for( uint_t i = 0; i < strlne; i++ )
        {
            if( ( str[ i ] != ( ' ' ) ) && ( str[ i ] != ( '[' ) ) )
                return false;
            else
                break;
        }

        for( uint_t i = strlne - 1; i >= 0; i-- )
        {
            if( ( str[ i ] != ( ' ' ) ) && ( str[ i ] != ( ']' ) ) )
                return false;
            else
                break;
        }
        return true;
    }

#define _UNKNOW_TYPE_NAME   ( "Json_Unknow" )
#define _OBJECT_TYPE_NAME  ( "Json_Object" )
#define _ARRAY_TYPE_NAME   ( "Json_Array" )
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
JsonType::JsonType()
{
    type_name_ = _UNKNOW_TYPE_NAME;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
JsonType::~JsonType()
{

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
uint_t JsonType::size()
{
    return pairs_.size();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring& JsonType::operator[]( const _mstring& field )
{
    if( pairs_.empty() )
        type_name_ = _OBJECT_TYPE_NAME;

    if( type_name_ == _OBJECT_TYPE_NAME )
    {
        return pairs_[ field ];
    }
    _mstring str;
    return str;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring& JsonType::operator[]( const uint_t& index )
{
    if( pairs_.empty() )
        type_name_ = _ARRAY_TYPE_NAME;

    if( type_name_ == _ARRAY_TYPE_NAME )
    {
        return pairs_[ pfstd::uitostr( index ) ];
    }
    _mstring str;
    return str;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring JsonType::Stringify()
{
    _mstring result_string;
    if( type_name_ == _OBJECT_TYPE_NAME )
    {
        result_string = ( "{" );

        for( std::map< _mstring, _mstring >::iterator it = pairs_.begin(); it != pairs_.end(); it++ )
        {
            _mstring pair_string;
            // 是一個陣列 或是一個 物件值
            if( ( IsArrayString( it->second.c_str() ) ) || ( IsObjectString( it->second.c_str() ) ) )
            {
                pfstd::strprintf( pair_string, ( "\"%s\":%s," ), EscapeString( it->first ).c_str(), it->second.c_str() );
            }
            else
            {
                pfstd::strprintf( pair_string, ( "\"%s\":\"%s\"," ), EscapeString( it->first ).c_str(), EscapeString( it->second ).c_str() );
            }
            result_string += pair_string;
        }
        result_string.erase( result_string.end() - strlen( ( "," ) ), result_string.end() );
        result_string += _T( "}" );
    }
    else if( type_name_ == _ARRAY_TYPE_NAME )
    {
        result_string = ( "[" );
        _mbsvector array_string;
        array_string.resize( pairs_.size() );

        for( std::map< _mstring, _mstring >::iterator it = pairs_.begin(); it != pairs_.end(); it++ )
        {
            uint_t cur_idx = _pf_ttoui( it->first );
            // 如果不夠 就加大空間
            if( cur_idx >= array_string.size() )
                array_string.resize( cur_idx );

            _mstring pair_string;
            // 是一個陣列 或是一個 物件值
            if( ( IsArrayString( it->second.c_str() ) ) || ( IsObjectString( it->second.c_str() ) ) )
            {
                array_string[ cur_idx ] = it->second;
            }
            else
            {
                array_string[ cur_idx ] = ( "\"" );
                array_string[ cur_idx ] += EscapeString( it->second );
                array_string[ cur_idx ] += ( "\"" );;
            }
        }

        for( uint_t array_idx = 0; array_idx < array_string.size(); array_idx++ )
        {
            result_string += array_string[ array_idx ] + ( "," );
        }
        result_string.erase( result_string.end() - strlen( ( "," ) ), result_string.end() );
        result_string += _T( "]" );
    }

    return result_string;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool JsonType::Parser( const char* str )
{
    int ret = false;
    type_name_ = _UNKNOW_TYPE_NAME;
    pairs_.clear();

    _mstring str_buf = str;
    pfstd::strextractstring( str_buf );

    if( IsObjectString( str_buf.c_str() ) )
    {
        type_name_ = _OBJECT_TYPE_NAME;
        // 去頭去尾
        str_buf.erase( str_buf.begin() );
        str_buf.pop_back();

        _mbsvector      out_vec;
        std::vector<char> delim_vec;

        sentence_pattern::SplitString01( out_vec, delim_vec, str_buf, ",;" );

        for( uint_t jsi = 0; jsi < out_vec.size(); jsi++ )
        {
            _mbsvector      obj_out_vec;
            std::vector<char> obj_delim_vec;

            sentence_pattern::SplitString01( obj_out_vec, obj_delim_vec, out_vec[ jsi ].c_str(), ":" );

            // 一定要是 'AAA' = BBB 的形式
            if( obj_out_vec.size() == 2 )
            {
                // 欄位一定要是字串
                if( ( obj_out_vec[ 0 ].front() == ( '\"' ) ) && ( obj_out_vec[ 0 ].back() == ( '\"' ) ) )
                {
                    pfstd::strextractstring( obj_out_vec[ 0 ] );
                    // 去頭去尾
                    obj_out_vec[ 0 ].erase( obj_out_vec[ 0 ].begin() );
                    obj_out_vec[ 0 ].pop_back();
                }
                if( ( obj_out_vec[ 1 ].front() == ( '\"' ) ) && ( obj_out_vec[ 1 ].back() == ( '\"' ) ) )
                {
                    pfstd::strextractstring( obj_out_vec[ 1 ] );
                    // 去頭去尾
                    //obj_out_vec[ 1 ].erase( obj_out_vec[ 1 ].begin() );
                    //obj_out_vec[ 1 ].pop_back();
                }
                pairs_.insert( _mbsmap::value_type( obj_out_vec[ 0 ], obj_out_vec[ 1 ] ) );
            }
        }
    }
    else if( IsArrayString( str_buf.c_str() ) )
    {
        type_name_ = _ARRAY_TYPE_NAME;
        // 去頭去尾
        str_buf.erase( str_buf.begin() );
        str_buf.pop_back();

        _mbsvector        out_vec;
        std::vector<char> delim_vec;

        sentence_pattern::SplitString01( out_vec, delim_vec, str_buf, "," );

        for( uint_t jsi = 0; jsi < out_vec.size(); jsi++ )
        {
            pfstd::strextractstring( out_vec[ jsi ] );
            if ( out_vec[ jsi ] == _T( "" ) )
                continue;

            // 去頭去尾
            if( ( out_vec[ jsi ].front() == ( '\"' ) ) && ( out_vec[ jsi ].back() == ( '\"' ) ) )
            {
                pfstd::strextractstring( out_vec[ jsi ] );
                // 去頭去尾
                //out_vec[ jsi ].erase( out_vec[ jsi ].begin() );
                //out_vec[ jsi ].pop_back();
            }
            pairs_.insert( _mbsmap::value_type( _pf_uitot( jsi ), out_vec[ jsi ] ) );
        }
    }
    return ret;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring JsonType::ToString( _mstring str )
{
    int bi = 0;
    int ei = str.size() - 1;
    if ( str[ bi ] == '\"' )
    {
        bi++;
    }

    if ( str[ ei ] == '\"' )
    {
        ei--;
    }
    return str.substr( bi, ei );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring JsonType::EscapeString( _mstring str )
{
    std::ostringstream ss;
    for ( auto iter = str.cbegin(); iter != str.cend(); iter++ ) 
    {
        //C++98/03:
        //for (_mstring::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch ( *iter ) {
        case '\\': ss << "\\\\"; break;
        case '"': ss << "\\\""; break;
        case '/': ss << "\\/"; break;
        case '\b': ss << "\\b"; break;
        case '\f': ss << "\\f"; break;
        case '\n': ss << "\\n"; break;
        case '\r': ss << "\\r"; break;
        case '\t': ss << "\\t"; break;
        default: ss << *iter; break;
        }
    }
    return ss.str();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
bool JsonType::ParserUri( const char* uri )
{
    char* buf = new char[ _tcslen( uri ) + 1 ];
    _tcscpy( buf, uri );
    char* p = strtok( buf, "&" );

    while( p )
    {
        char* pkey = p;
        char* pval = p;
        int   len = 0;
        {
            for( size_t i = 0; i < strlen( p ); i++ )
            {
                if( p[ i ] == '=' )
                {
                    len = i;
                    pval = &p[ i + 1 ];
                    break;
                }
            }
        }
        operator[]( pkey ) = pval;
        p = strtok( NULL, "&" );
    }

    SAFE_DELETE_ARRAY( buf );
    return false;
}
*/