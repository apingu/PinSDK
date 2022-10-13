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
#ifdef _WINDOWS
#include <windows.h>
#pragma comment ( lib, "Ws2_32.lib" )
#endif
#ifdef _LINUX
#include <unistd.h>
#include <signal.h>
#include <errno.h>  
#include <sys/wait.h>  
#endif
#include <stdarg.h>
#include <iostream>
#include <locale>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件
#include "memio.inl"
#include "Unicode.h"
#include "pf-stdlib.h"


//04. == 此專案內.h 文件


/*
#if defined(_PS3) && ENABLE_DRM_OPEN
#include <np/drm_inline.h>
#elif defined(_VITA)
#include <fios2.h>
#endif
*/

#ifndef _USE
#define _USE(loc, fac) use_facet<fac >(loc)
#endif

#if defined(_LINUX)

long64_t GetTickCount( void )
{
    return pftml::Time::GetTickMSTime();
}

bool Beep( _IN dword_t dwFreq, _IN  dword_t dwDuration )
{
    return false;
}

#endif

namespace pfstd
{
    void MSSleep( _IN int milliseconds )
    {
#ifdef _WINDOWS
        Sleep( milliseconds );
#elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = ( milliseconds % 1000 ) * 1000000;
        nanosleep( &ts, NULL );
#else
        usleep( milliseconds * 1000 );
#endif
    }

    void USSleep( _IN int microseconds )
    {
#ifdef _WINDOWS
        struct timeval tv;
        tv.tv_sec = microseconds / 1000000L;
        tv.tv_usec = microseconds % 1000000L;
        select( 0, 0, 0, 0, &tv );
#elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = microseconds;
        ts.tv_nsec = ( microseconds ) * 1000000;
        nanosleep( &ts, NULL );
#else
        usleep( milliseconds );
#endif
    }

    void memcpy( _OUT void* dest, _IN const void* src, _IN unsigned int n )
    {
        __pfmemcpy( dest, src, n );
    }

    void memset( _OUT void* dest, _IN int c, _IN unsigned int n )
    {
        __pfmemset( dest, c, n );
    }

    void strveccpy( _OUT _mbsvector& dest, _IN const _mbstset& src )
    {
        _mbstset::iterator iter = src.begin();
        while( iter != src.end() )
        {
            dest.push_back( *iter );
            iter++;
        }
    }

    void strveccpy( _OUT _wcsvector& dest, _IN const _wcstset& src )
    {
        _wcstset::iterator iter = src.begin();
        while( iter != src.end() )
        {
            dest.push_back( *iter );
            iter++;
        }
    }

    void strsetcpy( _OUT _mbstset& dest, _IN const _mbsvector& src )
    {
        for( uint_t i = 0; i < src.size(); i++ )
        {
            dest.insert( _mbstset::value_type( src[ i ] ) );
        }
    }

    void strsetcpy( _OUT _wcstset& dest, _IN const _wcsvector& src )
    {
        for( uint_t i = 0; i < src.size(); i++ )
        {
            dest.insert( _wcstset::value_type( src[ i ] ) );
        }
    }

    const char* strprintf( _OUT _mstring& str, _IN const char* fmt, _IN const _mbsvector& args )
    {
        str = fmt;
        size_t pos = 0;
        size_t psi = 0;
        while( ( ( pos = str.find( "%s" ) ) != _mstring::npos ) && ( psi < args.size() ) )
        {
            str.replace( pos, strlen( "%s" ), args[ psi ] );
            psi++;
        }
        return str.c_str();
    }

    const char* vstrprintf( _OUT _mstring& str, _IN const char* fmt, _IN va_list args )
    {
        int len = 0; //  // _vscprintf doesn't count   terminating '\0'  
        {
            va_list argcopy;
            va_copy( argcopy, args );
            len = vsnprintf( NULL, 0, fmt, argcopy ) + 1;
            va_end( argcopy );
        }
        char* buffer = new char[ len ];

        vsnprintf( buffer, len, fmt, args );

        str = buffer;
        delete[] buffer;
        return str.c_str();
    }

    const char* strprintf( _OUT _mstring& str, _IN const char* fmt, _IN... )
    {
        va_list args;
        va_start( args, fmt );
        vstrprintf( str, fmt, args );
        va_end( args );
        return str.c_str();
    }

    const char* strprintfa( _OUT _mstring& str, _IN const char* fmt, _IN... )
    {
        va_list args;
        char* buffer;
        va_start( args, fmt );
#ifdef _WINDOWS
        int len = _vscprintf( fmt, args ) + 1; //  // _vscprintf doesn't count   terminating '\0'  
        buffer = new char[ len ];
        vsprintf_s( buffer, len, fmt, args );
#else
        //int len = vprintf( fmt, args ) + 1; //  // _vscprintf doesn't count   terminating '\0'  
        int len = 0;
        {
            va_list argcopy;
            va_copy( argcopy, args );
            len = vsnprintf( NULL, 0, fmt, argcopy ) + 1;
            va_end( argcopy );
        }
        buffer = new char[ len ];
        vsnprintf( buffer, len, fmt, args );
        //vsprintf( buffer, fmt, args );
#endif
        va_end( args );
        str.append( buffer );
        delete[] buffer;
        return str.c_str();
    }

    const wchar_t* strprintf( _OUT _wstring& str, _IN const wchar_t* fmt, _IN const _wcsvector& args )
    {
        str = fmt;
        size_t pos = 0;
        size_t  psi = 0;
        while( ( ( pos = str.find( L"%s" ) ) != _wstring::npos ) && ( psi < args.size() ) )
        {
            str.replace( pos, wcslen( L"%s" ), args[ psi ] );
            psi++;
        }
        return str.c_str();
    }

    const wchar_t* vstrprintf( _OUT _wstring& str, _IN const wchar_t* fmt, _IN va_list args )
    {
        wchar_t* buffer;
        int len = 0;
        {
            va_list argcopy;
            va_copy( argcopy, args );
            len = vswprintf( NULL, 0, fmt, argcopy ) + 1;
            va_end( argcopy );
        }
        buffer = new wchar_t[ len ];

        vswprintf( buffer, len, fmt, args );

        str = buffer;
        delete[] buffer;
        return str.c_str();
    }

    const wchar_t* strprintf( _OUT _wstring& str, _IN const wchar_t* fmt, _IN ... )
    {
        va_list args;
        va_start( args, fmt );
        vstrprintf( str, fmt, args );
        va_end( args );
        return str.c_str();
    }

    const wchar_t* strprintfa( _OUT _wstring& str, _IN const wchar_t* fmt, _IN ... )
    {
        va_list args;
        wchar_t* buffer;
        va_start( args, fmt );
#ifdef _WINDOWS
        int len = _vscwprintf( fmt, args ) + 1; //  // _vscprintf doesn't count   terminating '\0'  
        buffer = new wchar_t[ len ];
        vswprintf_s( buffer, len, fmt, args );
#else
        //int len = vwprintf( fmt, args ) + 1; //  // _vscprintf doesn't count   terminating '\0'  
        int len = 0;
        {
            va_list argcopy;
            va_copy( argcopy, args );
            len = vswprintf( NULL, 0, fmt, argcopy ) + 1;
            va_end( argcopy );
        }
        buffer = new wchar_t[ len ];
        vswprintf( buffer, len, fmt, args );
#endif
        str.append( buffer );
        delete[] buffer;
        return str.c_str();
    }

    _wcsvector string_split( _IN _wstring str, _IN const wchar_t* div )
    {
        _wcsvector split_string;
        wchar_t* tmpstr = new wchar_t[ str.size() + 1 ];
        wcscpy( tmpstr, str.c_str() );

        wchar_t* token = NULL;
        wchar_t* next_token = NULL;
        token = wcstok_rc( tmpstr, div, &next_token );
        while( token )
        {
            split_string.push_back( token );
            token = wcstok_rc( NULL, div, &next_token );
        }
        SAFE_DELETE_ARRAY( tmpstr );
        return split_string;
    }

    _mbsvector string_split( _IN _mstring str, _IN const char* div )
    {
        _mbsvector split_string;
        char* tmpstr = new char[ str.size() + 1 ];
        strcpy( tmpstr, str.c_str() );

        char* token = NULL;
        char* next_token = NULL;
        token = strtok_rc( tmpstr, div, &next_token );
        while( token )
        {
            split_string.push_back( token );
            token = strtok_rc( NULL, div, &next_token );
        }
        SAFE_DELETE_ARRAY( tmpstr );
        return split_string;
    }

    void argument_strarray( _OUT _mbsvector& str, _IN uint_t len, _IN ... )
    {
        va_list argvlist;
        va_start( argvlist, len );
        mchar_t* arg = NULL;
        for( uint_t ui = 0; ui < len; ui++ )
        {
            arg = ( mchar_t* )va_arg( argvlist, mchar_t* );
            if( arg != NULL )
                str.push_back( "" );
        }
        va_end( argvlist );
        return;
    }

    void argument_strarray( _OUT _wcsvector& str, _IN uint_t len, _IN ... )
    {
        va_list argvlist;
        va_start( argvlist, len );
        wchar_t* arg = NULL;
        for( uint_t ui = 0; ui < len; ui++ )
        {
            arg = ( wchar_t* )va_arg( argvlist, wchar_t* );
            if( arg != NULL )
                str.push_back( L"" );
        }
        va_end( argvlist );
        return;
    }

    void strextractstring( _INOUT _mstring& inouts, _IN const char era )
    {
        for( size_t si = 0; si < inouts.size(); si++ )
        {
            if( inouts.front() == era )
            {
                inouts.erase( inouts.begin() );
                si--;
            }
            else
            {
                break;
            }
        }

        for( size_t si = inouts.size() - 1; si >= 0; si-- )
        {
            if( inouts.back() == era )
            {
                inouts.pop_back();
                si++;
            }
            else
            {
                break;
            }
        }
    }

    void strextractstring( _INOUT _wstring& inouts, _IN const wchar_t era )
    {
        for( size_t si = 0; si < inouts.size(); si++ )
        {
            if( inouts.front() == era )
            {
                inouts.erase( inouts.begin() );
                si--;
            }
            else
            {
                break;
            }
        }

        for( size_t si = inouts.size() - 1; si >= 0; si-- )
        {
            if( inouts.back() == era )
            {
                inouts.pop_back();
                si++;
            }
            else
            {
                break;
            }
        }
    }

    int strreplaceall( _INOUT _mstring& inouts, _IN const char* find_str, _IN const char* replace_str )
    {
        int cnt = 0;
        size_t pos = inouts.find( find_str );
        // Repeat till end is reached
        while( pos != std::string::npos )
        {
            cnt++;
            // Replace this occurrence of Sub String
            inouts.replace( pos, strlen( find_str ), replace_str );
            // Get the next occurrence from the current position
            pos = inouts.find( find_str, pos + strlen( replace_str ) );
        }
        return cnt;
    }

    int strreplaceall( _INOUT _wstring& inouts, _IN const wchar_t* find_str, _IN const wchar_t* replace_str )
    {
        int cnt = 0;
        size_t pos = inouts.find( find_str );
        // Repeat till end is reached
        while( pos != std::string::npos )
        {
            cnt++;
            // Replace this occurrence of Sub String
            inouts.replace( pos, wcslen( find_str ), replace_str );
            // Get the next occurrence from the current position
            pos = inouts.find( find_str, pos + wcslen( replace_str ) );
        }
        return cnt;
    }

    // 字串轉小寫
    _mstring strtolower( _IN _mstring str )
    {
        _mstring out = str;
        for( uint_t si = 0; si < out.size(); si++ )
        {
            if( ( out[ si ] >= 'A' ) && ( out[ si ] <= 'Z' ) )
            {
                out[ si ] = ( out[ si ] + 32 );
            }
        }
        return out;
    }

    _wstring strtolower( _IN _wstring str )
    {
        _wstring out = str;
        for( uint_t si = 0; si < out.size(); si++ )
        {
            if( ( out[ si ] >= _T( 'A' ) ) && ( out[ si ] <= _T( 'Z' ) ) )
            {
                out[ si ] = ( out[ si ] + 32 );
            }
        }
        return out;
    }

    // 字串轉大寫
    _mstring strtoupper( _IN _mstring str )
    {
        _mstring out = str;
        for( uint_t si = 0; si < out.size(); si++ )
        {
            if( ( out[ si ] >= 'a' ) && ( out[ si ] <= 'z' ) )
            {
                out[ si ] = ( out[ si ] - 32 );
            }
        }
        return out;
    }

    _wstring strtoupper( _IN _wstring str )
    {
        _wstring out = str;
        for( uint_t si = 0; si < out.size(); si++ )
        {
            if( ( out[ si ] >= _T( 'a' ) ) && ( out[ si ] <= _T( 'z' ) ) )
            {
                out[ si ] = ( out[ si ] - 32 );
            }
        }
        return out;
    }

    pos_t strcasefind( _IN const _mstring str, _IN const _mstring subs, _IN pos_t pos )
    {
        const char* bs = str.c_str() + pos;
        const char* s = strcasestr( bs, subs.c_str() );
        if( s != NULL )
        {
            return s - str.c_str();
        }
        return -1;
    }

    pos_t strcasefind( _IN const _wstring str, _IN const _wstring subs, _IN pos_t pos )
    {
        const wchar_t* bs = str.c_str() + pos;
        const wchar_t* s = wcscasestr( bs, subs.c_str() );
        if( s != NULL )
        {
            return s - bs;
        }
        return -1;
    }

    _mstring getpathdir( _IN const char* path )
    {
        char drive[ _MAX_LDRIVE ];
        char dir[ _MAX_LDIR ];
        char fname[ _MAX_LFNAME ];
        char ext[ _MAX_LEXT ];
        msplitfpath( path, drive, dir, fname, ext );
        return _mstring( drive ) + dir;
    }

    _wstring getpathdir( _IN const wchar_t* path )
    {
        wchar_t drive[ _MAX_LDRIVE ];
        wchar_t dir[ _MAX_LDIR ];
        wchar_t fname[ _MAX_LFNAME ];
        wchar_t ext[ _MAX_LEXT ];
        wsplitfpath( path, drive, dir, fname, ext );
        return _wstring( drive ) + dir;
    }

    _mstring getpathfname( _IN const char* path )
    {
        char drive[ _MAX_LDRIVE ];
        char dir[ _MAX_LDIR ];
        char fname[ _MAX_LFNAME ];
        char ext[ _MAX_LEXT ];
        msplitfpath( path, drive, dir, fname, ext );
        return _mstring( fname );
    }

    _wstring getpathfname( _IN const wchar_t* path )
    {
        wchar_t drive[ _MAX_LDRIVE ];
        wchar_t dir[ _MAX_LDIR ];
        wchar_t fname[ _MAX_LFNAME ];
        wchar_t ext[ _MAX_LEXT ];
        wsplitfpath( path, drive, dir, fname, ext );
        return _wstring( fname );
    }

    _mstring getpathext( _IN const char* path )
    {
        char drive[ _MAX_LDRIVE ];
        char dir[ _MAX_LDIR ];
        char fname[ _MAX_LFNAME ];
        char ext[ _MAX_LEXT ];
        msplitfpath( path, drive, dir, fname, ext );
        return _mstring( ext );
    }

    _wstring getpathext( _IN const wchar_t* path )
    {
        wchar_t drive[ _MAX_LDRIVE ];
        wchar_t dir[ _MAX_LDIR ];
        wchar_t fname[ _MAX_LFNAME ];
        wchar_t ext[ _MAX_LEXT ];
        wsplitfpath( path, drive, dir, fname, ext );
        return _wstring( ext );
    }

    _mstring getpathfnameext( _IN const char* path )
    {
        char drive[ _MAX_LDRIVE ];
        char dir[ _MAX_LDIR ];
        char fname[ _MAX_LFNAME ];
        char ext[ _MAX_LEXT ];
        msplitfpath( path, drive, dir, fname, ext );
        return _mstring( fname ) + ext;
    }

    _wstring getpathfnameext( _IN const wchar_t* path )
    {
        wchar_t drive[ _MAX_LDRIVE ];
        wchar_t dir[ _MAX_LDIR ];
        wchar_t fname[ _MAX_LFNAME ];
        wchar_t ext[ _MAX_LEXT ];
        wsplitfpath( path, drive, dir, fname, ext );
        return _wstring( fname ) + ext;
    }

    //////////////////////////////////////////////////////////////////////////
    int strtoi( _IN const _mstring& str )
    {
        return mtoi( str.c_str() );
    }

    _mstring itostr( _IN int val )
    {
        _mstring str;
        return strprintf( str, "%d", val );
    }

    int wcstoi( _IN const _wstring& str )
    {
        return wtoi( str.c_str() );
    }

    _wstring itowcs( _IN int val )
    {
        _wstring str;
        return strprintf( str, L"%d", val );
    }

    uint_t strtoui( _IN const _mstring& str )
    {
        return mtoui( str.c_str() );
    }

    _mstring uitostr( _IN uint_t val )
    {
        _mstring str;
        return strprintf( str, "%u", val );
    }

    uint_t wcstoui( _IN const _wstring& str )
    {
        return wtoui( str.c_str() );
    }

    _wstring uitowcs( _IN uint_t val )
    {
        _wstring str;
        return strprintf( str, L"%u", val );
    }

    long64_t strtoll( _IN const _mstring& str )
    {
        return mtoll( str.c_str() );
    }

    _mstring lltostr( _IN long64_t val )
    {
        _mstring str;
        return strprintf( str, "%lld", val );
    }

    long64_t wcstoll( _IN const _wstring& str )
    {
        return wtoll( str.c_str() );
    }

    _wstring lltowcs( _IN long64_t val )
    {
        _wstring wcs;
        return strprintf( wcs, L"%lld", val );
    }

    ulong64_t strtoull( _IN const _mstring& str )
    {
        return mtoull( str.c_str() );
    }

    _mstring ulltostr( _IN ulong64_t val )
    {
        _mstring str;
        return strprintf( str, "%llu", val );
    }

    ulong64_t wcstoull( _IN const _wstring& str )
    {
        return wtoull( str.c_str() );
    }

    _wstring ulltowcs( _IN ulong64_t val )
    {
        _wstring wcs;
        return strprintf( wcs, L"%llu", val );
    }

    double strtof( _IN const _mstring& str )
    {
        return mtof( str.c_str() );
    }

    _mstring ftostr( _IN double val )
    {
        _mstring str;
        return strprintf( str, "%f", val );
    }

    double wcstof( _IN const _wstring& str )
    {
        return wtof( str.c_str() );
    }

    _wstring ftowcs( _IN double val )
    {
        _wstring str;
        return strprintf( str, L"%f", val );
    }

    //////////////////////////////////////////////////////////////////////////
    // http://www.runoob.com/cprogramming/c-function-setlocale.html
    bool SetLocale( _IN int codepage )
    {
        switch( codepage )
        {
            case CP_ASCII:
                return ( setlocale( LC_ALL, "en-US" ) != NULL ) ? true : false;
            case CP_BIG5:
                return ( setlocale( LC_ALL, "zh-TW" ) != NULL ) ? true : false;
            case CP_EKR:
                return ( setlocale( LC_ALL, "ko-KR" ) != NULL ) ? true : false;
            case CP_JIS:
                return ( setlocale( LC_ALL, "ja-JP" ) != NULL ) ? true : false;
            case CP_GB2312:
                return ( setlocale( LC_ALL, "zh-CN" ) != NULL ) ? true : false;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
//    _wstring StringToUnicode( _IN const _mstring s )
//    {
//#ifdef _UNICODE
//        size32_t len = ( strlen( s.c_str() ) + 1 ) * 4;
//        UInt16* unc = new UInt16[ len ];
//        UTF8ToUnicode( unc, s.c_str(), len );
//        _wstring rr = (WCHAR*)unc;
//        delete[] unc;
//        return rr;
//#else
//        return L"";
//#endif
//    }
//
//    _mstring StringToUTF8( _IN const _wstring ws )
//    {
//#ifdef _UNICODE
//        int len = ( wcslen( ws.c_str() ) + 1 ) * 4;
//        char* s = new char[ len ];
//        UnicodeToUTF8( s, (UInt16*)ws.c_str(), len );
//        _mstring rr = s;
//        delete[] s;
//        return rr;
//#else
//        return "";
//#endif
//    }

    _wstring UTF8ToUnicode( _IN const _mstring s )
    {
        return MultiByteToUnicode( s, CP_UTF8 );
    }

    _mstring UnicodeToUTF8( _IN const _wstring ws )
    {
        return UnicodeToMultiByte( ws, CP_UTF8 );
    }

    _mstring UnicodeToUTF8BOM( _IN const _wstring ws )
    {
        return UTF8ToUTF8BOM( UnicodeToUTF8( ws ) );
    }

    _wstring JISToUnicode( _IN const _mstring s )
    {
        return MultiByteToUnicode( s, CP_JIS );
    }

    _mstring UnicodeToJIS( _IN const _wstring ws )
    {
        return UnicodeToMultiByte( ws, CP_JIS );
    }

    _wstring GBToUnicode( _IN const _mstring s )
    {
        return MultiByteToUnicode( s, CP_GB2312 );
    }

    _mstring UnicodeToGB( _IN const _wstring ws )
    {
        return UnicodeToMultiByte( ws, CP_GB2312 );
    }

    _wstring MultiByteToUnicode( _IN const _mstring s )
    {
        _wstring ret;

        if( !s.empty() )
        {
            ret.resize( s.length() );
            typedef std::ctype<wchar_t> CT;
            CT const& ct = std::_USE( std::locale(), CT );
            ct.widen( &s[ 0 ], &*s.begin() + s.size(), &ret[ 0 ] );
        }
        return ret;
    }

    _mstring UnicodeToMultiByte( _IN const _wstring ws )
    {
        _mstring ret;

        if( !ws.empty() )
        {
            ret.resize( ws.length() );
            typedef std::ctype<wchar_t> CT;
            CT const& ct = std::_USE( std::locale(), CT );
            ct.narrow( &ws[ 0 ], &*ws.begin() + ws.size(), '?', &ret[ 0 ] );
        }
        return ret;
    }

    _wstring MultiByteToUnicode( _IN const _mstring s, _IN int codepage )
    {
        size32_t len = ( size32_t )( s.size() + 1 ) * 4;
        //wchar_t* strW = new wchar_t[len];
        std::vector<wchar_t> unic( len + 1 );
        mtow( s.c_str(), &unic[ 0 ], len, codepage );
        unic[ len ] = 0;
        //_wstring ws = strW;
        //SAFE_DELETE_ARRAY( strW );
        return &unic[ 0 ];
    }

    _mstring UnicodeToMultiByte( _IN const _wstring ws, _IN int codepage )
    {
        // 因為如果是 muti-byte 至少要有兩個byte
        size32_t len = ( size32_t )( ws.size() + 1 ) * 4;
        //char_t* strA = new char_t[len];
        std::vector<char> utf8( len + 1 );
        wtom( ws.c_str(), &utf8[ 0 ], len, codepage );
        utf8[ len ] = 0;
        //_mstring ss = strA;
        //SAFE_DELETE_ARRAY( strA );
        return &utf8[ 0 ];
    }

    _mstring UTF8ToUTF8BOM( _IN const _mstring s )
    {
        size32_t len = ( size32_t )( s.size() + 10 );
        std::vector<char> utf8( len );
        ::UTF8ToUTF8BOM( &utf8[ 0 ], s.c_str() );
        return &utf8[ 0 ];
    }

    _binbuf MStringToBinary( _IN const char* str )
    {
        size32_t len = ( size32_t )strlen( str );
        _binbuf buffer( ( len * sizeof( char ) ) * 2 );
        strtobin( &buffer[ 0 ], ( size32_t )buffer.size(), ( const ubyte_t* )str, ( size32_t )( len * sizeof( char ) ) );
        return buffer;
    }

    _binbuf WStringToBinary( _IN const wchar_t* str )
    {
        size32_t len = ( size32_t )wcslen( str );
        _binbuf buffer( ( len * sizeof( wchar_t ) ) * 2 );
        strtobin( &buffer[ 0 ], ( size32_t )buffer.size(), ( const ubyte_t* )str, ( size32_t )( len * sizeof( wchar_t ) ) );
        return buffer;
    }

    const ubyte_t* BinaryToString( _OUT _binbuf& obin, _IN const _binbuf ibin )
    {
        obin.resize( ibin.size() );
        bintostr( &obin[ 0 ], ( size32_t )obin.size(), ( const ubyte_t* )&ibin[ 0 ], ( size32_t )ibin.size() );
        return &obin[ 0 ];
    }

    _mstring StringToUTF8( _IN const _mstring s )
    {
        int loacl = CP_UTF8;
#ifdef _WINDOWS
        //loacl = GetACP();
#endif
        if( loacl == CP_UTF8 )
        {
            return s;
        }
        _wstring ws = MultiByteToUnicode( s, loacl );
        return UnicodeToMultiByte( ws, CP_UTF8 );
    }

    _mstring StringToUTF8BOM( _IN const _mstring s )
    {
        return UTF8ToUTF8BOM( StringToUTF8( s ) );
    }

    _mstring StringToUTF8BOM( _IN const _wstring s )
    {
        return UTF8ToUTF8BOM( StringToUTF8( s ) );
    }

    _mstring StringToUTF8( _IN const _wstring s )
    {
        return UnicodeToUTF8( s );
    }

    _wstring StringToUnicode( _IN const _mstring s )
    {
        return UTF8ToUnicode( s );
    }

    _wstring StringToUnicode( _IN const _wstring s )
    {
        return s;
    }

    _mstring Big5ToUTF8( _IN const _mstring s )
    {
        _wstring ws = MultiByteToUnicode( s, CP_BIG5 );
        return UnicodeToMultiByte( ws, CP_UTF8 );
    }

    _mstring UTF8ToBig5( _IN const _mstring s )
    {
        _wstring ws = MultiByteToUnicode( s, CP_UTF8 );
        return UnicodeToMultiByte( ws, CP_BIG5 );
    }

    _wstring Big5ToUnicode( _IN const _mstring s )
    {
        return MultiByteToUnicode( s, CP_BIG5 );
    }

    _mstring UnicodeToBig5( _IN const _wstring s )
    {
        return UnicodeToMultiByte( s, CP_BIG5 );
    }

    //////////////////////////////////////////////////////////////////////////
#ifdef _LINUX
    void OnSigChild( _IN int iSig )
    {
        pid_t iPid;
        int iStat;
        while( ( iPid = waitpid( -1, &iStat, WNOHANG ) ) > 0 )
        {
            //cout << "<OnSigChild>, Pid=" << iPid << endl;
            MSSleep( 0 );
        }
    }
#endif

    int SystemExecute( _IN const char* command, _IN const char* parameters )
    {
        int ret = 0;
#ifdef _WINDOWS
        // If the function succeeds, it returns a value greater than 32
        ret = ( int )ShellExecuteA( NULL, NULL, command, parameters, NULL, SW_SHOW );
#endif
#ifdef _LINUX
        //signal( SIGCHLD, OnSigChild );
        signal( SIGCHLD, SIG_IGN );
        pid_t pid = fork();
        if( pid == 0 )
        {
            int curr_pid = getpid();
            //printf( "\n [export] in child %d / parent %d\n", curr_pid, getppid() );

            // Child process
            _mstring cmd_string = _mstring( command ) + " " + _mstring( parameters );
            printf( "\n command %s\n", cmd_string.c_str() );

            ret = system( cmd_string.c_str() );

            //printf( "\n [export] out child\n" );

            //printf( "\n [export] kill %d\n", curr_pid );
            kill( curr_pid, SIGKILL );
            exit( 0 );
        }
        else
        {
            // Parent process
            //printf( "Hello again parent process!\n" );
        }
#endif
        return ret;
    }

    int SystemExecute( _IN const wchar_t* command, _IN const wchar_t* parameters )
    {
        int ret = 0;
#ifdef _WINDOWS
        // If the function succeeds, it returns a value greater than 32
        ret = ( int )ShellExecuteW( NULL, NULL, command, parameters, NULL, SW_SHOW );
#endif
#ifdef _LINUX

        _wstring comm = command;
        _wstring para = parameters;

        signal( SIGCHLD, OnSigChild );
        pid_t pid = fork();
        if( pid == 0 )
        {
            int curr_pid = getpid();
            //printf( "\n [export] in child %d / parent %d\n", curr_pid, getppid() );

            // Child process
            _wstring cmd_string = comm + L" " + para;
            ret = system( StringToUTF8( cmd_string ).c_str() );

            //printf( "\n [export] out child\n" );
            kill( curr_pid, SIGKILL );
            exit( 0 );
        }
        else
        {
            // Parent process
            //printf( "Hello again parent process!\n" );
        }
#endif
        return ret;
    }
}

_binbuf& operator<<( _IN _binbuf& bin, _IN const _mstring str )
{
    bin.resize( str.size() );
    memcpy( &bin[ 0 ], str.c_str(), str.size() );
    return bin;
}

//////////////////////////////////////////////////////////////////////////
//
_mbsmap SwapKeyValue( const _IN _mbsmap mbsmap )
{
    _mbsmap out_mbsmap;
    for( _mbsmap::const_iterator iter = mbsmap.begin(); iter != mbsmap.end(); iter++ )
    {
        out_mbsmap[ iter->second ] = iter->first;
    }
    return out_mbsmap;
}

_wcsmap SwapKeyValue( const _IN _wcsmap wcsmap )
{
    _wcsmap out_wcsmap;
    for( _wcsmap::const_iterator iter = wcsmap.begin(); iter != wcsmap.end(); iter++ )
    {
        out_wcsmap[ iter->second ] = iter->first;
    }
    return out_wcsmap;
}


_mstring_set VecToSet( _IN const _mstring_vector vec )
{
    _mstring_set set;
    for( uint_t ui = 0; ui < vec.size(); ui++ )
    {
        set.insert( _mstring_set::value_type( vec[ ui ] ) );
    }
    return set;
}

_mstring_vector SetToVec( _IN const _mstring_set set )
{
    _mstring_vector vec;
    for( _mstring_set::iterator uit = set.begin(); uit != set.end(); uit++ )
    {
        vec.push_back( *uit );
    }
    return vec;
}

_wstring_set VecToSet( _IN const _wstring_vector vec )
{
    _wstring_set set;
    for( uint_t ui = 0; ui < vec.size(); ui++ )
    {
        set.insert( _wstring_set::value_type( vec[ ui ] ) );
    }
    return set;
}

_wstring_vector SetToVec( _IN const _wstring_set set )
{
    _wstring_vector vec;
    for( _wstring_set::iterator uit = set.begin(); uit != set.end(); uit++ )
    {
        vec.push_back( *uit );
    }
    return vec;

}

