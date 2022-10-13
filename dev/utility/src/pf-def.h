//pdef.h header file
//
//
//   (WIN32, _WINDOWS), _LINUX, _XBOX, _MACOS, _IPHONE, _PS2
//   _UNICODE, UNICODE
//   __GNUC__, __MSVC__
//
//
//
//   @MAC don't declare bool but if use .mm then it will declare it
//
//   base type define
//
//
//
//              Copyright (C) 1999 Pin
//
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//_______________________________________________________________
// OS
// if had define win32
// _WIN32 會出現在 32b 與64b 環境下
// https://msdn.microsoft.com/en-us/library/b0084kay.aspx
#if defined(_WIN32)
    #ifndef _WINDOWS
        #define _WINDOWS
    #endif
    //#define _USE_MATH_DEFINES
    #if defined(_WIN64)
        #define ENV64BIT
        #define _ENV64BIT_WINDOWS
    #else
        #define ENV32BIT
        #define _ENV32BIT_WINDOWS
    #endif
#endif

// https://gcc.gnu.org/onlinedocs/gcc-4.6.1/cpp/Predefined-Macros.html
#if defined(_LINUX)
    #if defined(__x86_64) || defined(__ppc64__)
        #define ENV64BIT
        #define _ENV64BIT_LINUX
    #else
        #define ENV32BIT
        #define _ENV32BIT_LINUX
    #endif
#endif


//_______________________________________________________________
// compiler
// ms c
#ifdef _MSC_VER
    #define __MSVC__  _MSC_VER
#endif

// just let you know
//#ifdef __GNUC__
//    #define __GNUC__
//#endif

// MSC _DEBUG GCC: DEBUG
#if defined(_DEBUG) || defined(DEBUG)
    #ifndef _DEBUG
        #define _DEBUG
    #endif
    #ifndef DEBUG
        #define DEBUG
    #endif
#endif

// MSC NDEBUG GCC: NDEBUG=1 RELEASE=1
#if defined(NDEBUG) || defined(RELEASE)
    #ifndef _RELEASE
        #define _RELEASE
    #endif
    #ifndef RELEASE
        #define RELEASE
    #endif
    #ifndef NDEBUG
        #define NDEBUG
    #endif
#endif

//_______________________________________________________________
//
typedef char                 char_t;
typedef char                 mchar_t;
typedef unsigned short       uncc_t;   // unicode char
typedef void                 void_t;
typedef char                 bool_t;   // .c don't have bool type
typedef int                  int_t;
typedef int                  long32_t;
typedef long long            long64_t;
typedef long long            longlong_t;
typedef float                float_t;
typedef double               double_t;
typedef char*                str_t;
typedef const char*          cstr_t;
typedef signed char          schar_t;
typedef char                 byte_t;
typedef unsigned char        ubyte_t;
typedef unsigned short       word_t;
typedef unsigned int         dword_t;
typedef unsigned char        uchar_t;
typedef unsigned short       ushort_t;
typedef unsigned int         uint_t;
typedef unsigned int         ulong32_t;
typedef unsigned long long   ulong64_t;
typedef unsigned long long   ulonglong_t;
typedef unsigned long long   qword_t;
//typedef unsigned float       ufloat_t;  // 沒有這種東西
//typedef unsigned double      udouble_t; // 沒有這種東西

typedef unsigned char        bin_t;


//
#if !defined(_STDINT_H) && !defined(_STDINT)
    typedef signed char                 int8_t;
    typedef short                       int16_t;
    typedef int                         int32_t;
    typedef unsigned char               uint8_t;
    typedef unsigned short              uint16_t;
    typedef unsigned int                uint32_t;
    #if defined(__MSVC__)
        #ifndef uint64_t
            typedef long long               int64_t;
            typedef unsigned long long      uint64_t;
        #endif
    #endif
    #if defined(__GNUC__)
        #ifndef uint64_t
            typedef signed long int         int64_t;
            typedef unsigned long int       uint64_t;
        #endif
    #endif
#endif


//_______________________________________________________________
// MFC 相容
#ifndef _WINDOWS
    #if !defined(OBJC_BOOL_DEFINED)
        typedef int              BOOL;
    #endif
    typedef BOOL*                PBOOL;
    typedef BOOL*                LPBOOL;
    typedef unsigned char        BYTE;
    typedef unsigned short       WORD;
    typedef unsigned int         DWORD;
    typedef float                FLOAT;
    typedef FLOAT*               PFLOAT;
    typedef BYTE*                PBYTE;
    typedef BYTE*                LPBYTE;
    typedef int*                 PINT;
    typedef int*                 LPINT;
    typedef WORD*                PWORD;
    typedef WORD*                LPWORD;
    typedef int*                 LPLONG;
    typedef DWORD*               PDWORD;
    typedef DWORD*               LPDWORD;
    typedef void*                LPVOID;
    typedef int                  INT;
    typedef int                  LONG32;
    typedef long long            LONG64;
    typedef unsigned int         UINT;
    typedef unsigned int*        PUINT;
    typedef long long            QWORD;
    typedef short                INT16;
    typedef unsigned short       UINT16;
    typedef int                  INT32;
    typedef unsigned int         UINT32;

    typedef int                  errno_t;

#endif

#if defined(_WINDOWS)
    #if !defined(_WCHAR_T_DEFINED) &&!defined(_WCHAR_T)
        typedef unsigned short       wchar_t;
        #define _WCHAR_T_DEFINED
        #define _WCHAR_T
    #endif
#endif

//_______________________________________________________________
typedef long64_t             stimen_t;   // 秒
typedef long64_t             mstimen_t;  // 毫秒 千分之一
typedef long64_t             ustimen_t;  // 微秒 百萬分之一

//
typedef long64_t             Pflag;      //64
//
typedef int                  error_t;

typedef void*                data_t;
typedef void*                vclass_t;

// 請儘量使用自己的 size_t 因為各平台 size_t的定義可能不盡相同 有些是 4 有些是 8
// dword size_t 因為 size32_t 很多 都有定義了
typedef unsigned int         size32_t;
typedef unsigned long long   size64_t;

typedef unsigned short       color16_t;
typedef unsigned int         color32_t;

// 用在編號  讓他有負的 也許用的到 還是不要有負的用不到
typedef ulonglong_t          uniqueid_t;

// 用在 檔案位置 或 buffer 位置
typedef long64_t             pos_t;

//================= UTF-8 ================
// https://docs.microsoft.com/zh-tw/cpp/cpp/string-and-character-literals-cpp?view=vs-2017
#ifndef _u8
    #define _u8(x)    u8##x
#endif

//================= UNICODE ================
// write unicode
#ifndef _W
    #define _W(x)   L##x
#endif

//================= CROSS ================
#if defined(_UNICODE) || defined(UNICODE)
    
    #ifndef _T
        #define _T(x)    L##x
    #endif
    
    // 定義win32或其他人會定義的
    #if !defined(_TCHAR_DEFINED)
        typedef wchar_t              TCHAR;
        #define _TCHAR_DEFINED
    #endif
    
    // 定義自己定義的
    #if !defined(_TCHAR_T_DEFINED)
        typedef wchar_t              tchar_t;
        #define _TCHAR_T_DEFINED
    #endif
    
    //================== ANSI =================
#else////defined(_UNICODE) || defined(UNICODE)
    
    #ifndef _T
        #define _T(x)    x
    #endif
    
    // 定義win32或其他人會定義的
    #if !defined(_TCHAR_DEFINED)
        typedef char                 TCHAR;
        #define _TCHAR_DEFINED
    #endif
    
    
    // 定義自己定義的
    #if !defined(_TCHAR_T_DEFINED)
        typedef char_t               tchar_t;
        #define _TCHAR_T_DEFINED
    #endif
    
#endif////defined(_UNICODE) || defined(UNICODE)

//auto c0 = 'A'; // char
//auto c1 = u8'A'; // char
//auto c2 = L'A'; // wchar_t
//auto c3 = u'A'; // char16_t
//auto c4 = U'A'; // char32_t

//// Multicharacter literals
//auto m0 = 'abcd'; // int, value 0x61626364

//// String literals
//auto s0 = "hello"; // const char*
//auto s1 = u8"hello"; // const char* before C++20, encoded as UTF-8,
//                     // const char8_t* in C++20
//auto s2 = L"hello"; // const wchar_t*
//auto s3 = u"hello"; // const char16_t*, encoded as UTF-16
//auto s4 = U"hello"; // const char32_t*, encoded as UTF-32

//// Raw string literals containing unescaped \ and "
//auto R0 = R"("Hello \ world")"; // const char*
//auto R1 = u8R"("Hello \ world")"; // const char* before C++20, encoded as UTF-8,
//                                  // const char8_t* in C++20
//auto R2 = LR"("Hello \ world")"; // const wchar_t*
//auto R3 = uR"("Hello \ world")"; // const char16_t*, encoded as UTF-16
//auto R4 = UR"("Hello \ world")"; // const char32_t*, encoded as UTF-32

//// Combining string literals with standard s-suffix
//auto S0 = "hello"s; // std::string
//auto S1 = u8"hello"s; // std::string before C++20, std::u8string in C++20
//auto S2 = L"hello"s; // std::wstring
//auto S3 = u"hello"s; // std::u16string
//auto S4 = U"hello"s; // std::u32string

//// Combining raw string literals with standard s-suffix
//auto S5 = R"("Hello \ world")"s; // std::string from a raw const char*
//auto S6 = u8R"("Hello \ world")"s; // std::string from a raw const char* before C++20, encoded as UTF-8,
//                                   // std::u8string in C++20
//auto S7 = LR"("Hello \ world")"s; // std::wstring from a raw const wchar_t*
//auto S8 = uR"("Hello \ world")"s; // std::u16string from a raw const char16_t*, encoded as UTF-16
//auto S9 = UR"("Hello \ world")"s; // std::u32string from a raw const char32_t*, encoded as UTF-32
        


//_______________________________________________________________
typedef void sighndlr_t( int );

//string template
enum class PixelFormat
{
    UNKNOW             = 0X0000,
    R5G6B5             = 0X0102,
    R5G5B5             = 0X0202,
    R5G5B6             = 0X0402,
    R6G5B5             = 0X1002,
    R8G8B8             = 0X2003,
    R8G8B8A8           = 0X4004,
    A8R8G8B8           = 0X8004,
    A8B8G8R8           = 0XC004
};

////////////////////////////////////////////////////////////////
//   01?? mouse   02?? keyboard
// // #ifdef WIN32 || WINDOWS
// enum eventmsg_t
// {
//  PE_NULL           = WM_NULL,
//  PE_MOUSEOVER      = WM_USER+20,
//  PE_MOUSEMOVE      = WM_MOUSEMOVE,
//  PE_MOUSEDOCK      = 0X0103,
//  PE_LBUTTONDOWN    = WM_LBUTTONDOWN,
//  PE_LBUTTONUP      = WM_LBUTTONUP,
//  PE_LDBCLICK       = WM_LBUTTONDBLCLK,
//  PE_LSLIPDOWN      = WM_USER+30,
//  PE_RBUTTONDOWN    = WM_RBUTTONDOWN,
//  PE_RBUTTONUP      = WM_RBUTTONUP,
//  PE_RDBCLICK       = WM_RBUTTONDBLCLK,
//  PE_RSSLIPDOWN     = WM_USER+32,
//  PE_MOUSEDRAG      = WM_USER+51,
//  PE_MOUSEDRAGDROP  = WM_USER+22,
//  PE_MOUSEWHEEL     = WM_MOUSEWHEEL,
//  PE_KEYDOWN        = WM_KEYDOWN,
//  PE_KEYUP          = WMKEYUP,
//  PE_USERDEF        = WM_USER+100, // you can define you message after this
//
//  PE_OMIT           = 0XFFFF
// };
//
// #else
enum class EventMessage
{
    NONE                  = 0X0000,
    MOUSEOVER             = 0X0101,
    MOUSEMOVE             = 0X0102,
    MOUSEDOCK             = 0X0103,
    LBUTTONDOWN           = 0X0110,
    LBUTTONUP             = 0X0111,
    LDBCLICK              = 0X0112,
    LSLIPDOWN             = 0X0116,
    RBUTTONDOWN           = 0X0120,
    RBUTTONUP             = 0X0121,
    RDBCLICK              = 0X0122,
    RSSLIPDOWN            = 0X0126,
    MOUSEDRAG             = 0X0180,
    MOUSEDRAGDROP         = 0X0181,
    MOUSEWHEEL            = 0X0182,
    KEYDOWN               = 0X0201,
    KEYUP                 = 0X0202,
    USERDEF               = 0x0500, // you can define you message after this
    OMIT                  = 0XFFFF
};
//#endif

//_______________________________________________________________
#ifndef NULL
    #ifdef __cplusplus
        #define NULL                 0
    #else
        #define NULL                 ((void *)0)
    #endif
#endif

#ifndef FALSE
    #define FALSE                0
#endif

#ifndef TRUE
    #define TRUE                 1
#endif

#ifndef _FULL_ALPHA
    #define _FULL_ALPHA          0XFF
#endif

#ifndef _MAX_BUF
    #define _MAX_BUF             256
#endif

#ifndef _MAX_LBUF
    #define _MAX_LBUF            1024
#endif

#ifndef _MAX_MSG
    #define _MAX_MSG             256
#endif

#ifndef _MAX_LONG_MSG
    #define _MAX_LONG_MSG        1024
#endif

#ifndef _MAX_PATH
    #define _MAX_PATH            260
#endif

#ifndef _MAX_NAME
    #define _MAX_NAME            256
#endif

#ifndef _MAX_IPv4
    #define _MAX_IPv4            32
#endif

#ifndef _MAX_IPv6
    #define _MAX_IPv6            128
#endif

#define _MAX_STRING_LEN      1024
#define _MAX_STRING_SIZE     1024

#define _MAX_INT             2147483647
#define _MIN_INT            -2147483648

#define _MAX_UINT            4294967295
#define _MIN_UINT            0

#define _MAX_LLONG           9223372036854775807
#define _MIN_LLONG          -9223372036854775808

#define _MAX_ULLONG          18446744073709551615
#define _MIN_ULLONG          0

#define _PI_                 3.14159f
#define _TWO_PI_             6.28318f //2.0*_PI_;
#define _HALF_PI_            1.570795 //0.5*_PI_;

#define _1KB_                1024
#define _1MB_                1048576

#define _RADIAN(a)           (float)(a / 180)) * _PI_

#define _ISINT(num)          ( ( num == ( ( int64_t )num ) )? true: false )

#ifndef ABS
    #define ABS(xx)              ( ( xx ) < 0 ? ( -( xx ) ) : ( xx ) )
#endif


// 在2008之下 這個不能定義
// #ifndef BOOLEAN
// #define BOOLEAN(b)     ((b) ? 1 : 0)
// #endif

#define BETWEEN(a, b, c)     ((((a) <  (b)) && ((b) <  (c))) ? 1 : 0)
#define BETWEENR(a, b, c)    ((((a) <  (b)) && ((b) <= (c))) ? 1 : 0)
#define BETWEENL(a, b, c)    ((((a) <= (b)) && ((b) <  (c))) ? 1 : 0)
#define BETWEENLR(a, b, c)   ((((a) <= (b)) && ((b) <= (c))) ? 1 : 0)

//_______________________________________________________________
// binary buffer;
#ifndef _binbuf
    #define _binbuf   std::vector<ubyte_t>
#endif

//_______________________________________________________________
// obj-c must put this before max and min marco ....
//#include <string>
//"wcs" 是 "wide-chararater string" 的縮寫， wide - char 是指一個 wchar_t type 就是一個字
//"mbs" 是 "multi-byte string" 的縮寫，multi - byte 是指數個 char 組成 一個字( 如全形字或中文字是由兩個 char 組成 )
//
#ifndef _ustring  // utf8
    #define _ustring  std::string
#endif

#ifndef _mstring
    #define _mstring  std::string
#endif

#ifndef _wstring
    #define _wstring  std::wstring
#endif

//================= UNICODE ================
#if defined(_UNICODE) || defined(UNICODE)
    #ifndef _tstring
        #define _tstring _wstring
    #endif
    //================== ANSI =================
#else////defined(_UNICODE) || defined(UNICODE)
    #ifndef _tstring
        #define _tstring _mstring
    #endif
#endif////defined(_UNICODE) || defined(UNICODE)


//_______________________________________________________________
// string = [0]
#ifndef _mbsvector
    #define _mstring_vector    std::vector<_mstring>
    #define _mbsvector         std::vector<_mstring>
#endif                         
                               
#ifndef _wcsvector             
    #define _wstring_vector    std::vector<_wstring>
    #define _wcsvector         std::vector<_wstring>
#endif                         
                               
#ifndef _tcsvector             
    #define _tstring_vector    std::vector<_tstring>
    #define _tcsvector         std::vector<_tstring>
#endif                         

//_______________________________________________________________
// string = [str_a]
#ifndef _mbsmap                
    #define _mstring_map       std::map<_mstring, _mstring>
    #define _mbsmap            std::map<_mstring, _mstring> 
#endif                         
                               
#ifndef _wcsmap                
    #define _wstring_map       std::map<_wstring, _wstring>
    #define _wcsmap            std::map<_wstring, _wstring>
#endif                         
                               
#ifndef _tcsmap                
    #define _tstring_map       std::map<_tstring, _tstring>
    #define _tcsmap            std::map<_tstring, _tstring>
#endif                      

//_______________________________________________________________
// vector< vector< string > >
//  string = [0][0]
#ifndef _mbstableii
    #define _mstring_tableii   std::vector<_mstring_vector>
    #define _mbstableii        std::vector<_mstring_vector>
#endif

#ifndef _wcstableii
    #define _wstring_tableii   std::vector<_wstring_vector>
    #define _wcstableii        std::vector<_wstring_vector>
#endif

#ifndef _tcstableii
    #define _tstring_tableii   std::vector<_tstring_vector>
    #define _tcstableii        std::vector<_tstring_vector>
#endif

//_______________________________________________________________
// map< vector< string > >
// string = [str_a][0]
#ifndef _mbstablesi
    #define _mstring_tablesi   std::map<_mstring, _mstring_vector>
    #define _mbstablesi        std::map<_mstring, _mstring_vector>
#endif                         
                               
#ifndef _wcstablesi            
    #define _wstring_tablesi   std::map<_wstring, _wstring_vector>
    #define _wcstablesi        std::map<_wstring, _wstring_vector>
#endif                         
                               
#ifndef _tcstablesi            
    #define _tstring_tablesi   std::map<_tstring, _tstring_vector>
    #define _tcstablesi        std::map<_tstring, _tstring_vector>
#endif

//_______________________________________________________________
// map< map< string > >
// string = [str_a][str_b]
#ifndef _mbstabless
    #define _mstring_tabless   std::map<_mstring, std::map<_mstring, _mstring>>
    #define _mbstabless        std::map<_mstring, std::map<_mstring, _mstring>>
#endif                         
                               
#ifndef _wcstabless            
    #define _wstring_tabless   std::map<_wstring, std::map<_wstring, _wstring>>
    #define _wcstabless        std::map<_wstring, std::map<_wstring, _wstring>>
#endif                         
                               
#ifndef _tcstabless            
    #define _tstring_tabless   std::map<_tstring, std::map<_tstring, _tstring>>
    #define _tcstabless        std::map<_tstring, std::map<_tstring, _tstring>>
#endif

//_______________________________________________________________
//
#ifndef _mbslist               
    #define _mstring_list      std::list<_mstring>
    #define _mbslist           std::list<_mstring>
#endif                         
                               
#ifndef _wcslist               
    #define _wstring_list      std::list<_wstring>
    #define _wcslist           std::list<_wstring>
#endif                         

#ifndef _tcslist               
    #define _tstring_list      std::list<_tstring>
    #define _tcslist           std::list<_tstring>
#endif                         

//_______________________________________________________________
#ifndef _mbstset               
    #define _mstring_set       std::set<_mstring>
    #define _mbstset           std::set<_mstring> 
#endif                         
                               
#ifndef _wcstset               
    #define _wstring_set       std::set<_wstring>
    #define _wcstset           std::set<_wstring>
#endif                         
                               
#ifndef _tcstset               
    #define _tstring_set       std::set<_tstring>
    #define _tcstset           std::set<_tstring>
#endif                         

//_______________________________________________________________
#ifndef _tcsidx                
    #define _tstring_idx       std::map<_tstring, int64_t>
    #define _tcsidx            std::map<_tstring, int64_t>
#endif


//_______________________________________________________________
#ifndef Clamp
    #define Clamp(x, min, max)   ((x) = ((x) < (min) ? (min) : (x) < (max) ? (x) : (max)));
#endif

// win32 define in stdlib
#ifndef __max
    #define __max(a,b)           (((a) > (b)) ? (a) : (b))
#endif

#ifndef __min
    #define __min(a,b)           (((a) < (b)) ? (a) : (b))
#endif

// win32 define in stdlib
//#ifndef NOMINMAX
//    #ifndef max
//        #define max(a,b)            (((a) > (b)) ? (a) : (b))
//    #endif
//    #ifndef min
//        #define min(a,b)            (((a) < (b)) ? (a) : (b))
//    #endif
//#endif  /* NOMINMAX */

#ifndef GETMAXADD
    #define GETMAXADD(v, a, M, ra)       if((v + a) > M){ra = M - v;} else{ ra = a ;}
#endif

/* =========================================================================== */
//位元欄位的設定/取消/檢查
#define SET_BIT_VALUE(flag,bit)          ( (flag) |=  (bit))
#define CANCEL_BIT_VALUE(flag,bit)       ( (flag) &= ~(bit))
#define IF_BIT_VALUE(flag,bit)           (((flag) &   (bit)) != 0)
#define IF_BIT_VALUE_EQUAL(flag,bit)     (((flag) &   (bit)) == (bit))

// 傳入第幾個bit
#define SET_BIT_FLAG(flag,bit)           ( (flag) |=  (1<<bit))
#define CANCEL_BIT_FLAG(flag,bit)        ( (flag) &= ~(1<<bit))
#define IF_BIT_FLAG(flag,bit)            (((flag) &   (1<<bit)) != 0)
#define IF_BIT_FLAG_EQUAL(flag,bit)      (((flag) &   (1<<bit)) == (bit))

#ifndef SAFE_DELETE
    #define SAFE_DELETE(a)               { if (a){ delete (a); a = NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(a)         { if (a){ delete [] (a); a = NULL; } }
#endif

//STL
#ifndef SAFE_RELEASE
    #define SAFE_RELEASE(a)              { if (a) { a->Release(); a = NULL; } }
#endif

#define DELETE_PVECTOR(v, i)             for(i = 0; i< v.size(); i++){ delete v[i]; } v.clear()

#define SWAPL(a,b)                       { long32_t  c = b; b = a; a = c; }
#define SWAPSS(a,b)                      { char* c = b; b = a; a = c; }
#define SWAPWS(a,b)                      { wchar_t* c = b; b = a; a = c; }
#define SWAPUS(a,b)                      { unsigned short* c = b; b = a; a = c; }

// macro 的寫法 可以像function一樣 寫在 start 與 stop之間
#define MACRO_START                      do
#define MACRO_STOP                       while ( 0 )

#ifndef  MAKELLONG
    #define MAKELLONG(a, b)              ((qword_t)(((dword_t)(a)) | ((qword_t)((dword_t)(b))) << 32))
#endif

#ifndef  LODWORD
    #define LODWORD(q)                   ((dword_t)(q))
#endif

#ifndef  HIDWORD
    #define HIDWORD(q)                   ((dword_t)(((qword_t)(q) >> 32)  &0xFFFFFFFF))
#endif

#ifndef _IN
    #define _IN
#endif

#ifndef _OUT
    #define _OUT
#endif

#ifndef _INOUT
    #define _INOUT
#endif

#ifndef _WIN32EOL_
    #define _WIN32EOL_           "\r\n"
#endif

#ifndef _UNIXEOL_
    #define _UNIXEOL_            "\n"
#endif

#ifndef _MACEOL_
    #define _MACEOL_             "\r"
#endif

#ifndef _MAX_PATH
    #define _MAX_PATH            260  /* max. length of full pathname */
#endif
#ifndef _MAX_DRIVE
    #define _MAX_DRIVE           3   /* max. length of drive component */
#endif
#ifndef _MAX_DIR
    #define _MAX_DIR             256  /* max. length of path component */
#endif
#ifndef _MAX_FNAME
    #define _MAX_FNAME           256  /* max. length of file name component */
#endif
#ifndef _MAX_EXT
    #define _MAX_EXT             256  /* max. length of extension component */
#endif

#define _MAX_LPATH               1032 /* max. length of full pathname */
#define _MAX_LDRIVE              3    /* max. length of drive component */
#define _MAX_LDIR                1024 /* max. length of path component */
#define _MAX_LFNAME              1024 /* max. length of file name component */
#define _MAX_LEXT                1024 /* max. length of extension component */

#if defined(_WINDOWS)
    #define __func__ __FUNCTION__
#endif

#ifndef _CALLBACK 
    #define _CALLBACK
#endif



///////////////////////////////////////////////////////////////////////////////////////
// about compiler
//
//
///////////////////////////////////////////////////////////////////////////////////////
//_______________________________________________________________
// VC
#ifdef __MSVC__
    #define __ASM_BEGIN__   __asm{
    #define __ASM_END__     }
    //#define __AL
    //#define AR__
    #define __volatile__
    #define __SLAPI      __declspec(dllexport) // linux .so [shared objects], window .dll
    //#define __PRAGMA_PUSH(n)  #pragma pack (push, n)
    //#define __PRAGMA_POP   #pragma pack ( pop )
    #define __ALIGNED(n)
#endif
//_______________________________________________________________
// GUNC
#ifdef __GNUC__
    #define __ASM_BEGIN__   __asm(
    #define __ASM_END__   );
    //#define __AL      "\""
    //#define AR__      "\""
    #define __SLAPI           // linux .so [shared objects], window .dll
    #define __cdecl           // different between PC and Mac
    //#define __PRAGMA_PUSH(n)
    //#define __PRAGMA_POP
    #define __ALIGNED(n)   __attribute__ ((aligned(n)))
#endif
/* =========================================================================== */


///////////////////////////////////////////////////////////////////////////////////////
// windows platform
//
///////////////////////////////////////////////////////////////////////////////////////
#ifdef _XBOX
    #include <XTL.h>
#endif

#if defined(_WINDOWS)

#define OutPutMessage(s, t)     MessageBox(NULL, s, t, MB_OK)

struct timezone
{
    int tz_minutestwest;
    int tz_dsttime;
};

#define MINBSDSOCKERR     (WSAEWOULDBLOCK)
#define MAXBSDSOCKERR     (MINBSDSOCKERR + \
                           (sizeof(bsdsocketerrs) / \
                            sizeof(bsdsocketerrs[0])))

#ifndef EMSGSIZE
    #define EMSGSIZE             WSAEMSGSIZE
#endif

#define set_errno(e)         SetLastError((e))
#define isvalidsock(s)       ((s) != SOCKET_ERROR)
#define bzero(b, n)          memset((b), 0, (n))


#ifndef _EOL_
    #define _EOL_                _WIN32EOL_
#endif

//#define _WIN32_WINNT 0x0500 //
#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _m128_get_f32
    #define _m128_get_f32( m, i )  m.m128_f32[i]
#endif

#endif //_WINDOWS

///////////////////////////////////////////////////////////////////////////////////////
// unix platform
//
///////////////////////////////////////////////////////////////////////////////////////
#if defined(_LINUX)
    // Under Linux, The two lines contains " will not compiled. I don't know why.
    //#if defined(WIN32)
    // #define __asm      asm
    
    
    #define MAKEWORD(a, b)       ((word_t) (((ubyte_t)(a))  | ((word_t)((ubyte_t)(b))) << 8))
    #define LOBYTE(w)            ((ubyte_t)(w))
    #define HIBYTE(w)            ((ubyte_t)(((word_t)(w) >> 8)  &0xFF))

    #define MAKELONG(a, b)       ((LONG) (((word_t)(a))  | ((dword_t)((word_t)(b))) << 16))
    #define LOWORD(l)            ((word_t)(l))
    #define HIWORD(l)            ((word_t)(((dword_t)(l) >> 16)  &0xFFFF))

    #define OutPutMessage(s, t)  printf("%s:%s", t, s)
    
    #define set_errno(e)         errno = e
    #define isvalidsock(s)       ((s) >= 0)
    
    #ifndef _EOL_
        #define _EOL_                _UNIXEOL_
    #endif
    
    #ifndef _m128_get_f32
        #define _m128_get_f32( m, i )  ((float*)&m)[i]
    #endif
    
#endif//_LINUX

///////////////////////////////////////////////////////////////////////////////////////
//__MACOS__
//
///////////////////////////////////////////////////////////////////////////////////////









///////////////////////////////////////////////////////////////////////////////////////
//__FreeBSD__
//
///////////////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////////////
// define type
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
// use for undefine type like int128
template <class _Type, unsigned int _Size>
class undeftype
{
    _Type m_buff[_Size];
    
public:

    undeftype( const undeftype& st )          { memcpy( m_buff, st.m_buff, _Size * sizeof( _Type ) ); }
    undeftype()                               {}
    ~undeftype()                              { memset( m_buff, 0, _Size * sizeof( _Type ) ); }
    
    _Type&     operator[] ( unsigned int id ) { return m_buff[id]; }
    int        operator== ( undeftype st )    { return ( memcmp( m_buff, st.m_buff, _Size * sizeof( _Type ) ) == 0 ) ? 1 : 0; }
    undeftype& operator=  ( undeftype st )    { memcpy( m_buff, st.m_buff, _Size * sizeof( _Type ) ); return ( *this ); }
    undeftype& operator=  ( _Type* st )       { memcpy( m_buff, st, _Size * sizeof( _Type ) ); return ( *this ); }
    void       clear      ( _Type v = 0 )     { memset( m_buff, v, _Size * sizeof( _Type ) ); }
};
#endif