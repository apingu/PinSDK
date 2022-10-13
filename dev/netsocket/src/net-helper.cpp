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
#if defined(_WINDOWS)
//#include <Iphlpapi.h>
//#pragma comment (lib, "Iphlpapi.lib")   /* link with Win32 lib */
#include "iocp-def.h"
#include <wincon.h>
#include <wsipx.h>
#include <Wsnwlink.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <time.h>

//#include <lm.h>//for nic address
//#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Netapi32.lib")//for nic address
//#include <Nb30.h>
#else
#include "epoll-def.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include <sys/sockio.h>
#include <net/if.h>
//#include <net/if_dl.h>
#include <sys/uio.h>
#include <ifaddrs.h>
#include <time.h>

#define MAXLINE 10
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 5555
#define INFTIM 1000
//sighndlr_t* signal(int sig,sighndlr_t* hndlr);
//void  init(char** argv);
#endif
#include <errno.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "net-helper.h"
#include "unp.h"


namespace net
{
    void NTPUpdate()
    {
        const char*  hostname = "64.27.26.1";   //"tick.usno.navy.mil";
        int    portno = 123;   //NTP is port 123
        //int    maxlen = 1024;     //check our buffers
        long32_t   i;    // misc var i
        char   msg[48] = { 010, 0, 0, 0, 0, 0, 0, 0, 0 }; // the packet we send
        char*  buf = new char[1024]; // the buffer we get back
        //struct in_addr ipaddr;  //
        struct protoent* proto;   //
        struct sockaddr_in server_addr;
        socket_t s;      // socket
        time_t tmit;   // the time -- This is a time_t sort of
#ifdef _WINDOWS
        //=====================================================================================
        //THIS IS WHAT IS MISSING MAJORILY
        //=====================================================================================
        //Initialise the winsock stack
        WSADATA wsaData;
        BYTE wsMajorVersion = 1;
        BYTE wsMinorVersion = 1;
        WORD wVersionRequested = MAKEWORD( wsMinorVersion, wsMajorVersion );

        if ( WSAStartup( wVersionRequested, &wsaData ) != 0 )
        {
            //_tprintf(_T("Failed to load winsock stack\n"));
            WSACleanup();
            return;
        }

        if ( LOBYTE( wsaData.wVersion ) != wsMajorVersion || HIBYTE( wsaData.wVersion ) != wsMinorVersion )
        {
            //_tprintf(_T("Winsock stack does not support version which this program requires\n"));
            WSACleanup();
            return;
        }

        //=====================================================================================
#endif
        //use Socket;
        //
        //#we use the system call to open a UDP socket
        //socket(SOCKET, PF_INET, SOCK_DGRAM, getprotobyname("udp")) or die "socket: $!";
        proto = getprotobyname( "udp" );
        //int err = errno;
        s = socket( PF_INET, SOCK_DGRAM, proto->p_proto );

        if ( s )
        {
            perror( "asd" );
            printf( "socket = %lld\n", s );
        }

        //
        //#convert hostname to ipaddress if needed
        //$ipaddr   = InetAToN4($HOSTNAME);
        memset( &server_addr, 0, sizeof( server_addr ) );
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr( hostname );
        //argv[1] );
        //i   = InetAToN4(hostname,&server_addr.sin_addr);
        server_addr.sin_port = htons( portno );
        //printf("ipaddr (in hex): %x\n",server_addr.sin_addr);
        /*
         * build a message.  Our message is all zeros except for a one in the
         * protocol version field
         * msg[] in binary is 00 001 000 00000000
         * it should be a total of 48 bytes long32_t
        */
        // send the data
        printf( "sending data..\n" );
        i = sendto( s, msg, sizeof( msg ), 0, ( struct sockaddr* )&server_addr, sizeof( server_addr ) );
        int iResult = -1;
        // Receive until the peer closes the connection
        //do {
        iResult = recv( s, buf, 1024, 0 );

        if ( iResult > 0 )
        {
            printf( "Bytes received: %d\n", iResult );
        }
        else if ( iResult == 0 )
        {
            printf( "Connection closed\n" );
        }
        else
        {
            printf( "recv failed: %d\n", errno );
        }

        //} while( iResult > 0 );
        /*
         * The high word of transmit time is the 10th word we get back
         * tmit is the time in seconds not accounting for network delays which
         * should be way less than a second if this is a local NTP server
         */
        tmit = ntohl( ( time_t )buf[10] ); //# get transmit time
        //printf("tmit=%d\n",tmit);
        /*
         * Convert time to unix standard time NTP is number of seconds since 0000
         * UT on 1 January 1900 unix time is seconds since 0000 UT on 1 January
         * 1970 There has been a trend to add a 2 leap seconds every 3 years.
         * Leap seconds are only an issue the last second of the month in June and
         * December if you don't try to set the clock then it can be ignored but
         * this is importaint to people who coordinate times with GPS clock sources.
         */
        tmit -= 2208988800U;
        //printf("tmit=%d\n",tmit);
        /* use unix library function to show me the local time (it takes care
         * of timezone issues for both north and south of the equator and places
         * that do Summer time/ Daylight savings time.
         */
         //#compare to system time
        printf( "Time: %s", ctime( &tmit ) );
        i = time( 0 );
        //printf("%d-%d=%d\n",i,tmit,i-tmit);
        printf( "System time is %lld seconds off\n", i - tmit );
    }
}