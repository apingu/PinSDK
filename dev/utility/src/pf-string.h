//
//
//
// my str function compute string
//
//
//
//
//  platform foundation string function
//  2018 12 12 將各式 字串的判別都放入其中 只要只是單純字串判斷 path url 或往後其他的
//
//
//              Copyright (C) 1999 Pin
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


//////////////////////////////////////////////////////////////////////////
// String

//
//
char            chartolower         ( _IN char ch );
wchar_t         chartolower         ( _IN wchar_t ch );

// change input string to all lower char
// @param  input string
// @return change char count
int             strtolower          ( _INOUT char* string );
int             wcstolower          ( _INOUT wchar_t* string );

// change input string to all upper char
// @param  input string
// @return change char count
int             strtoupper          ( _INOUT char* string );

// lower case find string in string   ex:strcasestr(path, ".txt")
// @param  input string
// @param  compare string
// @return the minus of the first different character in low case
int             strcaseicmp         ( _IN const char* ins1, _IN const char* ins2 );
int             wcscaseicmp         ( _IN const wchar_t* ins1, _IN const wchar_t* ins2 );


// compare two string include wildcard character  *
// @param  input string
// @param  compare string string has *
// @return if likeness 0 else -1
int             strlike             ( _IN const char* cmps, _IN const char* wchs, _IN const char wildcard = '*' );
int             wcslike             ( _IN const wchar_t* cmps, _IN const wchar_t* wchs, _IN const wchar_t wildcard = '*' );


#if defined(_WINDOWS)
// lower case find string in string
// gunc have this function but win don't
const char*     strcasestr          ( _IN const char* str, _IN const char* subs );
#endif
const wchar_t*  wcscasestr          ( _IN const wchar_t* str, _IN const wchar_t* subs );


// find string between two word and
// @return the next begin position of find word in ins   =>  AAAAA{DDDDDD}EEEE   between {}  return 'E';
const char*     strbtwnstr          ( _OUT char* outs, _IN int outl, _IN const char* ins, _IN const char* subs1, _IN const char* subs2 );
const wchar_t*  wcsbtwnstr          ( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t* subs1, _IN const wchar_t* subs2 );

// find sub string forward
// 順向 找子字串 從 ins 的 bpos 個位置開始找
const char*     strffindstr         ( _IN const char* ins, _IN int bpos, _IN const char* subs );
const wchar_t*  wcsffindstr         ( _IN const wchar_t* ins, _IN int bpos, _IN const wchar_t* subs );


// find sub string reverse 
// 逆向 找子字串 從 ins 的 bpos 個位置開始找
const char*     strrfindstr         ( _IN const char* ins, _IN int bpos, _IN const char* subs );
const wchar_t*  wcsrfindstr         ( _IN const wchar_t* ins, _IN int bpos, _IN const wchar_t* subs );


// strtok_r  + return current char
char*           strtok_rc           ( _INOUT char* s, _IN const char* delim, _OUT char** save_ptr, _OUT char* delimchar = NULL );
wchar_t*        wcstok_rc           ( _INOUT wchar_t* s, _IN const wchar_t* delim, _OUT wchar_t** save_ptr, _OUT wchar_t* delimchar = NULL );

// dump char from string all
//   ex : abbadgff  (dump b) ==> aadgff
int             strdumpchr          ( _INOUT char* inouts, _IN char dumpc );
int             wcsdumpchr          ( _INOUT wchar_t* inouts, _IN wchar_t dumpc );

// dump first string from string ( only first )
//   ex : abbadgff  (dump bad) ==> abgff
int             strdumpstr          ( _OUT char* outs, _IN int outl, _IN const char* str, _IN const char* dumps );
int             wcsdumpstr          ( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* str, _IN const wchar_t* dumps );

// dump all string from string
//int             strdumpallstr       ( _OUT char* outs, _IN int outl, _IN const char str, _IN const char* dumps );
//int             strdumpallstr       ( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t str, _IN const wchar_t* dumps );

//  n count of char 'c' in string "str"
int             strchrcount         ( _IN const char* ins, _IN char ch );
int             wcschrcount         ( _IN const wchar_t* ins, _IN wchar_t ch );

//  find n length
int             strchrcount         ( _IN const char* ins, _IN int len, _IN char ch );
int             wcschrcount         ( _IN const wchar_t* ins, _IN int len, _IN wchar_t ch );

// set char in string
//    ex  abcdeffff c = f pos = 1   ==> afcdeffff
void            strsetchr           ( _OUT char* outs, _IN char c, _IN unsigned int pos );
void            wcssetchr           ( _OUT wchar_t* outs, _IN wchar_t c, _IN unsigned int pos );

// set char in string
//    ex  abcdeffff c = f pos = 1 n = 3   ==> afffeffff
void            strsetnchr          ( _OUT char* outs, _IN char c, _IN unsigned int pos, _IN size32_t n );

//split string
int             strspliteol         ( _IN const char* str, _OUT char outsv[][_MAX_STRING_LEN], _IN int outlines );

// AAA0010DDD0020EEE040   ==>  0010   return address of '0'010DDD
const char*     strsplitnum         ( _IN const char* str, _OUT char* outs, _IN int outl );

//    ex  abdekkcgg  div = ekk   ==> cgg
//           ---
const char*     strsplitstr         ( _IN const char* str, _IN const char* div );

// replace word in string
// "aabbccccddee ffddd"  "ee ff" to "xx"   = >"aabbccccddxxddd", change all or change first
size_t          strreplace          ( _OUT char* outs, _IN int outl, _IN const char* str, _IN const char* finds, _IN const char* insteads,  _IN bool isall = true );
size_t          wcsreplace          ( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* str, _IN const wchar_t* finds, _IN const wchar_t* insteads, _IN bool isall = true );

int             strreplacec         ( _OUT char* outs, _IN int outl, _IN const char* str, _IN const char findc, _IN const char insteadc, _IN bool isall = true );
int             wcsreplacec         ( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* str, _IN const wchar_t findc, _IN const wchar_t insteadc, _IN bool isall = true );

// "  AAAAA AAAABBB  "   = >  "AAAAA AAAABBB"  era is a string
int             strextractstring    ( _INOUT char* inouts, _IN const char* era = " " );
int             wcsextractstring    ( _INOUT wchar_t* inouts, _IN const wchar_t* era = L" " );

// "  AAAAA AAAABBB  "   = >  "AAAAA AAAABBB"  era is one or more character linke  ? , \n \t  all character will be erase
int             strextractcharacter ( _INOUT char* inouts, _IN const char* era = " " );
int             wcsextractcharacter ( _INOUT wchar_t* inouts, _IN const wchar_t* era = L" " );

// "  AAAAA AAAABBB  "   = >  "AAAAA AAAABBB"   clear space char
//int             uncextractstring    ( _INOUT wchar_t* inouts, _IN const wchar_t* era = L" " );

// " \n AAAAA AAAABBB  \n DDDDDDD  \n   "   = >  "\nAAAAA AAAABBB\nDDDDDDD\n"
int             strextractline      ( _INOUT char* inouts, _IN const char* era = " " );

// aaaaa"bb bb bb" ccccc   = > aaaaa"bbFbbFbb" ccccc
//         FFFFFFFF FF FFFFFFFFF
int             splitstringstring   ( _OUT char* outmask, _INOUT char* instring );
int             makestringstring    ( _OUT char* outmask, _INOUT char* instring );


#if !defined(_WINDOWS)
// gunc don't have this function
char*           itoa                ( _IN int value, _OUT char* result, _IN int base );

/* wid character function*/
unsigned int    strlenW             ( _IN const wchar_t* str );
wchar_t*        strcpyW             ( _OUT wchar_t* dst, _IN const wchar_t* src );
int             strcmpW             ( _IN const wchar_t* str1, _IN const wchar_t* str2 );
int             strncmpW            ( _IN const wchar_t* str1, _IN const wchar_t* str2, _IN int n );
wchar_t*        strcatW             ( _INOUT wchar_t* dst, _IN const wchar_t* src );
wchar_t*        strchrW             ( _IN const wchar_t* str, _IN wchar_t ch );
   
#endif/*_WINDOWS*/

size32_t        strtoescapestr      ( _OUT char* outs, _IN size32_t outl, _IN const char* str, _IN size32_t inl );


//int             isnumber            ( _IN int n );
bool            isinteger           ( _IN double d );

// 整數或浮點數  0 => 不是   1 => 整數(0開頭的也不行)   2 => double
int             ismnumber           ( _IN const char*    str );
int             iswnumber           ( _IN const wchar_t* str );
bool            isxnumber           ( _IN const char*    str );


// convert binary res to string
//size32_t        maxbinlen           ( _IN size32_t strlen );
size32_t        bintostr            ( _OUT ubyte_t* outs, _IN size32_t outl, _IN const ubyte_t* str, _IN size32_t inl );
size32_t        strtobin            ( _OUT ubyte_t* outs, _IN size32_t outl, _IN const ubyte_t* str, _IN size32_t inl );

// pre get buffer size 
//#define _GetMAXBinaryLen( l )       maxbinlen( l )
//#define _MakeBinaryToString         bintostr
//#define _MakeStringToBinary         strtobin

#define         atohi( a )          ( ( short )atoi( a ) )
char*           lltom               ( _IN long64_t val, _OUT char* buf, _IN int radix );
wchar_t*        lltow               ( _IN long64_t val, _OUT wchar_t* buf, _IN int radix );


int             wtoi                ( _IN const wchar_t* str );
//float          wtof                ( _IN const wchar_t* str );

uint_t          mtoui               ( _IN const char* str );
uint_t          wtoui               ( _IN const wchar_t* str );

long64_t        mtoll               ( _IN const char* str );
long64_t        wtoll               ( _IN const wchar_t* str ); // 標準

ulong64_t       mtoull              ( _IN const char* str );
ulong64_t       wtoull              ( _IN const wchar_t* str );

double          mtof                ( _IN const char* str );
double          wtof                ( _IN const wchar_t* str );

//
char*           ftoa                ( _IN double n, _OUT char* s, int _IN digits );
//#define   _lltoa _i64toa
//#define   _atoll _atoi64

//
int             atox                ( _IN const char* s );

//Convert strings to a number value.
int             strtoi              ( _IN const char* str, _OUT int* n, _IN int base, _IN int min, _IN int max );
int             strtol              ( _IN const char* str, _OUT long32_t* n, _IN int base, _IN long32_t min, _IN long32_t max );
int             strtoul             ( _IN const char* str, _OUT ulong32_t* n, _IN int base, _IN ulong32_t min, _IN ulong32_t max );

// MultiByte to unicode  "tw","big5"; "jp", jis"; "ka"; "ca", "gb"; "us"; "utf8 = 65001"  
int             mtow                ( _IN const char* insA, _OUT wchar_t* strW, _IN int lenW, _IN int codepage = 65001 );
// unicode to MultiByte
int             wtom                ( _IN const wchar_t* strW, _OUT char* strA, _IN int lenA, _IN int codepage = 65001 );

//int             utf8tobig5          ( _IN const char* strutf8, _OUT char_t* strbig5, _IN int lenbig5 );
//int             big5toutf8          ( _IN const char* strbig5, _OUT char_t* strutf8, _IN int lenurf8 );
//
//void            utf8Toshiftjis      ( _IN const char* strutf8, _OUT char_t* strsjis, _IN int lenbig5 );
//void            shiftjisooutf8      ( _IN const char* strsjis, _OUT char_t* strutf8, _IN int lenurf8 );
//
//int             utf8tocode          ( _IN const char* strutf8, _OUT char_t* str, _IN int len, _IN int codepage );

// unicode to binary string
char*            wtos                ( _IN const wchar_t* strW, _OUT char* buff, _IN int bufflen );
wchar_t*         stow                ( _IN const char* str, _OUT wchar_t* wbuff, _IN int wbuflen );


//////////////////////////////////////////////////////////////////////////
/*
int             unclen              ( _IN const wchar_t* szUnicode );

wchar_t*        unccpy              ( _OUT wchar_t* strDestination, _IN const wchar_t* strSource );

wchar_t*        unccat              ( _INOUT wchar_t* strDestination, _IN const wchar_t* strSource );

int             unccmp              ( _IN const wchar_t* string1, _IN const wchar_t* string2 );

wchar_t*        uncstr              ( _IN const wchar_t* str, _IN const wchar_t* strSearch );

wchar_t*        uncchr              ( _IN  const wchar_t* str, _IN wchar_t c );

wchar_t*        unctok( _IN wchar_t* strToken, _IN const wchar_t* strDelimit );
*/


// only dump first in string
int              wcsdumpstr          ( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* str, _IN const wchar_t* dumps );
// dump all in string
int              wcsdumpallstr       ( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* str, _IN const wchar_t* dumps );

// replace word in string
// "aabbccccddee ffddd"  "ee ff" to "xx"   = >"aabbccccddxxddd", change all or change first
int              wcsreplace          ( _OUT wchar_t* outs, _IN wchar_t outl, _IN const wchar_t* str, _IN const wchar_t* finds, _IN const wchar_t* insteads, _IN bool isall = true );

int              iswcsdig            ( _IN const wchar_t* str );


//////////////////////////////////////////////////////////////////////////
//
// return the same number of byte in two buffer
int              memcmpcount        ( _IN const char* ins1, _IN size32_t inl1, _IN const char* ins2, _IN size32_t inl2 );
// if difference was in the range regarded as the same
int              memfuzzycmpcount   ( _IN const char* ins1, _IN size32_t inl1, _IN const char* ins2, _IN size32_t inl2, uint8_t range );



#if defined(_LINUX)
wchar_t*         wcstok             ( wchar_t *wcs, const wchar_t *delim );
#endif

#define          mtoi                    atoi
                 
#define          atoui                   mtoui
#define          atoull                  mtoull
                 
#define          lltoa                   lltom
                 
#ifdef _UNICODE  
    #define      _tcscasestr             wcscasestr
    #define      _tcscasecmp             wcscaseicmp
    #define      _tcslike                wcslike
    #define      _istnumber              iswnumber
    #define      _tcsbtwnstr             wcsbtwnstr
    #define      _tcsffindstr            wcsffindstr
    #define      _tcsrfindstr            wcsrfindstr
    #define      _tcsextractstring       wcsextractstring
    #define      _tcsextractcharacter    wcsextractcharacter
    #define      _tcsreplace             wcsreplace
    #define      _tcsreplacec            wcsreplacec
    #define      _ttoui                  wtoui
    #define      _ttoull                 wtoull
    #define      _tstoi                  wtoi
    #define      _ttoi                   wtoi
    #define      _lltot                  lltow
    #define      _itot                   _itow

    //#define    _ttof                   wtof
    //#define    _ttoll                  wtoll
    //#define    _ttoll                  wtoll
#else            
    #define      _tcscasestr             strcasestr
    #define      _tcscasecmp             strcaseicmp
    #define      _tcslike                strlike
    #define      _istnumber              ismnumber
    #define      _tcsbtwnstr             strbtwnstr
    #define      _tcsffindstr            strffindstr
    #define      _tcsrfindstr            strrfindstr
    #define      _tcsextractstring       strextractstring
    #define      _tcsextractcharacter    strextractcharacter
    #define      _tcsreplace             strreplace
    #define      _tcsreplacec            strreplacec
    #define      _ttoui                  mtoui
    #define      _ttoull                 mtoull
    #define      _tstoi                  atoi
    #define      _ttoi                   atoi
    #define      _lltot                  lltoa  
    #define      _itot                   _itoa

    //#define    _ttoll                  atoll
    //#define    _ttof                   mtof
    //#define    _ttoll                  mtoll
    //#define    _ttof                   mtof  
#endif

#ifdef _WINDOWS
    #ifdef _UNICODE
        #define  _tcsncasecmp            _wcsnicmp
    #else        
        #define  strcasecmp              stricmp
        #define  _tcsncasecmp            _strnicmp
    #endif       
#endif           
                 
#ifdef _LINUX    
    #define      atoll                   mtoll
    #define      _wtoi                   wtoi
    #ifdef _UNICODE
        #define  _tcsncasecmp            wcsncasecmp
        //#define _tcscpy                 wcscpy
        //#define _tcslen                 wcslen
        //#define _tcscmp                 wcscmp
        //#define _stprintf               _swprintf
        //#define  _ttof                   wtof
    #else
        #define  _tcsncasecmp            strncasecmp
        //#define _tcscpy                 strcpy
        //#define _tcslen                 strlen
        //#define _tcscmp                 strcmp
        //#define _stprintf               sprintf
        //#define  _ttof                   mtof  // tchar.h有
    #endif
#endif


#if defined(_PS2)// || defined(_XBOX)
    #define _T(x)  x
    #define strcpy      strcpy
    #define strlen      strlen
    #define strstr      strstr
    #define _stprintf  _stprintf
    #define atoi        atoi
#endif


//////////////////////////////////////////////////////////////////////////
// Path

//  decompose the sequence of a file path by "\\",
//  hierarchy is the ordinal number of "\\"
//      ex  C://a//b//c//d
//        hierarchy = 0  = > destpath = d
//        hierarchy = 1  = > destpath = c
//        hierarchy = 2  = > destpath = b
int             pathrdiv            ( _OUT char* outs, _IN int outl, _IN const char* ins, _IN int hierarchy );

//      ex  C://a//b//c//d
//        hierarchy = 0  = > destpath = d
//        hierarchy = 1  = > destpath = c//d
//        hierarchy = 2  = > destpath = b//c//d
int             pathrdivf           ( _OUT char* outs, _IN int outl, _IN const char* ins, _IN int hierarchy );

// if c is \ or /
int             ispathdiv           ( _IN const char c );

//      ex  C://a//b//c//d
//        n = 1  = > outs = c://a//b//c
//        n = 2  = > outs = c://a//b
int             pathrdump           ( _OUT char* outs, _IN int outl, _IN const char* ins, _IN int n );

// combine two path   c:\aaa1\bbb\ccc\ccc2 +  ..\..\ddd1\ddd2  = c:\aaa1\bbb\ddd1\ddd2
//void    combinepath   (_OUT char* outs, _IN int outl, _IN const char* path1, _IN const char* path2);
//
void            mmakefpath          ( _OUT char* path, _IN const char* drive, _IN const char* dir, const char* _IN fname, const char* _IN ext );
void            msplitfpath         ( _IN const char* path, _OUT char* drive, _OUT char* dir, _OUT char* fname, _OUT char* ext );

void            wmakefpath          ( _OUT wchar_t* path, _IN const wchar_t* drive, _IN const wchar_t* dir, const wchar_t* _IN fname, const wchar_t* _IN ext );
void            wsplitfpath         ( _IN const wchar_t* path, _OUT wchar_t* drive, _OUT wchar_t* dir, _OUT wchar_t* fname, _OUT wchar_t* ext );

// fix the input path string to current path formet
const char*     mcurrentpath        ( _OUT char* outs, _IN const char* ins, _IN const char* repchar = "/" );
const wchar_t*  wcurrentpath        ( _OUT wchar_t* outs, _IN const wchar_t* ins, _IN const wchar_t* repchar = L"/" );

// get the folder without fname form path string
const char*     getpathdir          ( _OUT char* outs, _IN const char* ins );
const char*     getpathfnameext     ( _OUT char* outs, _IN const char* ins );
const char*     getpathfname        ( _OUT char* outs, _IN const char* ins );
const char*     getpathext          ( _OUT char* outs, _IN const char* ins );

#if !defined(_WINDOWS)
/* implementation of the _splitpath runtime library function with ANSI character strings */
void            _splitpath          ( _IN const char* path, _OUT char* drive, _OUT char* dir, _OUT char* fname, _OUT char* ext );
void            _makepath           ( _OUT char*  path, _IN const char* drive, _IN const char* dir, _IN const char*  fname, _IN const char*  ext );

/* implementation of the _splitpath runtime library function with wide character strings */
void            _wsplitpath         ( _IN const wchar_t* path, _OUT wchar_t* drv, _OUT wchar_t* dir, _OUT wchar_t* name, _OUT wchar_t* ext );
void            _wmakepath          ( _OUT wchar_t* path, _IN const wchar_t* drive, _IN const wchar_t* dir, _IN const wchar_t* fname, _IN const wchar_t* ext );
#endif  //_WINDOWS


#ifdef _UNICODE
    #ifndef _tmakefpath
        #define   _tmakefpath             wmakefpath
    #endif
    #ifndef _tsplitfpath
        #define   _tsplitfpath            wsplitfpath
    #endif
    #ifndef   _tcurrentpath
        #define   _tcurrentpath           wcurrentpath
    #endif
#else
    #ifndef _tmakefpath
        #define   _tmakefpath             mmakefpath
    #endif
    #ifndef _tsplitfpath
        #define   _tsplitfpath            msplitfpath
    #endif
    #ifndef   _tcurrentpath
        #define   _tcurrentpath           mcurrentpath
    #endif
#endif


//////////////////////////////////////////////////////////////////////////
// URL
// Like https://example-node:123456@example.com:8080/aaa/bbb/ccc/ddd/html1.html?message;subject=Hello%20World#222221
//
//
struct uri_msyntax
{
    char scheme[ 256 ];
    char username[ 256 ];
    char password[ 256 ];
    char host[ 256 ];
    char port[ 256 ];
    char path[ 256 ];
    char page_name[ 256 ];
    char page_ext[ 256 ];
    char query[ 256 ];
    char fragment[ 256 ];
};

bool            msplituri( _IN const char* url, _OUT uri_msyntax& url_string );
bool            mmakeuri( _OUT char* url, _IN const uri_msyntax& url_string );

struct uri_wsyntax
{
    wchar_t scheme[ 256 ];
    wchar_t username[ 256 ];
    wchar_t password[ 256 ];
    wchar_t host[ 256 ];
    wchar_t port[ 256 ];
    wchar_t path[ 256 ];
    wchar_t page_name[ 256 ];
    wchar_t page_ext[ 256 ];
    wchar_t query[ 256 ];
    wchar_t fragment[ 256 ];
};

bool            wsplituri( _IN const wchar_t* url, _OUT  _OUT uri_wsyntax& url_string );
bool            wmakeuri( _OUT wchar_t* url, _IN const uri_wsyntax& url_string );


#ifdef _UNICODE
    #define _uri_tsyntax            uri_wsyntax
    #define _tsplituri              wsplituri
    #define _tmakeuri               wmakeuri
#else
    #define _uri_tsyntax            uri_msyntax
    #define _tsplituri              msplituri
    #define _tmakeuri               mmakeuri
#endif



//////////////////////////////////////////////////////////////////////////
// program
//char             get_escape_sequences( _IN const char* ins, _IN size32_t inl );
