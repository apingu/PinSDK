//pdef.h header file
//
//
//
//
//
//
//
//
//      base type define 
//
//
//
//                                               Copyright    (C)    1999    Pin
//
#ifndef PDEF_H
#define PDEF_H


typedef char           Pchar;
typedef void           Pvoid;
typedef int            Pbool;
typedef int            Pint;
typedef long           Plong;
typedef float          Pfloat;
typedef double         Pdouble;
typedef char*          Pstr;
typedef const char*    Pcstr;
typedef signed  char   Pschar;
typedef unsigned char  Pbyte;
typedef unsigned short Pword;
typedef unsigned int   Pdword;
typedef unsigned char  Puchar;
typedef unsigned short Pushort;
typedef unsigned int   Puint;
typedef unsigned long  Pulong;
typedef int            Perror;
typedef void*          Pdata;

//typedef char TCHAR;
typedef int            error_t;

#if !defined(UINT16)
typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned int   UINT32;
#endif


#ifdef _WIN32
    typedef  __int64          int64_t;
    typedef unsigned short    u_int16_t;
    typedef unsigned long     u_int32_t;
    typedef unsigned __int64  u_int64_t;
    typedef unsigned __int64  Pqword;
#endif

#ifdef _UNIX
    typedef longlong          int64_t;
    typedef unsigned short    u_int16_t;
    typedef unsigned long     u_int32_t;
    typedef unsigned longlong u_int64_t;
    typedef unsigned longlong Pqword;
#endif




//#ifndef _WIN32
    typedef unsigned long      DWORD;
    typedef int                BOOL;
    typedef unsigned char      BYTE;
    typedef unsigned short     WORD;
    typedef float              FLOAT;
    typedef FLOAT*             PFLOAT;
    typedef BOOL*              PBOOL;
    typedef BOOL*              LPBOOL;
    typedef BYTE*              PBYTE;
    typedef BYTE*              LPBYTE;
    typedef int*               PINT;
    typedef int*               LPINT;
    typedef WORD*              PWORD;
    typedef WORD*              LPWORD;
    typedef long*              LPLONG;
    typedef DWORD*             PDWORD;
    typedef DWORD*             LPDWORD;
    typedef void*              LPVOID;
    typedef int                INT;
    typedef long               LONG;
    typedef unsigned int       UINT;
    typedef unsigned int*      PUINT;
//#endif

//
typedef unsigned long  mstime_t;
//
typedef u_int64_t      QWORD;
//
typedef int            Pflag;
//
typedef void sighndlr_t( int );



typedef enum _PIXELFORMAT
{
    PIXEL_UNKNOW           = 0X0000,
    PIXEL_R5G6B5           = 0X0102,
    PIXEL_R5G5B5           = 0X0202,
    PIXEL_R5G5B6           = 0X0402,
    PIXEL_R6G5B5           = 0X1002,
    PIXEL_R8G8B8           = 0X2003,
    PIXEL_R8G8B8A8         = 0X4004,
    PIXEL_A8R8G8B8         = 0X8004,
	PIXEL_A8B8G8R8         = 0XC004
} PIXELFMT;


////////////////////////////////////////////////////////////////
//   01?? mouse      02?? keyboard
enum eventmsg_t
{
    PE_NULL           =    0X0000,
    PE_MOUSEOVER      =    0X0101,
    PE_MOUSEMOVE      =    0X0102,
    PE_MOUSEDOCK      =    0X0103,
    PE_LBUTTONDOWN    =    0X0110,
    PE_LBUTTONUP      =    0X0111,
    PE_LDBCLICK       =    0X0112,
    PE_LSLIPDOWN      =    0X0116,
    PE_RBUTTONDOWN    =    0X0120,
    PE_RBUTTONUP      =    0X0121,
    PE_RDBCLICK       =    0X0122,
    PE_RSSLIPDOWN     =    0X0126,
    PE_MOUSEDRAG      =    0X0180,
    PE_MOUSEDRAGDROP  =    0X0181,
    PE_KEYDOWN        =    0X0201,
    PE_KEYUP          =    0X0202,
    PE_OMIT           =    0XFFFF
};


#ifndef FALSE
#define FALSE                   0
#endif

#ifndef TRUE
#define TRUE                    1
#endif

#ifndef _FULL_ALPHA
#define _FULL_ALPHA             0XFF
#endif

#ifndef _MAX_MSG
#define _MAX_MSG                256
#endif

#ifndef _MAX_PATH
#define _MAX_PATH               260
#endif

#ifndef _MAX_NAME
#define _MAX_NAME               256
#endif

#ifndef _MAX_IPv4
#define _MAX_IPv4               32
#endif

#ifndef _MAX_IPv6
#define _MAX_IPv6               128
#endif

#define _MAX_UINT               0XFFFFFFFF


#define _PI_                    3.14159f
#define _TWO_PI_                6.28318f //2.0*_PI_;
#define _HALF_PI_               1.570795 //0.5*_PI_;

#define _1KB_                   1024
#define _1MB_                   1048576


#define _RADIAN( a )            ( (float)(a / 180) ) * _PI_

#define ABS(xx)                 ( (xx) < 0 ? (-(xx)) : (xx) )
#define BOOLEAN( b )            ( (b) ? 1 : 0 )
#define BETWEEN( a, b, c )      ( ( ( (a) < (b) ) && ( (b) < (c) ) ) ? 1 : 0 )
#define BETWEENR( a, b, c )     ( ( ( (a) < (b) ) && ( (b) <= (c) ) ) ? 1 : 0 )
#define BETWEENL( a, b, c )     ( ( ( (a) <= (b) ) && ( (b) < (c) ) ) ? 1 : 0 )
#define BETWEENLR( a, b, c )    ( ( ( (a) <= (b) ) && ( (b) <= (c) ) ) ? 1 : 0 )
#ifndef Clamp
#define Clamp(x, min, max)      (x) = ((x) < (min) ? (min) : (x) < (max) ? (x) : (max));
#endif


#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


inline void SWAPL( long& a,long& b ){ long c  = b; b = a; a = c; }



///////////////////////////////////////////////////////////////////////////////////////
// windows platform
#ifdef _WIN32

#ifdef _XBOX
#include <XTL.h>
#endif


//#define asm                 __asm 
#define __ASM_BEGIN__       __asm{
#define __ASM_END__           }
#define _AL                   
#define  AL_
#define __asm__               asm
#define __volatile__
#define asm(a)               __asm{a}
#define __DLLAPI             __declspec(dllexport)


#define OutPutMessage( s, t )    MessageBox( NULL, s, t, MB_OK )

struct timezone
{
    long tz_minutestwest;
    long tz_dsttime;
};


#define MINBSDSOCKERR     ( WSAEWOULDBLOCK )
#define MAXBSDSOCKERR     ( MINBSDSOCKERR + \
                          ( sizeof( bsdsocketerrs ) / \
                          sizeof( bsdsocketerrs[0] ) ) )

//unix compare
//#ifndef EINTR
//#define EINTR               WSAEINTR//ERRNO.h
//#endif

#define EMSGSIZE           WSAEMSGSIZE
#define set_errno(e)       SetLastError( (e) )
#define isvalidsock(s)     ( ( s ) != SOCKET_ERROR )
#define bzero(b, n)        memset( (b), 0, (n) )
#define sleep(t)          Sleep( t )





//#endif //_WIN32

//#ifdef _UNIX
#else
///////////////////////////////////////////////////////////////////////////////////////
// unix platform


// Under Linux, The two lines contains " will not compiled. I don't know why.
#if defined(_WIN32)
	#define __asm               asm
	#define __ASM_BEGIN__       asm(
	#define __ASM_END__          )
//	#define _AL                  "
//	#define AL_                  "
#endif

#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */

#ifndef NOMINMAX
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define OutPutMessage(s, t)    printf("%s:%s", t, s)

#define set_errno(e)        errno = e
#define isvalidsock(s)      ( ( s ) >= 0 )
//#define PSleep(t)           sleep(t)
//__MACOS__
//__FreeBSD__

#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) { if (a) { a->Release(); a=NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(a)  { if (a) { delete a; a=NULL; } }
#endif


#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a)	{ if (a){ delete [] a; a=NULL; } }
#endif


#define DELETE_PVECTOR(v, i)   for( i=0; i< v.size(); i++ )\
                                    delete v[i];\
                                v.clear()

//#define WAITFLAG( f )          while(f)
//#define LOCKFLAG( f )          f=true
//#define UNLOCKFLAG( f )        f=false


// use for undefine type like int128
template <class _Type, unsigned int _Size >
class undeftype
{
    _Type m_buff[_Size];
	
public:
	
    undeftype( const undeftype& st ){ memcpy( m_buff, st.m_buff, _Size*sizeof(_Type) ); }
	undeftype(){}
	~undeftype(){}

	
	//operator   _Type*(){ return m_buff; }
	_Type&     operator[] ( unsigned int id ){ return m_buff[id]; }
	int        operator== ( undeftype st ){ return (memcmp( m_buff, st.m_buff, _Size*sizeof(_Type) )==0)?1:0; }
    undeftype& operator= ( undeftype st ){ memcpy( m_buff, st.m_buff, _Size*sizeof(_Type) ); return (*this); }
	undeftype& operator= ( _Type* st ){ memcpy( m_buff, st, _Size*sizeof(_Type) ); return (*this); }

	void       clear( _Type v=0 ){ memset( m_buff, v, _Size*sizeof(_Type) ); }
};


typedef void* (*SaveDataCallBack)( Puint& len, void* buf );//, size_t buflen );
typedef void* (*LoadDataCallBack)( Puint& len, void* buf );//, size_t buflen );




/*
/// UNICODE
#pragma warning (disable:4530 4786)

#include <iostream>
#include <sstream>
#include <string>

//================= UNICODE  ================
#ifdef _UNICODE

typedef std::wstring String;
typedef std::wistream InStream;
typedef std::wostream OutStream;
typedef std::wistringstream InStrStream;
typedef std::wostringstream OutStrStream;
#define COut  std::wcout

#ifndef _T
typedef wchar_t _TCHAR;
#define _T(x)  L##x
#endif

//================= ANSI ================
#else

typedef std::string String;
typedef std::istream InStream;
typedef std::ostream OutStream;
typedef std::istringstream InStrStream;
typedef std::ostringstream OutStrStream;
#define COut  std::cout

#ifndef _T
typedef char _TCHAR;
#define _T(x)  x
#endif

#endif


//======= Conversions between UNICODE and ANSI string =============
inline
std::wstring &operator<<(std::wstring &ws, const std::string &as)
{
    wchar_t *buf = new wchar_t[as.size()+1];
    
	int length = mbstowcs(buf,as.c_str(),as.size());

	if(length >=0)
	{
		buf[length] = 0;
		ws = buf;
	}
	
	delete [] buf;
    return ws;
}

inline
std::string &operator<<(std::string &as, const std::wstring &ws)
{
    char *buf = new char[ws.size()*2+1];
    
	int length = wcstombs(buf,ws.c_str(),ws.size());
	
	if(length >=0)
	{
		buf[length] = 0;
		as = buf;
	}

    delete [] buf;
	return as;
}

inline
std::wstring operator>>(const std::wstring &ws, std::string &as)
{
	as << ws;
	return ws;
}

inline
std::string operator>>(const std::string &as, std::wstring &ws)
{
	ws << as;
	return as;
}

inline
std::wstring &operator<<(std::wstring &ws, const char *as)
{
    return ws << std::string(as);
}

inline
std::string &operator<<(std::string &as, const wchar_t *ws)
{
	return as << std::wstring(ws);
}
*/




#endif// PDEF_H
