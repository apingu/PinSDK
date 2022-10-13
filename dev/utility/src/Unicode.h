#ifndef __UNICODE_H__
#define __UNICODE_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define MAX_CONV_BUFFER_SIZE  512     // for internal buffer in UTF8ToXXX()


#ifndef _UINT8
#define _UINT8
typedef unsigned char      UInt8;
#endif

#ifndef _UINT16
#define _UINT16
typedef unsigned short     UInt16;
#endif

#ifndef _INT32
#define _INT32
typedef int                Int32;
#endif

#ifndef _UINT32
#define _UINT32
typedef unsigned int       UInt32;
#endif


#ifndef CP_ASCII
    #define CP_ASCII                  437         // 
#endif
#ifndef CP_JIS
    #define CP_JIS                    932         // 
#endif
#ifndef CP_GB2312
    #define CP_GB2312                 936         // 
#endif
#ifndef CP_EKR
    #define CP_EKR                    949         // 
#endif
#ifndef CP_BIG5
    #define CP_BIG5                   950         // 
#endif

#ifndef CP_UTF16
    #define CP_UTF16                  1200        // unicode
#endif

#ifndef CP_UTF7
    #define CP_UTF7                   65000       // 
#endif
#ifndef CP_UTF8
    #define CP_UTF8                   65001       // 
#endif


// Scan UTF-8 string and convert into Unicode(UCS2) string
int UTF8ToUnicode( UInt16* szUnicode, const char* szUtf8, int iBufSize );

// Scan Unicode(UCS2) and convert into UTF-8 string string
int UnicodeToUTF8( char* szUtf8, const UInt16* szUnicode, int iBufSize );

// utf8 add bom size must less then ibufsize + 10
void UTF8ToUTF8BOM( char* szUtf8wBom, const char* szUtf8 );

// Scan ASCII string and convert into Unicode(UCS2) string
int AsciiToUnicode( UInt16* szUnicode, const char* szAscii, int iBufSize );

// scan Unicode(UCS2) string and convert into ASCII string
int UnicodeToAscii( char* szAscii, const UInt16* szUnicode, int iBufSize );

int NaviUnicodeToUnicode( UInt16* szUnicode, const char* szNaviUnicode, int iBufSize );
int UnicodeToNaviUnicode( char* szNaviUnicode, const UInt16* szUnicode, int iBufSize );

/////////////////////////////////////////////////////////////////////////////
// code pages

/*
#include "BIG5.h"

#include "GBK.h"    //aka #include "GB2312.h"

#include "ISO8859.h"

#include "TIS620.h"

#include "CP1251.h"

#include "CP1256.h"

#include "CP1250.h"

#include "CP1252.h"

#include "CP1258.h"

#include "CP1253.h"

#include "CP1257.h"
*/

//////////////////////////////////////////////////////////////////////////

void AsciiStrToLower( char* str );  // note: only for below 0x7E else use ISO8859_1StrToLower
void AsciiStrToUpper( char* str );  // note: only for below 0x7E else use ISO8859_1StrToUpper

// DBCS string is multi-bytes string which
// mix 0x20~0x7E ASCII characters and 0x80+ leaded double byte characters
// this is used by BIG5/GB2312/GBK/KSC/JIS(?) coding string

void DBCSStrToLower( char* str );
void DBCSStrToUpper( char* str );

/////////////////////////////////////////////////////////////////////////////
// string checker

// check if the string contains only viewable(printable) ASCII chars
int IsAsciiStr( const char* str );

// check if the string contains only alphabetic chars
int IsAlphaStr( const char* str );

// Check the position in string is a byte of DBCS codes (see above the DBCS comment)
// return 0 for it is a ASCII byte
//        1 for 1st byte of DBCS
//        2 for 2rd byte of DBCS
int IsDBCSChar( const char* szStr, int iPosition );

int IsSymbol( UInt16 szUnicode );

// number include in symbol
int IsNumber( UInt16 szUnicode );

int IsFullWidthNumber( UInt16 szUnicode );

int IsNumberSign( UInt16 szUnicode );

// return breaking index
// 1 ~ 2 ( [ ,  3 ~ 4  < { , 5 ~ 7 , ⌈ ⌊ ⟨ , 8 〈  , 9 ~ 13〈《「『【 , 14 ~ 17 （［＜｛ , 18  ｢
int IsLBracketing( UInt16 szUnicode );
// 1 ~ 2 ) ] ,  3 ~ 4  > } , 5 ~ 7 , ⌉ ⌋ ⟩ , 8 〉  , 9 ~ 13 〉》」』】, 14 ~ 17 ）］＞｝ , 18  ｣
int IsRBracketing( UInt16 szUnicode );

// return bom = 2 other = 1
int IsUTF8( const char* rawtext );

int IsUnicode( const char* rawtext );

int IsShiftJIS( const char* rawtext );


//   "tw","big5"; "jp", jis"; "ka"; "ca", "gb"; "us"; "utf8"
int  GetCodePage( const char* n );

const char* GetCodePageName( int page );

const char* GetCodeName( const char* n );


#ifdef __cplusplus
}
#endif

#endif  // __UNICODE_H__

