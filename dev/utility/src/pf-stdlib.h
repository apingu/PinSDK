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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <set>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>
#include <pf-string.h>
#include <pf-file.h>
#include <pf-time.h>


//////////////////////////////////////////////////////////////////////////
// Linux platform function
#if defined(_LINUX)
    long64_t          GetTickCount( void );
    bool              Beep( _IN dword_t dwFreq, _IN  dword_t dwDuration );
#endif

//////////////////////////////////////////////////////////////////////////
//
namespace pfstd
{
    //////////////////////////////////////////////////////////////////////////
    void              MSSleep( _IN int milliseconds );
    void              USSleep( _IN int microseconds );
    
    //////////////////////////////////////////////////////////////////////////
    void              memcpy( _OUT void* dest, _IN const void* src, _IN unsigned int n );
    void              memset( _OUT void* dest, _IN int c, _IN unsigned int n );
                     
    void              strveccpy( _OUT _mbsvector& dest, _IN const _mbstset& src );
    void              strveccpy( _OUT _wcsvector& dest, _IN const _wcstset& src );
                     
    void              strsetcpy( _OUT _mbstset& dest, _IN const _mbsvector& src );
    void              strsetcpy( _OUT _wcstset& dest, _IN const _wcsvector& src );

    //////////////////////////////////////////////////////////////////////////
    const char*       strprintf( _OUT _mstring& str, _IN const char* fmt, _IN const _mbsvector& args );
    const char*       vstrprintf( _OUT _mstring& str, _IN const char* fmt, _IN va_list args );
    const char*       strprintf( _OUT _mstring& str, _IN const char* fmt, _IN ... );
    // append on str
    const char*       strprintfa( _OUT _mstring& str, _IN const char* fmt, _IN ... );

    const wchar_t*    strprintf( _OUT _wstring& str, _IN const wchar_t* fmt, _IN const _wcsvector& args );
    const wchar_t*    vstrprintf( _OUT _wstring& str, _IN const wchar_t* fmt, _IN va_list args ); // 不能用同名 會有 認錯的問題
    const wchar_t*    strprintf( _OUT _wstring& str, _IN const wchar_t* fmt, _IN ... );
    // append on str
    const wchar_t*    strprintfa( _OUT _wstring& str, _IN const wchar_t* fmt, _IN ... );

    // 以div 將字串做拆解成 vector
    _mbsvector        string_split( _IN _mstring str, _IN const char* div );
    _wcsvector        string_split( _IN _wstring str, _IN const wchar_t* div );

    void              argument_strarray( _OUT _mbsvector& str, _IN uint_t len, _IN ... );
    void              argument_strarray( _OUT _wcsvector& str, _IN uint_t len, _IN ... );

    // 將字串前後的era 做刪除
    void              strextractstring( _INOUT _mstring& inouts, _IN const char era = ' ' );
    void              strextractstring( _INOUT _wstring& inouts, _IN const wchar_t era = L' ' );
    
    // 替換掉子字串
    // return 替換幾次
    int               strreplaceall( _INOUT _mstring& inouts, _IN const char* find_str, _IN const char* replace_str );
    int               strreplaceall( _INOUT _wstring& inouts, _IN const wchar_t* find_str, _IN const wchar_t* replace_str );

    // 字串轉小寫
    _mstring          strtolower( _IN _mstring str );
    _wstring          strtolower( _IN _wstring str );

    // 字串轉大寫
    _mstring          strtoupper( _IN _mstring str );
    _wstring          strtoupper( _IN _wstring str );

    // 不分大小寫 搜尋子字串
    // 傳回 字串位置
    pos_t             strcasefind( _IN const _mstring str, _IN const _mstring subs, _IN pos_t pos = 0 );
    pos_t             strcasefind( _IN const _wstring str, _IN const _wstring subs, _IN pos_t pos = 0 );

    //////////////////////////////////////////////////////////////////////////
    // 路徑相關
    // 取得路徑的資料夾
    _mstring          getpathdir( _IN const char* path );
    _wstring          getpathdir( _IN const wchar_t* path );

    // 取得路徑的檔名
    _mstring          getpathfname(_IN const char* path );
    _wstring          getpathfname(_IN const wchar_t* path );

    // 取得路徑的副檔名
    _mstring          getpathext( _IN const char* path );
    _wstring          getpathext( _IN const wchar_t* path );

    // 取得路徑的完整檔名
    _mstring          getpathfnameext( _IN const char* path );
    _wstring          getpathfnameext( _IN const wchar_t* path );

    //////////////////////////////////////////////////////////////////////////
    int               strtoi( _IN const _mstring& str );
    _mstring          itostr( _IN int val );
    int               wcstoi( _IN const _wstring& str );
    _wstring          itowcs( _IN int val );
                      
    uint_t            strtoui( _IN const _mstring& str );
    _mstring          uitostr( _IN uint_t val );
    uint_t            wcstoui( _IN const _wstring& str );
    _wstring          uitowcs( _IN uint_t val );
                      
    long64_t          strtoll( _IN const _mstring& str );
    _mstring          lltostr( _IN long64_t val );
    long64_t          wcstoll( _IN const _wstring& str );
    _wstring          lltowcs( _IN long64_t val );
                      
    ulong64_t         strtoull( _IN const _mstring& str );
    _mstring          ulltostr( _IN ulong64_t val );
    ulong64_t         wcstoull( _IN const _wstring& str );
    _wstring          ulltowcs( _IN ulong64_t val );
                      
    double            strtof( _IN const _mstring& str );
    _mstring          ftostr( _IN double val );
    double            wcstof( _IN const _wstring& str );
    _wstring          ftowcs( _IN double val );
            
    //////////////////////////////////////////////////////////////////////////
    //
    bool              SetLocale( _IN int codepage );

   
    //////////////////////////////////////////////////////////////////////////
    // character code
    // WString not unicode
    //_wstring        UTF8ToWString( _IN const _mstring s );
    //_mstring        WStringToUTF8( _IN const _wstring ws );

    _wstring          UTF8ToUnicode( _IN const _mstring s );
    _mstring          UnicodeToUTF8( _IN const _wstring ws );
    _mstring          UnicodeToUTF8BOM( _IN const _wstring ws );
                      
    _wstring          JISToUnicode( _IN const _mstring s );
    _mstring          UnicodeToJIS( _IN const _wstring ws );
                      
    _wstring          MultiByteToUnicode( _IN const _mstring s );
    _mstring          UnicodeToMultiByte( _IN const _wstring ws );
                      
    _wstring          GBToUnicode( _IN const _mstring s );
    _mstring          UnicodeToGB( _IN const _wstring ws );

    //   "tw","big5"; "jp", jis"; "ka"; "ca", "gb"; "us"; "utf8"
    _wstring          MultiByteToUnicode( _IN const _mstring s, _IN int codepage );
    _mstring          UnicodeToMultiByte( _IN const _wstring ws, _IN int codepage );
                     
    _mstring          UTF8ToUTF8BOM( _IN const _mstring s );
                     
    _binbuf           MStringToBinary( _IN const char* str );
    _binbuf           WStringToBinary( _IN const wchar_t* str );

    // 是不是w 或 m 自行轉換
    const ubyte_t*    BinaryToString( _OUT _binbuf& obin,  _IN const _binbuf ibin );

    // 共同平排用
    // 預設的國碼轉換為 uft8
    _mstring          StringToUTF8( _IN const _mstring s );
    _mstring          StringToUTF8( _IN const _wstring s );
                      
    _mstring          StringToUTF8BOM( _IN const _mstring s );
    _mstring          StringToUTF8BOM( _IN const _wstring s );
                      
    _wstring          StringToUnicode( _IN const _mstring s );
    _wstring          StringToUnicode( _IN const _wstring s );
                      
    _mstring          Big5ToUTF8( _IN const _mstring s );
    _mstring          UTF8ToBig5( _IN const _mstring s );
                      
    _wstring          Big5ToUnicode( _IN const _mstring s );
    _mstring          UnicodeToBig5( _IN const _wstring s );

    // win: If the function succeeds, it returns a value greater than 32
    int               SystemExecute( _IN const char* command, _IN const char* parameters );
    int               SystemExecute( _IN const wchar_t* command, _IN const wchar_t* parameters );

    // 將檔案讀成字串( 文字檔 )
    //void            ReadFileToString( _OUT _mstring& file_string, _IN FILE* fp );
    //void            ReadFileToString( _OUT _wstring& file_string, _IN FILE* fp );


    //void mextractstring( _mstring& str, _IN const char* era = " " );
    //void wextractstring( _wstring& str, _IN const wchar_t* era = " " );

    //////////////////////////////////////////////////////////////////////////
    // 容器操作
    _mbsmap          SwapKeyValue( _IN const _mbsmap mbsmap );
    _wcsmap          SwapKeyValue( _IN const _wcsmap wcsmap );

    _mstring_set     VecToSet ( _IN const _mstring_vector vec );                     
    _mstring_vector  SetToVec ( _IN const _mstring_set set );
                     
    _wstring_set     VecToSet( _IN const _wstring_vector vec );                     
    _wstring_vector  SetToVec( _IN const _wstring_set set );

}

#ifdef _UNICODE
    #define  _pf_tstrprintf               pfstd::strprintf
    #define  _pf_vtstrprintf              pfstd::vstrprintf
    #define  _pf_tstrprintfa              pfstd::strprintfa
    #define  _pf_tcstok_rc                wcstok_rc
    #define  _pf_tcsextractstring         pfstd::wcsextractstring
    #define  _pf_tstrfileexist            pfsfl::FileExist
    #define  _pf_tcsdumpchr               wcsdumpchr
    #define  _pf_tstring_split            pfstd::string_split

    #define  _pf_tgetpathdir              pfstd::getpathdir
    #define  _pf_tgetpathfname            pfstd::getpathfname
    #define  _pf_tgetpathext              pfstd::getpathext
    #define  _pf_tgetpathfnameext         pfstd::getpathfnameext

    #define  _pf_ttoi                     pfstd::wcstoi
    #define  _pf_itot                     pfstd::itowcs
    #define  _pf_ttoui                    pfstd::wcstoui
    #define  _pf_uitot                    pfstd::uitowcs
    #define  _pf_ttoll                    pfstd::wcstoll
    #define  _pf_lltot                    pfstd::lltowcs
    #define  _pf_ttoull                   pfstd::wcstoull
    #define  _pf_ulltot                   pfstd::ulltowcs
    #define  _pf_ttof                     pfstd::wcstof
    #define  _pf_ftot                     pfstd::ftowcs

    #define  _pf_tcstoutf8                pfstd::StringToUTF8
    #define  _pf_tcstounicode             pfstd::StringToUnicode
    #define  _pf_tcstombs                 pfstd::StringToUTF8
    #define  _pf_tcstowcs                 pfstd::StringToUnicode

    //#define  _pf_tcstotcs                 pfstd::StringToUnicode
    #define  _pf_mbstotcs                 pfstd::UTF8ToUnicode
    #define  _pf_wcstotcs
    #define  _pf_strtotcs                 pfstd::StringToUnicode   
#else
    #define  _pf_tstrprintf               pfstd::strprintf
    #define  _pf_vtstrprintf              pfstd::vstrprintf
    #define  _pf_tstrprintfa              pfstd::strprintfa
    #define  _pf_tcstok_rc                strtok_rc
    #define  _pf_tcsextractstring         pfstd::strextractstring
    #define  _pf_tstrfileexist            pfsfl::FileExist
    #define  _pf_tcsdumpchr               strdumpchr
    #define  _pf_tstring_split            pfstd::string_split

    #define  _pf_tgetpathdir              pfstd::getpathdir
    #define  _pf_tgetpathfname            pfstd::getpathfname
    #define  _pf_tgetpathext              pfstd::getpathext
    #define  _pf_tgetpathfnameext         pfstd::getpathfnameext

    #define  _pf_ttoi                     pfstd::strtoi
    #define  _pf_itot                     pfstd::itostr
    #define  _pf_ttoui                    pfstd::strtoui
    #define  _pf_uitot                    pfstd::uitostr
    #define  _pf_ttoll                    pfstd::strtoll
    #define  _pf_lltot                    pfstd::lltostr
    #define  _pf_ttoull                   pfstd::strtoull
    #define  _pf_ulltot                   pfstd::ulltostr
    #define  _pf_ttof                     pfstd::strtof
    #define  _pf_ftot                     pfstd::ftostr

    #define  _pf_tcstoutf8                pfstd::StringToUTF8
    #define  _pf_tcstounicode             pfstd::StringToUnicode
    #define  _pf_tcstombs                 pfstd::StringToUTF8
    #define  _pf_tcstowcs                 pfstd::StringToUnicode

    //#define  _pf_tcstotcs                 pfstd::StringToUTF8
    #define  _pf_mbstotcs                 
    #define  _pf_wcstotcs                 pfstd::UnicodeToUTF8
    #define  _pf_strtotcs                 pfstd::StringToUTF8
#endif


#ifdef _PSDK
    #include <pf-sfile.h>
    
    typedef void*                         uPfFile;
    #define _pffile(f)                    f
    #define _pffileno(f)                  ((int)f)
    #define _pfInit()                     PfsInit()
    #define _pfmopen                      Pfsopen
    #define _pfseek                       Pfsseek
    #define _pftell                       Pfstell
    #define _pfread                       Pfsread
    #define _pfwrite                      Pfswrite
    #define _pfclose                      Pfsclose
    #define _pfputs                       Pfsputs
    #define _pfgets                       Pfsgets
    #define _pfeof                        Pfseof
    #define _pfflush                      Pfsflush
    #define _pfgetc                       Pfsgetc
    #define _pfputc                       Pfsputc
    #define _pffilelength(f)              Pfsfilelength(f)
#else
    #include <string.h>
    #include <pf-file.h>
    
    typedef FILE*                         uPfFile;
    #define _pffile(f)                    ((FILE*)f)
    #define _pffileno(f)                  ::fileno(f)
    #define _pfInit()                     
    #define _pfmopen                      fopen
    #define _pfwopen                      _wfopen
    #define _pfseek                       fseek
    #define _pftell                       ftell
    #define _pfread                       fread
    #define _pfwrite                      fwrite
    #define _pfclose                      fclose
    #define _pfputs                       fputs
    #define _pfgets                       fgets
    #define _pfeof                        feof
    #define _pfflush                      fflush
    #define _pfgetc                       fgetc
    #define _pfputc                       fputc
    #ifdef _WINDOWS                       
        #define _pffilelength(f)          _filelength(::_fileno(f) )
    #endif
    #ifdef _LINUX
        #define _pffilelength(f)          filelength(f)
    #endif
#endif

#ifndef _UNICODE
    #define _pftopen                      fopen
    #define _pfgettc                      fputc
#else
    #define _pftopen                      _wfopen
    #define _pfgettc                      fputwc
#endif


_binbuf& operator<<( _binbuf& bin, const _mstring str );
