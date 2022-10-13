//str.cpp my string function
//
//
//
//
//
//
//
//              Copyright (C) 1999 Pin
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

//01. == C 系統文件/C++ 系統文件
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
//#include <string.h>
#include <errno.h>
#include <assert.h>

#if defined(_WINDOWS)
#include <Windows.h>
#endif
#if defined(_LINUX)
#include <wchar.h>
#include <iconv.h>
#include <ctype.h>
#include <wctype.h>
#endif

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "memio.inl"
#include "Unicode.h"
#include "pf-string.h"

//#include "PfConfigure.h"
//#include <pf-stdlib.h>





//////////////////////////////////////////////////////////////////////////
// Cross platform


//////////////////////////////////////////////////////////////////////////
// String

//=============================================================================
// NAME: strtolower
// DESC:
//=============================================================================
char chartolower( _IN char ch )
{
    if ( ch >= 'A' && ch <= 'Z' )
    {
        return ch + 0x20;
    }
    return ch;

}

wchar_t chartolower( _IN wchar_t ch )
{
    if ( ch >= L'A' && ch <= L'Z' )
    {
        return ch + 0x20;
    }
    return ch;

}


//=============================================================================
// NAME: strtolower
// DESC:
//=============================================================================
int strtolower( _INOUT char* string )
{
    if( !string ) return 0;

    int chc = 0;

    for ( size32_t i = 0; i < strlen( string ); i++ )
    {
        if ( !islower( string[ i ] ) )
        {
            chc++;
            string[ i ] = chartolower( string[ i ] );
        }
    }

    return chc;
}

int wcstolower( _INOUT wchar_t* string )
{
    if( !string ) return 0;

    int chc = 0;

    for ( size32_t i = 0; i < wcslen( string ); i++ )
    {
        wchar_t ch = string[ i ];
        if ( ch >= L'A' && ch <= L'Z' )
        {
            chc++;
            string[ i ] = chartolower( string[ i ] );
        }
    }

    return chc;
}


//=============================================================================
// NAME: strtoupper
// DESC:
//=============================================================================
int strtoupper( _INOUT char* string )
{
    if( !string ) return 0;

    int chc = 0;

    for ( size32_t i = 0; i < strlen( string ); i++ )
    {
        int cc = string[ i ];

        if ( cc < 0 )
        {
            continue;
        }

        if ( !isalpha( cc ) )
        {
            continue;
        }

        if ( islower( cc ) )
        {
            chc++;
            string[ i ] = (char)toupper( string[ i ] );
        }
    }

    return chc;
}


//=============================================================================
// NAME: strcaseicmp
// DESC:
//=============================================================================
int strcaseicmp( _IN const char* ins1, _IN const char* ins2 )
{
    if( !ins1 || !ins2 ) return 0;

    size32_t n1 = (size32_t)strlen( ins1 );
    size32_t n2 = (size32_t)strlen( ins2 );
    size32_t n = __min( n1, n2 );

    for ( unsigned int i = 0; i < n; i++ )
    {
        //int ret = ( tolower( ins1[ i ] ) - tolower( ins2[ i ] ) );
        int ret = ( chartolower( ins1[ i ] ) - chartolower( ins2[ i ] ) );
        if ( ret != 0 )
        {
            return ret;
        }
    }

    if ( n1 > n2 )
    {
        return ins1[ n2 ];
    }

    if ( n1 < n2 )
    {
        return ins2[ n1 ];
    }

    return 0;
}

int wcscaseicmp( _IN const wchar_t* ins1, _IN const wchar_t* ins2 )
{
    if( !ins1 || !ins2 ) return 0;

    size32_t n1 = (size32_t)wcslen( ins1 );
    size32_t n2 = (size32_t)wcslen( ins2 );
    size32_t n = __min( n1, n2 );

    for ( unsigned int i = 0; i < n; i++ )
    {
        int ret = ( chartolower( ins1[ i ] ) - chartolower( ins2[ i ] ) );
        if ( ret != 0 )
        {
            return ret;
        }
    }

    if ( n1 > n2 )
    {
        return ins1[ n2 ];
    }

    if ( n1 < n2 )
    {
        return ins2[ n1 ];
    }

    return 0;
}

//=============================================================================
// NAME: strlike
// DESC:
//=============================================================================
int strlike( _IN const char* cmps, _IN const char* wchs, _IN const char wildcard )
{
    // 來鬧的吧
    if ( ( cmps == NULL ) || ( wchs == NULL ) )
    {
        if ( cmps == wchs )
        {
            return 0;
        }
        return -1;
    }

    // 接就萬用字元 比個屁
    if ( strcmp( wchs, &wildcard ) == 0 )
    {
        return 0;
    }

    // 如果一滴個字不是萬用字元 就先直接比對吧
    //if ( ( wchs[ 0 ] != wildcard ) && ( wchs[ 0 ] != cmps[ 0 ] ) )
    //{
    //    return -1;
    //}

    int ret = 0;
    size_t len = strlen( wchs );
    char* wchs_temp = new char[ len + sizeof( char ) ];
    //memset( ndir, 0, _MAX_LDIR );
    // 先把有萬用字源的字串複製過去
    strcpy( wchs_temp, wchs );

    //  以萬用字元 為間隔 將字串切割出來
    char* next_token = NULL;
    char* fp = strtok_rc( wchs_temp, &wildcard, &next_token );

    //bool mcmp = true;
    // 沒有萬用字元
    //if ( ( wchs_temp + len ) == next_token )
    if( strcmp( wchs_temp, wchs ) == 0 )
    {
        ret = strcmp( cmps, wchs_temp );
        //mcmp = false;
    }
    //if( mcmp )
    else
    {
        const char* sp = cmps;
        while ( fp != NULL )
        {
            // 找找看 有沒有在 比對字串中
            const char* fs = strstr( sp, fp );
            // 沒找到 直接出去
            if ( fs == NULL )
            {
                ret = -1;
                break;
            }
            // 移動到剛剛找到的字串後面
            sp = fs + strlen( fp );
            // 找下一個
            fp = strtok_rc( NULL, &wildcard, &next_token );
        }
    }
    SAFE_DELETE_ARRAY( wchs_temp );
    return ret;
}

int wcslike( _IN const wchar_t* cmps, _IN const wchar_t* wchs, _IN const wchar_t wildcard )
{
    // 來鬧的吧
    if ( ( cmps == NULL ) || ( wchs == NULL ) )
    {
        if ( cmps == wchs )
        {
            return 0;
        }
        return -1;
    }

    // 接就萬用字元 比個屁
    if( wcscmp( wchs, &wildcard ) == 0 )
    {
        return 0;
    }

    // 如果一滴個字不是萬用字元 就先直接比對吧
    //if ( ( wchs[ 0 ] != wildcard ) && ( wchs[ 0 ] != cmps[ 0 ] ) )
    //{
    //    return -1;
    //}

    int ret = 0;
    size_t len = wcslen( wchs );
    wchar_t* wchs_temp = new wchar_t[ len + sizeof( wchar_t ) ];
    //memset( ndir, 0, _MAX_LDIR );
    // 先把有萬用字源的字串複製過去
    wcscpy( wchs_temp, wchs );

    //  以萬用字元 為間隔 將字串切割出來
    wchar_t* next_token = NULL;
    wchar_t* fp = wcstok_rc( wchs_temp, &wildcard, &next_token );

    //bool mcmp = true;
    // 沒有萬用字元
    //if ( ( wchs_temp + len ) == next_token )
    if( wcscmp( wchs_temp, wchs ) == 0 )
    {
        ret = wcscmp( cmps, wchs_temp );
        //mcmp = false;
    }
    //if ( mcmp )
    else
    {
        const wchar_t* sp = cmps;
        while ( fp != NULL )
        {
            // 找找看 有沒有在 比對字串中
            const wchar_t* fs = wcsstr( sp, fp );
            // 沒找到 直接出去
            if ( fs == NULL )
            {
                ret = -1;
                break;
            }
            // 移動到剛剛找到的字串後面
            sp = fs + wcslen( fp );
            // 找下一個
            fp = wcstok_rc( NULL, &wildcard, &next_token );
        }
    }
    SAFE_DELETE_ARRAY( wchs_temp );
    return ret;
}

//=============================================================================
// NAME: strcasestr
// DESC:
//=============================================================================
#if defined(_WINDOWS)
const char* strcasestr( _IN const char* ins, _IN const char* subs )
{
    if( !ins || !subs || !*subs ) return NULL;

    const char * ptr = ins;    

    while ( *ptr )
    {
        if ( toupper( *ptr ) == toupper( *subs ) )
        {
            const char * cur1 = ptr + 1;
            const char * cur2 = subs + 1;
            while ( *cur1 && *cur2 && toupper( *cur1 ) == toupper( *cur2 ) )
            {
                cur1++;
                cur2++;
            }
            if ( !*cur2 )
            {
                return ptr;
            }
        }
        ptr++;
    }
    return NULL;
}
#endif

const wchar_t* wcscasestr( _IN const wchar_t* ins, _IN const wchar_t* subs )
{
    if( !ins || !subs || !*subs ) return NULL;

    const wchar_t * ptr = ins;

    while ( *ptr )
    {
        if ( toupper( *ptr ) == toupper( *subs ) )
        {
            const wchar_t * cur1 = ptr + 1;
            const wchar_t * cur2 = subs + 1;
            while ( *cur1 && *cur2 && toupper( *cur1 ) == toupper( *cur2 ) )
            {
                cur1++;
                cur2++;
            }
            if ( !*cur2 )
            {
                return ptr;
            }
        }
        ptr++;
    }
    return NULL;
}

//=============================================================================
// NAME: strbtwnstr
// DESC:
//=============================================================================
const char* strbtwnstr( _OUT char* outs, _IN int outl, _IN const char* ins, _IN const char* subs1, _IN const char* subs2 )
{
    if( ( outs == NULL ) || ( ins == NULL ) || ( subs1 == NULL ) || ( subs2 == NULL ) )
    {
        return NULL;
    }

    memset( outs, 0, outl );
    //int inl = strlen(ins);
    //char* retp = NULL;
    //size32_t subs1len = ( size32_t )strlen( subs1 );
    //char* cmp1 = new char[ subs1len + 1 ];
    //memset( cmp1, 0, subs1len + 1 );
    const char* pos = NULL;
    const char* p1 = strstr( ins, subs1 );

    if ( p1 != NULL )
    {
        pos = p1 + strlen( subs1 );
        const char* p2 = strstr( pos, subs2 );
        if ( p2 != NULL )
        {
            strncpy( outs, pos, p2 - pos );
            return p2 + 1;
        }

        //char* p1 = ( char* )p2 - subs1len;

        //while( p1 >= ins )
        //{
        //  memcpy( cmp1, p1, subs1len );
        //
        //  if( strcmp( cmp1, subs1 ) != 0 )
        //  {
        //      p1--;
        //  }
        //  else
        //  {
        //      const char* np1 = p1 + subs1len;
        //      size_t nel = p2 - np1;
        //      memcpy( outs, np1, ( size_t )__min( ( size_t )nel, ( size_t )outl ) );
        //      retp = p1;
        //      break;
        //  }
        //}
    }

    //SAFE_DELETE_ARRAY( cmp1 );
    /*
    const char* p1 = strstr(ins, subs1);
    if(p1 != NULL)
    {
     const char* np1 = p1 + strlen(subs1);
     if(np1 > (ins+inl))
      return NULL;
     const char* p2 = strstr(np1, subs2);
     if(p2 != NULL)
     {
      const char* np2 = p2 + strlen(subs2);
      if(np2 > (ins+inl))
       return NULL;

      int nel = p2 - np1;
      memcpy(outs, np1, __min(nel, outl));
      return p1;
     }
    }
    */
    return NULL;
}

const wchar_t* wcsbtwnstr( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t* subs1, _IN const wchar_t* subs2 )
{
    if( ( outs == NULL ) || ( ins == NULL ) || ( subs1 == NULL ) || ( subs2 == NULL ) )
    {
        return NULL;
    }

    memset( outs, 0, outl * sizeof( wchar_t ) );
    const wchar_t* pos = NULL;
    const wchar_t* p1 = wcsstr( ins, subs1 );

    if ( p1 != NULL )
    {
        pos = p1 + wcslen( subs1 );
        const wchar_t* p2 = wcsstr( pos, subs2 );
        if ( p2 != NULL )
        {
            wcsncpy( outs, pos, p2 - pos );
            return p2 + 1;
        }
    }
    return NULL;
}


//=============================================================================
// NAME:
// DESC: find sub string forward  順向 找子字串 從 ins 的 bpos 個位置開始找
//=============================================================================
const char* strffindstr( _IN const char* ins, _IN int bpos, _IN const char* subs )
{
    if( ( ins == NULL ) || ( subs == NULL ) )
    {
        return NULL;
    }

    const size_t ins_len = strlen( ins );
    if ( ( bpos < 0 ) || ( bpos >= ins_len ) )
    {
        return NULL;
    }

    const size_t subs_len = strlen( subs );
    while ( strncmp( ( ins + bpos ), subs, subs_len ) != 0 )
    {
        bpos++;
        if ( bpos >= ins_len )
        {
            return NULL;
        }
    }
    return ( ins + bpos );
}

const wchar_t* wcsffindstr( _IN const wchar_t* ins, _IN int bpos, _IN const wchar_t* subs )
{
    if( ( ins == NULL ) || ( subs == NULL ) )
    {
        return NULL;
    }

    const size_t ins_len = wcslen( ins );
    if( ( bpos < 0 ) || ( bpos >= ins_len ) )
    {
        return NULL;
    }

    const size_t subs_len = wcslen( subs );
    while ( wcsncmp( ( ins + bpos ), subs, subs_len ) != 0 )
    {
        bpos++;
        if ( bpos >= ins_len )
        {
            return NULL;
        }
    }
    return ( ins + bpos );
}

//=============================================================================
// NAME:
// DESC: find sub string reverse  逆向 找子字串 從 ins 的 bpos 個位置開始找
//=============================================================================
const char* strrfindstr( _IN const char* ins, _IN int bpos, _IN const char* subs )
{
    if( ( ins == NULL ) || ( subs == NULL ) )
    {
        return NULL;
    }

    const size_t ins_len = strlen( ins );
    if ( ( bpos < 0 ) || ( bpos >= ins_len ) )
    {
        return NULL;
    }

    const size_t subs_len = strlen( subs );
    while ( strncmp( ( ins + bpos ), subs, subs_len ) != 0 )
    {
        bpos--;
        if ( bpos < 0 )
        {
            return NULL;
        }
    }
    return ( ins + bpos );
}

const wchar_t* wcsrfindstr( _IN const wchar_t* ins, _IN int bpos, _IN const wchar_t* subs )
{
    if( ( ins == NULL ) || ( subs == NULL ) )
    {
        return NULL;
    }

    const size_t ins_len = wcslen( ins );
    if ( ( bpos < 0 ) || ( bpos >= ins_len ) )
    {
        return NULL;
    }

    const size_t subs_len = wcslen( subs );
    while ( wcsncmp( ( ins + bpos ), subs, subs_len ) != 0 )
    {
        bpos--;
        if ( bpos < 0 )
        {
            return NULL;
        }
    }
    return ( ins + bpos );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
char* strtok_rc( _INOUT char* s, _IN const char* delim, _OUT char** save_ptr, _OUT char* delimchar )
{
    if( delim == NULL )
    {
        return NULL;
    }

    char* token;
    if ( s == NULL )
        s = *save_ptr;

    if ( delimchar != NULL )
        ( *delimchar ) = '\0';

    /* Scan leading delimiters.  */
    s += strspn( s, delim );
    if ( *s == '\0' )
    {
        *save_ptr = s;
        return NULL;
    }

    /* Find the end of the token.  */
    token = s;
    s = strpbrk( token, delim );
    if ( s == NULL )
        /* This token finishes the string.  */
        * save_ptr = strchr( token, '\0' );
    else
    {
        if ( delimchar != NULL )
            ( *delimchar ) = *s;
        /* Terminate the token and make *SAVE_PTR point past it.  */
        *s = '\0';
        *save_ptr = s + 1;
    }
    return token;

}

wchar_t* wcstok_rc( _INOUT wchar_t* s, _IN const wchar_t* delim, _OUT wchar_t** save_ptr, _OUT wchar_t* delimchar )
{
    if( delim == NULL )
    {
        return NULL;
    }

    wchar_t* token;
    if ( s == NULL )
        s = *save_ptr;

    if ( delimchar != NULL )
        ( *delimchar ) = L'\0';

    /* Scan leading delimiters.  */
    s += wcsspn( s, delim );
    if ( *s == '\0' )
    {
        *save_ptr = s;
        return NULL;
    }

    /* Find the end of the token.  */
    token = s;
    s = wcspbrk( token, delim );
    if ( s == NULL )
        /* This token finishes the string.  */
        * save_ptr = wcschr( token, L'\0' );
    else
    {
        if ( delimchar != NULL )
            ( *delimchar ) = *s;
        /* Terminate the token and make *SAVE_PTR point past it.  */
        *s = L'\0';
        *save_ptr = s + 1;
    }
    return token;

}

//=============================================================================
// NAME: strdumpchr
// DESC:
//=============================================================================
int strdumpchr( _INOUT char* inouts, _IN char dumpc )
{
    if( inouts == NULL )
    {
        return 0;
    }

    size32_t strsize = (size32_t)strlen( inouts );
    char* tempstr = new char[ strsize + 1 ];
    strcpy( tempstr, inouts );
    memset( inouts, 0, strsize * sizeof( char ) );
    //strset(strptr, NULL);
    int charptr = 0;

    for ( size32_t i = 0; i < strsize; i++ )
    {
        //
        if ( tempstr[ i ] == dumpc )
        {
            continue;
        }

        inouts[ charptr ] = tempstr[ i ];
        charptr++;
    }

    //if(tempstr[strsize - 1] != dumpc)
    // strptr[charptr] = tempstr[strsize - 1];
    delete[] tempstr;
    return charptr;
}

int wcsdumpchr( _INOUT wchar_t* inouts, _IN wchar_t dumpc )
{
    if( inouts == NULL )
    {
        return 0;
    }

    size_t strsize = wcslen( inouts );
    wchar_t* tempstr = new wchar_t[ strsize + 1 ];
    wcscpy( tempstr, inouts );
    memset( inouts, 0, strsize * sizeof( wchar_t ) );
    //strset(strptr, NULL);
    int charptr = 0;

    for ( size32_t i = 0; i < strsize; i++ )
    {
        //
        if ( tempstr[ i ] == dumpc )
        {
            continue;
        }

        inouts[ charptr ] = tempstr[ i ];
        charptr++;
    }

    //if(tempstr[strsize - 1] != dumpc)
    // strptr[charptr] = tempstr[strsize - 1];
    delete[] tempstr;
    return charptr;
}

//=============================================================================
// NAME: strdumpstr
// DESC:
//=============================================================================
int strdumpstr( _OUT char* outs, _IN int outl, _IN const char* ins, _IN const char* dumps )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return 0;
    }

    size32_t minlen = __min( (size32_t)outl, (size32_t)( strlen( ins ) + 1 ) );
    __pfmemcpy( outs, ins, minlen * sizeof( char ) );
    char* fstr = strstr( outs, dumps );

    if ( fstr == NULL )
    {
        return 0;
    }

    char* cstr = fstr + strlen( dumps );
    size32_t cpmlen = (size32_t)strlen( cstr );
    __pfmemcpy( fstr, cstr, cpmlen * sizeof( char ) );
    fstr[ cpmlen ] = 0;
    return 1;
}

int wcsdumpstr( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t* dumps )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return 0;
    }

    size32_t minlen = __min( ( size32_t )outl, ( size32_t )( wcslen( ins ) + 1 ) );
    __pfmemcpy( outs, ins, minlen * sizeof( wchar_t ) );
    wchar_t* fstr = wcsstr( outs, dumps );

    if( fstr == NULL )
    {
        return 0;
    }

    wchar_t* cstr = fstr + wcslen( dumps );
    size32_t cpmlen = ( size32_t )wcslen( cstr );
    __pfmemcpy( fstr, cstr, cpmlen * sizeof( wchar_t ) );
    fstr[ cpmlen ] = 0;
    return 1;
}

//=============================================================================
// NAME: uncdumpallstr
// DESC:
//=============================================================================
int wcsdumpallstr( _OUT char* outs, _IN int outl, _IN const char* ins, _IN const char* dumps )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return 0;
    }

    int buflen = outl;
    char* bufptr = new char[ buflen ];
    strcpy( bufptr, ins );
    char* inbuf = bufptr;
    char* outbuf = outs;
    int c = 0;

    while ( strdumpstr( outbuf, buflen, inbuf, dumps ) == 1 )
    {
        SWAPSS( outbuf, inbuf );
        c++;
    }

    if ( outs != outbuf )
    {
        strcpy( outs, outbuf );
    }

    return c;
}


//=============================================================================
// NAME: strchrcount
// DESC:
//=============================================================================
int strchrcount( _IN const char* ins, _IN char ch )
{
    return strchrcount( ins, strlen( ins ), ch );
}

int wcschrcount( _IN const wchar_t* ins, _IN wchar_t ch )
{
    return wcschrcount( ins, wcslen( ins ), ch );
}

int strchrcount( _IN const char* ins, _IN int len, _IN char ch )
{
    if( ins == NULL )
    {
        return 0;
    }

    int count = 0;

    int ins_len = __max( strlen( ins ), len );
    for ( int i = 0; i < ins_len; i++ )
    {
        if ( ins[ i ] == ch )
        {
            count++;
        }
    }
    return count;
}

int wcschrcount( _IN const wchar_t* ins, _IN int len, _IN wchar_t ch )
{
    if( ins == NULL )
    {
        return 0;
    }

    int count = 0;

    size_t ins_len = __max( wcslen( ins ), len );
    for ( size_t i = 0; i < ins_len; i++ )
    {
        if ( ins[ i ] == ch )
        {
            count++;
        }
    }
    return count;
}


//=============================================================================
// NAME: strsetchr
// DESC:
//=============================================================================
void strsetchr( _OUT char* outs, _IN char c, _IN unsigned int pos )
{
    if( outs != NULL )
    {
        outs[ pos ] = c;
    }
    return;
}

void wcssetchr( _OUT wchar_t* outs, _IN wchar_t c, _IN unsigned int pos )
{
    if( outs != NULL )
    {
        outs[ pos ] = c;
    }
    return;
}

//=============================================================================
// NAME: strsetnchr
// DESC:
//=============================================================================
void strsetnchr( _OUT char* outs, _IN char c, _IN unsigned int pos, _IN size32_t n )
{
    if( outs == NULL )
    {
        return;
    }

    size_t outslen = strlen( outs );
    unsigned int cc = 0;

    for ( unsigned int i = pos; i < outslen; i++ )
    {
        outs[ pos ] = c;
        cc++;

        if ( cc >= n )
        {
            return;
        }
    }
}

//=============================================================================
// NAME: strspliteol
// DESC:
//=============================================================================
int strspliteol( _IN char* ins, _OUT char outsv[][ _MAX_STRING_LEN ], _IN int outlines )
{
    if( ins == NULL )
    {
        return 0;
    }

    char* ptr = ins;
    int Strsize = 0;
    int StrLen = 0;

    for ( int i = 0; i < outlines; i++ )
    {
        if ( ( *ptr != 0X09 ) && ( *ptr != 0X20 ) && ( *ptr != 0X0D ) && ( *ptr != 0X0A ) )
        {
            outsv[ Strsize ][ StrLen ] = ( *ptr );
            StrLen++;
        }
        else if ( ( *( ptr - 1 ) != 0X09 ) && ( *( ptr - 1 ) != 0X20 ) && ( *( ptr - 1 ) != 0X0D ) && ( *( ptr - 1 ) != 0X0A ) )
        {
            Strsize++;
            StrLen = 0;
        }

        ptr++;
    }

    return Strsize + 1;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
const char* strsplitnum( _IN const char* ins, _OUT char* outs, _IN int outl )
{
    if( ins == NULL )
    {
        return NULL;
    }

    int fd = -1;
    int j = 0;

    for ( unsigned int i = 0; i < strlen( ins ); i++ )
    {
        if ( ( ins[ i ] <= -1 ) || ( ins[ i ] >= 255 ) )
        {
            continue;
        }

        if ( isdigit( ins[ i ] ) )
        {
            if ( j < outl )
            {
                outs[ j ] = ins[ i ];
                j++;
            }
            else
            {
                break;
            }

            if ( fd == -1 )
            {
                fd = i;
            }
        }
        else
        {
            if ( fd >= 0 )
            {
                break;
            }
        }
    }

    outs[ j ] = 0;

    if ( fd == -1 )
    {
        return NULL;
    }

    return ins + fd;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
const char* strsplitstr( _IN const char* ins, _IN const char* div )
{
    if( ins == NULL )
    {
        return NULL;
    }

    char* s = (char*)strstr( ins, div );

    if ( s != NULL )
    {
        s += ( strlen( div ) );
        return s;
    }

    return NULL;
}

//=============================================================================
// NAME: strreplace
// DESC:
//=============================================================================
size_t strreplace( _OUT char* outs, _IN int outl, _IN const char* ins, _IN const char* finds, _IN const char* insteads, _IN bool isall )
{
    if( ( outs == NULL ) || ( outl == NULL ) || ( ins == NULL ) || ( finds == NULL ) || ( insteads == NULL ) )
    {
        return 0;
    }

    if ( outs == ins )
    {
        assert( 0 && "in buffer equal out" );
        return -1;
    }

    if ( outl <= 0 )
    {
        return -1;
    }

    outs[ 0 ] = 0;

    if ( strcmp( finds, "" ) == 0 )
    {
        return 0;
    }

    char* ret = NULL, *r = NULL;
    const char* p, *q;
    size_t oldlen = strlen( finds );
    size_t newlen = strlen( insteads );
    size_t count = 0, retlen = 0;
    size_t srclen = strlen( ins );

    if ( !isall )
    {
        for ( count = 0, p = ins; ( q = strstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            count++;
            break;
        }
    }
    else
    {
        for ( count = 0, p = ins; ( q = strstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            count++;
        }
    }

    if ( strcmp( finds, insteads ) == 0 )
    {
        strcpy( outs, ins );
        return count;
    }

    if ( count > 0 )
    {
        if ( oldlen != newlen )
        {
            /* this is undefined if p - str > PTRDIFF_MAX */
            retlen = strlen( p ) + ( p - ins ) + ( count * ( newlen - oldlen ) );
        }
        else
        {
            retlen = strlen( ins );
        }

        ret = new char[ retlen + 32 ];

        for ( r = ret, p = ins; ( q = strstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            /* this is undefined if q - p > PTRDIFF_MAX */
#if defined(_WINDOWS) || defined(_XBOX)
            ptrdiff_t l = q - p;
#else
            long64_t l = q - p;
#endif
            __pfmemcpy( r, p, l );
            r += l;
            __pfmemcpy( r, insteads, newlen );
            r += newlen;

            if ( !isall )
            {
                p = q + oldlen;
                break;
            }
        }

        strcpy( r, p );
        int minlen = __min( (int)retlen, (int)( outl - 1 ) );
        __pfmemcpy( outs, ret, minlen );
        outs[ minlen ] = 0;
        delete[] ret;
    }
    else
    {
        int minlen = __min( (int)srclen, (int)( outl - 1 ) );
        __pfmemcpy( outs, ins, minlen );
        outs[ minlen ] = 0;
    }

    return count;
}

size_t wcsreplace( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t* finds, _IN const wchar_t* insteads, _IN bool isall )
{
    if( ( outs == NULL ) || ( outl == NULL ) || ( ins == NULL ) || ( finds == NULL ) || ( insteads == NULL ) )
    {
        return 0;
    }

    if ( outs == ins )
    {
        assert( 0 && "in buffer equal out" );
        return -1;
    }

    if ( outl <= 0 )
    {
        return -1;
    }

    outs[ 0 ] = 0;

    if ( wcscmp( finds, L"" ) == 0 )
    {
        return 0;
    }

    wchar_t* ret = NULL, *r = NULL;
    const wchar_t* p, *q;
    size_t oldlen = wcslen( finds );
    size_t newlen = wcslen( insteads );
    size_t count = 0, retlen = 0;
    size_t srclen = wcslen( ins );

    if ( !isall )
    {
        for ( count = 0, p = ins; ( q = wcsstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            count++;
            break;
        }
    }
    else
    {
        for ( count = 0, p = ins; ( q = wcsstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            count++;
        }
    }

    if ( wcscmp( finds, insteads ) == 0 )
    {
        wcscpy( outs, ins );
        return count;
    }

    if ( count > 0 )
    {
        if ( oldlen != newlen )
        {
            /* this is undefined if p - str > PTRDIFF_MAX */
            retlen = ( wcslen( p ) + ( p - ins ) + ( count * ( newlen - oldlen ) ) ) * 2;
        }
        else
        {
            retlen = ( wcslen( ins ) ) * 2;
        }

        ret = new wchar_t[ retlen + 32 ];

        for ( r = ret, p = ins; ( q = wcsstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            /* this is undefined if q - p > PTRDIFF_MAX */
#if defined(_WINDOWS) || defined(_XBOX)
            ptrdiff_t l = q - p;
#else
            long64_t l = q - p;
#endif
            __pfmemcpy( r, p, l * 2 );
            r += ( l );
            __pfmemcpy( r, insteads, newlen * 2 );
            r += ( newlen );

            if ( !isall )
            {
                p = q + ( oldlen );
                break;
            }
        }

        wcscpy( r, p );
        int minlen = __min( (int)retlen, (int)( outl - 1 ) );
        __pfmemcpy( outs, ret, minlen * 2 );
        outs[ minlen ] = 0;
        delete[] ret;
    }
    else
    {
        int minlen = __min( (int)srclen, (int)( outl - 1 ) );
        __pfmemcpy( outs, ins, ( minlen * 2 ) );
        outs[ minlen ] = 0;
    }

    return count;
}

//=============================================================================
// NAME: strreplacec
// DESC:
//=============================================================================
int strreplacec( _OUT char* outs, _IN int outl, _IN const char* ins, _IN const char findc, _IN const char insteadc, _IN bool isall )
{
    if( ( outs == NULL ) || ( outl == NULL ) || ( ins == NULL ) || ( findc == NULL ) || ( insteadc == NULL ) )
    {
        return 0;
    }

    char* newstr = new char[ strlen( ins ) + 1 ];
    strcpy( newstr, ins );
    size32_t retlen = (size32_t)strlen( newstr );

    for ( size32_t i = 0; i < retlen; i++ )
    {
        if ( newstr[ i ] == findc )
        {
            newstr[ i ] = insteadc;
        }

        if ( isall == false )
        {
            break;
        }
    }

    size32_t minlen = __min( (size32_t)retlen, (size32_t)( outl - 1 ) );
    __pfmemcpy( outs, newstr, minlen );
    outs[ minlen ] = 0;
    delete[] newstr;
    return 1;
}

int wcsreplacec( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t findc, _IN const wchar_t insteadc, _IN bool isall )
{
    if( ( outs == NULL ) || ( outl == NULL ) || ( ins == NULL ) || ( findc == NULL ) || ( insteadc == NULL ) )
    {
        return 0;
    }

    wchar_t* newstr = new wchar_t[ wcslen( ins ) + 1 ];
    wcscpy( newstr, ins );
    size32_t retlen = (size32_t)wcslen( newstr );

    for ( size32_t i = 0; i < retlen; i++ )
    {
        if ( newstr[ i ] == findc )
        {
            newstr[ i ] = insteadc;
        }

        if ( isall == false )
        {
            break;
        }
    }

    size32_t minlen = __min( (size32_t)retlen, (size32_t)( outl - 1 ) );
    __pfmemcpy( outs, newstr, minlen * 2 );
    outs[ minlen ] = 0;
    delete[] newstr;
    return 1;
}

//=============================================================================
// NAME: strextractstring
// DESC:
//=============================================================================
int strextractstring( _INOUT char* inouts, _IN const char* era )
{
    if( ( inouts == NULL ) || ( era == NULL ) )
    {
        return 0;
    }

    size32_t bufflen = (size32_t)strlen( inouts );
    char*    temp_buff = new char[ bufflen * 2 ];
    char*    pbegin = temp_buff;
    strcpy( pbegin, inouts );
    char*    pend = &( pbegin[ bufflen ] );
    int      bchange = 0;
    size32_t cmplen = (size32_t)strlen( era );

    if ( bufflen < cmplen )
        return bchange;

    bufflen++;

    while ( pbegin <= pend )
    {
        if ( memcmp( pbegin, era, cmplen * sizeof( char ) ) == 0 )
        {
            pbegin += cmplen;
        }
        else
        {
            break;
        }
        bchange += cmplen;
    }

    while ( pend >= pbegin )
    {
        if ( memcmp( pend - cmplen, era, cmplen * sizeof( char ) ) == 0 )
        {
            pend -= cmplen;
        }
        else
        {
            break;
        }
        bchange += cmplen;
    }

    pend[ 0 ] = 0;
    //inouts[] = 0;
    //memcpy( inouts, pbegin, pend - pbegin );
    strcpy( inouts, pbegin );
    SAFE_DELETE_ARRAY( temp_buff );
    return bchange;
}

int wcsextractstring( _INOUT wchar_t* inouts, _IN const wchar_t* era )
{
    if( ( inouts == NULL ) || ( era == NULL ) )
    {
        return 0;
    }

    size32_t bufflen = (size32_t)wcslen( inouts );
    wchar_t* temp_buff = new wchar_t[ bufflen * 2 ];
    wchar_t* pbegin = temp_buff;
    wcscpy( pbegin, inouts );
    wchar_t* pend = &( pbegin[ bufflen ] );
    int      bchange = 0;
    size32_t cmplen = (size32_t)wcslen( era );

    if ( bufflen < cmplen )
        return bchange;

    bufflen++;

    while ( pbegin <= pend )
    {
        if ( memcmp( pbegin, era, cmplen * sizeof( wchar_t ) ) == 0 )
        {
            pbegin += cmplen;
        }
        else
        {
            break;
        }
        bchange += cmplen;
    }

    while ( pend >= pbegin )
    {
        if ( memcmp( pend - cmplen, era, cmplen * sizeof( wchar_t ) ) == 0 )
        {
            pend -= cmplen;
        }
        else
        {
            break;
        }
        bchange += cmplen;
    }

    //for ( int i = 0; i < bufflen; i++ )
    //{
    //    //if(inouts[i]!= ' ')
    //    if ( wcschr( era, inouts[i] ) == 0 )
    //    {
    //        pbegin = &( inouts[i] );
    //        break;
    //    }
    //
    //    bchange++;
    //}
    //
    //for ( int i = bufflen - 1; i >= 0; i-- )
    //{
    //    //if(inouts[i] == ' ')
    //    if ( wcschr( era, inouts[i] ) != 0 )
    //    {
    //        inouts[i] = 0;
    //        pend = &( inouts[i] );
    //        bchange++;
    //    }
    //    else
    //    {
    //        break;
    //    }
    //}

    pend[ 0 ] = 0;
    wcscpy( inouts, pbegin );
    //memcpy( inouts, pbegin, pend - pbegin );
    SAFE_DELETE_ARRAY( temp_buff );
    return bchange;
}

/*
int uncextractstring( _INOUT wchar_t* inouts, _IN const wchar_t* era )
{
    int      bufflen = unclen( inouts ) + 1;
    wchar_t* pbegin  = inouts;
    wchar_t* pend    = &( inouts[bufflen - 1] );
    int      bchange = 0;

    for ( int i = 0; i < bufflen; i++ )
    {
        //if(inouts[i]!= ' ')
        if ( uncchr( era, inouts[i] ) == 0 )
        {
            pbegin = &( inouts[i] );
            break;
        }
        bchange++;
    }

    for ( int i = bufflen - 1; i >= 0; i-- )
    {
        //if(inouts[i] == ' ')
        if ( uncchr( era, inouts[i] ) != 0 )
        {
            inouts[i] = 0;
            pend = &( inouts[i] );
            bchange++;
        }
        else
        {
            break;
        }
    }

    unccpy( inouts, pbegin );
    int ll = pend - pbegin;
    inouts[ll] = 0;
    return bchange;
}
*/

//=============================================================================
// NAME: strextractstring
// DESC:
//=============================================================================
int strextractcharacter( _INOUT char* inouts, _IN const char* era )
{
    if( ( inouts == NULL ) || ( era == NULL ) )
    {
        return 0;
    }

    size_t   bufflen = (size32_t)strlen( inouts );
    char*    pbegin = inouts;
    char*    pend = &( inouts[ bufflen ] );
    int      bchange = 0;
    size_t   cmplen = (size32_t)strlen( era );
    size_t   eralen = strlen( era );

    if ( bufflen < cmplen )
        return bchange;

    bufflen++;

    while ( pbegin <= pend )
    {
        for ( size_t i = 0; i < eralen; era++ )
        {
            if ( ( *pbegin ) == era[ i ] )
                pbegin++;
        }
        bchange += cmplen;
    }

    while ( pend >= pbegin )
    {
        for ( size_t i = 0; i < eralen; era++ )
        {
            if ( ( *pend ) == era[ i ] )
                pend--;
        }
        bchange += cmplen;
    }

    pend[ 0 ] = 0;
    //inouts[] = 0;
    strcpy( inouts, pbegin );
    return bchange;
}

int wcsextractcharacter( _INOUT wchar_t* inouts, _IN const wchar_t* era )
{
    if( ( inouts == NULL ) || ( era == NULL ) )
    {
        return 0;
    }

    size_t   bufflen = (size32_t)wcslen( inouts );
    wchar_t* pbegin = inouts;
    wchar_t* pend = &( inouts[ bufflen ] );
    int      bchange = 0;
    size_t   cmplen = (size32_t)wcslen( era );
    size_t   eralen = wcslen( era );

    if ( bufflen < cmplen )
        return bchange;

    bufflen++;

    while ( pbegin <= pend )
    {
        for ( size_t i = 0; i < eralen; era++ )
        {
            if ( ( *pbegin ) == era[ i ] )
                pbegin++;
        }
        bchange += cmplen;
    }

    while ( pend >= pbegin )
    {
        for ( size_t i = 0; i < eralen; era++ )
        {
            if ( ( *pend ) == era[ i ] )
                pend--;
        }
        bchange += cmplen;
    }

    pend[ 0 ] = 0;
    //inouts[] = 0;
    wcscpy( inouts, pbegin );
    return bchange;
}

//=============================================================================
// NAME: strextractline
// DESC:
//=============================================================================
int strextractline( _INOUT char* inouts, _IN const char* era )
{
    if( ( inouts == NULL ) || ( era == NULL ) )
    {
        return 0;
    }

    size32_t n = (size32_t)strlen( inouts );
    char* buffA = new char[ n + 1 ];
    char* buffB = new char[ n + 1 ];
    int bchange = 0;
    char* next_token = NULL;
    char* p = strtok_rc( inouts, "\r\n", &next_token );

    if ( p != NULL )
    {
        strcpy( buffB, p );
        bchange += strextractstring( buffB, era );
        strcpy( buffA, buffB );
        p = strtok_rc( NULL, "\r\n", &next_token );

        while ( p != NULL )
        {
            strcpy( buffB, p );
            bchange += strextractstring( buffB, era );
            strcat( buffA, "\n" );
            strcat( buffA, buffB );
            p = strtok_rc( NULL, "\r\n", &next_token );
        }

        strcpy( inouts, buffA );
    }

    SAFE_DELETE_ARRAY( buffA );
    SAFE_DELETE_ARRAY( buffB );
    return bchange;
}


//=============================================================================
// NAME: splitstringstring
// DESC:
//=============================================================================
int splitstringstring( _OUT char* outmask, _INOUT char* instring )
{
    if( ( outmask == NULL ) || ( instring == NULL ) )
    {
        return 0;
    }

    size_t len = strlen( instring );
    bool bmask = false;
    int  maskc = 0;

    for ( size_t i = 0; i < len; i++ )
    {
        outmask[ i ] = -1;

        if ( bmask )
        {
            if ( instring[ i ] == ' ' )
            {
                outmask[ i ] = instring[ i ];
                instring[ i ] = -1;
                maskc++;
            }

            if ( instring[ i ] == '\"' )
            {
                bmask = false;
            }
        }
        else
        {
            if ( instring[ i ] == '\"' )
            {
                bmask = true;
            }
        }
    }

    outmask[ len ] = 0;
    return maskc;
}


//=============================================================================
// NAME: makestringstring
// DESC:
//=============================================================================
int makestringstring( _OUT char* outmask, _INOUT char* instring )
{
    if( ( outmask == NULL ) || ( instring == NULL ) )
    {
        return 0;
    }

    size_t len = strlen( outmask );
    //bool bmask = false;
    int  maskc = 0;

    for ( size_t i = 0; i < len; i++ )
    {
        if ( outmask[ i ] == -1 )
        {
            continue;
        }

        instring[ i ] = outmask[ i ];
    }

    return maskc;
}

//=============================================================================
// NAME: strtoescapestr
// DESC:
//=============================================================================
size32_t strtoescapestr( _OUT char* outs, _IN size32_t outl, _IN const char* ins, _IN size32_t inl )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return 0;
    }

    if ( outl < inl )
    {
        return 0;
    }

    size32_t index = 0;
    bool bcheck = true;

    for ( unsigned int i = 0; i < inl; i++ )
    {
        if ( bcheck )
        {
            switch ( ins[ i ] )
            {
            case '\x00':
                {
                    outs[ index ] = '/'; //"//"
                    index++;
                    outs[ index ] = '0';
                    index++;
                }
                break;

            case '\n':
                {
                    outs[ index ] = '/'; //"//"
                    index++;
                    outs[ index ] = ins[ i ];
                    index++;
                }
                break;

            case '\r':
                {
                    outs[ index ] = '/'; //"//"
                    index++;
                    outs[ index ] = ins[ i ];
                    index++;
                }
                break;

            case '\\':
                {
                    outs[ index ] = '/'; ////"//"
                    index++;
                    outs[ index ] = ins[ i ];
                    index++;
                }
                break;

            case '\'':
                {
                    outs[ index ] = '\'';
                    index++;
                    outs[ index ] = ins[ i ];
                    index++;
                }
                break;

            case '\"':
                {
                    outs[ index ] = '/'; //"//"
                    index++;
                    outs[ index ] = ins[ i ];
                    index++;
                }
                break;

            case '\x1a':
                {
                    outs[ index ] = '/'; //"//"
                    index++;
                    outs[ index ] = ins[ i ];
                    index++;
                }
                break;

            default:
                {
                    outs[ index ] = ins[ i ];
                    index++;
                }
                break;
            }
        }
        else
        {
            outs[ index ] = ins[ i ];
            index++;
        }

        int r = isascii( ins[ i ] );

        if ( r )
        {
            bcheck = true;
        }
        else
        {
            bcheck = false;
        }

        if ( index > outl )
        {
            return 0;
        }
    }

    outs[ index ] = 0;
    //printf("++++++++++++++++++Index %d~~~~~~~~~~~~~\n", index);
    return ( index );
}

//=============================================================================
// NAME: isnumber
// DESC:
//=============================================================================
//int isnumber( int n )
//{
//    if ( ( n >= -1 ) || ( n <= 255 ) )
//        return false;
//    return ::isdigit( n );
//}

//=============================================================================
// NAME: isinteger
// DESC:
//=============================================================================
bool isinteger( _IN double d )
{
    double intpart;
    return ( modf( d, &intpart ) == 0.0 ) ? true : false;
}

//=============================================================================
// NAME: isdigit
// DESC:
//=============================================================================
int ismnumber( _IN const char* ins )
{
    if( ins == NULL )
    {
        return 0;
    }

    size_t len = strlen( ins );
    if ( len == 0 )
        return 0;

    // 為0
    if ( ( len == 1 ) && ( ins[ 0 ] == L'0' ) )
        return 1;

    size_t si = 0;

    // 可能是負數
    if ( ins[ 0 ] == '-' )
        si++;

    int dc = 0;

    for ( size_t i = si; i < len; i++ )
    {
        if ( !IsNumber( (UInt16)ins[ i ] ) )
        {
            // 小數
            if ( ins[ i ] != '.' )
            {
                return 0;
            }
            else
            {
                //  超過一個點了 一定不會是 double
                if ( dc > 1 )
                    return 0;
                dc++;
            }
        }
    }

    // 只有一個點 加數字 所以是double
    if ( dc == 1 )
    {
        return 2;
    }
    // 可能是整數 如果第一位字元為0 e.g. 05897562  也當成是字串
    else if ( ( dc == 0 ) && ( ins[ si ] != '0' ) )
    {
        return 1;
    }
    return 0;
}

//=============================================================================
// NAME: isdigit
// DESC:
//=============================================================================
int iswnumber( _IN const wchar_t* ins )
{
    if( ins == NULL )
    {
        return 0;
    }

    size_t len = wcslen( ins );
    if ( len == 0 )
        return 0;

    // 為0
    if ( ( len == 1 ) && ( ins[ 0 ] == L'0' ) )
        return 1;

    size_t si = 0;

    // 可能是負數
    if ( ins[ 0 ] == L'-' )
        si++;

    int dc = 0;

    for ( size_t i = si; i < len; i++ )
    {
        if ( !IsNumber( (int)ins[ i ] ) )
        {
            // 小數
            if ( ins[ i ] != L'.' )
            {
                return 0;
            }
            else
            {
                //  超過一個點了 一定不會是 double
                if ( dc > 1 )
                    return 0;
                dc++;
            }
        }
    }

    // 只有一個點 加數字 所以是double
    if ( dc == 1 )
    {
        return 2;
    }
    // 可能是整數 如果第一位字元為0 e.g. 05897562  也當成是字串
    else if ( ( dc == 0 ) && ( ins[ si ] != L'0' ) )
    {
        return 1;
    }
    return 0;
}

//=============================================================================
// NAME: isxdigit
// DESC:
//=============================================================================
bool isxnumber( _IN const char* ins )
{
    if( ins == NULL )
    {
        return false;
    }

    int b = 0;
    size_t len = strlen( ins );

    if ( ( ins[ 0 ] == '0' ) &&
        ( ins[ 1 ] == 'X' ) )
    {
        b = 2;
    }

    for ( size_t i = b; i < len; i++ )
    {
        if ( !::isxdigit( (int)ins[ i ] ) )
        {
            return false;
        }
    }

    return true;
}


//=============================================================================
// NAME: getbinlen
// DESC:
//=============================================================================
size32_t maxbinlen( _IN size32_t strlen )
{
    return ( strlen * 2 ) + 20;
}

//=============================================================================
// NAME: bintostr
// DESC:
//  \0  :  一個ASCII 0 (NUL)字符。
//  \n  :  一個新行符。
//  \t  :  一個定位符。
//  \r  :  一個回車符。
//  \b  :  一個退格符。
//  \'  :  一個單引號(“'”)符。
//  \"  :  一個雙引號(“"”)符。
//  \\  :  一個反斜線(“\”)符。
//  \%  :  一個“%”符。它用于在正文中搜索“%”的文字實例，否則這里“%”將解釋為一個通配符。
//  \_  :
//   The character code ddd, where ddd is in octal.
//=============================================================================
int s_escape_string_array[ 256 ] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
};

size32_t bintostr( _OUT ubyte_t* outs, _IN size32_t outl, _IN const ubyte_t* ins, _IN size32_t inl )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return 0;
    }

    //pBuffer = new TCHAR [buff_len];
    if ( outl < inl )
    {
        return 0;
    }

    size32_t index = 0;
    //__pfmemcpy(res, buf, buflen);
    for ( unsigned int i = 0; i < inl; i++ )
    {
        switch ( ins[ i ] )
        {
        case 0:
            {
                outs[ index ] = '/'; //'\\"
                index++;
                outs[ index ] = '0';
                index++;
            }
            break;

        case '\\':
            {
                outs[ index ] = '/';//'\\"
                index++;
                outs[ index ] = ins[ i ];
                index++;
            }
            break;

        case '\'':
            {
                outs[ index ] = '\'';
                index++;
                outs[ index ] = ins[ i ];
                index++;
            }
            break;

        case '\"':
            {
                outs[ index ] = '/';//'\\"
                index++;
                outs[ index ] = ins[ i ];
                index++;
            }
            break;

        default:
            {
                if ( s_escape_string_array[ ( (unsigned char)ins[ i ] ) ] )
                {
                    outs[ index ] = '/';//'\\"
                    index++;
                    outs[ index ] = ins[ i ];
                    index++;
                }
                else
                {
                    outs[ index ] = ins[ i ];
                    index++;
                }
            }
            break;
        }

        if ( index > outl )
        {
            return 0;
        }
    }

    outs[ index ] = 0;
    //printf("++++++++++++++++++Index %d~~~~~~~~~~~~~\n", index);
    return ( index );
}


//=============================================================================
// NAME: strtobin
// DESC:
//=============================================================================
size32_t strtobin( _OUT ubyte_t* outs, _IN size32_t outl, _IN const ubyte_t* ins, _IN size32_t inl )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return 0;
    }

    //if(outl < inl)
    //return -1;
    size32_t size = 0;
    size32_t index = 0;

    //__pfmemcpy(res, buf, buflen);
    for ( unsigned int i = 0; i < inl; i++ )
    {
        switch ( ins[ i ] )
        {
        case '\'':
        case '\\':
        case '\"':
            index++;
            break;

        default:
            outs[ index++ ] = ins[ i ];
            size++;
            //res;
            break;
        }

        //if(index >= outl)
        //return -1;
    }

    //buf[index++] = '\'';
    //buf[index++] = ')';
    //buf[index++] = 0;
    return size;
}

//=============================================================================
// NAME: wtoi
// DESC:
//=============================================================================
int wtoi( _IN const wchar_t* str )
{
    if( str == NULL )
    {
        return 0;
    }

#ifdef _WINDOWS
    return _wtoi( str );
#else
    return (int)wcstol( str, 0, 10 );
#endif


    register int result = 0;
    register unsigned int digit;
    int sign;

    /* * Skip any leading blanks.  */

    while ( isspace( *str ) )
    {
        str += 1;
    }

    /* Check for a sign. */
    if ( *str == '-' )
    {
        sign = 1;
        str += 1;
    }
    else
    {
        sign = 0;
        if ( *str == '+' )
        {
            str += 1;
        }
    }

    for ( ; ; str += 1 )
    {
        digit = *str - '0';
        if ( digit > 9 )
        {
            break;
        }
        result = ( 10 * result ) + digit;
    }

    if ( sign )
    {
        return -result;
    }
    return result;

}

//=============================================================================
// NAME: wtof
// DESC:
//=============================================================================
//float wtof( _IN const wchar_t* str )
//{
//    float rval = 0.0f;
//    return rval;
//}

//=============================================================================
// NAME: _lltoa
// DESC:
//=============================================================================
char* lltom( _IN long64_t _val, _OUT char* _buf, _IN int _radix )
{
    if( _buf == NULL )
    {
        return NULL;
    }

#ifdef _WINDOWS
    return _i64toa( _val, _buf, _radix );
#else
    sprintf( _buf, "%lld", _val );
    return _buf;
#endif
    return NULL;
}

//=============================================================================
// NAME: _lltoa
// DESC:
//=============================================================================
wchar_t* lltow( _IN long64_t _val, _OUT wchar_t* _buf, _IN int _radix )
{
    if( _buf == NULL )
    {
        return NULL;
    }

#ifdef _WINDOWS
    return _i64tow( _val, _buf, _radix );
#else
    swprintf( _buf, 16, L"%lld", _val );
    return _buf;
#endif
    return NULL;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
uint_t mtoui( _IN const char* _ins )
{
    if( _ins == NULL )
    {
        return 0;
    }

    char* stopstring;
    return (uint_t)strtoul( _ins, &stopstring, 10 );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
uint_t wtoui( _IN const wchar_t* _ins )
{
    if( _ins == NULL )
    {
        return 0;
    }

    wchar_t* stopstring;
    return (uint_t)wcstoul( _ins, &stopstring, 10 );
}

//=============================================================================
// NAME: _atoll
// DESC:
//=============================================================================
long64_t mtoll( _IN const char* _ins )
{
    if( _ins == NULL )
    {
        return 0;
    }

#ifdef _WINDOWS
    return _atoi64( _ins );
#else
    char* stopstring;
    return strtoll( _ins, &stopstring, 10 );
#endif
    return 0;
}

//=============================================================================
// NAME: _lltoa
// DESC:
//=============================================================================
long64_t wtoll( _IN const wchar_t* _ins )
{
    if( _ins == NULL )
    {
        return 0;
    }

#ifdef _WINDOWS
    return _wtoi64( _ins );
#else
    wchar_t* stopstring;
    return wcstoll( _ins, &stopstring, 10 );
#endif
    return 0;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
ulong64_t mtoull( _IN const char* _ins )
{
    if( _ins == NULL )
    {
        return 0;
    }

    char* stopstring;
    return strtoull( _ins, &stopstring, 10 );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
ulong64_t wtoull( _IN const wchar_t* _ins )
{
    if( _ins == NULL )
    {
        return 0;
    }

    wchar_t* stopstring;
    return wcstoull( _ins, &stopstring, 10 );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
double mtof( _IN const char* str )
{
    if( str == NULL )
    {
        return 0;
    }

    return atof( str );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
double wtof( _IN const wchar_t* str )
{
    if( str == NULL )
    {
        return 0;
    }

#ifdef _WINDOWS
    return _wtof( str );
#else
    wchar_t * pEnd = NULL;
    return wcstod( str, &pEnd );
#endif

}

//=============================================================================
// NAME: ftoa
// DESC:
//=============================================================================
const double eps = 1e-12;
char reverser_s[ 30 ];

void reverse( _IN char* s )
{
    if( s == NULL )
    {
        return;
    }

    size_t i, j;
    size_t len = strlen( reverser_s );

    for ( i = 0, j = len - 1; i < j; i++, j-- )
    {
        reverser_s[ i ] ^= reverser_s[ j ] ^= reverser_s[ i ] ^= reverser_s[ j ];
    }
}

/* ftoa:   convert double n to string s with digits*/
char* ftoa( _IN double n, _OUT char* s, int _IN digits )
{
    if( s == NULL )
    {
        return NULL;
    }

    memset( s, 0, sizeof( s ) );
    int i = 0, p;
    bool sign = true;

    if ( n < 0 )
    {
        sign = false;
        n = -n;
    }

    long32_t t = (long32_t)n;
    n = n - t;

    if ( t == 0 )
    {
        s[ i++ ] = '0';
    }
    else
    {
        while ( t != 0 )
        {
            p = t % 10;
            t /= 10;
            s[ i++ ] = p + '0';
        }

        if ( !sign )
        {
            s[ i++ ] = '-';
        }

        reverse( s );
    }

    if ( fabs( n - 0.0 ) > eps )
    {
        s[ i++ ] = '.';
        int cur_d = 0;

        while ( cur_d < digits )
        {
            cur_d++;
            n *= 10.0;
            p = (long32_t)n;
            n = n - p;
            s[ i++ ] = p + '0';
        }
    }

    s[ i ] = '\0';
    return s;
}


//=============================================================================
// NAME: atox
// DESC:
//=============================================================================
int atox( _IN const char* s )
{
    if( s == NULL )
    {
        return 0;
    }

    size_t xlen = strlen( s ) - 1;

    if ( ( s[ 0 ] == '0' ) &&
        ( ( s[ 1 ] == 'X' ) || ( s[ 1 ] == 'x' ) ) )
    {
        s += 2;
        xlen -= 2;
    }

    int num = 0;
    int x = 1;

    for ( size_t i = xlen; i >= 0; i-- )
    {
        num += ( s[ i ] - 48 ) * ( x );
        x = x * 16;
    }

    return num;
}

//=============================================================================
// NAME: mtow
// DESC:
//=============================================================================
int mtow( _IN const char* insA, _OUT wchar_t* strW, _IN int lenW, _IN int codepage )
{
    if( ( insA == NULL ) || ( strW == NULL ) )
    {
        return -1;
    }

#ifdef _WINDOWS
    size_t l = strlen( insA );

    if ( lenW < ( ( l ) * 2 ) )
    {
        return -1;
    }

    memset( strW, 0, lenW );
    // output is assumed to be as twice as ansistr in length
    return MultiByteToWideChar( codepage, 0, insA, -1, strW, lenW );
#else
    /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
     *           IGNORE ：遇到无法转换字符跳过*/
    const char *encTo = "UNICODE//IGNORE";
    /* 源编码 */
    const char *encFrom = "UTF-8";

    /* 获得转换句柄
     *@param encTo 目标编码方式
     *@param encFrom 源编码方式
     *
     * */
    iconv_t cd = iconv_open( encTo, GetCodePageName( codepage ) );
    if ( cd == (iconv_t)-1 )
    {
        perror( "iconv_open" );
    }

    /* 由于iconv()函数会修改指针，所以要保存源指针 */
    char* tmpin = (char*)insA;
    char* tmpout = (char*)strW;
    size_t insize = strlen( insA );
    size_t outsize = lenW;

    /* 进行转换
     *@param cd iconv_open()产生的句柄
     *@param srcstart 需要转换的字符串
     *@param inlen 存放还有多少字符没有转换
     *@param tempoutbuf 存放转换后的字符串
     *@param outlen 存放转换后,tempoutbuf剩余的空间
     *
     * */
    size_t ret = iconv( cd, &tmpin, &insize, &tmpout, &outsize );
    if ( ret == -1 )
    {
        perror( "iconv" );
    }

    /* 关闭句柄 */
    iconv_close( cd );

    return 0;
#endif
}


//=============================================================================
// NAME: wtom
// DESC:
//=============================================================================
int wtom( _IN const wchar_t* insW, _OUT char* strA, _IN int lenA, _IN int codepage )
{
    if( ( insW == NULL ) || ( strA == NULL ) )
    {
        return -1;
    }

#ifdef _WINDOWS
    //BOOL usedefault;
    size_t l = wcslen( insW );

    if ( l > lenA - 1 )
    {
        return -1;
    }

    memset( strA, 0, lenA );
    return WideCharToMultiByte( codepage, 0, insW, -1, strA, lenA, NULL, NULL );
#else
    /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
     *           IGNORE ：遇到无法转换字符跳过*/
     //char *encTo = "UTF-8//IGNORE";
    char encTo[ 64 ];
    /* 源编码 */
    const char *encFrom = "UNICODE";
    sprintf( encTo, "%s//IGNORE", GetCodePageName( codepage ) );

    /* 获得转换句柄
     *@param encTo 目标编码方式
     *@param encFrom 源编码方式
     *
     * */
    iconv_t cd = iconv_open( encTo, encFrom );
    if ( cd == (iconv_t)-1 )
    {
        perror( "iconv_open" );
    }

    /* 由于iconv()函数会修改指针，所以要保存源指针 */
    char* tmpin = (char*)insW;
    char* tmpout = (char*)strA;
    size_t insize = wcslen( insW ) * sizeof( wchar_t );
    size_t outsize = lenA;

    /* 进行转换
     *@param cd iconv_open()产生的句柄
     *@param srcstart 需要转换的字符串
     *@param inlen 存放还有多少字符没有转换
     *@param tempoutbuf 存放转换后的字符串
     *@param outlen 存放转换后,tempoutbuf剩余的空间
     *
     * */
    size_t ret = iconv( cd, &tmpin, &insize, &tmpout, &outsize );
    if ( ret == -1 )
    {
        perror( "iconv" );
    }

    /* 关闭句柄 */
    iconv_close( cd );

    return 0;
#endif
}

//=============================================================================
// NAME: utf8tobig5
// DESC:
//=============================================================================
int utf8tobig5( _IN const char* strutf8, _OUT char_t* strbig5, _IN int lenbig5 )
{
    if( ( strutf8 == NULL ) || ( strbig5 == NULL ) )
    {
        return -1;
    }

    int ret = 0;
#ifdef _WINDOWS
    int wszsize = ( lenbig5 * 2 ) + 1;
    WCHAR* wszUni = new WCHAR[ wszsize ]; //Unicode
    MultiByteToWideChar( CP_UTF8, 0, strutf8, -1, wszUni, wszsize );   //UTF-8轉Unicode
    ret = WideCharToMultiByte( 950, 0, wszUni, -1, strbig5, lenbig5, NULL, NULL ); //Unicode轉Big5
    delete[] wszUni;
#endif
    return ret;
}

//=============================================================================
// NAME: big5toutf8
// DESC:
//=============================================================================
int big5toutf8( _IN const char* strbig5, _OUT char_t* strutf8, _IN int lenurf8 )
{
    if( ( strbig5 == NULL ) || ( strutf8 == NULL ) )
    {
        return -1;
    }

    int ret = 0;
#ifdef _WINDOWS
    // big5 -> unicode
    int wszsize = ( lenurf8 * 2 ) + 1;
    WCHAR* wszUni = new WCHAR[ wszsize ];
    MultiByteToWideChar( CP_ACP, 0, strbig5, -1, wszUni, wszsize );
    // unicode -> utf_8
    ret = WideCharToMultiByte( CP_UTF8, 0, wszUni, -1, strutf8, lenurf8, NULL, NULL );
    delete[] wszUni;
#endif
    return ret;
}

//=============================================================================
// NAME: UTF8ToShiftJis
// DESC:
//=============================================================================
void utf8Toshiftjis( _IN const char* strutf8, _OUT char_t* strsjis, _IN int lenbig5 )
{
    if( ( strutf8 == NULL ) || ( strsjis == NULL ) )
    {
        return;
    }

#ifdef _WINDOWS
    wchar_t bufUnicode[ _MAX_LPATH ];
    int iLenUnicode = MultiByteToWideChar( CP_UTF8, 0, strutf8, (int)strlen( strutf8 ) + 1, NULL, 0 );

    if ( iLenUnicode <= sizeof( bufUnicode ) / sizeof( bufUnicode[ 0 ] ) )
    {
        MultiByteToWideChar( CP_UTF8, 0, strutf8, strlen( strutf8 ) + 1, bufUnicode, _MAX_LPATH );
        int iLenUtf8 = WideCharToMultiByte( CP_ACP, 0, bufUnicode, iLenUnicode, NULL, 0, NULL, NULL );

        if ( iLenUtf8 <= sizeof( strutf8 ) )
        {
            WideCharToMultiByte( CP_ACP, 0, bufUnicode, iLenUnicode, strsjis, sizeof( strsjis ), NULL, NULL );
        }
    }
#endif
}


//=============================================================================
// NAME: UTF8ToShiftJis
// DESC:
//=============================================================================
void shiftjisooutf8( _IN const char* strsjis, _OUT char_t* strutf8, _IN int lenurf8 )
{
    if( ( strsjis == NULL ) || ( strutf8 == NULL ) )
    {
        return;
    }

#ifdef _WINDOWS
    wchar_t bufUnicode[ _MAX_LPATH ];
    int iLenUnicode = MultiByteToWideChar( CP_ACP, 0, strsjis, (int)strlen( strsjis ) + 1, NULL, 0 );

    if ( iLenUnicode <= sizeof( bufUnicode ) / sizeof( bufUnicode[ 0 ] ) )
    {
        MultiByteToWideChar( CP_ACP, 0, strsjis, (int)strlen( strsjis ) + 1, bufUnicode, _MAX_LPATH );
        int iLenUtf8 = WideCharToMultiByte( CP_UTF8, 0, bufUnicode, iLenUnicode, NULL, 0, NULL, NULL );

        if ( iLenUtf8 <= sizeof( strutf8 ) )
        {
            WideCharToMultiByte( CP_UTF8, 0, bufUnicode, iLenUnicode, strutf8, sizeof( strutf8 ), NULL, NULL );
        }
    }

#else
    assert( 0 && "shiftjisooutf8" );
#endif
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int utf8tocode( _IN const char* strutf8, _OUT char_t* str, _IN int len, _IN int codepage )
{
    if( ( strutf8 == NULL ) || ( str == NULL ) )
    {
        return -1;
    }

    int ret = 0;
#ifdef _WINDOWS
    int wszsize = ( len * 2 ) + 1;
    WCHAR* wszUni = new WCHAR[ wszsize ]; //Unicode蝺函Ⅳ??蝚?
    MultiByteToWideChar( CP_UTF8, 0, strutf8, -1, wszUni, wszsize ); //UTF-8頧nicode
    //?身頛詨?TF_8?賣3?yte(?喟Ⅱ摰銝剜?)
    ret = WideCharToMultiByte( codepage, 0, wszUni, -1, str, len, NULL, NULL ); //Unicode頧ig5
    delete[] wszUni;
#endif
    return ret;
}

//=============================================================================
// NAME: wtos
// DESC: translate WCHAR to string
//=============================================================================
char* wtos( _IN const wchar_t* strW, _OUT char* buff, _IN int bufflen )
{
    if( ( strW == NULL ) || ( buff == NULL ) )
    {
        return NULL;
    }

    bintostr( (ubyte_t*)buff, bufflen, (const ubyte_t*)strW, (size32_t)( wcslen( strW ) * sizeof( wchar_t ) ) );
    return buff;
}

//=============================================================================
// NAME: wtos
// DESC: translate WCHAR to string
//=============================================================================
wchar_t* stow( _IN const char* ins, _OUT wchar_t* wbuff, _IN int wbuflen )
{
    if( ( ins == NULL ) || ( wbuff == NULL ) )
    {
        return NULL;
    }

    strtobin( (ubyte_t*)wbuff, wbuflen, (const ubyte_t*)ins, (size32_t)strlen( ins ) );
    return wbuff;
}


//=============================================================================
// NAME: strtoi
// DESC:
//=============================================================================
int strtoi( _IN const char* ins, _OUT int* n, _IN int base, _IN int __min, _IN int max )
{
    if( ins == NULL )
    {
        return -1;
    }

    char*  endp;
    long32_t  i = (long32_t)strtol( ins, &endp, base );

    if ( ins == endp )
    {
        return -1;
    }

    //if (errno == ERANGE)
    // return -2;
    if ( __min != max && ( i < __min || i > max ) )
    {
        return -3;
    }

    *n = i;
    return 0;
}


//=============================================================================
// NAME: strtol
// DESC:
//=============================================================================
int strtol( _IN const char* ins, _OUT long32_t* n, _IN int base, _IN long32_t __min, _IN long32_t max )
{
    if( ins == NULL )
    {
        return -1;
    }

    char*  endp;
    long32_t  l = (long32_t)strtol( ins, &endp, base );

    if ( ins == endp )
    {
        return -1;
    }

    //if (errno == ERANGE)
    // return -2;
    if ( __min != max && ( l < __min || l > max ) )
    {
        return -3;
    }

    *n = l;
    return 0;
}


//=============================================================================
// NAME: strtoul
// DESC:
//=============================================================================
int strtoul( _IN const char* ins, _OUT ulong32_t* n, _IN int base, _IN ulong32_t __min, _IN ulong32_t max )
{
    if( ins == NULL )
    {
        return -1;
    }

    char*  endp;
    ulong32_t ul = (ulong32_t)strtol( ins, &endp, base );

    if ( ins == endp )
    {
        return -1;
    }

    //if (errno == ERANGE)
    //return -2;
    if ( __min != max && ( ul < __min || ul > max ) )
    {
        return -3;
    }

    *n = ul;
    return 0;
}

#if !defined(_WINDOWS)

//=============================================================================
// NAME: itoa
// DESC:
//   gunc don't have this function
//=============================================================================
char* itoa( _IN int value, _OUT char* result, _IN int base )
{
    if( result == NULL )
    {
        return NULL;
    }

    // check that the base if valid
    if ( base < 2 || base > 36 )
    {
        *result = '\0';
        return result;
    }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[ 35 + ( tmp_value - value * base ) ];
    } while ( value );

    // Apply negative sign
    if ( tmp_value < 0 )
    {
        *ptr++ = '-';
    }

    *ptr-- = '\0';

    while ( ptr1 < ptr )
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }

    return result;
}

unsigned int strlenW( _IN const wchar_t* str )
{
    if( str == NULL )
    {
        return 0;
    }

    const wchar_t* s = str;

    while ( *s )
    {
        s++;
    }

    return (unsigned int)( s - str );
}

wchar_t* strcpyW( _OUT wchar_t* dst, _IN const wchar_t* src )
{
    if( ( dst == NULL ) || ( src == NULL ) )
    {
        return NULL;
    }

    wchar_t* p = dst;

    while ( ( *p++ = *src++ ) );

    return dst;
}

int strcmpW( _IN const wchar_t* str1, _IN const wchar_t* str2 )
{
    if( ( str1 == NULL ) || ( str2 == NULL ) )
    {
        return 0;
    }

    while ( *str1 && ( *str1 == *str2 ) )
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

int strncmpW( _IN const wchar_t* str1, _IN const wchar_t* str2, _IN int n )
{
    if( ( str1 == NULL ) || ( str2 == NULL ) )
    {
        return 0;
    }

    if ( n <= 0 )
    {
        return 0;
    }

    while ( ( --n > 0 ) && *str1 && ( *str1 == *str2 ) )
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

wchar_t* strcatW( _INOUT wchar_t* dst, _IN const wchar_t* src )
{
    if( ( dst == NULL ) || ( src == NULL ) )
    {
        return NULL;
    }

    strcpyW( dst + strlenW( dst ), src );
    return dst;
}

wchar_t* strchrW( _IN const wchar_t* str, _IN wchar_t ch )
{
    if( str == NULL )
    {
        return NULL;
    }

    do
    {
        if ( *str == ch )
        {
            return (wchar_t*)(unsigned int*)str;
        }
    } while ( *str++ );

    return NULL;
}

#endif

//////////////////////////////////////////////////////////////////////////
//
size32_t unclen( _IN const wchar_t* szUnicode )
{
    if( szUnicode == NULL )
    {
        return 0;
    }

    const wchar_t* eos = szUnicode;

    while ( *eos++ );

    return ( (size32_t)( eos - szUnicode - 1 ) );
    /*
    int i = 0;
    while(szUnicode[i] != 0)
    {
     i++;
    }
    return i;
    */
}

wchar_t* unccpy( _OUT wchar_t* strDestination, _IN const wchar_t* strSource )
{
    if( ( strDestination == NULL ) || ( strSource == NULL ) )
    {
        return NULL;
    }

    wchar_t* cp = strDestination;

    while ( ( ( *( cp++ ) ) = ( *( strSource++ ) ) ) ); /* Copy src over dst */

    return ( strDestination );
}

wchar_t* unccat( _INOUT wchar_t* strDestination, _IN const wchar_t* strSource )
{
    if( ( strDestination == NULL ) || ( strSource == NULL ) )
    {
        return NULL;
    }

    wchar_t* cp = strDestination;

    while ( *cp )
    {
        cp++;
    }       /* find end of dst */

    while ( ( ( *( cp++ ) ) = ( *( strSource++ ) ) ) ); /* Copy src to end of dst */

    return ( strDestination );     /* return dst */
    /*
    wchar_t* pDest = strDestination;

    while((*pDest) == 0)
    {
     pDest++;
    }

    int ulen = uncLen(strSource);
    memcpy(pDest, strSource, ulen * sizeof(wchar_t));
    pDest[ulen + 1] = 0;
    return strDestination;
    */
}

int unccmp( _IN const wchar_t* string1, _IN const wchar_t* string2 )
{
    if( ( string1 == NULL ) || ( string2 == NULL ) )
    {
        return -1;
    }

    int ret = 0;

    while ( !( ret = (int)( *string1 - *string2 ) ) && *string2 )
    {
        ++string1, ++string2;
    }

    if ( ret < 0 )
    {
        ret = -1;
    }
    else if ( ret > 0 )
    {
        ret = 1;
    }

    return ( ret );
    /*
    const wchar_t* pDest1 = string1;
    const wchar_t* pDest2 = string1;

    while(((*pDest1) == (*pDest2)) ||
     ((*pDest1) == 0) ||
     ((*pDest2) == 0))
    {
     pDest1++;
     pDest2++;
    }

    return (*pDest1) - (*pDest2);
    */
}

wchar_t* uncstr( _IN const wchar_t* str, _IN const wchar_t* str_search )
{
    if( ( str == NULL ) || ( str_search == NULL ) )
    {
        return NULL;
    }

    wchar_t* cp = (wchar_t*)str;
    wchar_t* s1, *s2;

    if ( !*str_search )
    {
        return ( (wchar_t*)str );
    }

    while ( *cp )
    {
        s1 = cp;
        s2 = (wchar_t*)str_search;

        while ( *s1 && *s2 && !( *s1 - *s2 ) )
        {
            s1++, s2++;
        }

        if ( !*s2 )
        {
            return ( cp );
        }

        cp++;
    }

    return ( NULL );
    /*
    const wchar_t* pDest = str;
    int     searchlen = uncLen(str_search) * sizeof(wchar_t);

    while((*pDest) == 0)
    {
     if(memcmp(pDest, str_search, searchlen) == 0)
      return pDest;
     pDest++;
    }
    return NULL;
    */
}

wchar_t* uncchr( _IN  const wchar_t* str, _IN wchar_t c )
{
    if( str == NULL )
    {
        return NULL;
    }

    const wchar_t* s = str;

    do
    {
        if ( *s == c )
        {
            return ( (wchar_t*)s );
        }
    } while ( *s++ != '\0' );

    return NULL;
}

wchar_t* unctok( _IN wchar_t* strToken, _IN const wchar_t* strDelimit )
{
    if( ( strToken == NULL ) || ( strDelimit == NULL ) )
    {
        return NULL;
    }

    register wchar_t* spanp;
    register int c, sc;
    wchar_t* tok;
    static wchar_t* last;

    if ( strToken == NULL && ( strToken = last ) == NULL )
    {
        return ( NULL );
    }

    /*
     * Skip (span) leading delimiters (strToken += strspn(strToken, strDelimit), sort of).
     */
cont:
    c = *strToken++;

    for ( spanp = (wchar_t*)strDelimit; ( sc = *spanp++ ) != 0; )
    {
        if ( c == sc )
        {
            goto cont;
        }
    }

    if ( c == 0 )
    {
        /* no non-delimiter characters */
        last = NULL;
        return ( NULL );
    }

    tok = strToken - 1;

    /*
     * Scan token (scan for delimiters: strToken += strcspn(strToken, strDelimit), sort of).
     * Note that strDelimit must have one NUL; we stop if we see that, too.
     */
    for ( ;; )
    {
        c = *strToken++;
        spanp = (wchar_t*)strDelimit;

        do
        {
            if ( ( sc = *spanp++ ) == c )
            {
                if ( c == 0 )
                {
                    strToken = NULL;
                }
                else
                {
                    strToken[ -1 ] = 0;
                }

                last = strToken;
                return ( tok );
            }
        } while ( sc != 0 );
    }

    /* NOTREACHED */
    return ( NULL );
}


int wcsdumpallstr( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t* dumps )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return 0;
    }

    int buflen = outl;
    wchar_t* bufptr = new wchar_t[ buflen ];
    wcscpy( bufptr, ins );
    wchar_t* inbuf = bufptr;
    wchar_t* outbuf = outs;
    int c = 0;

    while ( wcsdumpstr( outbuf, buflen, inbuf, dumps ) == 1 )
    {
        SWAPWS( outbuf, inbuf );
        c++;
    }

    if ( outs != outbuf )
    {
        wcscpy( outs, outbuf );
    }

    return c;
}

//=============================================================================
// NAME: uncreplace
// DESC:
//=============================================================================
int wcsreplace( _OUT wchar_t* outs, _IN wchar_t outl, _IN const wchar_t* ins, _IN const wchar_t* finds, _IN const wchar_t* insteads, _IN bool isall )
{
    if( ( outs == NULL ) || ( ins == NULL ) || ( finds == NULL ) || ( insteads == NULL ) )
    {
        return 0;
    }

    if ( outl <= 0 )
    {
        return 0;
    }

    outs[ 0 ] = 0;

    if ( wcscmp( finds, L"" ) == 0 )
    {
        return 0;
    }

    wchar_t* ret = NULL, *r = NULL;
    const wchar_t* p, *q;
    size32_t oldlen = (size32_t)wcslen( finds );
    size32_t newlen = (size32_t)wcslen( insteads );
    int      count = 0;
    size32_t retlen = 0;
    size32_t srclen = (size32_t)wcslen( ins );

    if ( !isall )
    {
        for ( count = 0, p = ins; ( q = wcsstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            count++;
            break;
        }
    }
    else
    {
        for ( count = 0, p = ins; ( q = wcsstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            count++;
        }
    }

    if ( wcscmp( finds, insteads ) == 0 )
    {
        wcscpy( outs, ins );
        return count;
    }

    if ( count > 0 )
    {
        if ( oldlen != newlen )
        {
            /* this is undefined if p - str > PTRDIFF_MAX */
            retlen = ( (size32_t)wcslen( p ) ) + (size32_t)( p - ins ) + (size32_t)( count * ( newlen - oldlen ) );
        }
        else
        {
            retlen = ( (size32_t)wcslen( ins ) );
        }

        ret = new wchar_t[ retlen + 32 ];

        for ( r = ret, p = ins; ( q = wcsstr( p, finds ) ) != NULL; p = q + oldlen )
        {
            /* this is undefined if q - p > PTRDIFF_MAX */
#if defined(_WINDOWS) || defined(_XBOX)
            ptrdiff_t l = q - p;
#else
            long64_t l = q - p;
#endif
            __pfmemcpy( r, p, l * sizeof( wchar_t ) );
            r += l;
            __pfmemcpy( r, insteads, newlen * sizeof( wchar_t ) );
            r += newlen;

            if ( !isall )
            {
                p = q + oldlen;
                break;
            }
        }

        wcscpy( r, p );
        int minlen = __min( (int)retlen, (int)( outl - 1 ) );
        __pfmemcpy( outs, ret, minlen * sizeof( wchar_t ) );
        outs[ minlen ] = 0;
        delete[] ret;
    }
    else
    {
        int minlen = __min( (int)srclen, (int)( outl - 1 ) );
        __pfmemcpy( outs, ins, minlen * sizeof( wchar_t ) );
        outs[ minlen ] = 0;
    }

    return count;
}


int iswcsdig( _IN const wchar_t* ins )
{
    if( ins == NULL )
    {
        return 0;
    }

    size32_t len = (size32_t)wcslen( ins );

    for ( size32_t i = 0; i < len; i++ )
    {
        if ( ( (int)ins[ i ] < 48 ) || ( (int)ins[ i ] > 57 ) )
        {
            return 0;
        }
    }
    return 1;
}

//////////////////////////////////////////////////////////////////////////
//

//=============================================================================
// NAME: memcmpcnt
// DESC:
//=============================================================================
int memcmpcount( _IN const char* ins1, _IN size32_t inl1, _IN const char* ins2, _IN size32_t inl2 )
{
    if( ( ins1 == NULL ) || ( ins2 == NULL )  )
    {
        return 0;
    }

    int cnt = 0;
    size32_t ll = __min( inl1, inl2 );

    for ( unsigned int i = 0; i < ll; i++ )
    {
        if ( ins1[ i ] == ins2[ i ] )
        {
            cnt++;
        }
    }

    return cnt;
}

int memfuzzycmpcount( _IN const char* ins1, _IN size32_t inl1, _IN const char* ins2, _IN size32_t inl2, uint8_t range )
{
    if( ( ins1 == NULL ) || ( ins2 == NULL ) )
    {
        return 0;
    }

    //int maxrng=0;
    int cnt = 0;
    size32_t ll = __min( inl1, inl2 );

    for ( unsigned int i = 0; i < ll; i++ )
    {
        int mut = abs( ins1[ i ] - ins2[ i ] );

        if ( mut <= range )
        {
            //if(mut>maxrng)
            // maxrng = mut;
            cnt++;
        }
    }

    return cnt;
}

#if defined(_LINUX)
wchar_t* wcstok( wchar_t *wcs, const wchar_t *delim )
{
    if( ( wcs == NULL ) || ( delim == NULL ) )
    {
        return NULL;
    }

    wchar_t* state;
    //return unctok( wcs, delim );
    return wcstok( wcs, delim, &state );
}

#endif

//#endif /* _WINDOWS */


//////////////////////////////////////////////////////////////////////////
// Path

//=============================================================================
// NAME: pathrdiv
// DESC: divide path string like
//      ex  C://a//b//c//d
//        hierarchy = 0  = > destpath = d
//        hierarchy = 1  = > destpath = c
//        hierarchy = 2  = > destpath = b
//=============================================================================
int pathrdiv( char* outs, int outl, const char* ins, int hierarchy )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return -1;
    }

    hierarchy++;
    int count = 0;
    size32_t len = (size32_t)strlen( ins );
    size32_t end = len;
    size32_t i = len;

    for ( int i = len; i > 0; i-- )
    {
        if ( ispathdiv( ins[ i ] ) )
        {
            count++;

            if ( count == hierarchy )
            {
                size32_t copylen = (size32_t)__min( (size32_t)( (end)-i + 1 ), (size32_t)outl );
                __pfmemcpy( outs, ins + i + 1, copylen );
                outs[ copylen - 1 ] = '\0';
                break;
            }

            end = i;
        }
    }

    return ( end - 1 ) - i;
}


//=============================================================================
// NAME: pathrdivf
// DESC: divide path string like
//      ex  C://a//b//c//d
//        hierarchy = 0  = > destpath = d
//        hierarchy = 1  = > destpath = c//d
//        hierarchy = 2  = > destpath = b//c//d
//=============================================================================
int pathrdivf( char* outs, int outl, const char* ins, int hierarchy )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return -1;
    }

    hierarchy++;
    int count = 0;
    int len = (int)strlen( ins );
    int i = len;

    for ( i = len; i > 0; i-- )
    {
        if ( ispathdiv( ins[ i ] ) )
        {
            count++;

            if ( count == hierarchy )
            {
                int copylen = __min( (len)-i + 1, outl );
                __pfmemcpy( outs, ins + i + 1, copylen );
                outs[ copylen - 1 ] = '\0';
                break;
            }
        }
    }

    return len - i - 1;
}

//=============================================================================
// NAME: ispathdiv
// DESC:
//=============================================================================
int ispathdiv( const char  c )
{
    if ( ( c == '\\' ) || ( c == '/' ) )
    {
        return 1;
    }

    return 0;
}

//=============================================================================
// NAME: pathrdump
// DESC:
//=============================================================================
int pathrdump( char* outs, int outl, const char* ins, int n )
{
    if( ( outs == NULL ) || ( ins == NULL ) )
    {
        return -1;
    }

    outs[ 0 ] = 0;
    int end = 0;

    for ( size32_t i = ( (size32_t)strlen( ins ) ) - 1; i >= 0; i-- )
    {
        if ( ( ins[ i ] == '\\' ) || ( ins[ i ] == '/' ) )
        {
            end = i - 1;
        }
        else
        {
            break;
        }
    }

    if ( end <= 0 )
    {
        return -1;
    }

    int rc = 0;
    int e = end;

    for ( int i = e; i >= 0; i-- )
    {
        if ( ( ins[ i ] == '\\' ) || ( ins[ i ] == '/' ) )
        {
            rc++;

            if ( rc >= n )
            {
                end = i - 1;
                break;
            }
        }
    }

    if ( end <= 0 )
    {
        return -1;
    }

    int ol = __min( end, outl );
    __pfmemcpy( outs, ins, ol );
    outs[ ol - 1 ] = 0;
    return 1;
}

//=============================================================================
// NAME: mmakefpath
// DESC:
//=============================================================================
void mmakefpath( char* path, const char* drive, const char* dir, const char* fname, const char* ext )
{
    if ( ( path == NULL ) || ( drive == NULL ) || ( dir == NULL ) || ( fname == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    char mext[ _MAX_LEXT ];
    sprintf( mext, "%s", ext );

    if ( strcmp( ext, "" ) != 0 )
    {
        if ( ext[ 0 ] != '.' )
        {
            sprintf( mext, ".%s", ext );
        }
    }

    sprintf( path, "%s%s%s%s", drive, dir, fname, mext );
    return;
}

void msplitfpath( const char* path, char* drive, char* dir, char* fname, char* ext )
{
    if ( ( path == NULL ) || ( drive == NULL ) || ( dir == NULL ) || ( fname == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    register signed char* p;
    signed char* last_slash = NULL, *dot = NULL;
    size_t len;

    /* we assume that the path argument has the following form, where any
     * or all of the components may be missing.
     *
     *  <drive><dir><fname><ext>
     *
     * and each of the components has the following expected form(s)
     *
     *  drive:
     *  0 to _MAX_DRIVE-1 characters, the last of which, if any, is a
     *  ':'
     *  dir:
     *  0 to _MAX_LDIR-1 characters in the form of an absolute path
     *  (leading '/' or '\') or relative path, the last of which, if
     *  any, must be a '/' or '\'.  E.g -
     *  absolute path:
     *   \top\next\last\  ; or
     *   /top/next/last/
     *  relative path:
     *   top\next\last\  ; or
     *   top/next/last/
     *  Mixed use of '/' and '\' within a path is also tolerated
     *  fname:
     *  0 to _MAX_LFNAME-1 characters not including the '.' character
     *  ext:
     *  0 to _MAX_LEXT-1 characters where, if any, the first must be a
     *  '.'
     *
     */
     /* extract drive letter and :, if any */
    if ( ( strlen( path ) >= ( _MAX_DRIVE - 2 ) ) && ( *( path + _MAX_DRIVE - 2 ) == ':' ) )
    {
        if ( drive )
        {
            strncpy( drive, path, _MAX_DRIVE - 1 );
            *( drive + _MAX_DRIVE - 1 ) = '\0';
        }

        path += _MAX_DRIVE - 1;
    }
    else if ( drive )
    {
        *drive = '\0';
    }

    /* extract path string, if any.  Path now points to the first character
     * of the path, if any, or the fname or ext, if no path was
     * specified.  Scan ahead for the last occurence, if any, of a '/' or
     * '\' path separator character.  If none is found, there is no path.
     * We will also note the last '.' character found, if any, to aid in
     * handling the ext.
     */
    for ( last_slash = NULL, p = (signed char*)path; *p; p++ )
    {
        if ( *p == '/' || *p == '\\' )
            /* point to one beyond for later copy */
        {
            last_slash = p + 1;
        }
        else if ( *p == '.' )
        {
            dot = p;
        }
    }

    if ( last_slash )
    {
        /* found a path - copy up through last_slash or max. characters
         * allowed, whichever is smaller
         */
        if ( dir )
        {
            len = __min( ( (char*)last_slash - (char*)path ) / sizeof( signed char ), ( _MAX_LDIR - 1 ) );
            strncpy( dir, path, len );
            *( dir + len ) = '\0';
        }

        path = (const char*)last_slash;
    }
    else if ( dir )
    {
        /* no path found */
        *dir = '\0';
    }

    /* extract file name and ext, if any.  Path now points to the
     * first character of the file name, if any, or the ext if no
     * file name was given.  Dot points to the '.' beginning the ext,
     * if any.
     */
    if ( dot && ( dot >= (signed char*)path ) )
    {
        /* found the marker for an ext - copy the file name up to
         * the '.'.
         */
        if ( fname )
        {
            len = __min( ( (char*)dot - (char*)path ) / sizeof( signed char ), ( _MAX_LFNAME - 1 ) );
            strncpy( fname, path, len );
            *( fname + len ) = '\0';
        }

        /* now we can get the ext - remember that p still points
         * to the terminating nul character of path.
         */
        if ( ext )
        {
            len = __min( ( (char*)p - (char*)dot ) / sizeof( signed char ), ( _MAX_LEXT - 1 ) );
            strncpy( ext, (const char*)dot, len );
            *( ext + len ) = '\0';
        }
    }
    else
    {
        /* found no ext, give empty ext and copy rest of
         * string into fname.
         */
        if ( fname )
        {
            len = __min( ( (char*)p - (char*)path ) / sizeof( signed char ), ( _MAX_LFNAME - 1 ) );
            strncpy( fname, path, len );
            *( fname + len ) = '\0';
        }

        if ( ext )
        {
            *ext = '\0';
        }
    }

    return;
}

//=============================================================================
// NAME:msplitfpath
// DESC:
//=============================================================================
void wmakefpath( wchar_t* path, const wchar_t* drive, const wchar_t* dir, const wchar_t* fname, const wchar_t* ext )
{
    if ( ( path == NULL ) || ( drive == NULL ) || ( dir == NULL ) || ( fname == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    wchar_t mext[ _MAX_LEXT ];
    swprintf( mext, _MAX_LEXT, L"%s", ext );

    if ( wcscmp( ext, L"" ) != 0 )
    {
        if ( ext[ 0 ] != L'.' )
        {
            swprintf( mext, _MAX_LEXT, L".%s", ext );
        }
    }

    swprintf( path, _MAX_LEXT, L"%s%s%s%s", drive, dir, fname, mext );
    return;
}

void wsplitfpath( const wchar_t* path, wchar_t* drive, wchar_t* dir, wchar_t* fname, wchar_t* ext )
{
    if ( ( path == NULL ) || ( drive == NULL ) || ( dir == NULL ) || ( fname == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    register wchar_t* p;
    wchar_t* last_slash = NULL, *dot = NULL;
    size_t len;

    /* we assume that the path argument has the following form, where any
     * or all of the components may be missing.
     *
     *  <drive><dir><fname><ext>
     *
     * and each of the components has the following expected form(s)
     *
     *  drive:
     *  0 to _MAX_DRIVE-1 characters, the last of which, if any, is a
     *  ':'
     *  dir:
     *  0 to _MAX_LDIR-1 characters in the form of an absolute path
     *  (leading '/' or '\') or relative path, the last of which, if
     *  any, must be a '/' or '\'.  E.g -
     *  absolute path:
     *   \top\next\last\  ; or
     *   /top/next/last/
     *  relative path:
     *   top\next\last\  ; or
     *   top/next/last/
     *  Mixed use of '/' and '\' within a path is also tolerated
     *  fname:
     *  0 to _MAX_LFNAME-1 characters not including the '.' character
     *  ext:
     *  0 to _MAX_LEXT-1 characters where, if any, the first must be a
     *  '.'
     *
     */
     /* extract drive letter and :, if any */
    if ( ( wcslen( path ) >= ( _MAX_DRIVE - 2 ) ) && ( *( path + _MAX_DRIVE - 2 ) == ':' ) )
    {
        if ( drive )
        {
            wcsncpy( drive, path, _MAX_DRIVE - 1 );
            *( drive + _MAX_DRIVE - 1 ) = '\0';
        }

        path += _MAX_DRIVE - 1;
    }
    else if ( drive )
    {
        *drive = '\0';
    }

    /* extract path string, if any.  Path now points to the first character
     * of the path, if any, or the fname or ext, if no path was
     * specified.  Scan ahead for the last occurence, if any, of a '/' or
     * '\' path separator character.  If none is found, there is no path.
     * We will also note the last '.' character found, if any, to aid in
     * handling the ext.
     */
    for ( last_slash = NULL, p = (wchar_t*)path; *p; p++ )
    {
        if ( *p == '/' || *p == '\\' )
            /* point to one beyond for later copy */
        {
            last_slash = p + 1;
        }
        else if ( *p == '.' )
        {
            dot = p;
        }
    }

    if ( last_slash )
    {
        /* found a path - copy up through last_slash or max. characters
         * allowed, whichever is smaller
         */
        if ( dir )
        {
            len = __min( ( (wchar_t*)last_slash - (wchar_t*)path ), ( _MAX_LDIR - 1 ) );
            wcsncpy( dir, path, len );
            *( dir + len ) = '\0';
        }

        path = (const wchar_t*)last_slash;
    }
    else if ( dir )
    {
        /* no path found */
        *dir = '\0';
    }

    /* extract file name and ext, if any.  Path now points to the
     * first character of the file name, if any, or the ext if no
     * file name was given.  Dot points to the '.' beginning the ext,
     * if any.
     */
    if ( dot && ( dot >= (wchar_t*)path ) )
    {
        /* found the marker for an ext - copy the file name up to
         * the '.'.
         */
        if ( fname )
        {
            len = __min( ( (wchar_t*)dot - (wchar_t*)path ), ( _MAX_LFNAME - 1 ) );
            wcsncpy( fname, path, len );
            *( fname + len ) = '\0';
        }

        /* now we can get the ext - remember that p still points
         * to the terminating nul character of path.
         */
        if ( ext )
        {
            len = __min( ( (wchar_t*)p - (wchar_t*)dot ), ( _MAX_LEXT - 1 ) );
            wcsncpy( ext, (const wchar_t*)dot, len );
            *( ext + len ) = '\0';
        }
    }
    else
    {
        /* found no ext, give empty ext and copy rest of
         * string into fname.
         */
        if ( fname )
        {
            len = __min( ( (wchar_t*)p - (wchar_t*)path ), ( _MAX_LFNAME - 1 ) );
            wcsncpy( fname, path, len );
            *( fname + len ) = '\0';
        }

        if ( ext )
        {
            *ext = '\0';
        }
    }

    return;
}

//=============================================================================
// NAME: mcurrentpath
// DESC:
//=============================================================================
const char* mcurrentpath( char* outs, const char* ins, const char* repchar )
{
    if ( ( outs == NULL ) || ( ins == NULL ) || ( repchar == NULL ) )
    {
        return NULL;
    }

    // .? /? \?
    if ( strlen( ins ) <= 1 )
        return NULL;

    char*  insbuff = new char[ ( strlen( ins ) * 2 ) + 1 ];
    strcpy( insbuff, ins );
    outs[ 0 ] = 0;
    char pathdiv[ 10 ];
#if defined(_WINDOWS)
    strcpy( pathdiv, "\\/" );
#else
    strcpy( pathdiv, "\\/:" );
#endif
    bool  babsolutepath = false;
    char* next_token = NULL;
    char* p = strtok_rc( insbuff, pathdiv, &next_token );
    if ( p != NULL )
    {
        while ( strcmp( p, "." ) == 0 )
        {
            p = strtok_rc( NULL, pathdiv, &next_token );
            if ( p == NULL )
                return NULL;
        }

#if defined(_WINDOWS)

        // if a absolute path
        if ( strstr( p, ":" ) != 0 )
        {
            //  C:/XXX/BBBB
            sprintf( outs, "%s", p );
            babsolutepath = true;
            p = strtok_rc( NULL, pathdiv, &next_token );

            if ( p != NULL )
            {
                while ( strcmp( p, "." ) == 0 )
                {
                    p = strtok_rc( NULL, pathdiv, &next_token );
                }
            }
        }

#else

        if ( p != insbuff )
        {
            char c00 = ( *( p - 1 ) );

            // root
            if ( c00 == '/' )
            {
                //sprintf(outs, "%s", repchar);
                babsolutepath = true;
            }
        }

#endif

        if ( !babsolutepath )
        {
            sprintf( outs, "." );
        }
    }

    while ( p != NULL )
    {
        sprintf( outs, "%s%s%s", outs, repchar, p );
        p = strtok_rc( NULL, pathdiv, &next_token );

        if ( p != NULL )
        {
            while ( strcmp( p, "." ) == 0 )
            {
                p = strtok_rc( NULL, pathdiv, &next_token );
            }
        }
    }
    SAFE_DELETE_ARRAY( insbuff );
    return outs;
}

const wchar_t* wcurrentpath( wchar_t* outs, const wchar_t* ins, const wchar_t* repchar )
{
    if ( ( outs == NULL ) || ( ins == NULL ) || ( repchar == NULL ) )
    {
        return NULL;
    }

    size_t count = ( wcslen( ins ) * 2 ) + 1;
    wchar_t*  insbuff = new wchar_t[ count ];
    wcscpy( insbuff, ins );
    outs[ 0 ] = 0;
    wchar_t pathdiv[ 10 ];
#if defined(_WINDOWS)
    wcscpy( pathdiv, L"\\/" );
#else
    wcscpy( pathdiv, L"\\/:" );
#endif
    bool babsolutepath = false;
    wchar_t* next_token = NULL;
    wchar_t* p = wcstok_rc( insbuff, pathdiv, &next_token );
    if ( p != NULL )
    {
        while ( wcscmp( p, L"." ) == 0 )
        {
            p = wcstok_rc( NULL, pathdiv, &next_token );
            if ( p == NULL )
                return NULL;
        }

#if defined(_WINDOWS)

        // if a absolute path
        if ( wcsstr( p, L":" ) != 0 )
        {
            //  C:/XXX/BBBB
            swprintf( outs, count, L"%s", p );
            babsolutepath = true;
            p = wcstok_rc( NULL, pathdiv, &next_token );;

            if ( p != NULL )
            {
                while ( wcscmp( p, L"." ) == 0 )
                {
                    p = wcstok_rc( NULL, pathdiv, &next_token );;
                }
            }
        }

#else

        if ( p != insbuff )
        {
            wchar_t c00 = ( *( p - 1 ) );

            // root
            if ( c00 == L'/' )
            {
                //sprintf(outs, "%s", repchar);
                babsolutepath = true;
            }
        }

#endif

        if ( !babsolutepath )
        {
            swprintf( outs, count, L"." );
        }
    }

    while ( p != NULL )
    {
        swprintf( outs, count, L"%s%s%s", outs, repchar, p );
        p = wcstok_rc( NULL, pathdiv, &next_token );

        if ( p != NULL )
        {
            while ( wcscmp( p, L"." ) == 0 )
            {
                p = wcstok_rc( NULL, pathdiv, &next_token );
            }
        }
    }

    SAFE_DELETE_ARRAY( insbuff );
    return outs;
}

//=============================================================================
// NAME: getpathdir
// DESC:
//=============================================================================
const char* getpathdir( char* outs, const char* ins )
{
    if ( ( outs == NULL ) || ( ins == NULL ) )
    {
        return NULL;
    }

    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_LDIR ];
    char fname[ _MAX_LFNAME ];
    char ext[ _MAX_LEXT ];
    msplitfpath( ins, drive, dir, fname, ext );
    mmakefpath( outs, drive, dir, "", "" );
    mcurrentpath( outs, outs );
    strcat( outs, "/" );
    return outs;
}


//=============================================================================
// NAME: getpathfnameext
// DESC:
//=============================================================================
const char* getpathfnameext( char* outs, const char* ins )
{
    if ( ( outs == NULL ) || ( ins == NULL ) )
    {
        return NULL;
    }

    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_LDIR ];
    char fname[ _MAX_LFNAME ];
    char ext[ _MAX_LEXT ];
    msplitfpath( ins, drive, dir, fname, ext );
    mmakefpath( outs, "", "", fname, ext );
    //mcurrentpath(outs, outs);
    return outs;
}


//=============================================================================
// NAME: getpathfname
// DESC:
//=============================================================================
const char* getpathfname( char* outs, const char* ins )
{
    if ( ( outs == NULL ) || ( ins == NULL ) )
    {
        return NULL;
    }

    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_LDIR ];
    char fname[ _MAX_LFNAME ];
    char ext[ _MAX_LEXT ];
    msplitfpath( ins, drive, dir, fname, ext );
    mmakefpath( outs, "", "", fname, "" );
    return outs;
}


//=============================================================================
// NAME: getpathext
// DESC:
//=============================================================================
const char* getpathext( char* outs, const char* ins )
{
    if ( ( outs == NULL ) || ( ins == NULL ) )
    {
        return NULL;
    }

    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_LDIR ];
    char fname[ _MAX_LFNAME ];
    char ext[ _MAX_LEXT ];
    msplitfpath( ins, drive, dir, fname, ext );
    mmakefpath( outs, "", "", "", ext );
    return outs;
}


#if !defined(_WINDOWS)

/* implementation of the _splitpath runtime library function with ANSI character strings */
void _splitpath( const char* path, char* drv, char* dir, char* name, char* ext )
{
    if( ( path == NULL ) || ( drv == NULL ) || ( dir == NULL ) || ( name == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    const char* end; /* end of processed string */
    const char* p;   /* search pointer */
    const char* s;   /* copy pointer */

    /* extract drive name */
    if ( path[ 0 ] && path[ 1 ] == ':' )
    {
        if ( drv )
        {
            *drv++ = *path++;
            *drv++ = *path++;
            *drv = '\0';
        }
    }
    else if ( drv )
    {
        *drv = '\0';
    }

    /* search for end of string or stream separator */
    for ( end = path; *end && *end != ':'; )
    {
        end++;
    }

    /* search for begin of file ext */
    for ( p = end; p > path && *--p != '\\' && *p != '/'; )
        if ( *p == '.' )
        {
            end = p;
            break;
        }

    if ( ext )
        for ( s = end; ( *ext = *s++ ); )
        {
            ext++;
        }

    /* search for end of dir name */
    for ( p = end; p > path; )
        if ( *--p == '\\' || *p == '/' )
        {
            p++;
            break;
        }

    if ( name )
    {
        for ( s = p; s < end; )
        {
            *name++ = *s++;
        }

        *name = '\0';
    }

    if ( dir )
    {
        for ( s = path; s < p; )
        {
            *dir++ = *s++;
        }

        *dir = '\0';
    }
}


/*********************************************************************
 *  _makepath (MSVCRT.@)
 *
 * Create a pathname.
 *
 * PARAMS
 *  path   [O] Destination for created pathname
 *  drive  [I] Drive letter (e.g. "A:")
 *  dir [I] dir
 *  fname  [I] Name of the file, excluding ext
 *  ext [I] File ext (e.g. ".TXT")
 *
 * RETURNS
 *  Nothing. If path is not large enough to hold the resulting pathname,
 *  random process memory will be overwritten.
 */


void _makepath( char*  path, const char*  drive, const char* dir, const char*  fname, const char*  ext )
{
    if( ( path == NULL ) || ( drive == NULL ) || ( dir == NULL ) || ( fname == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    char ch;
    char tmpPath[ _MAX_LPATH ];

    //TRACE("got %s %s %s %s\n", debugstr_a(drive), debugstr_a(dir),
    //debugstr_a(fname), debugstr_a(ext));
    if ( !path )
    {
        return;
    }

    tmpPath[ 0 ] = '\0';

    if ( drive && drive[ 0 ] )
    {
        tmpPath[ 0 ] = drive[ 0 ];
        tmpPath[ 1 ] = ':';
        tmpPath[ 2 ] = 0;
    }

    if ( dir && dir[ 0 ] )
    {
        strcat( tmpPath, dir );
        ch = tmpPath[ strlen( tmpPath ) - 1 ];

        if ( ch != '/' && ch != '\\' )
        {
            strcat( tmpPath, "/" );
        }
    }

    if ( fname && fname[ 0 ] )
    {
        strcat( tmpPath, fname );

        if ( ext && ext[ 0 ] )
        {
            if ( ext[ 0 ] != '.' )
            {
                strcat( tmpPath, "." );
            }

            strcat( tmpPath, ext );
        }
    }

    strcpy( path, tmpPath );
    //TRACE("returning %s\n",path);
}



/* implementation of the _splitpath runtime library function with wide character strings */
void _wsplitpath( const wchar_t* path, wchar_t* drv, wchar_t* dir, wchar_t* name, wchar_t* ext )
{
    if( ( path == NULL ) || ( drv == NULL ) || ( dir == NULL ) || ( name == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    const wchar_t* end; /* end of processed string */
    const wchar_t* p;   /* search pointer */
    const wchar_t* s;   /* copy pointer */

    /* extract drive name */
    if ( path[ 0 ] && path[ 1 ] == ':' )
    {
        if ( drv )
        {
            *drv++ = *path++;
            *drv++ = *path++;
            *drv = '\0';
        }
    }
    else if ( drv )
    {
        *drv = '\0';
    }

    /* search for end of string or stream separator */
    for ( end = path; *end && *end != ':'; )
    {
        end++;
    }

    /* search for begin of file ext */
    for ( p = end; p > path && *--p != '\\' && *p != '/'; )
        if ( *p == '.' )
        {
            end = p;
            break;
        }

    if ( ext )
        for ( s = end; ( *ext = *s++ ); )
        {
            ext++;
        }

    /* search for end of dir name */
    for ( p = end; p > path; )
        if ( *--p == '\\' || *p == '/' )
        {
            p++;
            break;
        }

    if ( name )
    {
        for ( s = p; s < end; )
        {
            *name++ = *s++;
        }

        *name = '\0';
    }

    if ( dir )
    {
        for ( s = path; s < p; )
        {
            *dir++ = *s++;
        }

        *dir = '\0';
    }
}


/*********************************************************************
*  _makepathW (MSVCRT.@)
*
* Unicode version of _makepathW.
*/

void _wmakepath( wchar_t* path, const wchar_t* drive, const wchar_t* dir, const wchar_t* fname, const wchar_t* ext )
{
    if( ( path == NULL ) || ( drive == NULL ) || ( dir == NULL ) || ( fname == NULL ) || ( ext == NULL ) )
    {
        return;
    }

    wchar_t ch;

    //TRACE("%s %s %s %s\n", debugstr_w(drive), debugstr_w(dir),
    //debugstr_w(fname), debugstr_w(ext));
    if ( !path )
    {
        return;
    }

    path[ 0 ] = 0;

    if ( drive && drive[ 0 ] )
    {
        path[ 0 ] = drive[ 0 ];
        path[ 1 ] = ':';
        path[ 2 ] = 0;
    }

    if ( dir && dir[ 0 ] )
    {
        strcatW( path, dir );
        ch = path[ strlenW( path ) - 1 ];

        if ( ch != '/' && ch != '\\' )
        {
            static const wchar_t backslashW[] = { '\\', 0 };
            strcatW( path, backslashW );
        }
    }

    if ( fname && fname[ 0 ] )
    {
        strcatW( path, fname );

        if ( ext && ext[ 0 ] )
        {
            if ( ext[ 0 ] != '.' )
            {
                static const wchar_t dotW[] = { '.', 0 };
                strcatW( path, dotW );
            }

            strcatW( path, ext );
        }
    }

    //TRACE("returning %s\n", debugstr_w(path));
}


#endif //!_WINDOWS


//////////////////////////////////////////////////////////////////////////
// URL
/*
 * Check whether the character is permitted in scheme string
 */
int _is_scheme_char( int c )
{
    return ( !isalpha( c ) && '+' != c && '-' != c && '.' != c ) ? 0 : 1;
}

int _is_scheme_wchar( int c )
{
    return ( !iswalpha( c ) && L'+' != c && L'-' != c && L'.' != c ) ? 0 : 1;
}

bool msplituri( const char* url, uri_msyntax& url_syntax )
{
    if( url == NULL )
    {
        return false;
    }

    memset( &url_syntax, 0, sizeof( url_syntax ) );

    const char* tmpstr;
    const char* curstr;
    int len;
    int i;
    int userpass_flag;
    int bracket_flag;
    uri_msyntax* purl = &url_syntax;

    curstr = url;

    /*
     * <scheme>:<scheme-specific-part>
     * <scheme> := [a-z\+\-\.]+
     *             upper case = lower case for resiliency
     */
     /* Read scheme */
    tmpstr = strchr( curstr, ':' );
    if ( NULL == tmpstr )
    {
        /* Not found the character */
        // 有可能只有檔名
        {
            tmpstr = strchr( curstr, '.' );
            if ( NULL != tmpstr )
            {
                char_t path[ _MAX_PATH ] = { "" };
                char_t drive[ _MAX_DRIVE ] = { "" };
                _splitpath( url, drive, url_syntax.path, url_syntax.page_name, url_syntax.page_ext );
                return true;
            }
        }
        return false;
    }
    /* Get the scheme length */
    len = (int)(tmpstr - curstr);
    /* Check restrictions */
    for ( i = 0; i < len; i++ )
    {
        if ( !_is_scheme_char( curstr[ i ] ) )
        {
            /* Invalid format */
            return false;
        }
    }
    /* Copy the scheme to the storage */

    (void)strncpy( purl->scheme, curstr, len );
    purl->scheme[ len ] = '\0';
    /* Make the character to lower if it is upper case. */
    for ( i = 0; i < len; i++ )
    {
        purl->scheme[ i ] = tolower( purl->scheme[ i ] );
    }
    /* Skip ':' */
    tmpstr++;
    curstr = tmpstr;

    /*
     * //<user>:<password>@<host>:<port>/<url-path>
     * Any ":", "@" and "/" must be encoded.
     */
     /* Eat "//" */
    for ( i = 0; i < 2; i++ )
    {
        if ( '/' != *curstr )
        {
            return false;
        }
        curstr++;
    }

    /* Check if the user (and password) are specified. */
    userpass_flag = 0;
    tmpstr = curstr;
    while ( '\0' != *tmpstr )
    {
        if ( '@' == *tmpstr )
        {
            /* Username and password are specified */
            userpass_flag = 1;
            break;
        }
        else if ( '/' == *tmpstr )
        {
            /* End of <host>:<port> specification */
            userpass_flag = 0;
            break;
        }
        tmpstr++;
    }

    /* User and password specification */
    tmpstr = curstr;
    if ( userpass_flag )
    {
        /* Read username */
        while ( '\0' != *tmpstr && ':' != *tmpstr && '@' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);

        (void)strncpy( purl->username, curstr, len );
        purl->username[ len ] = '\0';
        /* Proceed current pointer */
        curstr = tmpstr;
        if ( ':' == *curstr )
        {
            /* Skip ':' */
            curstr++;
            /* Read password */
            tmpstr = curstr;
            while ( '\0' != *tmpstr && '@' != *tmpstr )
            {
                tmpstr++;
            }
            len = (int)(tmpstr - curstr);
            (void)strncpy( purl->password, curstr, len );
            purl->password[ len ] = '\0';
            curstr = tmpstr;
        }
        /* Skip '@' */
        if ( '@' != *curstr )
        {
            return false;
        }
        curstr++;
    }

    if ( '[' == *curstr )
    {
        bracket_flag = 1;
    }
    else
    {
        bracket_flag = 0;
    }
    /* Proceed on by delimiters with reading host */
    tmpstr = curstr;
    while ( '\0' != *tmpstr )
    {
        if ( bracket_flag && ']' == *tmpstr )
        {
            /* End of IPv6 address. */
            tmpstr++;
            break;
        }
        else if ( !bracket_flag && ( ':' == *tmpstr || '/' == *tmpstr ) )
        {
            /* Port number is specified. */
            break;
        }
        tmpstr++;
    }
    len = (int)(tmpstr - curstr);
    (void)strncpy( purl->host, curstr, len );
    purl->host[ len ] = '\0';
    curstr = tmpstr;

    /* Is port number specified? */
    if ( ':' == *curstr )
    {
        curstr++;
        /* Read port number */
        tmpstr = curstr;
        while ( '\0' != *tmpstr && '/' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);

        (void)strncpy( purl->port, curstr, len );
        purl->port[ len ] = '\0';
        curstr = tmpstr;
    }

    /* End of the string */
    if ( '\0' == *curstr )
    {
        return purl;
    }

    /* Skip '/' */
    if ( '/' != *curstr )
    {
        return false;
    }
    curstr++;

    /* Parse path */
    tmpstr = curstr;
    //while( '\0' != *tmpstr && '#' != *tmpstr  && '?' != *tmpstr )
    while ( '\0' != *tmpstr && '#' != *tmpstr  && '?' != *tmpstr )
    {
        tmpstr++;
    }
    len = (int)(tmpstr - curstr);

    {
        char tmp_path[ 1024 ];
        (void)strncpy( tmp_path, curstr, len );
        tmp_path[ len ] = '\0';

        char_t path[ _MAX_PATH ] = { "" };
        char_t drive[ _MAX_DRIVE ] = { "" };
        _splitpath( tmp_path, drive, url_syntax.path, url_syntax.page_name, url_syntax.page_ext );
    }
    //purl->path[ len ] = '\0';
    curstr = tmpstr;

    /* Is query specified? */
    if ( '?' == *curstr )
    {
        /* Skip '?' */
        curstr++;
        /* Read query */
        tmpstr = curstr;
        while ( '\0' != *tmpstr && '#' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);
        (void)strncpy( purl->query, curstr, len );
        purl->query[ len ] = '\0';
        curstr = tmpstr;
    }

    /* Is fragment specified? */
    if ( '#' == *curstr )
    {
        /* Skip '#' */
        curstr++;
        /* Read fragment */
        tmpstr = curstr;
        while ( '\0' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);
        (void)strncpy( purl->fragment, curstr, len );
        purl->fragment[ len ] = '\0';
        curstr = tmpstr;
    }
    return true;
}

bool mmakeuri( _OUT char* url, _IN const uri_msyntax& url_syntax )
{
    if( url == NULL )
    {
        return false;
    }

    strcpy( url, url_syntax.scheme );
    strcat( url, "://" );

    if ( strcmp( url_syntax.username, "" ) != 0 )
    {
        strcat( url, url_syntax.username );
        if ( strcmp( url_syntax.password, "" ) != 0 )
        {
            strcat( url, ":" );
            strcat( url, url_syntax.password );
        }
        strcat( url, "@" );
    }

    strcat( url, url_syntax.host );

    if ( strcmp( url_syntax.port, "" ) != 0 )
    {
        strcat( url, ":" );
        strcat( url, url_syntax.port );
    }

    strcat( url, "/" );

    if ( strcmp( url_syntax.path, "" ) != 0 )
    {
        strcat( url, url_syntax.path );
    }

    if ( strcmp( url_syntax.page_name, "" ) != 0 )
    {
        strcat( url, url_syntax.page_name );

        if ( strcmp( url_syntax.page_ext, "" ) != 0 )
        {
            strcat( url, url_syntax.page_ext );
        }
    }

    if ( strcmp( url_syntax.query, "" ) != 0 )
    {
        strcat( url, "?" );
        strcat( url, url_syntax.query );
    }

    if ( strcmp( url_syntax.fragment, "" ) != 0 )
    {
        strcat( url, "#" );
        strcat( url, url_syntax.fragment );
    }
    return true;
}

bool wsplituri( const wchar_t* url, uri_wsyntax& url_syntax )
{
    if( url == NULL )
    {
        return false;
    }

    memset( &url_syntax, 0, sizeof( url_syntax ) );

    const wchar_t *tmpstr;
    const wchar_t *curstr;
    int len;
    int i;
    int userpass_flag;
    int bracket_flag;
    uri_wsyntax* purl = &url_syntax;

    curstr = url;

    /*
     * <scheme>:<scheme-specific-part>
     * <scheme> := [a-z\+\-\.]+
     *             upper case = lower case for resiliency
     */
     /* Read scheme */
    tmpstr = wcschr( curstr, L':' );
    if ( NULL == tmpstr )
    {
        /* Not found the character */
        // 有可能只有檔名
        {
            tmpstr = wcschr( curstr, '.' );
            if ( NULL != tmpstr )
            {
                wchar_t path[ _MAX_PATH ] = { L"" };
                wchar_t drive[ _MAX_DRIVE ] = { L"" };
                _wsplitpath( url, drive, url_syntax.path, url_syntax.page_name, url_syntax.page_ext );
                return true;
            }
        }
        return false;
    }
    /* Get the scheme length */
    len = (int)(tmpstr - curstr);
    /* Check restrictions */
    for ( i = 0; i < len; i++ )
    {
        if ( !_is_scheme_wchar( curstr[ i ] ) )
        {
            /* Invalid format */
            return false;
        }
    }
    /* Copy the scheme to the storage */

    (void)wcsncpy( purl->scheme, curstr, len );
    purl->scheme[ len ] = L'\0';
    /* Make the character to lower if it is upper case. */
    for ( i = 0; i < len; i++ )
    {
        purl->scheme[ i ] = towlower( purl->scheme[ i ] );
    }
    /* Skip ':' */
    tmpstr++;
    curstr = tmpstr;

    /*
     * //<user>:<password>@<host>:<port>/<url-path>
     * Any ":", "@" and "/" must be encoded.
     */
     /* Eat "//" */
    for ( i = 0; i < 2; i++ )
    {
        if ( L'/' != *curstr )
        {
            return false;
        }
        curstr++;
    }

    /* Check if the user (and password) are specified. */
    userpass_flag = 0;
    tmpstr = curstr;
    while ( L'\0' != *tmpstr )
    {
        if ( L'@' == *tmpstr )
        {
            /* Username and password are specified */
            userpass_flag = 1;
            break;
        }
        else if ( L'/' == *tmpstr )
        {
            /* End of <host>:<port> specification */
            userpass_flag = 0;
            break;
        }
        tmpstr++;
    }

    /* User and password specification */
    tmpstr = curstr;
    if ( userpass_flag )
    {
        /* Read username */
        while ( L'\0' != *tmpstr && L':' != *tmpstr && L'@' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);

        (void)wcsncpy( purl->username, curstr, len );
        purl->username[ len ] = L'\0';
        /* Proceed current pointer */
        curstr = tmpstr;
        if ( L':' == *curstr )
        {
            /* Skip ':' */
            curstr++;
            /* Read password */
            tmpstr = curstr;
            while ( '\0' != *tmpstr && '@' != *tmpstr )
            {
                tmpstr++;
            }
            len = (int)(tmpstr - curstr);
            (void)wcsncpy( purl->password, curstr, len );
            purl->password[ len ] = L'\0';
            curstr = tmpstr;
        }
        /* Skip '@' */
        if ( L'@' != *curstr )
        {
            return false;
        }
        curstr++;
    }

    if ( L'[' == *curstr )
    {
        bracket_flag = 1;
    }
    else
    {
        bracket_flag = 0;
    }
    /* Proceed on by delimiters with reading host */
    tmpstr = curstr;
    while ( L'\0' != *tmpstr )
    {
        if ( bracket_flag && L']' == *tmpstr )
        {
            /* End of IPv6 address. */
            tmpstr++;
            break;
        }
        else if ( !bracket_flag && ( L':' == *tmpstr || L'/' == *tmpstr ) )
        {
            /* Port number is specified. */
            break;
        }
        tmpstr++;
    }
    len = (int)(tmpstr - curstr);
    (void)wcsncpy( purl->host, curstr, len );
    purl->host[ len ] = L'\0';
    curstr = tmpstr;

    /* Is port number specified? */
    if ( L':' == *curstr )
    {
        curstr++;
        /* Read port number */
        tmpstr = curstr;
        while ( L'\0' != *tmpstr && L'/' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);

        (void)wcsncpy( purl->port, curstr, len );
        purl->port[ len ] = L'\0';
        curstr = tmpstr;
    }

    /* End of the string */
    if ( L'\0' == *curstr )
    {
        return purl;
    }

    /* Skip '/' */
    if ( L'/' != *curstr )
    {
        return false;
    }
    curstr++;

    /* Parse path */
    tmpstr = curstr;
    while ( L'\0' != *tmpstr && L'#' != *tmpstr  && L'?' != *tmpstr )
    {
        tmpstr++;
    }
    len = (int)(tmpstr - curstr);

    {
        wchar_t tmp_path[ 1024 ];
        (void)wcsncpy( tmp_path, curstr, len );
        tmp_path[ len ] = L'\0';

        wchar_t path[ _MAX_PATH ] = { L"" };
        wchar_t drive[ _MAX_DRIVE ] = { L"" };
        _wsplitpath( tmp_path, drive, url_syntax.path, url_syntax.page_name, url_syntax.page_ext );
    }
    //purl->path[ len ] = '\0';
    curstr = tmpstr;

    /* Is query specified? */
    if ( L'?' == *curstr )
    {
        /* Skip '?' */
        curstr++;
        /* Read query */
        tmpstr = curstr;
        while ( L'\0' != *tmpstr && L'#' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);
        (void)wcsncpy( purl->query, curstr, len );
        purl->query[ len ] = L'\0';
        curstr = tmpstr;
    }

    /* Is fragment specified? */
    if ( L'#' == *curstr )
    {
        /* Skip '#' */
        curstr++;
        /* Read fragment */
        tmpstr = curstr;
        while ( L'\0' != *tmpstr )
        {
            tmpstr++;
        }
        len = (int)(tmpstr - curstr);
        (void)wcsncpy( purl->fragment, curstr, len );
        purl->fragment[ len ] = L'\0';
        curstr = tmpstr;
    }
    return true;
}

bool wmakeuri( _OUT wchar_t* url, _IN const uri_wsyntax& url_syntax )
{
    if( url == NULL )
    {
        return false;
    }

    wcscpy( url, url_syntax.scheme );
    wcscat( url, L"://" );

    if ( wcscmp( url_syntax.username, L"" ) != 0 )
    {
        wcscat( url, url_syntax.username );
        if ( wcscmp( url_syntax.password, L"" ) != 0 )
        {
            wcscat( url, L":" );
            wcscat( url, url_syntax.password );
        }
        wcscat( url, L"@" );
    }

    wcscat( url, url_syntax.host );

    if ( wcscmp( url_syntax.port, L"" ) != 0 )
    {
        wcscat( url, L":" );
        wcscat( url, url_syntax.port );
    }

    wcscat( url, L"/" );

    if ( wcscmp( url_syntax.path, L"" ) != 0 )
    {
        wcscat( url, url_syntax.path );
    }

    if ( wcscmp( url_syntax.page_name, L"" ) != 0 )
    {
        wcscat( url, url_syntax.page_name );

        if ( wcscmp( url_syntax.page_ext, L"" ) != 0 )
        {
            wcscat( url, url_syntax.page_ext );
        }
    }

    if ( wcscmp( url_syntax.query, L"" ) != 0 )
    {
        wcscat( url, L"?" );
        wcscat( url, url_syntax.query );
    }

    if ( wcscmp( url_syntax.fragment, L"" ) != 0 )
    {
        wcscat( url, L"#" );
        wcscat( url, url_syntax.fragment );
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
// program
//char get_escape_sequences( _IN const char* ins, _IN size32_t inl )
//{
//    if( strlen( ins ) < 2 )
//    {
//        return '\0';
//    }
//
//
//    if( ins[ 0 ] == '\\' )
//    {
//        if( ins[ 1 ] == 'a' )       {  return '\a';  }
//        else if( ins[ 1 ] == 'b' )  {  return '\b';  }
//        else if( ins[ 1 ] == 'e' )  { return '\e'; }
//        else if( ins[ 1 ] == 'f' )  { return '\f'; }
//        else if( ins[ 1 ] == 'n' )  { return '\n'; }
//        else if( ins[ 1 ] == 'r' )  { return '\r'; }
//        else if( ins[ 1 ] == 't' )  { return '\t'; }
//        else if( ins[ 1 ] == 'v' )  { return '\v'; }
//        else if( ins[ 1 ] == '\\' ) { return '\\'; }
//        else if( ins[ 1 ] == '\'' ) { return '\''; }
//        else if( ins[ 1 ] == '\"' ) { return '\"'; }
//        else if( ins[ 1 ] == '?' )  { return '\?'; }
//        else { return ins[ 1 ];  }
//    }
//
//}