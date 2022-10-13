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
#include <string.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
//#include "DeviceAPI.h"
//#include "Macros.h"
//#include "pf-debug.h"
#include "pf-def.h"
#include "pf-string.h"
#include "Unicode.h"


#define UTF8_COMPUTE(cChar, Mask, Len)                    \
    MACRO_START                                           \
    {                                                     \
        if (cChar < 128)                                  \
        { Len = 1;  Mask = 0x7f; }                        \
        else if ((cChar & 0xe0) == 0xc0)                  \
        { Len = 2; Mask = 0x1f; }                         \
        else if ((cChar & 0xf0) == 0xe0)                  \
        { Len = 3; Mask = 0x0f; }                         \
        else if ((cChar & 0xf8) == 0xf0)                  \
        { Len = 4; Mask = 0x07; }                         \
        else if ((cChar & 0xfc) == 0xf8)                  \
        { Len = 5; Mask = 0x03; }                         \
        else if ((cChar & 0xfe) == 0xfc)                  \
        { Len = 6; Mask = 0x01; }                         \
        else                                              \
            Len = -1;                                     \
    }                                                     \
    MACRO_STOP

#define UTF8_GET(Result, Chars, count_, Mask, Len)        \
    MACRO_START                                           \
    {                                                     \
        (Result) = (Chars)[0] & (Mask);                   \
        for ((count_) = 1; (count_) < (Len); ++(count_))  \
        {                                                 \
            if (((Chars)[(count_)] & 0xc0) != 0x80)       \
            {                                             \
                (Result) = -1;                            \
                break;                                    \
            }                                             \
            (Result) <<= 6;                               \
            (Result) |= ((Chars)[(count_)] & 0x3f);       \
        }                                                 \
    }                                                     \
    MACRO_STOP

/*
Convert one set UTF-8 characters into the Unicode(UCS2) value.
arguments:
        utf8    pointer to the valid UTF-8 character.
        result  pointer to the location into which the UNICODE value of the
                UTF-8 character returns.
return:
        If the character is not valid, result holds -1.
        The function returns a pointer to the start of the next UTF-8 character.
        If utf8 is not well format, it will return 0.
*/
char* _Utf8ConvUnicode( UInt16* result, const char* utf8 )
{
    Int32 unicode = *result;
    int i, mask = 0, len;
    UInt8 c = (UInt8)* utf8;

    if ( !c )
    {
        return 0;
    }

    UTF8_COMPUTE( c, mask, len );

    if ( len == -1 )
    {
        return 0;
    }

    UTF8_GET( unicode, utf8, i, mask, len );

    if ( unicode == -1 )
    {
        return 0;
    }

    *result = (UInt16)unicode;
    return (char*)utf8 + len;
}

int UTF8ToUnicode( UInt16* szUnicode, const char* szUtf8, int iBufSize )
{
    char* str;
    int len;

    if ( szUnicode == 0 || szUtf8 == 0 )
    {
        return -1;
    }

    str = (char*)szUtf8;
    len = 0;

    while ( ( str = _Utf8ConvUnicode( szUnicode++, str ) ) != 0 && len < iBufSize )
    {
        len++;
    }

    *( --szUnicode ) = 0;
    return len;
}

int _UnicodeConvUtf8( char* result, const UInt16* unicode )
{
    UInt16 c = (UInt16)* unicode;

    if ( c == 0 )
    {
        return 0;
    }

    if ( c < (UInt16)0x80 )
    {
        // 0*******
        if ( result )
        {
            result[ 0 ] = (char)c;
            result[ 1 ] = 0;
        }

        return 1;
    }
    else if ( c < (UInt16)0x800 )
    {
        // 110***** 10******
        if ( result )
        {
            result[ 0 ] = 0xc0 | ( c >> 6 );
            result[ 1 ] = 0x80 | ( c & 0x3f );
            result[ 2 ] = 0;
        }

        return 2;
    }
    else
    {
        // 1110**** 10****** 10******
        if ( result )
        {
            result[ 0 ] = 0xe0 | ( c >> 12 );
            result[ 1 ] = 0x80 | ( ( c >> 6 ) & 0x3f );
            result[ 2 ] = 0x80 | ( c & 0x3f );
            result[ 3 ] = 0;
        }

        return 3;
    }

    // UTF-8 is defined for words of up to 31 bits,
    // but we need only 16 bits here
    return 0;
}

int UnicodeToUTF8( char* szUtf8, const UInt16* szUnicode, int iBufSize )
{
    int len;
    int bytes;

    if ( szUtf8 == 0 || szUnicode == 0 )
    {
        return -1;
    }

    len = 0;

    while ( ( bytes = _UnicodeConvUtf8( 0, szUnicode ) ) != 0 && ( len + bytes ) < iBufSize )
    {
        _UnicodeConvUtf8( szUtf8, szUnicode );
        szUnicode++;
        szUtf8 += bytes;
        len += bytes;
    }

    *( szUtf8 ) = 0;
    return len;
}

void UTF8ToUTF8BOM( char* szUtf8wBom, const char* szUtf8 )
{
    szUtf8wBom[ 0 ] = 0XEF;
    szUtf8wBom[ 1 ] = 0XBB;
    szUtf8wBom[ 2 ] = 0XBF;
    strcpy( szUtf8wBom + 3, szUtf8 );
}

int AsciiToUnicode( UInt16* szUnicode, const char* szAscii, int iBufSize )
{
    UInt8* str, ch;
    int len;

    if ( szUnicode == 0 || szAscii == 0 )
    {
        return -1;
    }

    str = (UInt8*)szAscii;
    len = 0;
    iBufSize >>= 1; // /= 2;

    while ( ( ch = *str ) && len < iBufSize - 1 )
    {
#ifdef __BIG_ENDIAN__
        *szUnicode = (UInt16)( ch << 8 );
#else
        *szUnicode = (UInt16)( ch );
#endif
        szUnicode++;
        len++;
        str++;
    }

    *( szUnicode ) = (UInt16)0; // put EOS
    return len;
}

int UnicodeToAscii( char* szAscii, const UInt16* szUnicode, int iBufSize )
{
    UInt16* str, ch;
    int len;

    if ( szUnicode == 0 || szAscii == 0 )
    {
        return -1;
    }

    str = (UInt16*)szUnicode;
    len = 0;

    while ( ( ch = *str ) && len < iBufSize - 1 )
    {
#ifdef _WINDOWS

        if ( ch > 0x00FF )
        {
            //TRACE("UnicodeToAscii: unknown U+%04X\n", ch);
            *szAscii = '?';
        }
        else
#endif
#ifdef __BIG_ENDIAN__
            *szAscii = (UInt8)( ch >> 8 );

#else
            *szAscii = (UInt8)( ch );
#endif
        szAscii++;
        len++;
        str++;
    }

    *( szAscii ) = 0; // put EOS
    return len;
}

//////////////////////////////////////////////////////////////////////////

int UnicodeToNaviUnicode( char* szNaviUnicode, const UInt16* szUnicode, int iBufSize )
{
    //TODO: make  a unicode version strncpy
    if ( szNaviUnicode && iBufSize > 1 )
    {
        szNaviUnicode[ 0 ] = 0;
    }

    return 0;
}

int NaviUnicodeToUnicode( UInt16* szUnicode, const char* szNaviUnicode, int iBufSize )
{
    //TODO: make  a unicode version strncpy
    if ( szUnicode && iBufSize > 1 )
    {
        szUnicode[ 0 ] = 0;
    }

    return 0;
}

void AsciiStrToLower( char* str )
{
    unsigned char ch;

    if ( str == 0 )
    {
        return;
    }

    while ( ( ch = *(unsigned char*)str ) )
    {
        if ( ch >= 'A' && ch <= 'Z' )
        {
            *str = ch + 0x20;
        }

        str++;
    }
}

void AsciiStrToUpper( char* str )
{
    unsigned char ch;

    if ( str == 0 )
    {
        return;
    }

    while ( ( ch = *(unsigned char*)str ) )
    {
        if ( ch >= 'a' && ch <= 'z' )
        {
            *str = ch - 0x20;
        }

        str++;
    }
}

void DBCSStrToLower( char* str )
{
    unsigned char ch;

    // in linux no define 0
    //if (str == 0)
    if ( str == 0 )
    {
        return;
    }

    while ( ( ch = *(unsigned char*)str ) )
    {
        if ( ch >= 'A' && ch <= 'Z' )
        {
            *str = ch + 0x20;
        }
        else if ( ch > 0x7E )
        {
            // this is lead(hi) byte of double byte character
            // just pass the low byte for it may contains A~Z
            // and should not convert it single to lower case
            str++;
        }

        str++;
    }
}

void DBCSStrToUpper( char* str )
{
    unsigned char ch;

    if ( str == 0 )
    {
        return;
    }

    while ( ( ch = *(unsigned char*)str ) )
    {
        if ( ch >= 'a' && ch <= 'z' )
        {
            *str = ch - 0x20;
        }
        else if ( ch > 0x7E )
        {
            // this is lead(hi) byte of double byte character
            // just pass the low byte for it may contains a~z
            // and should not convert it single to upper case
            str++;
        }

        str++;
    }
}

/////////////////////////////////////////////////////////////////////////////
// string checker

#include <string.h>

int IsAsciiStr( const char* str )
{
    unsigned char c;

    if ( str == 0 )
    {
        return 0;
    }

    while ( ( c = *(unsigned char*)str ) )
    {
        if ( c < 0x20 || c > 0x7e )
        {
            return 0;
        }

        str++;
    }

    return 1;
}

int IsAlphaExLetter( unsigned char ch )
{
    if ( ch >= 192 )
    {
        // 0x7f
        if ( ( ( ch >= 192 ) && ( ch <= 198 ) ) || ( ( ch >= 224 ) && ( ch <= 230 ) ) )
        {
            return 1;//ch = 'A';
        }
        else if ( ( ch == 199 ) || ( ch == 231 ) )
        {
            return 1;//ch = 'C';
        }
        else if ( ( ( ch >= 200 ) && ( ch <= 203 ) ) || ( ( ch >= 232 ) && ( ch <= 235 ) ) )
        {
            return 1;//ch = 'E';
        }
        else if ( ( ( ch >= 204 ) && ( ch <= 207 ) ) || ( ( ch >= 236 ) && ( ch <= 239 ) ) )
        {
            return 1;//ch = 'I';
        }
        else if ( ( ch == 209 ) || ( ch == 241 ) )
        {
            return 1;//ch = 'N';
        }
        else if ( ( ( ch >= 210 ) && ( ch <= 216 ) ) || ( ( ch >= 242 ) && ( ch <= 248 ) ) )
        {
            return 1;//ch = 'O';
        }
        else if ( ( ( ch >= 217 ) && ( ch <= 220 ) ) || ( ( ch >= 249 ) && ( ch <= 252 ) ) )
        {
            return 1;//ch = 'U';
        }
        else if ( ( ch == 221 ) || ( ch == 253 ) )
        {
            return 1;//ch = 'Y';
        }
    }

    return 0;
}

int IsAlphaStr( const char* str )
{
    //unsigned char c;
    if ( str == 0 )
    {
        return 0;
    }

    /*
    if (GFIsChineseFont() == TRUE)
    {
       while ((c = *(unsigned char*)str))
       {
    //     if (c < 'A' || (c > 'Z' && c < 'a') || c >'z' || IsAlphaExLetter(c))
           if (c < 'A' || (c > 'Z' && c < 'a') || c >'z')
               return 0;
           str++;
       }
       return 1;
    }
    else
    {
       return 1;
    }
    */
    return 1;
}

int IsDBCSChar( const char* szStr, int iPosition )
{
    int i = 0;  // from begin

    if ( szStr == 0 )
    {
        return 0;
    }

    while ( szStr[ i ] )
    {
        // check if it's a ASCII char.
        if ( (unsigned char)szStr[ i ] < 0x7F )
        {
            if ( i == iPosition )
            {
                return 0;
            }
        }
        else // this may be a two bytes code char.
        {
            if ( i == iPosition )
            {
                return 1;
            }

            i++;

            if ( i == iPosition )
            {
                return 2;
            }
        }

        i++;
    }

    return 0;
}


int IsSymbol( UInt16 szUnicode )
{
    if ( szUnicode <= 256 )
    {
        char ac = (char)szUnicode;

        //if(((ac>'a')&&(ac<'z')) ||
        // ((ac>'A')&&(ac<'A')))
        if ( ( ( ac >= 'a' ) && ( ac <= 'z' ) ) || ( ( ac >= 'A' ) && ( ac <= 'Z' ) ) )
        {
        }
        else
        {
            return 1;
        }
    }

    if ( ( szUnicode > 0X02B0 ) && ( szUnicode < 0X02FF ) )
    {
        return 1;
    }

    if ( ( szUnicode > 0X0300 ) && ( szUnicode < 0X036F ) )
    {
        return 1;
    }

    // Pin
    if ( ( szUnicode > 0X2000 ) && ( szUnicode < 0X27FF ) )
    {
        return 1;
    }

    // Pin
    if ( ( szUnicode > 0X2E00 ) && ( szUnicode < 0X2FFF ) )
    {
        return 1;
    }

    if ( ( szUnicode > 0X3000 ) && ( szUnicode < 0X303F ) )
    {
        return 1;
    }

    // Pin
    if ( ( szUnicode >= 0XFF00 ) && ( szUnicode <= 0XFFEF ) )
    {
        char ac = (char)szUnicode;

        if ( ( ( ac >= 0XFF21 ) && ( ac <= 0XFF3A ) ) || ( ( ac >= 0XFF41 ) && ( ac <= 0XFF5A ) ) )
        {
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

int IsNumber( UInt16 szUnicode )
{
    if ( ( szUnicode >= 0X30 ) && ( szUnicode <= 0X39 ) )
    {
        return 1;
    }

    return 0;
}

int IsFullWidthNumber( UInt16 szUnicode )
{
    if ( ( szUnicode >= 0XFF10 ) && ( szUnicode <= 0XFF19 ) )
    {
        return 1;
    }

    return 0;
}

int IsNumberSign( UInt16 szUnicode )
{
    if ( IsSymbol( szUnicode ) == 0 )
    {
        return 0;
    }

    if ( ( szUnicode == '%' ) ||
        ( szUnicode == '+' ) ||
        ( szUnicode == '-' ) ||
        ( szUnicode == '*' ) ||
        ( szUnicode == '/' ) ||
        ( szUnicode == '=' ) ||
        ( szUnicode == '>' ) ||
        ( szUnicode == '<' ) ||
        ( szUnicode == '{' ) ||
        ( szUnicode == '}' ) ||
        ( szUnicode == '(' ) ||
        ( szUnicode == ')' ) ||
        ( szUnicode == '[' ) ||
        ( szUnicode == ']' ) ||
        ( szUnicode == 0XFF05 ) || // full-width %
        ( szUnicode == 0XFF0B ) || // full-width +
        ( szUnicode == 0XFF0D ) || // full-width -
        ( szUnicode == 0XFF0A ) || // full-width *
        ( szUnicode == 0XFF0F ) || // full-width 1
        ( szUnicode == 0XFE1D ) || // full-width =
        ( szUnicode == 0XFF1C ) || // full-width <
        ( szUnicode == 0XFF1E ) || // full-width >
        ( szUnicode == 0XFF5B ) || // full-width {
        ( szUnicode == 0XFE5D ) || // full-width }
        ( szUnicode == 0XFF08 ) || // full-width (
        ( szUnicode == 0XFF09 ) || // full-width )
        ( szUnicode == 0XFF3B ) ||
        ( szUnicode == 0XFF3D ) )
    {
        return 1;
    }

    return 0;
};


// http://en.wikipedia.org/wiki/Bracket
UInt16 LBracketTable[] =
{
    0X0028,  // (
    0X005B,  // [

    0X003C,  // <
    0X007B,  // {

    0X2308,  // ⌈
    0X230A,  // ⌊
    0X27E8,  // ⟨

    0X2329,  // 〈

    0X3008,  // 〈   9
    0X300A,  // 《
    0X300C,  // 「
    0X300E,  // 『
    0X3010,  // 【

    0XFF08,  // （  14
    0XFF3B,  // ［

    0XFF1C,   // ＜
    0XFF5B,   // ｛

    0XFF62,  // ｢   => with 29
};

UInt16 RBracketTable[] =
{
    0X0029,  // )
    0X005D,  // ]

    0X003E,  // >
    0X007D,  // }

    0X2309,  // ⌉
    0X230B,  // ⌋
    0X27E9,  // ⟩

    0X232A,  // 〉

    0X3009,  // 〉
    0X300B,  // 》
    0X300D,  // 」
    0X300F,  // 』
    0X3011,  // 】

    0XFF09,  // ）
    0XFF3D,  // ］

    0XFF1E,  // ＞
    0XFF5D,  // ｝

    0XFF63,  // ｣
};

int IsLBracketing( UInt16 szUnicode )
{
    for ( unsigned int i = 0; i < ( sizeof( LBracketTable ) / sizeof( UInt16 ) ); i++ )
    {
        if ( szUnicode == LBracketTable[ i ] )
        {
            return i + 1;
        }
    }

    return 0;
}

int IsRBracketing( UInt16 szUnicode )
{
    for ( unsigned int i = 0; i < ( sizeof( RBracketTable ) / sizeof( UInt16 ) ); i++ )
    {
        if ( szUnicode == RBracketTable[ i ] )
        {
            return i + 1;
        }
    }

    return 0;
}

bool has_utf8_bom( const char *in_char )
{
    uint8_t *in = (uint8_t *)in_char;
    return ( in && in[ 0 ] == 0xef && in[ 1 ] == 0xbb && in[ 2 ] == 0xbf );
}

int IsUTF8( const char* rawtext )
{
    int64_t score = 0;
    size_t i, rawtextlen = 0;
    int goodbytes = 0, asciibytes = 0;
    // Maybe also use UTF8 Byte Order Mark: EF BB BF
    // Check to see if characters fit into acceptable ranges
    rawtextlen = strlen( rawtext );

    for ( i = 0; i < rawtextlen; i++ )
    {
        if ( ( rawtext[ i ] & (char)0x7F ) == rawtext[ i ] )
        {
            // 最高位是0的ASCII字符
            asciibytes++;
            // Ignore ASCII, can throw off count
        }
        else if ( -64 <= rawtext[ i ] && rawtext[ i ] <= -33
            //-0x40~-0x21
            && // Two bytes
            i + 1 < rawtextlen && -128 <= rawtext[ i + 1 ]
            && rawtext[ i + 1 ] <= -65 )
        {
            goodbytes += 2;
            i++;
        }
        else if ( -32 <= rawtext[ i ]
            && rawtext[ i ] <= -17
            && // Three bytes
            i + 2 < rawtextlen && -128 <= rawtext[ i + 1 ]
            && rawtext[ i + 1 ] <= -65 && -128 <= rawtext[ i + 2 ]
            && rawtext[ i + 2 ] <= -65 )
        {
            goodbytes += 3;
            i += 2;
        }
    }

    if ( asciibytes == rawtextlen )
    {
        return 0;
    }

    score = 100 * goodbytes / ( rawtextlen - asciibytes );

    // If not above 98, reduce to zero to prevent coincidental matches
    // Allows for some (few) bad formed sequences
    if ( score > 98 )
    {
        if ( has_utf8_bom( rawtext ) )
        {
            return 2;
        }
        return 1;
    }
    else if ( score > 95 && goodbytes > 30 )
    {
        if ( has_utf8_bom( rawtext ) )
        {
            return 2;
        }
        return 1;
    }

    return 0;
}


int IsUnicode( const char* rawtext )
{
    uint8_t *in = (uint8_t *)rawtext;
    return ( in && in[ 0 ] == 0xFF && in[ 1 ] == 0xFE );
}

// https://www.itread01.com/p/1390097.html
// https://kuyur.info/blog/archives/635
// 還不能用 暫記
int IsShiftJIS( const char* rawtext )
{
    if ( NULL == rawtext )
    {
        return false;
    }
    /*

    char* b = new char[ strlen( rawtext ) ];
    for ( int i = 0; i < strlen( rawtext ); i++ )
    {
        char c = rawtext[ i ];
        //b = Character.toString( c ).getBytes( "Shift_JIS" );

        // Shift-JISの1バイトコード(半形文字)のエリア
        if ( strlen( b ) == 1 )
        {
            int firstByte = b[ 0 ];
            if ( firstByte < 0 )
            {
                firstByte += 256;
            }

            // 0x00~0x1F、0x7F は制御コードです
            if ( ( 0x00 <= firstByte && firstByte <= 0x1F ) || firstByte == 0x7F )
            {
                return false;
            }
            // 0x20~0x7E はASCII文字です
            else if ( 0x20 <= firstByte && firstByte <= 0x7E )
            {
                //Unicode -> Shift-Jisの場合、非Shift-JIS文字が[?]に変化
                if ( firstByte != c )
                {
                    return false;
                }
                continue;
            }
            // 0xA1~0xDF は半形カタカナです
            else if ( 0xA1 <= firstByte && firstByte <= 0xDF )
            {
                continue;
            }
            // Shift-JIS以下の文字
            else
            {
                return false;
            }
        }
        // シフトJISの2バイトコード(全形文字)のエリア(JIS X 0208の漢字エリア)
        else if ( strlen( b ) == 2 )
        {
            int firstByte = b[ 0 ];
            int secondByte = b[ 1 ];

            if ( firstByte < 0 )
            {
                firstByte += 256;
            }

            if ( secondByte < 0 )
            {
                secondByte += 256;
            }

            // 上位1バイト0x81~0x9F、 0xE0~0xEF
            // 下位1バイト0x40~0x7E、 0x80~0xFC
            if ( ( 0x81 <= firstByte && firstByte <= 0x9F || 0xE0 <= firstByte && firstByte <= 0xEF )
                && ( 0x40 <= secondByte && secondByte <= 0x7E || 0x80 <= secondByte && secondByte <= 0xFC ) )
            {
                String sHex = Integer.toHexString( firstByte ) + Integer.toHexString( secondByte );
                int code = Integer.parseInt( sHex, 16 );
                if ( 0x81AD <= code && code <= 0x81B7
                    || 0x81C0 <= code && code <= 0x81C7
                    || 0x81CF <= code && code <= 0x81D9
                    || 0x81E9 <= code && code <= 0x81EF
                    || 0x81F8 <= code && code <= 0x81FC
                    || 0x8240 <= code && code <= 0x824e
                    || 0x8259 <= code && code <= 0x825f
                    || 0x827A <= code && code <= 0x8280
                    || 0x829B <= code && code <= 0x829E
                    || 0x82F2 <= code && code <= 0x82FC
                    || 0x8397 <= code && code <= 0x839E
                    || 0x83B7 <= code && code <= 0x83BE
                    || 0x83D7 <= code && code <= 0x83FC
                    || 0x8461 <= code && code <= 0x846F
                    || 0x8492 <= code && code <= 0x849E
                    || 0x84BF <= code && code <= 0x84FC
                    || 0x8540 <= code && code <= 0x889E
                    || 0x9873 <= code && code <= 0x989E
                    || 0xEBA5 <= code && code <= 0xEBFC
                    || 0xEB40 <= code && code <= 0xEFFC )
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    SAFE_DELETE_ARRAY( b );
    */

    return false;
    return true;
}

int GetCodePage( const char* n )
{
    //https://zh.wikipedia.org/zh-tw/ISO_3166-1
    if ( ( strcaseicmp( n, "utf8" ) == 0 ) || ( strcaseicmp( n, "UTF-8" ) == 0 ) )
    {
        return 65001;
    }
    else if ( ( strcaseicmp( n, "tw" ) == 0 ) || ( strcaseicmp( n, "big5" ) == 0 ) || ( strcaseicmp( n, "BIG-5" ) == 0 ) )
    {
        return 950;
    }
    else if ( ( strcaseicmp( n, "jp" ) == 0 ) || ( strcaseicmp( n, "jis" ) == 0 ) || ( strcaseicmp( n, "Shift-JIS" ) == 0 ) )
    {
        return 932;
    }
    else if ( ( strcaseicmp( n, "ka" ) == 0 ) || ( strcaseicmp( n, "EUC-KR" ) == 0 ) )
    {
        return 949;
    }
    else if ( ( strcaseicmp( n, "ca" ) == 0 ) || ( strcaseicmp( n, "gb" ) == 0 ) || ( strcaseicmp( n, "GB2312" ) == 0 ) )
    {
        return 936;
    }
    else if ( ( strcaseicmp( n, "us" ) == 0 ) || ( strcaseicmp( n, "US-ASCII" ) == 0 ) )//美國/加拿大英語
    {
        return 437;
    }

    return -1;
}

const char* GetCodePageName( int page )
{
    //https://zh.wikipedia.org/zh-tw/ISO_3166-1
    switch ( page )
    {
    case CP_UTF8:
        return "UTF-8";
    case CP_BIG5:
        return "BIG-5";
    case CP_EKR:
        return "EUC-KR";
    case CP_GB2312:
        return "GB2312";
    case CP_JIS:
        return "Shift-JIS";
    case CP_ASCII:
        return "US-ASCII";
    }
    return "";
}

const char* GetCodeName( const char* n )
{
    // iconv --list
    if ( strcaseicmp( n, "utf8" ) == 0 )
    {
        return "UTF-8";
    }
    else if ( ( strcaseicmp( n, "tw" ) == 0 ) || ( strcaseicmp( n, "big5" ) == 0 ) )
    {
        return "BIG-5";
    }
    else if ( ( strcaseicmp( n, "jp" ) == 0 ) || ( strcaseicmp( n, "jis" ) == 0 ) )
    {
        return "Shift-JIS";
    }
    else if ( strcaseicmp( n, "ka" ) == 0 )
    {
        return "EUC-KR";
    }
    else if ( ( strcaseicmp( n, "ca" ) == 0 ) || ( strcaseicmp( n, "gb" ) == 0 ) )
    {
        return "GB2312";
    }
    else if ( strcaseicmp( n, "us" ) == 0 ) //美國/加拿大英語
    {
        return "US-ASCII";
    }

    return NULL;

}
