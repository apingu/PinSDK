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
#include "sentence_pattern.h"


namespace sentence_pattern
{
    const _tstring _def_delime = _T( ", " );

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring SeriesString01( const _tcsvector& values, const tchar_t* edge )
    {
        _tstring querystring;
        for ( int i = 0; i < (int)values.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( "%s%s%s%s" ), edge, values[ i ].c_str(), edge, _def_delime.c_str() );
        }
        if ( !querystring.empty() )
            querystring.erase( querystring.begin() + ( querystring.size() - _def_delime.size() ), querystring.end() );
        return querystring;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    _tstring SeriesString02( const _tcsvector& values )
    {
        _tstring querystring;
        for ( int i = 0; i < values.size(); i++ )
        {
            _tstring queryfield;
            _pf_tstrprintf( queryfield, _T( "'%s'%s" ), values[i].c_str(), _def_delime.c_str() );
            querystring += queryfield;
        }
        querystring.erase( querystring.size() - _def_delime.size(), _def_delime.size() );
        return querystring;
    }
    */

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring SeriesString04( const _tcsvector& values, const tchar_t* edge, const tchar_t* delim )
    {
        _tstring querystring;
        for ( int i = 0; i < (int)values.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( "%s%s%s%s" ), edge, values[ i ].c_str(), edge, delim );
        }
        if ( !querystring.empty() )
            querystring.erase( querystring.begin() + ( querystring.size() - _tcslen( delim ) ), querystring.end() );
        return querystring;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring SeriesString07( const _tcsvector& fields, const _tcsvector& values, const tchar_t* edge, const tchar_t* delim )
    {
        if ( fields.size() != values.size() )
        {
            return _T( "" );
        }

        _tstring querystring;
        for ( int i = 0; i < (int)fields.size(); i++ )
        {
            _pf_tstrprintfa( querystring, _T( "%s = %s%s%s%s" ), fields[ i ].c_str(), edge, values[ i ].c_str(), edge, delim );
        }
        if ( !querystring.empty() )
            querystring.erase( querystring.begin() + ( querystring.size() - _tcslen( delim ) ), querystring.end() );
        return querystring;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring SeriesMaskString01( _tcsvector& out_vec, const _tcsvector& masks, const _tcsvector& values, const tchar_t* edge, const tchar_t* delim )
    {
        if ( masks.size() != values.size() )
        {
            return _T( "" );
        }

        out_vec.clear();

        _tstring querystring;
        for ( int i = 0; i < (int)values.size(); i++ )
        {
            if ( masks[ i ] != _T( "" ) )
            {
                _pf_tstrprintfa( querystring, _T( "%s%s%s%s" ), edge, values[ i ].c_str(), edge, delim );
                out_vec.push_back( values[ i ] );
            }
        }
        if ( !querystring.empty() )
            querystring.erase( querystring.begin() + ( querystring.size() - _tcslen( delim ) ), querystring.end() );
        return querystring;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring SeriesMaskString05( _tcsvector& out_vec, const _tcsvector& masks, const _tcsvector& fields, const _tcsvector& values, const tchar_t* edge, const tchar_t* delim )
    {
        if ( ( masks.size() != values.size() ) || ( masks.size() != fields.size() ) )
        {
            return _T( "" );
        }

        out_vec.clear();

        _tstring querystring;
        for ( int i = 0; i < (int)values.size(); i++ )
        {
            if ( masks[ i ] != _T( "" ) )
            {
                _pf_tstrprintfa( querystring, _T( "%s%s%s%s" ), edge, values[ i ].c_str(), edge, delim );
                out_vec.push_back( values[ i ] );
            }
            else
            {
                _pf_tstrprintfa( querystring, _T( "%s%s" ), fields[ i ].c_str(), delim );
            }
        }
        if ( !querystring.empty() )
            querystring.erase( querystring.begin() + ( querystring.size() - _tcslen( delim ) ), querystring.end() );
        return querystring;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring SeriesMaskString12( _tcsvector& out_vec, const _tcsvector& masks, const _tcsvector& values, const tchar_t* edge, const tchar_t* delim )
    {
        if ( masks.size() != values.size() )
        {
            return _T( "" );
        }

        out_vec.clear();

        _tstring querystring;
        for ( int i = 0; i < (int)values.size(); i++ )
        {
            if ( masks[ i ] == _T( "" ) )
            {
                _pf_tstrprintfa( querystring, _T( "%s%s%s%s" ), edge, values[ i ].c_str(), edge, delim );
                out_vec.push_back( values[ i ] );
            }
            else
            {
                _pf_tstrprintfa( querystring, _T( "%s%s%s%s" ), edge, _T( "" ), edge, delim );
                out_vec.push_back( _T( "" ) );
            }
        }
        if ( !querystring.empty() )
            querystring.erase( querystring.begin() + ( querystring.size() - _tcslen( delim ) ), querystring.end() );
        return querystring;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    _tstring SeriesMaskString02( _OUT _tcsvector& retval, _IN const std::vector<int>& masks, _IN const _tcsvector& values )
    {
        if ( masks.size() != values.size() )
        {
            return _T( "" );
        }

        retval.clear();

        _tstring querystring;
        for ( int i = 0; i < values.size(); i++ )
        {
            if ( masks[i] == 1 )
            {
                _tstring queryfield;
                _pf_tstrprintf( queryfield, _T( "'%s'%s" ), values[i].c_str(), _def_delime.c_str() );
                querystring += queryfield;
                retval.push_back( values[i] );
            }
        }
        querystring.erase( querystring.size() - _def_delime.size(), _def_delime.size() );
        return querystring;
    }
    */

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    const char_t* ExtraEmpty( _mstring& s )
    {
        if ( !s.empty() )
        {
            char_t* buff = new char[ s.size() + 1 ];

            // 去除前後的空格
            if ( ( s[ 0 ] == ( ' ' ) ) || ( s.back() == ( ' ' ) ) )
            {
                strcpy( buff, s.c_str() );
                strextractstring( buff );
                s = buff;
            }
            //}
            if ( !s.empty() )
            {
                if ( ( s[ 0 ] == ( '\t' ) ) || ( s.back() == ( '\t' ) ) )
                {
                    strcpy( buff, s.c_str() );
                    strextractstring( buff, ( "\t" ) );
                    s = buff;
                }
            }
            if ( !s.empty() )
            {
                if ( ( s[ 0 ] == ( '\n' ) ) || ( s.back() == ( '\n' ) ) )
                {
                    strcpy( buff, s.c_str() );
                    strextractstring( buff, ( "\n" ) );
                    s = buff;
                }
            }

            SAFE_DELETE_ARRAY( buff );
        }

        return s.c_str();
    }

    const wchar_t* ExtraEmpty( _wstring& s )
    {

        if ( !s.empty() )
        {
            wchar_t* buff = new wchar_t[ s.size() + 1 ];

            // 去除前後的空格
            if ( ( s[ 0 ] == ( L' ' ) ) || ( s.back() == ( L' ' ) ) )
            {
                wcscpy( buff, s.c_str() );
                wcsextractstring( buff );
                s = buff;
            }
            //}
            if ( !s.empty() )
            {
                if ( ( s[ 0 ] == ( L'\t' ) ) || ( s.back() == ( L'\t' ) ) )
                {
                    wcscpy( buff, s.c_str() );
                    wcsextractstring( buff, ( L"\t" ) );
                    s = buff;
                }
            }
            if ( !s.empty() )
            {
                if ( ( s[ 0 ] == ( L'\n' ) ) || ( s.back() == ( L'\n' ) ) )
                {
                    wcscpy( buff, s.c_str() );
                    wcsextractstring( buff, ( L"\n" ) );
                    s = buff;
                }
            }

            SAFE_DELETE_ARRAY( buff );
        }

        return s.c_str();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int _finddv( const char_t* delim, char_t c )
    {
        for ( uint_t dvi = 0; dvi < strlen( delim ); dvi++ )
        {
            if ( delim[ dvi ] == c )
                return dvi;
        }
        return -1;
    }

    int _finddv( const wchar_t* delim, wchar_t c )
    {
        for ( uint_t dvi = 0; dvi < wcslen( delim ); dvi++ )
        {
            if ( delim[ dvi ] == c )
                return dvi;
        }
        return -1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void SplitString01( _mbsvector& out_vec, std::vector<char_t>& delim_vec, const _mstring instr, const char_t* delim )
    {
        char_t str_edge = ( '\"' );
        int edge = 0;
        int stg = 0;
        int bs = 0;
        for ( uint_t si = 0; si < instr.size(); si++ )
        {
            int fdv = -1;
            // 進入字串模式的狀態下 碰到 下一個一樣的 才關閉
            // " A'B' C' D' " => 裡面的 ' 都不具有特別意義
            // ' A"B" C" D" ' => 裡面的 " 都不具有特別意義
            // 離開字串
            if ( ( edge == 1 ) && ( instr[ si ] == str_edge ) )
            {
                edge = 0;
            }
            // 進入字串
            else if ( ( edge == 0 ) && ( ( instr[ si ] == ( '\"' ) ) || ( instr[ si ] == ( '\'' ) ) ) )
            {
                edge = 1;
                str_edge = instr[ si ];
            }

            // 沒有在字串中
            if ( edge == 0 )
            {
                if ( ( instr[ si ] == ( '(' ) ) || ( instr[ si ] == ( '[' ) ) || ( instr[ si ] == ( '{' ) ) )
                {
                    stg++;
                }
                else if ( ( instr[ si ] == ( ')' ) ) || ( instr[ si ] == ( ']' ) ) || ( instr[ si ] == ( '}' ) ) )
                {
                    stg--;
                }
                // 如果在字串中 就不檢查切割
                //  e.g. "AAA,BBB"
                fdv = _finddv( delim, instr[ si ] );
            }

            if ( ( fdv != -1 ) && ( stg == 0 ) )
            {
                delim_vec.push_back( delim[ fdv ] );
                _mstring sub = instr.substr( bs, ( si - bs ) );
                out_vec.push_back( ExtraEmpty( sub ) );
                bs = si + 1;
            }
        }

        {
            _mstring laststr = instr.substr( bs, ( instr.size() - bs ) );
            ExtraEmpty( laststr );
            //if( !laststr.empty() )
            out_vec.push_back( laststr );
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void SplitString01( _wcsvector& out_vec, std::vector<wchar_t>& delim_vec, const _wstring instr, const wchar_t* delim )
    {
        wchar_t str_edge = ( L'\"' );
        int edge = 0;
        int stg = 0;
        int bs = 0;
        for ( uint_t si = 0; si < instr.size(); si++ )
        {
            int fdv = -1;
            // 進入字串模式的狀態下 碰到 下一個一樣的 才關閉
            // " A'B' C' D' " => 裡面的 ' 都不具有特別意義
            // ' A"B" C" D" ' => 裡面的 " 都不具有特別意義
            // 離開字串
            if ( ( edge == 1 ) && ( instr[ si ] == str_edge ) )
            {
                edge = 0;
            }
            // 進入字串
            else if ( ( edge == 0 ) && ( ( instr[ si ] == ( L'\"' ) ) || ( instr[ si ] == ( L'\'' ) ) ) )
            {
                edge = 1;
                str_edge = instr[ si ];
            }

            // 沒有在字串中
            if ( edge == 0 )
            {
                if ( ( instr[ si ] == ( L'(' ) ) || ( instr[ si ] == ( L'[' ) ) || ( instr[ si ] == ( L'{' ) ) )
                {
                    stg++;
                }
                else if ( ( instr[ si ] == ( L')' ) ) || ( instr[ si ] == ( L']' ) ) || ( instr[ si ] == ( L'}' ) ) )
                {
                    stg--;
                }
                // 如果在字串中 就不檢查切割
                //  e.g. "AAA,BBB"
                fdv = _finddv( delim, instr[ si ] );
            }

            if ( ( fdv != -1 ) && ( stg == 0 ) )
            {
                delim_vec.push_back( delim[ fdv ] );
                _wstring sub = instr.substr( bs, ( si - bs ) );
                out_vec.push_back( ExtraEmpty( sub ) );
                bs = si + 1;
            }
        }

        {
            _wstring laststr = instr.substr( bs, ( instr.size() - bs ) );
            ExtraEmpty( laststr );
            //if( !laststr.empty() )
            out_vec.push_back( laststr );
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void SplitString02( _OUT _tcsvector& out_vec, _IN const _tstring instr, _IN const _tstring sub )
    {
        int prev_pos = 0;
        int next_pos = 0;
        next_pos = instr.find( sub, prev_pos );
        while ( next_pos != -1 )
        {
            out_vec.push_back( instr.substr( prev_pos, next_pos - prev_pos ) );
            next_pos += sub.size();
            prev_pos = next_pos;
            next_pos = instr.find( sub, prev_pos );
        }
    }

    //=============================================================================
    // NAME:
    // DESC: cash; credit 變成 vector  ; or ,
    //=============================================================================
    void SplitStringVec( _tcsvector& out_vec, std::vector<tchar_t>& delim_vec, const _tstring instr, const tchar_t* delim )
    {
        tchar_t str_edge = _T( '\"' );
        int edge = 0;
        int bs = 0;
        for ( uint_t si = 0; si < instr.size(); si++ )
        {
            int fdv = -1;
            // 進入字串模式的狀態下 碰到 下一個一樣的 才關閉
            // " A'B' C' D' " => 裡面的 ' 都不具有特別意義
            // ' A"B" C" D" ' => 裡面的 " 都不具有特別意義
            // 離開字串
            if ( ( edge == 1 ) && ( instr[ si ] == str_edge ) )
            {
                edge = 0;
            }
            // 進入字串
            else if ( ( edge == 0 ) && ( ( instr[ si ] == _T( '\"' ) ) || ( instr[ si ] == _T( '\'' ) ) ) )
            {
                edge = 1;
                str_edge = instr[ si ];
            }

            // 沒有在字串中
            if ( edge == 0 )
            {
                // 如果在字串中 就不檢查切割
                //  e.g. "AAA,BBB"
                fdv = _finddv( delim, instr[ si ] );
            }

            if ( ( fdv != -1 ) && ( edge == 0 ) )
            {
                _tstring strsA = instr.substr( bs, ( si - bs ) );
                ExtraEmpty( strsA );
                delim_vec.push_back( delim[ fdv ] );
                out_vec.push_back( strsA );
                bs = si + 1;
            }
        }

        {
            _tstring laststr = instr.substr( bs, ( instr.size() - bs ) );
            ExtraEmpty( laststr );
            //if( !laststr.empty() )
            out_vec.push_back( laststr );
        }
        return;

        //_tcsvector strsA = _pf_tstring_split( instr, delim );
        //for( uint_t si = 0; si < strsA.size(); si++ )
        //{
        //    string_vec.push_back( ExtraEmpty( strsA[ si ] ) );
        //}
    }

    //=============================================================================
    // NAME:
    // DESC: cash = 1;後付款 = 1; credit = 2  => 變成 map  ; or ,
    //=============================================================================
    void SplitStringMap( _tcsmap& out_map, std::vector<tchar_t>& delim_vec, const _tstring instr, const tchar_t* delim )
    {
        tchar_t str_edge = _T( '\"' );
        int edge = 0;
        int bs = 0;
        for ( uint_t si = 0; si < instr.size(); si++ )
        {
            int fdv = -1;
            // 進入字串模式的狀態下 碰到 下一個一樣的 才關閉
            // " A'B' C' D' " => 裡面的 ' 都不具有特別意義
            // ' A"B" C" D" ' => 裡面的 " 都不具有特別意義
            // 離開字串
            if ( ( edge == 1 ) && ( instr[ si ] == str_edge ) )
            {
                edge = 0;
            }
            // 進入字串
            else if ( ( edge == 0 ) && ( ( instr[ si ] == _T( '\"' ) ) || ( instr[ si ] == _T( '\'' ) ) ) )
            {
                edge = 1;
                str_edge = instr[ si ];
            }

            // 沒有在字串中
            if ( edge == 0 )
            {
                // 如果在字串中 就不檢查切割
                //  e.g. "AAA,BBB"
                fdv = _finddv( delim, instr[ si ] );
            }

            if ( ( fdv != -1 ) && ( edge == 0 ) )
            {
                _tstring strsA = instr.substr( bs, ( si - bs ) );
                ExtraEmpty( strsA );
                _tcsvector strsB = _pf_tstring_split( strsA, _T( "=" ) );
                delim_vec.push_back( delim[ fdv ] );
                if ( strsB.size() == 2 )
                {
                    out_map.insert( std::map<_tstring, _tstring>::value_type( ExtraEmpty( strsB[ 0 ] ), ExtraEmpty( strsB[ 1 ] ) ) );
                }
                else if ( strsB.size() == 1 )
                {
                    out_map.insert( std::map<_tstring, _tstring>::value_type( ExtraEmpty( strsB[ 0 ] ), _T( "" ) ) );
                }
                bs = si + 1;
            }
        }

        {
            _tstring laststr = instr.substr( bs, ( instr.size() - bs ) );
            ExtraEmpty( laststr );
            //if( !laststr.empty() )
            {
                _tcsvector strsB = _pf_tstring_split( laststr, _T( "=" ) );
                if ( strsB.size() == 2 )
                {
                    out_map.insert( std::map<_tstring, _tstring>::value_type( ExtraEmpty( strsB[ 0 ] ), ExtraEmpty( strsB[ 1 ] ) ) );
                }
                if ( strsB.size() == 1 )
                {
                    out_map.insert( std::map<_tstring, _tstring>::value_type( ExtraEmpty( strsB[ 0 ] ), _T( "" ) ) );
                }
            }
        }
        return;

        //_tcsvector strsA = _pf_tstring_split( instr, delim );
        //for( uint_t si = 0; si < strsA.size(); si++ )
        //{
        //    _tcsvector strsB = _pf_tstring_split( strsA[ si ], _T( "=" ) );
        //    if( strsB.size() == 2 )
        //    {
        //        string_map.insert( std::map<_tstring, _tstring>::value_type( ExtraEmpty( strsB[ 0 ] ), ExtraEmpty( strsB[ 1 ] ) ) );
        //        continue;
        //    }
        //    if( strsB.size() == 1 )
        //    {
        //        string_map.insert( std::map<_tstring, _tstring>::value_type( ExtraEmpty( strsB[ 0 ] ), _T( "" ) ) );
        //        continue;
        //    }
        //}
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring ReplaceString( _tstring instr, const _tcsmap* repstr, _tstring fstr, _tstring estr )
    {
        _tstring out_str;

        tchar_t* cp = &instr.front();

        // 搜尋字串
        while ( ( *cp ) != _T( '\0' ) )
        {
            tchar_t* fp = _tcsstr( cp, fstr.c_str() );
            // 找不到 前引標記
            if ( fp == NULL )
            {
                out_str.append( cp );
                break;
            }

            tchar_t* ep = _tcsstr( fp + fstr.size(), estr.c_str() );
            // 找到 前引標記 卻沒有後引
            if ( fp == NULL )
            {
                // 一樣當沒有
                break;
            }

            // 先放入新字串中
            out_str.append( cp, fp - cp );

            fp += fstr.size();
            _tstring key;
            key.assign( fp, ep - fp );

            // 去除前後空白
            ExtraEmpty( key );

            const _tcsmap::const_iterator iter = repstr->find( key );
            if ( iter != repstr->end() )
            {
                _tstring value = iter->second;
                out_str.append( value );
            }
            cp = ep + 1;
        }

        return out_str;
    }
}