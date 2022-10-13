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
//
#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4786)

#include <pf-def.h>
#if defined(_WINDOWS)  /* Windows platforms */
    #include <winsock2.h>
    #include <MSWSOCK.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #include <stdio.h>
    //                  
    #pragma comment(lib, "Ws2_32.lib")
    #pragma comment (lib, "Mswsock.lib")
    //#ifndef _INC_WINDOWS  /* already define windows.h */
    //    #include <winsock2.h>
    //    #include <ws2tcpip.h>
    //    #include <MSWSOCK.h>
    //    #include <Ws2ipdef.h>
    //    #include <iphlpapi.h>
    //    #pragma comment (lib, "Ws2_32.lib")
    //    #pragma comment (lib, "Mswsock.lib")
    //#else
    //    #include <winsock.h>
    //#endif
    //#include <In6addr.h>
#endif
#if defined(_LINUX)
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <string.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <stdint.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <vector>

//
//
using namespace std;

#if defined(_WINDOWS) /* Windows platforms */
    
    //void init( tchar_t** argv );
    //#define INIT()       init(argv);
    typedef SOCKET       socket_t;
    typedef int          socklen_t;
    #define ioctl        ioctlsocket
    //typedef uint_t socklen_t
    //#define close(x) closesocket(x)
    
#else
    typedef uint_t       SOCKET;
    typedef uint_t       socket_t;
    
    //sighndlr_t* signal( int sig, sighndlr_t* hndlr );
    
    //#define INIT()   (program_name = \
    //                  strrchr(argv[0], '/')) ? \
    //                  program_name++ : \
    //                  (program_name = argb[0])
    
    
    //typedef hostent *LPHOSTENT;
    
#endif

#ifdef FD_SETSIZE
    #undef FD_SETSIZE
#endif
#define  FD_SETSIZE             1024

#ifndef _MAX_HOST_NAME
    #define _MAX_HOST_NAME      256
#endif

#define   EXIT(s)               exit(s)
#define   CLOSE(s)              if( close( s ) ) error( 1, errno, _T("close failed") );

#define   MSG_INTERVAL          200
#define   NLISTEN               5
#define   NSMB                  5
#define   SMBUFSZ               256

typedef   in_addr               ipn4_t;
typedef   in6_addr              ipn6_t;
typedef   std::string           ipa_t;
typedef   sockaddr_storage      addrs_t;
typedef   longlong_t            macn_t;
typedef   uchar_t*              maca_t;

typedef   char_t                ntchar_t;

#ifndef INVALID_SOCKET
    #define INVALID_SOCKET      0
#endif
#ifndef NULL_SOCKET
    #define NULL_SOCKET         0
#endif

#ifndef SOCKET_ERROR
    #define SOCKET_ERROR       -1
#endif

#ifndef SD_RECEIVE
    #define SD_RECEIVE          0x00
#endif

#ifndef SD_SEND
    #define SD_SEND             0x01
#endif

#ifndef SD_BOTH
    #define SD_BOTH             0x02
#endif

#ifndef _LONG_BUFFER_SIZE_
    #define _LONG_BUFFER_SIZE_  1024
#endif

#ifndef _SHORT_BUFFER_SIZE_
    #define _SHORT_BUFFER_SIZE_ 256
#endif

#define _MAX_SOCKET_BUFF        81920

#define RCV_BUF                 SO_RCVBUF
#define SNE_BUF                 SO_ENDBUF

#ifndef NI_MAXHOST
    #define NI_MAXHOST          1025
#endif

#ifndef NI_MAXSERV
    #define NI_MAXSERV          32
#endif


struct Databuf
{
    size32_t len;
    ubyte_t* buf;
};


#if defined(_WINDOWS) /* Windows platforms */
    #define _IF_TYPE_ETHERNET    IF_TYPE_ETHERNET_CSMACD    // ethernet network interface
    #define _IF_TYPE_LOOKBACK    IF_TYPE_SOFTWARE_LOOPBACK  // A software loopback network interface.
#else
    #define _IF_TYPE_ETHERNET    6
    #define _IF_TYPE_LOOKBACK    24
#endif


struct EthIF
{
    std::string          name;               // interface name
    macn_t               macn;               // mac address
    dword_t              iftype;
    std::vector<addrs_t> unicast_addrs;      // unicast  address
    std::vector<addrs_t> multicast_addrs;    // mutlicast or boradcast address
    std::vector<addrs_t> netmask;            // netmask
};


//#define   SOCKET_ERROR -1


