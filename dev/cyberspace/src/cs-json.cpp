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

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "cs-json.h"


namespace cyberspace
{
    //////////////////////////////////////////////////////////////////////////
    // 
    JsonResult::JsonResult()
    {
    }

    JsonResult::~JsonResult()
    {
    }

    bool JsonResult::IsJson( const _cs_string& str )
    {
        if( ( str[ 0 ] == '{' ) && ( str.back() == '}' ) )
        {
            return true;
        }

        if( ( str[ 0 ] == '[' ) && ( str.back() == ']' ) )
        {
            return true;
        }
        return false;
    }

    void JsonResult::InsertPair( _cs_string name, _cs_string value )
    {
        std::map< _cs_string, _cs_string >::iterator it = m_pairs.find( name );
        if( it != m_pairs.end() )
        {
            it->second = value;
        }
        else
        {
            m_pairs.insert( std::map< _cs_string, _cs_string >::value_type( name, value ) );
        }
    }

    void JsonResult::RemovePair( _cs_string name )
    {
        m_pairs.erase( name );
    }

    bool JsonResult::InsertArrays( _cs_string name, const _cs_string_vector& record )
    {
        if( record.empty() )
            return false;

        _cs_string_vector* pArray = NULL;
        std::map< _cs_string, _cs_string_vector >::iterator it = m_arrays.find( name );
        if( it != m_arrays.end() )
        {
            pArray = &it->second;
        }
        else
        {
            _cs_string_vector Array;
            m_arrays.insert( std::map< _cs_string, _cs_string_vector >::value_type( name, Array ) );
            pArray = &m_arrays.find( name )->second;
        }

        //for( int row = 0; row < pXdb->GetResultSize( result ); row++ )
        {
            _cs_string record_string;
            for( uint_t col = 0; col < record.size(); col++ )
            {
                _cs_string data = record[ col ];
                if( IsJson( data ) )
                {
                    _pf_tstrprintfa( record_string, ( "%s," ), data.c_str() );
                }
                else
                {
                    _pf_tstrprintfa( record_string, ( "\"%s\"," ), data.c_str() );
                }
            }
            record_string.erase( record_string.end() - _tcslen( ( "," ) ), record_string.end() );
            pArray->push_back( record_string );
        }
        return true;
    }

    bool JsonResult::InsertArrays( _cs_string name, const _cs_string_tableii& table )
    {
        _cs_string_vector* pArray = NULL;
        std::map< _cs_string, _cs_string_vector >::iterator it = m_arrays.find( name );
        if( it != m_arrays.end() )
        {
            pArray = &it->second;
        }
        else
        {
            _cs_string_vector Array;
            m_arrays.insert( std::map< _cs_string, _cs_string_vector >::value_type( name, Array ) );
            pArray = &m_arrays.find( name )->second;
        }

        for( uint_t row = 0; row < table.size(); row++ )
        {
            _cs_string record_string;
            for( uint_t col = 0; col < table[ row ].size(); col++ )
            {
                _cs_string data = pfstd::StringToUTF8( table[ row ][ col ] );
                if( IsJson( data ) )
                {
                    _pf_tstrprintfa( record_string, ( "%s," ), data.c_str() );
                }
                else
                {
                    _pf_tstrprintfa( record_string, ( "\"%s\"," ), data.c_str() );
                }
            }
            record_string.erase( record_string.end() - _tcslen( ( "," ) ), record_string.end() );
            pArray->push_back( record_string );
        }
        return true;
    }

    bool JsonResult::InsertArrays( _cs_string name, const RawCells* cells )
    {
        _cs_string_vector* pArray = NULL;
        std::map< _cs_string, _cs_string_vector >::iterator it = m_arrays.find( name );
        if( it != m_arrays.end() )
        {
            pArray = &it->second;
        }
        else
        {
            _cs_string_vector Array;
            m_arrays.insert( std::map< _cs_string, _cs_string_vector >::value_type( name, Array ) );
            pArray = &m_arrays.find( name )->second;
        }

        for( uint_t row = 0; row < cells->GetMaxRowSize(); row++ )
        {
            _cs_string record_string;
            for( uint_t col = 0; col < cells->GetMaxColSize(); col++ )
            {
                _cs_string data = pfstd::StringToUTF8( cells->GetCell( col + 1, row + 1 ) );
                if( IsJson( data ) )
                {
                    _pf_tstrprintfa( record_string, ( "%s," ), data.c_str() );
                }
                else
                {
                    _pf_tstrprintfa( record_string, ( "\"%s\"," ), data.c_str() );
                }
            }
            record_string.erase( record_string.end() - _tcslen( ( "," ) ), record_string.end() );
            pArray->push_back( record_string );
        }
        return true;
    }

    bool JsonResult::InsertArrays( _cs_string name, XDatabase* pXdb, XDBResult result )
    {
        _cs_string_vector* pArray = NULL;
        std::map< _cs_string, _cs_string_vector >::iterator it = m_arrays.find( name );
        if( it != m_arrays.end() )
        {
            pArray = &it->second;
        }
        else
        {
            _cs_string_vector Array;
            m_arrays.insert( std::map< _cs_string, _cs_string_vector >::value_type( name, Array ) );
            pArray = &m_arrays.find( name )->second;
        }

        if( pXdb == NULL )
        {
            return false;
        }

        if( !pXdb->GetQuerySuccessful( result ) )
        {
            return false;
        }

        for( uint_t row = 0; row < pXdb->GetResultSize( result ); row++ )
        {
            _cs_string record_string;
            for( uint_t col = 0; col < pXdb->GetFieldsSize( result ); col++ )
            {
                _cs_string data = pXdb->GetQueryResultT( result, col, row );
                if( IsJson( data ) )
                {
                    _pf_tstrprintfa( record_string, ( "%s," ), data.c_str() );
                }
                else
                {
                    _pf_tstrprintfa( record_string, ( "\"%s\"," ), data.c_str() );
                }
            }
            record_string.erase( record_string.end() - _tcslen( ( "," ) ), record_string.end() );
            pArray->push_back( record_string );
        }
        return true;
    }

    void JsonResult::RemoveArrays( _cs_string name )
    {
        m_arrays.erase( name );
    }

    _cs_string JsonResult::ToJsonString()
    {
        _cs_string result_string = ( "{" );

        for( std::map< _cs_string, _cs_string >::iterator it = m_pairs.begin(); it != m_pairs.end(); it++ )
        {
            _cs_string pair_string;
            _pf_tstrprintf( pair_string, ( "\"%s\":\"%s\"," ), it->first.c_str(), it->second.c_str() );
            result_string += pair_string;
        }

        if( m_arrays.empty() )
        {
            result_string.erase( result_string.end() - _tcslen( ( "," ) ), result_string.end() );
        }
        else
        {
            for( std::map< _cs_string, _cs_string_vector >::iterator it = m_arrays.begin(); it != m_arrays.end(); it++ )
            {
                _cs_string array_string;
                _pf_tstrprintf( array_string, ( "\"%s\":[" ), it->first.c_str() );
                result_string += array_string;
                for( uint_t i = 0; i < it->second.size(); i++ )
                {
                    result_string += ( "[" );
                    result_string += it->second[ i ];  // AA, BB, CC, DD.......
                    result_string += ( "]," );
                }
                if( !it->second.empty() )
                {
                    result_string.erase( result_string.end() - _tcslen( ( "," ) ), result_string.end() );
                }
                result_string += ( "]," );
            }
            result_string.erase( result_string.end() - _tcslen( ( "," ) ), result_string.end() );
        }

        result_string += ( "}" );
        return result_string;
    }
}