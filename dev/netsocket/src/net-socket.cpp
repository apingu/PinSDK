//Socket.cpp base socket class
//
//
//
//
//
//   use for all stand server
//               Apingu
//                  Sat Mar 16 2002
//

//01. == C 系統文件/C++ 系統文件
#include <assert.h>
#include <map>
#include <set>
#include <algorithm>
#if defined(_WINDOWS)
//#include <Iphlpapi.h>
//#pragma comment (lib, "Iphlpapi.lib")   /* link with Win32 lib */
#include "iocp-def.h"
#include <wincon.h>
#include <wsipx.h>
#include <Wsnwlink.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <lm.h>//for nic address
//#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Netapi32.lib") //for nic address
#pragma comment(lib, "Iphlpapi.lib") //for ip  address
//#include <Nb30.h>
#endif
#ifdef _LINUX
#include "epoll-def.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include <sys/sockio.h>
#include <net/if.h>
#include <errno.h>
//#include <net/if_dl.h>
#include <ifaddrs.h>
#include <stdarg.h>

#define OPEN_MAX  100
#define LISTENQ   20
#define SERV_PORT 5555
#define INFTIM    1000
//sighndlr_t* signal(int sig,sighndlr_t* hndlr);
//void  init(char** argv);

#endif

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <pf-debug.h>
//
#include <memio.inl>

//03. == 其他 使用 函式庫的.h 文件


//04. == 此專案內.h 文件
#include "net-socket.h"



//void GetGetAdaptersMacAddress(void);

//char* program_name;

//uint_t g_tcpbuffsize=32168;
//////////////////////////////////////////////////////////////////////////////////////////////
//
//  socket class
//
namespace net
{

#if defined(_WINDOWS)
    int   g_bInitWinsocket = 0;
#endif

    //int Socket::count = 0;

    namespace
    {
        T_Lock            socket_uid_lock;
        dword_t           socket_uid_incr = 0;  // 每一個soket 建立時都會給予他一個編號

        //std::set<dword_t> uid_set;
        dword_t CreateUID()
        {
            TS_Lock lock( &socket_uid_lock );
            if( socket_uid_incr >= _MAX_INT )
                socket_uid_incr = 0;

            socket_uid_incr++;
            return socket_uid_incr;

            //_mstring ss;
            //while ( 1 )
            //{
            //    TS_Lock lock( &uid_lock );
            //    pfstd::strprintf( ss, "socket_%s_%d", n, GetTickCount() );
            //    dword_t new_id = crc32::Encode( ss.c_str(), ss.size() );
            //    std::set<dword_t>::iterator it = uid_set.find( new_id );
            //    if ( it == uid_set.end() )
            //    {
            //        uid_set.insert( std::set<dword_t>::value_type( new_id ) );
            //        return;
            //    }
            //}

        }
    }

    //=============================================================================
    // NAME:
    // DESC: Constructor
    //=============================================================================
    Socket::Socket()
    {
        socket_uid_ = CreateUID();
        socket_fd_ = 0;
        stime_last_create_ = 0;
#if defined(_WINDOWS)

        if( g_bInitWinsocket == 0 )
        {
            WORD wVersionRequested;
            WSADATA wsaData;
            int err;
            wVersionRequested = MAKEWORD( 2, 2 );
            /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
            err = WSAStartup( wVersionRequested, &wsaData );

            //count++;
            if( err != 0 )
            {
                /* Tell the user that we could not find a usable */
                /* WinSock DLL. */
                net::Socket::PrintError( "ERROR: Start up winsock dll failed ~_~b !!!" );
                //MessageBox(NULL,"Start up failed"," Socket Failed", MB_OK);
            }
        }

        g_bInitWinsocket++;
#endif
    }

    //=============================================================================
    // NAME:
    // DESC: Distructor
    //=============================================================================
    Socket::~Socket()
    {
        //DeleteUId( m_uid );
        Close();
#if defined(_WINDOWS)
        g_bInitWinsocket--;

        if( g_bInitWinsocket == 0 )
        {
            WSACleanup();
        }

#endif
        //m_Blockbuf = NULL;
        //m_OPState = OP_INVALID;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Socket::operator socket_t& ( )
    {
        //TS_Lock lock( &m_SkLock );
        TS_Lock lock( &socket_ut_lock_ );
        return socket_fd_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t Socket::GetFD()
    {
        //TS_Lock lock( &m_SkLock );
        TS_Lock lock( &socket_ut_lock_ );
        return socket_fd_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    dword_t Socket::GetUID()
    {
        TS_Lock lock( &socket_ut_lock_ );
        return socket_uid_;
    }

    //=============================================================================
    // NAME: create_socket()
    // DESC: Create one sock
    //=============================================================================
    socket_t Socket::Create( _IN int domain, _IN int streamtype, _IN int protocol )
    {
        //m_SerialNumber = 0;
        //TS_Lock lock( &m_SkLock );
        TS_Lock lock( &socket_ut_lock_ );
        if( socket_fd_ != NULL_SOCKET )
        {
            net::Socket::PrintError( "ERROR: socket already create ~_~b:" );
            return 0;
        }

#if defined(_WINDOWS)

        if( ( socket_fd_ = WSASocket( domain, streamtype, protocol, NULL, 0, WSA_FLAG_OVERLAPPED ) ) == NULL_SOCKET )
        {
            //sprintf(StatuMsg,"Create Socket failed ~_~b !!!");
            net::Socket::PrintError( "ERROR: Create win32 socket fail ~_~b:" );
            //           switch(WSAGetLastError())
            //           {
            //           case WSANOTINITIALISED: DebugOutPutf("WSANOTINITIALISED\n", "socket"); assert(0&&"=WSANOTINITIALISED=\n"); break;
            //           case WSAENETDOWN:    DebugOutPutf("WSAENETDOWN\n", "socket");assert(0&&"=WSAENETDOWN=\n"); break;
            //           case WSAEAFNOSUPPORT:   DebugOutPutf("WSAEAFNOSUPPORT\n", "socket");assert(0&&"=WSAEAFNOSUPPORT=\n"); break;
            //           case WSAEINPROGRESS:    DebugOutPutf("WSAEINPROGRESS\n", "socket");assert(0&&"=WSAEINPROGRESS=\n"); break;
            //           case WSAEMFILE:   DebugOutPutf("WSAEMFILE\n", "socket");assert(0&&"=WSAEMFILE=\n"); break;
            //           case WSAENOBUFS:
            //               {
            //                   if(g_tcpbuffsize< 32168000)
            //                   {
            //                  g_tcpbuffsize<<1;
            //                  SetTCPBufSize(SO_RCVBUF|SO_SNDBUF,g_tcpbuffsize);
            //                  DebugOutPutf("WSAENOBUFS\n", "socket");assert(0&&"=WSAENOBUFS=\n");
            //                     return Create(domain,streamtype,protocol);
            //                   }
            //                   else
            //                   {
            //                       return 0;
            //
            //                   }
            //               }
            //               break;
            //
            //           case WSAEPROTONOSUPPORT:   DebugOutPutf("WSAEPROTONOSUPPORT\n", "socket");assert(0&&"=WSAEPROTONOSUPPORT=\n"); break;
            //           case WSAEPROTOTYPE:  DebugOutPutf("WSAEPROTOTYPE\n", "socket");assert(0&&"=WSAEPROTOTYPE=\n"); break;
            //   case WSAESOCKTNOSUPPORT:   DebugOutPutf("WSAESOCKTNOSUPPORT\n", "socket");assert(0&&"=WSAESOCKTNOSUPPORT=\n"); break;
            //           case WSAEINVAL:   DebugOutPutf("WSAEINVAL\n", "socket");assert(0&&"=WSAEINVAL=\n"); break;
            //           case WSAEFAULT:   DebugOutPutf("WSAEFAULT\n", "socket");assert(0&&"=WSAEFAULT=\n"); break;
            //           default:       DebugOutPutf("undefine\n", "socket");assert(0&&"=undefine error=\n"); break;
            //           }
            //MessageBox(NULL,"Create Socket failed"," Create Socket Failed", MB_OK);
            //m_OPState = OP_CREATE_FAILURE;
            return 0;
        }

#endif
#if defined(_LINUX)

        //if(Socket < 0)
        if( !isvalidsock( socket_fd_ = socket( domain, streamtype, protocol ) ) )
        {
            //sprintf(StatuMsg,"Create Socket failed ~_~b !!!");
            net::Socket::PrintError( "ERROR: Unix socket call failed ~_~b:" );
            //cout << "Create Socket failed ~_~b !!! \n";
            return 0;
        }

#endif
        stime_last_create_ = pftml::Time::GetSTime();
        return socket_fd_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t& Socket::operator = ( _IN const socket_t& sk )
    {
        //TS_Lock lock(&m_SkLock);
        TS_Lock lock( &socket_ut_lock_ );
        socket_fd_ = sk;
        stime_last_create_ = pftml::Time::GetSTime();
        return socket_fd_;
    }

    //=============================================================================
    // NAME: close()
    // DESC: close this socket
    //=============================================================================
    int Socket::Close( void )
    {
        TS_Lock lock( &socket_ut_lock_ );
        int skret = 0;
        //TS_Lock lock( &m_SkLock );

        //m_bLive = false;
        if( socket_fd_ != NULL_SOCKET )
        {
            //
            socket_t closefd = socket_fd_;
            //int   skret = 0;
#if defined(_WINDOWS)
            skret = closesocket( closefd );

            if( skret == SOCKET_ERROR )
            {
                int errcode = WSAGetLastError();
                PrintErrorCode( errcode );

                switch( errcode )
                {
                    case WSAEWOULDBLOCK:
                        {
                        }
                        break;

                    default:
                        {
                            assert( NULL && "socket close failure" );
                        }
                        break;
                }
            }

            //memset(&m_localaddr, 0, sizeof(sockaddr_in));
            //memset(&m_peeraddr,  0, sizeof(sockaddr_in));
#else
            skret = close( closefd );
#endif
            socket_fd_ = NULL_SOCKET;
            //if ( skret != SOCKET_ERROR )
            //{
            //    return 1;
            //}
        }

        //pfstd::MSSleep( 1 );
        return skret;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Socket::SetKey( _IN const ntchar_t* k, _IN void* v )
    {
        TS_Lock lock( &socket_ut_lock_ );
        if( v == NULL )
        {
            user_key_.erase( k );
            return true;
        }

        std::map< _mstring, void* >::iterator iter = user_key_.find( k );
        if( iter != user_key_.end() )
        {
            return false;
        }

        user_key_.insert( std::map< _mstring, void* >::value_type( k, v ) );
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void* Socket::GetKey( _IN const ntchar_t* k )
    {
        TS_Lock lock( &socket_ut_lock_ );
        std::map< _mstring, void* >::iterator iter = user_key_.find( k );
        if( iter == user_key_.end() )
        {
            return NULL;
        }
        return iter->second;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Socket::Bind( _IN const sockaddr* addrptr, _IN int addrlen )
    {
        TS_Lock lock( &socket_ut_lock_ );
        return bind( socket_fd_, addrptr, addrlen );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Socket::GetAddress( _OUT sockaddr* addrptr, _IN int addrlen )
    {
        TS_Lock lock( &socket_ut_lock_ );
        socklen_t socklen = addrlen;
        return getsockname( socket_fd_, ( struct sockaddr* )addrptr, &socklen );
    }

    //=============================================================================
    // NAME: OIControl()
    // DESC:
    //
    //   Block mode (F_SETFL, O_NONBLOCK)   (FIONBIO, 0 or NONZERO)
    //=============================================================================
    int Socket::Ioctl( _IN long32_t cmd, _IN ulong32_t* argp )
    {
        TS_Lock lock( &socket_ut_lock_ );
#if defined(_WINDOWS)
        return ioctlsocket( socket_fd_, cmd, ( u_long* )argp );
#else
        return ioctl( socket_fd_, cmd, argp );
#endif
    }

    //=============================================================================
    // NAME: SetOpt()
    // DESC:
    //=============================================================================
    int Socket::SetOpt( _IN  int level, _IN int optname, _IN const void* optval, _IN socklen_t optlen )
    {
        TS_Lock lock( &socket_ut_lock_ );
        return setsockopt( socket_fd_, level, optname, ( const char* )optval, optlen );
    }

    //=============================================================================
    // NAME: SetOpt()
    // DESC:
    //=============================================================================
    int Socket::GetOpt( _IN int level, _IN int optname, _OUT void* optval, _INOUT int* optlen )
    {
        TS_Lock lock( &socket_ut_lock_ );
        return getsockopt( socket_fd_, level, optname, ( char* )optval, ( socklen_t* )optlen );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    stimen_t& Socket::GetLastCreateTime( void )
    {
        TS_Lock lock( &socket_ut_lock_ );
        return stime_last_create_;
    }

    //=============================================================================
    // NAME: Shutdown()
    // DESC: Shutdown
    //=============================================================================
    //int Socket::Shutdown(int how)
    //{
    //  int ret = 0;
    // TS_Lock lock(&m_SkLock);
    //  m_bLive=false;
    //
    // if (socket_fd_ != 0)
    // {
    //  ret = ::shutdown(socket_fd_, how);
    //      // this shouldn't be 0, because it just not allow
    //  //socket_fd_ = 0;
    // }
    //  //SetKey(NULL);
    //  return ret;
    //}

    //=============================================================================
    // NAME: GetNicAddress()
    // DESC: Get Nic Address
    //=============================================================================
    /*
    static void PrintMACaddress(unsigned char MACData[])
    {
     printf("MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
      MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
    }
    */

    //=============================================================================
    // NAME:
    // DESC: unix only
    //=============================================================================
    /*
    ipa_t Socket::EthIfGetAddrInfo( const char* ethif, int mode, int family )
    {
        struct ifaddrs* ifaddr, *ifa;
        int iffamily, s;
        char* host, *host_tmp;
        host = ( char* ) malloc( sizeof( char ) * NI_MAXHOST );
        host_tmp = ( char* ) malloc( sizeof( char ) * NI_MAXHOST );

        if ( getifaddrs( &ifaddr ) == -1 )
        {
        return NULL;
        }

        for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next )
        {
        iffamily = ifa->ifa_addr->sa_family;

        if ( ( iffamily == family ) && !( strcmp( ethif, ifa->ifa_name ) ) )
        {
        //if (family == AF_INET || family == AF_INET6) {
        #if 0
        printf( "%s  address family: %d%s\n", ifa->ifa_name, family,
        ( family == AF_PACKET ) ? " (AF_PACKET)" :
        ( family == AF_INET ) ? " (AF_INET)" :
        ( family == AF_INET6 ) ? " (AF_INET6)" : "" );
        #endif
        // mode = 0, Netmask ; mode = 1 Address
        s = getnameinfo( ( mode == 0 ) ? ifa->ifa_netmask : ifa->ifa_addr, sizeof( struct sockaddr_in6 ), host_tmp, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );

        if ( s != 0 )
        {
        //printf("getnameinfo() failed: %s\n", gai_strerror(s));
        return NULL;
        }

        //printf("address: <%s>\n", host);
        }
        }

        freeifaddrs( ifaddr );
        return host;
        return "";
    }
    */


    ////=============================================================================
    //// NAME:
    //// DESC:
    ////=============================================================================
    //void Socket::GetAddressIn( _OUT sockaddr_in& addrin )
    //{
    //    //addrs_t
    //    //int   len = sizeof(sockaddr_in);
    //    socklen_t   len = sizeof( sockaddr_in );
    //    getpeername( socket_fd_, ( sockaddr* ) &addrin, &len );
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Socket::GetAddrNameInfo( _IN const struct sockaddr* sa, _OUT char* host, _IN dword_t hostlen, _OUT char* serv, _IN dword_t servlen, _IN int flags )
    {
        socklen_t salen = 0;

        if( sa->sa_family == AF_INET )
        {
            salen = sizeof( sockaddr_in );
        }
        else if( sa->sa_family == AF_INET6 )
        {
            salen = sizeof( sockaddr_in6 );
        }
        else
        {
            return 0;
        }

        return getnameinfo( sa, salen, host, hostlen, serv, servlen, flags );
    }

    //=============================================================================
    // NAME: HGetIPA()
    // DESC:
    //=============================================================================
    void Socket::GetAddrNameInfo( _OUT std::vector<addrs_t>& addrlist, _IN const ntchar_t* hostname, _IN const ntchar_t* service, _IN int family, _IN int socktype )
    {
        struct addrinfo* result = NULL;
        struct addrinfo* ptr = NULL;
        struct addrinfo hints;
        //in_addr ipn;
        //memset( &ipn, 0, sizeof( in_addr ) );
        memset( &hints, 0, sizeof( hints ) );
        hints.ai_family = family;
        hints.ai_socktype = socktype;
        hints.ai_protocol = 0;
        hints.ai_flags = AI_PASSIVE; // 幫我填好我的 IP
#ifndef UNICODE
        int dwRetval = getaddrinfo( hostname, service, &hints, &result );
#endif

        if( dwRetval != 0 )
        {
            return;
        }
        else
        {
            //int ei = 0;

            for( ptr = result; ptr != NULL; ptr = ptr->ai_next )
            {
                addrs_t retaddr;
                memset( &retaddr, 0, sizeof( addrs_t ) );
                memcpy( &retaddr, ptr->ai_addr, ptr->ai_addrlen );
                addrlist.push_back( retaddr );
            }
        }
        return;
    }

    //=============================================================================
    // NAME: SetAddress()
    // DESC: set one sockaddr_in value
    //=============================================================================
    void Socket::SetAddress( _IN const ntchar_t* ipaddr, _IN const ntchar_t* portnum, _OUT struct sockaddr_in* sap, _IN const ntchar_t* sockettype )
    {
        struct servent* sp;
        struct hostent* hp;
        char* endptr;
        short port;
        memset( sap, 0, sizeof( *sap ) );
        sap->sin_family = AF_INET;

        if( ipaddr != NULL )
        {
            sap->sin_addr = IPA4ToN4( ipaddr );
            //if ( memcmp(&sap->sin_addr, 0 ) != 0 )
            {
                hp = gethostbyname( ipaddr );

                if( hp == NULL )
                {
                    char msg[ 255 ];
                    sprintf( msg, _T( "unknow host: %s\n" ), ipaddr );
                    net::Socket::PrintError( msg );
                }
                else
                {
                    sap->sin_addr = *( struct in_addr* )hp->h_addr;
                }
            }
        }
        else
        {
            sap->sin_addr.s_addr = htonl( INADDR_ANY );
        }

        port = ( short )strtol( portnum, &endptr, 0 );

        if( *endptr == '\0' )
        {
            sap->sin_port = htons( port );
        }
        else
        {
            sp = getservbyname( portnum, sockettype );

            if( sp == NULL )
            {
                char msg[ 255 ];
                sprintf( msg, _T( "unknow service: %s\n" ), portnum );
                net::Socket::PrintError( msg );
                //DebugMessage(msg, "set address ");
                //error(1, 0, "unknow service: %s\n", portnum);
                return;
            }

            sap->sin_port = sp->s_port;
        }

        //memcpy(&m_address, sap, sizeof(m_address));
        return;
    }

    //=============================================================================
    // NAME: SetAddress()
    // DESC: set one sockaddr_in value
    //=============================================================================
    void Socket::SetAddress( _IN const ntchar_t* ipaddr, _IN const ntchar_t* service, _OUT struct sockaddr* sap, _IN int family, _IN int socktype )
    {
        //ntchar_t buf[50];
        std::vector<addrs_t> addrinfo_list;
        net::Socket::GetAddrNameInfo( addrinfo_list, ipaddr, service, family, socktype );
        if( addrinfo_list.empty() )
            return;
        memcpy( sap, &( addrinfo_list[ 0 ] ), sizeof( sockaddr ) );
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    // NAME: GetHostName()
    // DESC: get local host name
    //=============================================================================
    char* Socket::GetLocalHostname( _OUT ntchar_t* buf, _OUT size32_t len )
    {
        if( gethostname( buf, len ) != 0 )
        {
            return NULL;
        }

        return buf;
    }

    //=============================================================================
    // NAME: GetIP()
    // DESC:
    //=============================================================================
    //ipn4_t Socket::GetLocalIPAddress( uint_t ethif )
    //{
    //    char buff[ _MAX_HOST_NAME ];
    //    hostent* lpHost;
    //    in_addr ipn;
    //    memset( &ipn, 0, sizeof( in_addr ) );
    //    //ipn.S_un.S_addr = 0;
    //    int ret = gethostname( buff, _MAX_HOST_NAME );
    //
    //    if ( ( lpHost = gethostbyname( buff ) ) == NULL )
    //    {
    //        return ipn;
    //    }
    //
    //    if ( lpHost->h_addr_list[ ethif ] == NULL )
    //    {
    //        return ipn;
    //    }
    //
    //    return *( in_addr* ) ( lpHost->h_addr_list[ ethif ] );
    //}

    //=============================================================================
    // NAME:
    // DESC: get local service name by port
    //=============================================================================
    char* Socket::GetLocalPortService( _IN ushort_t port, _IN const ntchar_t* proto )
    {
        struct servent* s;
        s = getservbyport( htons( port ), proto );
        return s->s_name;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Socket::GetEthIFInfo( _OUT std::vector<EthIF>& ethif_list )
    {
#if defined(_WINDOWS)
        //INTERFACE_INFO
        //PIP_ADAPTER_INFO
        IP_ADAPTER_ADDRESSES* adapter_addresses( NULL );
        IP_ADAPTER_ADDRESSES* adapter( NULL );
        // Start with a 16 KB buffer and resize if needed -
        // multiple attempts in case interfaces change while
        // we are in the middle of querying them.
        DWORD adapter_addresses_buffer_size = _1KB_;
        //DWORD adapter_addresses_buffer_size = 0;
        //GetAdaptersAddresses( AF_UNSPEC, 0, NULL, adapter_addresses, &adapter_addresses_buffer_size );

        for( int attempts = 0; attempts != 3; ++attempts )
        {
            adapter_addresses = ( IP_ADAPTER_ADDRESSES* )malloc( adapter_addresses_buffer_size );
            assert( adapter_addresses );
            DWORD error = ::GetAdaptersAddresses(
                AF_UNSPEC,
                //GAA_FLAG_SKIP_ANYCAST |
                //GAA_FLAG_SKIP_MULTICAST |
                //GAA_FLAG_SKIP_DNS_SERVER |
                //GAA_FLAG_SKIP_FRIENDLY_NAME,
                GAA_FLAG_INCLUDE_PREFIX,
                NULL,
                adapter_addresses,
                &adapter_addresses_buffer_size );

            if( ERROR_SUCCESS == error )
            {
                // We're done here, people!
                break;
            }
            else if( ERROR_BUFFER_OVERFLOW == error )
            {
                // Try again with the new size
                free( adapter_addresses );
                adapter_addresses = NULL;
                continue;
            }
            else
            {
                // Unexpected error code - log and throw
                free( adapter_addresses );
                adapter_addresses = NULL;
                // @todo
                //LOG_AND_THROW_HERE();
            }
        }

        //std::map <std::string, EthIF*> eilist;

        // Iterate through all of the adapters
        for( adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next )
        {
            // Skip loopback adapters
            //if ( IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType )
            //{
            //    continue;
            //}
            EthIF eif;
            eif.name = adapter->AdapterName;
            eif.iftype = adapter->IfType;
            //eif.name = adapter->FriendlyName;
            eif.macn = 0;
            maca_t maca = MACNToA( eif.macn );

            for( uint_t mi = 0; mi < adapter->PhysicalAddressLength; mi++ )
            {
                maca[ mi ] = ( unsigned char )adapter->PhysicalAddress[ mi ];
            }

            // Parse all IPv4 and IPv6 addresses
            for( IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; NULL != address; address = address->Next )
            {
                addrs_t addrs;
                memset( &addrs, 0, sizeof( addrs_t ) );
                __pfmemcpy( &addrs, address->Address.lpSockaddr, address->Address.iSockaddrLength );
                eif.unicast_addrs.push_back( addrs );
                //
                //auto family = address->Address.lpSockaddr->sa_family;
                //
                //if ( AF_INET == family )
                //{
                //    // IPv4
                //    SOCKADDR_IN* ipv4 = reinterpret_cast< SOCKADDR_IN* >( address->Address.lpSockaddr );
                //    char str_buffer[ INET_ADDRSTRLEN ] = { 0 };
                //    inet_ntop( AF_INET, &( ipv4->sin_addr ), str_buffer, INET_ADDRSTRLEN );
                //    //ipAddrs.mIpv4.push_back( str_buffer );
                //}
                //else if ( AF_INET6 == family )
                //{
                //    // IPv6
                //    SOCKADDR_IN6* ipv6 = reinterpret_cast< SOCKADDR_IN6* >( address->Address.lpSockaddr );
                //    char str_buffer[ INET6_ADDRSTRLEN ] = { 0 };
                //    inet_ntop( AF_INET6, &( ipv6->sin6_addr ), str_buffer, INET6_ADDRSTRLEN );
                //    std::string ipv6_str( str_buffer );
                //    // Detect and skip non-external addresses
                //    bool is_link_local( false );
                //    bool is_special_use( false );
                //
                //    if ( 0 == ipv6_str.find( "fe" ) )
                //    {
                //        char c = ipv6_str[ 2 ];
                //
                //        if ( c == '8' || c == '9' || c == 'a' || c == 'b' )
                //        {
                //            is_link_local = true;
                //        }
                //    }
                //    else if ( 0 == ipv6_str.find( "2001:0:" ) )
                //    {
                //        is_special_use = true;
                //    }
                //
                //    if ( !( is_link_local || is_special_use ) )
                //    {
                //        //ipAddrs.mIpv6.push_back( ipv6_str );
                //    }
                //}
                //else
                //{
                //    // Skip all other types of addresses
                //    continue;
                //}
            }

            for( IP_ADAPTER_MULTICAST_ADDRESS* address = adapter->FirstMulticastAddress; NULL != address; address = address->Next )
            {
                addrs_t addrs;
                memset( &addrs, 0, sizeof( addrs_t ) );
                __pfmemcpy( &addrs, address->Address.lpSockaddr, address->Address.iSockaddrLength );
                eif.multicast_addrs.push_back( addrs );
            }

            ethif_list.push_back( eif );
        }

        // Cleanup
        free( adapter_addresses );
        adapter_addresses = NULL;
#else
        socket_t sock_mac = socket( AF_INET, SOCK_STREAM, 0 );

        if( sock_mac == -1 )
        {
            net::Socket::PrintError( "create socket falise...mac\n" );
            return;
        }

        struct ifaddrs* ifaddr, * ifa;

        //int iffamily, s;

        //char* host, *host_tmp;
        //host = ( char* ) malloc( sizeof( char ) * NI_MAXHOST );
        //host_tmp = ( char* ) malloc( sizeof( char ) * NI_MAXHOST );
        if( getifaddrs( &ifaddr ) == -1 )
        {
            return;
        }

        std::map<std::string, EthIF*> tmplist;

        for( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next )
        {
            EthIF* eif;
            std::map<std::string, EthIF*>::iterator it = tmplist.find( ifa->ifa_name );

            if( it != tmplist.end() )
            {
                eif = it->second;
            }
            else
            {
                eif = new EthIF;
                eif->name = ifa->ifa_name;
                eif->macn = 0;

                if( eif->name == "lo" )
                {
                    eif->iftype = _IF_TYPE_LOOKBACK;
                }
                else
                {
                    eif->iftype = _IF_TYPE_ETHERNET;
                }

                tmplist.insert( std::map<std::string, EthIF*>::value_type( eif->name, eif ) );
            }

            // get macaddr
            {
                struct ifreq ifr;
                ifr.ifr_addr.sa_family = AF_INET;
                strncpy( ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1 );

                if( ( ioctl( sock_mac, SIOCGIFHWADDR, &ifr ) ) < 0 )
                {
                    printf( "mac ioctl error\n" );
                    continue;
                }

                ioctl( sock_mac, SIOCGIFHWADDR, &ifr );
                maca_t maca = MACNToA( eif->macn );
                maca[ 0 ] = ( unsigned char )ifr.ifr_hwaddr.sa_data[ 0 ];
                maca[ 1 ] = ( unsigned char )ifr.ifr_hwaddr.sa_data[ 1 ];
                maca[ 2 ] = ( unsigned char )ifr.ifr_hwaddr.sa_data[ 2 ];
                maca[ 3 ] = ( unsigned char )ifr.ifr_hwaddr.sa_data[ 3 ];
                maca[ 4 ] = ( unsigned char )ifr.ifr_hwaddr.sa_data[ 4 ];
                maca[ 5 ] = ( unsigned char )ifr.ifr_hwaddr.sa_data[ 5 ];
            }
            addrs_t addrs;

            if( ifa->ifa_addr != NULL )
            {
                memset( &addrs, 0, sizeof( addrs_t ) );
                __pfmemcpy( &addrs, ifa->ifa_addr, sizeof( addrs_t ) );
                eif->unicast_addrs.push_back( addrs );
            }

            if( ifa->ifa_broadaddr != NULL )
            {
                memset( &addrs, 0, sizeof( addrs_t ) );
                __pfmemcpy( &addrs, ifa->ifa_broadaddr, sizeof( addrs_t ) );
                eif->multicast_addrs.push_back( addrs );
            }

            //ethif_list.push_back( eif );
        }

        freeifaddrs( ifaddr );
        close( sock_mac );

        for( std::map<std::string, EthIF*>::iterator it = tmplist.begin(); it != tmplist.end(); it++ )
        {
            ethif_list.push_back( *it->second );
            SAFE_DELETE( it->second );
        }

        tmplist.clear();
#endif

        struct sort_address
        {
            bool operator() ( addrs_t a, addrs_t b )
            {
                return ( a.ss_family < b.ss_family );
            }  // 降序排列
        } my_sort_address;

        for( uint_t i = 0; i < ethif_list.size(); i++ )
        {
            std::sort( ethif_list[ i ].unicast_addrs.begin(), ethif_list[ i ].unicast_addrs.end(), my_sort_address );
            std::sort( ethif_list[ i ].multicast_addrs.begin(), ethif_list[ i ].multicast_addrs.end(), my_sort_address );
            std::sort( ethif_list[ i ].netmask.begin(), ethif_list[ i ].netmask.end(), my_sort_address );
        }

        return;
        // Cheers!
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    sockaddr_in* Socket::GetSockAddrIN4( _IN const sockaddr* addr )
    {
        if( addr == NULL )
        {
            return NULL;
        }

        if( addr->sa_family == AF_INET )
        {
            return ( struct sockaddr_in* )addr;
        }

        Pf_ERROR( _T( "ip6 get socket_in failure" ) );
        return NULL;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    sockaddr_in6* Socket::GetSockAddrIN6( _IN const sockaddr* addr )
    {
        if( addr == NULL )
        {
            return NULL;
        }

        if( addr->sa_family == AF_INET6 )
        {
            return ( struct sockaddr_in6* )addr;
        }

        Pf_ERROR( _T( "ip4 get socket_in6 failure" ) );
        return NULL;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    ipa_t Socket::GetSockAddrIPA( _IN const sockaddr* addr )
    {
        if( addr != NULL )
        {
            if( addr->sa_family == AF_INET )
            {
                return  IPN4ToA4( ( ( struct sockaddr_in* )addr )->sin_addr );
            }
            else if( addr->sa_family == AF_INET6 )
            {
                return  IPN6ToA6( ( ( struct sockaddr_in6* )addr )->sin6_addr );
            }
        }

        return "";
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    ipn4_t Socket::GetSockAddrIPN4( _IN const sockaddr* addr )
    {
        if( addr != NULL )
        {
            if( addr->sa_family == AF_INET )
            {
                return  ( ( struct sockaddr_in* )addr )->sin_addr;
            }
            else if( addr->sa_family == AF_INET6 )
            {
                return  IPN6ToN4( ( ( struct sockaddr_in6* )addr )->sin6_addr );
            }
        }

        ipn4_t ipn4;
        memset( &ipn4, 0, sizeof( ipn4_t ) );
        return ipn4;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    ipn6_t Socket::GetSockAddrIPN6( _IN const sockaddr* addr )
    {
        if( addr != NULL )
        {
            if( addr->sa_family == AF_INET )
            {
                return  IPN4ToN6( ( ( struct sockaddr_in* )addr )->sin_addr );
            }
            else if( addr->sa_family == AF_INET6 )
            {
                return  ( ( ( struct sockaddr_in6* )addr )->sin6_addr );
            }
        }

        ipn6_t ipn6;
        memset( &ipn6, 0, sizeof( ipn6_t ) );
        return ipn6;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    ushort_t Socket::GetSockAddrPort( _IN const sockaddr* addr )
    {
        if( addr != NULL )
        {
            if( addr->sa_family == AF_INET )
            {
                return ntohs( ( ( struct sockaddr_in* )addr )->sin_port );
            }
            else if( addr->sa_family == AF_INET6 )
            {
                return ntohs( ( ( struct sockaddr_in6* )addr )->sin6_port );
            }
        }

        return 0;
    }

    //ipn4_t Socket::GetInternetAddr( const sockaddr_in* addrin )
    //{
    //    if ( addrin == NULL )
    //    {
    //        ipn4_t ipn;
    //        memset( &ipn, 0, sizeof( ipn4_t ) );
    //        return ipn;
    //    }
    //
    //    return addrin->sin_addr;
    //}
    //
    //ushort_t Socket::GetSockAddrPort( const sockaddr_in* addrin )
    //{
    //    if ( addrin == NULL )
    //    {
    //        return -1;
    //    }
    //
    //    return ntohs( addrin->sin_port );
    //}
    //
    //ipn6_t Socket::GetInternetAddr( _IN const sockaddr_in6* addrin6 )
    //{
    //    if ( addrin6 == NULL )
    //    {
    //        ipn6_t ipn6;
    //        memset( &ipn6, 0, sizeof( ipn6_t ) );
    //        return ipn6;
    //    }
    //
    //    return addrin6->sin6_addr;
    //}
    //
    //ushort_t Socket::GetSockAddrPort( _IN const sockaddr_in6* addrin6 )
    //{
    //    if ( addrin6 == NULL )
    //    {
    //        return -1;
    //    }
    //
    //    return ntohs( addrin6->sin6_port );
    //}

    ////=============================================================================
    //// NAME: GetPeerAddress
    //// DESC: get remote address
    ////=============================================================================
    //ipn4_t Socket::GetInternetAddress( void )
    //{
    //    sockaddr_in peeraddr;
    //    //int   len = sizeof(sockaddr_in);
    //    socklen_t   len = sizeof( sockaddr_in );
    //    getpeername( socket_fd_, ( sockaddr* )&peeraddr, &len );
    //    return peeraddr.sin_addr;
    //}

    //=============================================================================
    // NAME: GetPeerPort
    // DESC: get remote port
    //=============================================================================
    //ushort_t Socket::GetPeerPort( void )
    //{
    //    sockaddr_in peeraddr;
    //    //int   len = sizeof(sockaddr_in);
    //    socklen_t   len = sizeof( sockaddr_in );
    //    getpeername( socket_fd_, ( sockaddr* )&peeraddr, &len );
    //    return ntohs( peeraddr.sin_port );
    //}



    //////////////////////////////////////////////////////////////////////////
    //
    ipa_t Socket::IPN6ToA4( ipn6_t ipn )
    {
        return IPN4ToA4( IPN6ToN4( ipn ) );
    }

    ipa_t Socket::IPN6ToA6( ipn6_t ipn )
    {
        ntchar_t buffer[ INET6_ADDRSTRLEN ];
        memset( buffer, 0, sizeof( buffer ) );
        inet_ntop( AF_INET6, &ipn, buffer, INET6_ADDRSTRLEN );
        return buffer;
    }

    ipn4_t Socket::IPN6ToN4( _IN ipn6_t ipn6 )
    {
        ipn4_t ipn4;
        memset( &ipn4, 0, sizeof( ipn4_t ) );
#ifdef _WINDOWS
        ipn4.S_un.S_un_w.s_w1 = ipn6.s6_words[ 6 ];
        ipn4.S_un.S_un_w.s_w2 = ipn6.s6_words[ 7 ];
#endif
#ifdef _LINUX
        ipn4.s_addr = ipn6.s6_addr32[ 3 ];
#endif
        return ipn4;
    }

    ipa_t Socket::IPN4ToA4( ipn4_t ipn )
    {
        //in_addr addr;
        //addr = ipn;
        //return inet_ntoa( ipn );
        ntchar_t buffer[ INET_ADDRSTRLEN ];
        memset( buffer, 0, sizeof( buffer ) );
        inet_ntop( AF_INET, &ipn, buffer, INET_ADDRSTRLEN );
        return buffer;
    }

    ipn6_t Socket::IPN4ToN6( ipn4_t ipn4 )
    {
        ipn6_t ipn6;
        memset( &ipn6, 0, sizeof( ipn6_t ) );
#ifdef _WINDOWS
        ipn6.s6_words[ 5 ] = ( ushort_t )0xFFFF;
        ipn6.s6_words[ 6 ] = ipn4.S_un.S_un_w.s_w1;
        ipn6.s6_words[ 7 ] = ipn4.S_un.S_un_w.s_w2;
#endif
#ifdef _LINUX
        ipn6.s6_addr32[ 3 ] = ipn4.s_addr;
        ipn6.s6_addr16[ 5 ] = ( ushort_t )0xFFFF;
#endif
        return ipn6;
    }

    ipn4_t Socket::IPA4ToN4( ipa_t ipa )
    {
        in_addr addr4;
        memset( &addr4, 0, sizeof( in_addr ) );
        inet_pton( AF_INET, ipa.c_str(), ( char* )&addr4 );
        return addr4;
    }

    ipn6_t Socket::IPA6ToN6( ipa_t ipa )
    {
        in6_addr addr6;
        memset( &addr6, 0, sizeof( in6_addr ) );
        inet_pton( AF_INET6, ipa.c_str(), ( char* )&addr6 );
        return addr6;
    }


    ipa_t Socket::IPA4ToA6( _IN ipa_t ipa )
    {
        ipn4_t ipn4 = IPA4ToN4( ipa );
        ipn6_t ipn6 = IPN4ToN6( ipn4 );
        return IPN6ToA6( ipn6 );
    }

    ipa_t Socket::IPA6ToA4( _IN ipa_t ipa )
    {
        ipn6_t ipn6 = IPA6ToN6( ipa );
        ipn4_t ipn4 = IPN6ToN4( ipn6 );
        return IPN4ToA4( ipn4 );
    }

    macn_t Socket::MACAToN( maca_t maca )
    {
        macn_t macn;
        memcpy( &macn, maca, sizeof( macn_t ) );
        return macn;
    }
    maca_t Socket::MACNToA( macn_t& macn )
    {
        return ( maca_t )( &macn );
    }

    //=============================================================================
    // NAME: SetBlock()
    // DESC: block socket
    //=============================================================================
    bool Socket::SetBlock( bool isBlk )
    {
        TS_Lock lock( &socket_ut_lock_ );

        if( socket_fd_ == NULL_SOCKET )
        {
            return false;
        }

        //bool non_blk = !isBlk;
        //Ioctl( FIONBIO, ( ulong32_t* ) ( &non_blk ) );
#if defined(_WINDOWS)
        //setsockopt( fd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, ( ntchar_t* ) &fd, sizeof( SOCKET ) );
        // turn on keep-alive monitoring
        //ulong32_t flag = 1;
        //setsockopt( fd, SOL_SOCKET, SO_KEEPALIVE, ( char* ) &flag, sizeof( flag ) );
        //set socket no delay
        //ulong32_t flag = 1;
        //setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, ( char* ) &flag, sizeof( flag ) ); // no delay
        u_long mode = 1;  // 1 to enable non-blocking socket
        ioctlsocket( socket_fd_, FIONBIO, &mode );
#else
        int flags, s;
        flags = fcntl( socket_fd_, F_GETFL, 0 );

        if( flags == -1 )
        {
            net::Socket::PrintError( "fcntl" );
            return false;
        }

        flags |= O_NONBLOCK;
        s = fcntl( socket_fd_, F_SETFL, flags );

        if( s == -1 )
        {
            net::Socket::PrintError( "fcntl" );
            return false;
        }
#endif
        return true;
    }

    //=============================================================================
    // NAME: SetKeepAlive()
    // DESC:
    //=============================================================================
    void Socket::SetKeepAlive( _IN bool isKeep )
    {
        TS_Lock lock( &socket_ut_lock_ );

        if( socket_fd_ == NULL_SOCKET )
        {
            return;
        }

        /* Set the option active */
        int optval = ( isKeep == true ) ? 1 : 0;
        socklen_t optlen = sizeof( optval );

        if( setsockopt( socket_fd_, SOL_SOCKET, SO_KEEPALIVE, ( const char* )&optval, optlen ) < 0 )
        {
            net::Socket::PrintError( "setsockopt()" );
            //close( s );
            //exit( EXIT_FAILURE );
        }

        //int keepalive = 1;     // 开启keepalive属性  
        //int keepidle = 60;     // 如该连接在60秒内没有任何数据往来,则进行探测  
        //int keepinterval = 5;  // 探测时发包的时间间隔为5 秒  
        //int keepcount = 3;     // 探测尝试的次数。如果第1次探测包就收到响应了,则后2次的不再发。  
        //setsockopt( rs, SOL_SOCKET, SO_KEEPALIVE, ( void * ) &keepalive, sizeof( keepalive ) );
        //setsockopt( rs, SOL_TCP, TCP_KEEPIDLE, ( void* ) &keepidle, sizeof( keepidle ) );
        //setsockopt( rs, SOL_TCP, TCP_KEEPINTVL, ( void * ) &keepinterval, sizeof( keepinterval ) );
        //setsockopt( rs, SOL_TCP, TCP_KEEPCNT, ( void * ) &keepcount, sizeof( keepcount ) );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Socket::IsLive( void )
    {
        TS_Lock lock( &socket_ut_lock_ );

        if( socket_fd_ == NULL_SOCKET )
        {
            return false;
        }

        /*
        //TODO - make this better, because its probably wrong (but seems to work)
        int optval;
        socklen_t optlen = sizeof( optval );

        int res = getsockopt( socket_fd_, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen );

        if ( optval == 0 && res == 0 ) return true;
//#endif
        return false;
        */

        /*
        int error = 0;
        socklen_t len = sizeof( error );
        int retval = getsockopt( socket_fd_, SOL_SOCKET, SO_ERROR, ( char* )&error, &len );

        if( retval != 0 )
        {
            // there was a problem getting the error code
            fprintf( stderr, "error getting socket error code: %s\n", strerror( retval ) );
            return false;
        }

        if( error != 0 )
        {
            // socket has a non zero error status
            fprintf( stderr, "socket error: %s\n", strerror( error ) );
            return false;
        }
        */

        // 會被block 住
        //int nread = 0;
        //char buff[ 10 ];
        //nread = recv( socket_fd_, buff, 10, MSG_PEEK );
        //
        //if ( nread > 0 )
        //{
        //    return true;
        //}
        //else if ( nread < 0 )
        //{
        //    /* interrupted? */
        //    /* If errno == EAGAIN, that means we have read all
        //    data. So go back to the main loop. */
        //    if ( ( errno == EINTR ) || ( errno == EAGAIN ) )
        //    {
        //        return true;
        //    }
        //}

        return false;
        //return m_bLive;
        //return ( socket_fd_ == NULL_SOCKET )?true:false;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Socket::GetReadLength()
    {
        TS_Lock lock( &socket_ut_lock_ );

        unsigned long nread = 0;
        int recvret = ioctl( socket_fd_, FIONREAD, &nread );
        return nread;
    }

    //=============================================================================
    // NAME: GetLastError()
    // DESC: get last error number
    //=============================================================================
    int Socket::GetLastError( void )
    {
#if defined(_WINDOWS)
        return WSAGetLastError();
#else
        return errno;
#endif
    }

    //=============================================================================
    // NAME: error()
    // DESC: print error status
    //=============================================================================
    void Socket::SetErrorCode( int status, int err, const char* fmt, ... )
    {
        va_list ap;
        va_start( ap, fmt );
        //if ( program_name )
        //{
        //    fprintf( stderr, "%s: ", program_name );
        //    vfprintf( stderr, fmt, ap );
        //}
        va_end( ap );

        if( err )
        {
            fprintf( stderr, ": %s (%d)\n", strerror( err ), err );
        }

        if( status )
        {
            //EXIT( status );
        }

        return;
    }
    // //=============================================================================
    // // NAME:
    // // DESC:
    // //=============================================================================
    //  Socket::OPSTATE Socket::GetOPState(void)
    //  {
    //      TS_Lock lock(&m_SkLock);
    //      return m_OPState;
    //  }
    //
    //
    // //=============================================================================
    // // NAME:
    // // DESC:
    // //=============================================================================
    //  void Socket::SetOPState(Socket::OPSTATE op)
    //  {
    //      TS_Lock lock(&m_SkLock);
    //      m_OPState=op;
    //      return;
    //  }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Socket::PrintErrorCode( int code )
    {
        char buff[ 512 ];
#if defined(_WINDOWS)

        switch( code )
        {
            case ERROR_NETNAME_DELETED:
                net::Socket::PrintError( "The specified network name is no longer available.\n" );
                break;

            case WSANOTINITIALISED:
                net::Socket::PrintError( "ERROR Shutdown: A successful WSAStartup call must occur before using this function\n" );
                break;

            case WSAENETDOWN:
                net::Socket::PrintError( "ERROR Shutdown: The network subsystem has failed\n" );
                break;

            case WSAEINVAL:
                net::Socket::PrintError( "ERROR Shutdown: The how parameter is not valid, or is not consistent with the socket optype. For example, SD_SEND is used with a UNI_RECV socket optype\n" );
                break;

            case WSAEINPROGRESS:
                net::Socket::PrintError( "ERROR Shutdown: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function\n" );
                break;

            case WSAENOTCONN:
                net::Socket::PrintError( "ERROR Shutdown: The socket is not connected (connection-oriented sockets only)\n" );
                break;

            case WSAENOTSOCK:
                net::Socket::PrintError( "ERROR Shutdown: The descriptor is not a socket\n" );
                break;

            default:
                {
                    strcpy( buff, strerror( code ) );
                    net::Socket::PrintError( buff );
                }
                break;
        }

#else
        strcpy( buff, strerror( code ) );
        net::Socket::PrintError( buff );
#endif
    }

    int Socket::PrintError( const char* errstr )
    {
        perror( errstr );
        //#ifdef _WINDOWS
        //        Pf_ERROR( errstr );
        //#else
        //        perror( errstr );
        //#endif
        return 1;
    }


    ///////////////////////////////////////////////////////////////////////////////
    // TCP socket
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    TCPSocket::TCPSocket()
    {
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* recvov = new SocketOV( this );
        recv_buffer_ = recvov;
        //SocketOV* sendov = new SocketOV( this );
        //m_Sendbuf = sendov;
#endif
        stime_last_recv_ = 0;
        stime_last_send_ = 0;
    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    TCPSocket::~TCPSocket()
    {
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* rov = ( SocketOV* )recv_buffer_;
        //SocketOV* sov = ( SocketOV* )m_Sendbuf;
        SAFE_DELETE( rov );
        //SAFE_DELETE( sov );
#endif
    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t TCPSocket::Create( int domain )
    {
        //m_SerialNumber = 0;
        TS_Lock lock( &tcpsocket_ut_lock_ );
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* recvov = ( SocketOV* )recv_buffer_;

        if( recvov != 0 )
        {
            recvov->Reset( /*OP_ACCEPTING*/ );
        }

        //SocketOV* sendov = ( SocketOV* )m_Sendbuf;
        //
        //if ( sendov != NULL )
        //{
        //    sendov->Reset( OP_ACCEPTING );
        //}
#endif
        return Socket::Create( domain, SOCK_STREAM, IPPROTO_TCP );
    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
//    socket_t TCPSocket::Accept()
//    {
//        TS_Lock lock( &tcpsocket_ut_lock_ );
//#if defined(_WIN32IOCOMPLETIONPORT)  /* Windows platforms */
//        // need to update accept context
//        socket_t sockett = socket_fd_;
//        setsockopt( sockett, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, ( ntchar_t* )&sockett, sizeof( SOCKET ) );
//        // turn on keep-alive monitoring
//        ulong32_t flag = 1;
//        setsockopt( sockett, SOL_SOCKET, SO_KEEPALIVE, ( char* )&flag, sizeof( flag ) );
//        //set socket no delay
//        //ulong32_t flag = 1;
//        setsockopt( sockett, IPPROTO_TCP, TCP_NODELAY, ( char* )&flag, sizeof( flag ) ); // no delay
//        /*
//        // 預設socket 的屬性是
//        // 設定 未送完的資料 在 10的timeout 時間內送完 再close
//        struct linger li = { 1, 10 };
//        setsockopt(s.socket_fd_, SOL_SOCKET, SO_LINGER, (char*)&li, sizeof(li));
//        */
//        SocketOV* ov = ( SocketOV* )recv_buffer_;
//        int locallen = 0, remotelen = 0;
//        sockaddr_in localaddr;
//        sockaddr_in peeraddr;  // 如果是CLIENT 連上來的那機器資料
//        ov->DoAccept( localaddr, locallen, peeraddr, remotelen );
//        // 使用  CreateIoCompletionPort
//        //HANDLE h = CreateIoCompletionPort((HANDLE)s.socket_fd_,
//        //           hIOCP_,
//        //           (dword_t)&s,
//        //           0);
//        //if (h != hIOCP_)
//        //{
//        // return NULL_SOCKET;
//        //}
//        //reset iocp buf
//        ( ( SocketOV* )( recv_buffer_ ) )->Reset( net::OP_READING );
//        //( ( SocketOV* )( m_Sendbuf ) )->Reset( net::OP_WRITING );
//#else
//        struct sockaddr_in peeraddr;
//        socklen_t          peerlen;
//        socket_t sockfd = accept( socket_fd_, ( struct sockaddr* )&peeraddr, &peerlen );
//
//        if ( sockfd == -1 )
//        {
//            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) )
//            {
//                /* We have processed all incoming
//                connections. */
//                return NULL_SOCKET;
//            }
//            else
//            {
//                net::Socket::PrintError( "accept" );
//                return NULL_SOCKET;
//            }
//        }
//
//        struct sockaddr info_addr;
//
//        socklen_t info_len;
//
//        int infd;
//
//        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
//
//        int ret = getnameinfo( &info_addr, info_len, hbuf, sizeof( hbuf ), sbuf, sizeof( sbuf ), NI_NUMERICHOST | NI_NUMERICSERV );
//
//        if ( ret == 0 )
//        {
//            printf( "Accepted connection on descriptor %d ""(host=%s, port=%s)\n", infd, hbuf, sbuf );
//        }
//
//        Socket::operator=( sockfd );
//#endif
//        return socket_fd_;
//    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    int TCPSocket::Shutdown(int how)
    {
       TS_Lock lock(&tcpsocket_ut_lock_);
    #ifdef _WIN32IOCOMPLETIONPORT
       if((how == SD_RECEIVE) || (how == SD_BOTH))
       {
           SocketOV* recvov = (SocketOV*)recv_buffer_;
           if(recvov !=0)
           {
               recvov->Reset(OP_CLOSED);
           }
       }

       if((how == SD_SEND) || (how == SD_BOTH))
       {
           SocketOV* sendov=(SocketOV*)m_Sendbuf;
           if(sendov !=NULL )
           {
               sendov->Reset(OP_CLOSED);
           }
       }
    #endif
       return Socket::Shutdown(how);
    }
    */
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int TCPSocket::Close( void )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* recvov = ( SocketOV* )recv_buffer_;

        if( recvov != 0 )
        {
            recvov->Reset( /*OP_CLOSED*/ );
        }

        //SocketOV* sendov = ( SocketOV* )m_Sendbuf;
        //if ( sendov != NULL )
        //{
        //    sendov->Reset( OP_CLOSED );
        //}
#endif
        return Socket::Close();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    sockaddr* TCPSocket::GetPeerAddr( _OUT addrs_t& addr )
    {
        socklen_t len = sizeof( addr );
        if( getpeername( GetFD(), ( struct sockaddr* )&addr, &len ) != 0 )
        {
            char buff[ 512 ];
            int errcode = Socket::GetLastError();
            strcpy( buff, strerror( errcode ) );
            net::Socket::PrintError( buff );
            return NULL;
        }
        /*
        if ( addr.ss_family == AF_INET )
        {
        addrin4 = ( struct sockaddr_in* )&addr;
        //port = ntohs( s->sin_port );
        //inet_ntop( AF_INET, &s->sin_addr, ipstr, sizeof ipstr );
        }
        else
        {
        // // AF_INET6
        addrin6 = ( struct sockaddr_in6* )&addr;
        //port = ntohs( s->sin6_port );
        //inet_ntop( AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr );
        }
        */
        return ( struct sockaddr* )&addr;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int TCPSocket::Ioctl( long32_t cmd, ulong32_t* argp )
    {
        //#ifdef _WIN32IOCOMPLETIONPORT
        //
        //        if ( ( recv_buffer_ ) && ( cmd == FIONREAD ) )
        //        {
        //            SocketOV* recvov = ( SocketOV* )recv_buffer_;
        //            ( *argp ) = recvov->GetUsedsize();
        //            return ( *argp );
        //        }
        //
        //#endif
        return Socket::Ioctl( cmd, argp );
    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    stimen_t TCPSocket::GetLastSendTime()
    {
        return stime_last_send_;
    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    stimen_t TCPSocket::GetLastRecvTime()
    {
        return stime_last_recv_;
    }
    //=============================================================================
    // NAME: set_tcp_buf_size()
    // DESC: server : SO_RCVBUF client : SO_SENDBUF
    //=============================================================================
    void TCPSocket::SetTCPBufSize( int buf_type, int size )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return;
        }

        //if (GetOPState() == OP_LISTENING)
        //DebugMessage(" call before build !! ", "error");
        int ret = 0;

        if( ret = setsockopt( GetFD(), SOL_SOCKET, buf_type, ( char* )&size, sizeof( size ) ) )
        {
            int errorno = net::Socket::GetLastError();
            PrintErrorCode( errorno );
            SetErrorCode( 1, errno, "setsockopt buffer size failed" );
        }
    }
    //=============================================================================
    // NAME: GetTCPBufSize()
    // DESC: server : SO_RCVBUF client : SO_SENDBUF
    //=============================================================================
    void TCPSocket::GetTCPBufSize( int buf_type, int& size )
    {
#ifdef _WIN32IOCOMPLETIONPORT

        switch( buf_type )
        {
            case SO_RCVBUF:
                {
                    //if ( recv_buffer_ == NULL )
                    //{
                    int optlen = sizeof( size );
                    getsockopt( GetFD(), SOL_SOCKET, buf_type, ( char* )&size, &optlen );
                    //}
                    //else
                    //{
                    //    size = ( ( SocketOV* )recv_buffer_ )->GetBufsize();
                    //}
                }
                break;

            case SO_SNDBUF:
                {
                    //if ( recv_buffer_ == NULL )
                    //{
                    int optlen = sizeof( size );
                    getsockopt( GetFD(), SOL_SOCKET, buf_type, ( char* )&size, &optlen );
                    //}
                    //else
                    //{
                    //    size = ( ( SocketOV* )m_Sendbuf )->GetBufsize();
                    //}
                }
                break;

            default:
                {
                    size = 0;
                }
        }

#else

        switch( buf_type )
        {
            case SO_RCVBUF:
                {
                    int optlen = sizeof( size );
                    getsockopt( GetFD(), SOL_SOCKET, buf_type, ( char* )&size, ( socklen_t* )&optlen );
                }
                break;

            case SO_SNDBUF:
                {
                    int optlen = sizeof( size );
                    getsockopt( GetFD(), SOL_SOCKET, buf_type, ( char* )&size, ( socklen_t* )&optlen );
                }
                break;

            default:
                {
                    size = 0;
                }
        }

#endif
        return;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int TCPSocket::GetReadLength()
    {
        int read_len = Socket::GetReadLength();
#ifdef _WIN32IOCOMPLETIONPORT
        // 有可能已經被iocp ov 搬走了，所以傳搬走的那部分
        if( read_len == 0 )
        {

            // WSARecv 之後tcpbuffer 已經沒有資料
            // 先傳出 已經被 iocp 讀掉的資料
            SocketOV* ov = ( SocketOV* )recv_buffer_;
            // 先嘗試把buffer 裡面的資料再讀一次到iocp中
            read_len = ov->GetUsedsize();
            //ov->PostRead();
        }
#endif
        return read_len;
    }

    //=============================================================================
    // NAME: sendpk()  &recvpk()
    // DESC:
    //=============================================================================
    int TCPSocket::Send( const ubyte_t* bufptr, size32_t buflen, uint_t flag )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        stime_last_send_ = pftml::Time::GetSTime();
        return send( GetFD(), ( const char* )bufptr, buflen, flag );
    }
    //=============================================================================
    // NAME: sendpk()  &recvpk()
    // DESC:
    //=============================================================================
    int TCPSocket::Recv( ubyte_t* bufptr, size32_t buflen, uint_t flag )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        stime_last_recv_ = pftml::Time::GetSTime();
#ifdef _WIN32IOCOMPLETIONPORT

        //
        //
        if( recv_buffer_ == NULL )
        {
            /*  Databuf dbuf;
             dbuf.buf = buf;
             dbuf.buflen = buflen;
             return Recv(&dbuf, 1);  */
            return recv( GetFD(), ( char* )bufptr, buflen, flag );
        }
        else
        {
            SocketOV* ov = ( SocketOV* )recv_buffer_;
            return ov->DoRead( bufptr, buflen, flag );
        }

#else
        return recv( GetFD(), bufptr, buflen, flag );
#endif
    }
    //=============================================================================
    // NAME: Sendn()
    // DESC:
    //=============================================================================
    int TCPSocket::Sendn( const ubyte_t* bufptr, size32_t buflen, uint_t flag )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        int cnt = buflen;

        while( cnt > 0 )
        {
            int sc = ( int )send( GetFD(), ( const char* )bufptr, cnt, flag );

            if( sc < 0 ) /* read error? */
            {
                if( errno == EINTR ) /* interrupted? */
                {
                    continue;
                }  /* restart the read */
                else if( errno == EAGAIN )
                {
                    pfstd::MSSleep( 10 );
                    continue;
                }
                return -1;
            }

            if( sc == 0 ) /* EOF? */
            {
                return buflen - cnt;
            }  /* return short count */

            bufptr += sc;
            cnt -= sc;
        }

        stime_last_send_ = pftml::Time::GetSTime();
        return buflen;
        //return send( GetFD(), bufptr, buflen, MSG_OOB );
    }
    //=============================================================================
    // NAME: Recvn()
    // DESC:
    //=============================================================================
    int TCPSocket::Recvn( ubyte_t* bufptr, size32_t buflen, uint_t flag )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        stime_last_recv_ = pftml::Time::GetSTime();

        int read_len = 0;
        int read_ret = 0;
        {
#ifdef _WIN32IOCOMPLETIONPORT
            SocketOV* ov = ( SocketOV* )recv_buffer_;
            if( ( recv_buffer_ == NULL ) || ( ov->GetUsedsize() == 0 ) )
            {
                read_ret = recv( GetFD(), ( char* )bufptr, buflen, flag );
            }
            else
            {
                read_ret = ov->DoRead( bufptr, buflen, flag );
            }
#else
            read_ret = recv( GetFD(), ( char* )bufptr, buflen, flag );
#endif
        }

        //int last_read_len = -1;  // 以免第一次就被加
        //while( buflen > 0 )
        //  問了幾次 以免陷入無限迴圈
        for( uint_t qi = 0; qi < 20; qi++ )
        {
            if( buflen <= 0 )
            {
                break;
            }

            //
            //printf( "Recvn 6 client %d read %d\n", GetFD(), buflen );
            if( read_ret == -1 ) /* read error? SOCKET_ERROR */
            {
                //printf( "Recvn 6-0 client %d read %d\n", GetFD(), buflen );
                if( errno == EINTR ) /* interrupted? */
                {
                    //printf( "Recvn 6-1 client %d read %d\n", GetFD(), buflen );
                    //continue;
                }  /* restart the read */
                else if( errno == EAGAIN )
                {
                    //printf( "Recvn 6-2 client %d read %d\n", GetFD(), buflen );
                    pfstd::MSSleep( 10 );
                    //continue;
                }
                else
                {
                    //printf( "Recvn 6-3 client %d read %d\n", GetFD(), buflen );
                    //return -1;
                    // 真的收不到了 你不能逼我
                    return read_len;
                }
            }
            else if( read_ret == 0 ) // 對方已關閉
            {
                return 0;
            }
            else
            {
                read_len += read_ret;
                bufptr += read_ret;
                buflen -= read_ret;
            }

            // 還有剩餘的要接收的資料
            if( buflen > 0 )
            {
                read_ret = recv( GetFD(), ( char* )bufptr, buflen, flag );
            }
        }
        return read_len;
    }
    //=============================================================================
    // NAME: sendpk()  &recvpk()
    // DESC:
    //=============================================================================
    int TCPSocket::Sendb( const Databuf* bps, size32_t lens, uint_t flag )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        stime_last_send_ = pftml::Time::GetSTime();
        //SetOPState(OP_WRITING);
        int ret = -1;
#if defined(_WINDOWS)
        //if ( m_Sendbuf == NULL )
        //{
        WSABUF* wbuf = new WSABUF[ lens ];

        for( UINT i = 0; i < lens; i++ )
        {
            wbuf[ i ].buf = ( CHAR* )bps[ i ].buf;
            wbuf[ i ].len = bps[ i ].len;
        }

        // ul=1;
        //ioctlsocket(socket_fd_, FIONBIO, &ul);
        ulong32_t sent = 0;
        ret = WSASend( GetFD(), wbuf, lens, ( LPDWORD )&sent, flag, NULL, NULL );
        delete[] wbuf;

        if( ret == 0 )
        {
            ret = sent;
        }

#else
        struct iovec* iov = new iovec[ lens ];

        for( UINT i = 0; i < lens; i++ )
        {
            iov[ i ].iov_base = bps[ i ].buf;
            iov[ i ].iov_len = bps[ i ].len;
        }

        //n = htonl(iov[1].iov_len);
        ret = writev( GetFD(), iov, lens );
        delete[] iov;
#endif
        return ret;
    }
    //=============================================================================
    // NAME: sendpk()  &recvpk()
    // DESC:
    //=============================================================================
    int TCPSocket::Recvb( Databuf* bps, size32_t lens, uint_t flag )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        stime_last_recv_ = pftml::Time::GetSTime();
        //SetOPState(OP_READING);
#if defined(_WINDOWS)
        WSABUF* wbuf = new WSABUF[ lens ];

        //dword_t recvlen;
        for( UINT i = 0; i < lens; i++ )
        {
            wbuf[ i ].buf = ( CHAR* )bps[ i ].buf;
            wbuf[ i ].len = bps[ i ].len;
        }

        DWORD dwIoSize = 0;
        ULONG ulflag = 0;
        DWORD dwErr = 0;
        OVERLAPPED m_ovIN;
        memset( &m_ovIN, 0, sizeof( OVERLAPPED ) );
        //int ret = WSARecv(socket_fd_, wbuf, 2, &dwIoSize, 0, NULL, NULL);
        UINT iRel = WSARecv( GetFD(), wbuf, lens, &dwIoSize, &ulflag, &m_ovIN, NULL );
        memset( wbuf, 0, sizeof( wbuf ) * lens );
        delete[] wbuf;

        if( iRel == 0 )
        {
            iRel = dwIoSize;
        }

        return iRel;
#else
        struct iovec* iov = new iovec[ lens ];

        for( UINT i = 0; i < lens; i++ )
        {
            iov[ i ].iov_base = bps[ i ].buf;
            iov[ i ].iov_len = bps[ i ].len;
        }

        //n = htonl(iov[1].iov_len);
        int ret = readv( GetFD(), iov, lens );
        delete[] iov;
        return ret;
#endif
        return -1;
    }
    //=============================================================================
    // NAME: readvrec
    // DESC:
    //=============================================================================
    int TCPSocket::Readvrec( ubyte_t* bufptr, size32_t buflen )
    {
        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        stime_last_recv_ = pftml::Time::GetSTime();
        uint32_t reclen;
        /* Retrieve the length of the record */
        int rc = Recvn( ( ubyte_t* )&reclen, sizeof( uint32_t ) );

        if( rc != sizeof( uint32_t ) )
        {
            return rc < 0 ? -1 : 0;
        }

        reclen = ntohl( reclen );

        if( reclen > buflen )
        {
            /* not enough room for the record discard it and return an error */
            while( reclen > 0 )
            {
                rc = Recvn( bufptr, buflen );

                if( rc != static_cast< int >( buflen ) )
                {
                    return rc < 0 ? -1 : 0;
                }

                reclen -= buflen;

                if( reclen < buflen )
                {
                    buflen = reclen;
                }
            }

            set_errno( EMSGSIZE );
            return -1;
        }

        /* Retrieve the record itself */
        rc = Recvn( bufptr, reclen );

        if( rc != static_cast< int >( reclen ) )
        {
            return rc < 0 ? -1 : 0;
        }

        return rc;
    }
    //=============================================================================
    // NAME: readline
    // DESC: read one line and return number of bytes read or -1 on error
    //=============================================================================
    int TCPSocket::Readline( ubyte_t* bufptr, size32_t buflen )
    {
        TS_Lock lock( &tcpsocket_ut_lock_ );

        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        stime_last_recv_ = pftml::Time::GetSTime();
        //SetOPState(OP_READING);
        ubyte_t* bufx = ( ubyte_t* )bufptr;
        //static char* bufptr;
        //static int cnt = 0;
        //static char b[ 1500 ];
        ubyte_t* buf;
        int cnt = 0;
        ubyte_t b[ 1500 ];
        ubyte_t c;

        while( --buflen > 0 )
        {
            if( --cnt <= 0 )
            {
                cnt = recv( GetFD(), ( char* )b, sizeof( b ), 0 );

                if( cnt < 0 )
                {
                    if( errno == EINTR )
                    {
                        buflen++;
                        continue;
                    }

                    return -1;
                }

                if( cnt == 0 )
                {
                    return 0;
                }

                buf = b;
            }

            c = *buf++;
            *buf++ = c;

            if( c == '\n' )
            {
                *buf = '\0';
                return buf - bufx;
            }
        }

        set_errno( EMSGSIZE );
        return -1;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // UDP socket
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    UDPSocket::UDPSocket()
    {
        //Create(AF_INET, SOCK_DGRAM, 0);
    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    UDPSocket::~UDPSocket()
    {
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t UDPSocket::Create( int domain )
    {
        TS_Lock lock( &udpsocket_ut_lock_ );
        return Socket::Create( domain, SOCK_DGRAM, IPPROTO_UDP );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPSocket::Close( void )
    {
        TS_Lock lock( &udpsocket_ut_lock_ );
        return Socket::Close();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool UDPSocket::IsLive( void )
    {
        return ( GetFD() == 0 ) ? false : true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPSocket::SendTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN uint_t flag )
    {
        return ::sendto( GetFD(), ( char* )bufptr, buflen, flag, srcaddr, addrlen );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPSocket::RecvFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN uint_t flag )
    {
        return ::recvfrom( GetFD(), ( char* )bufptr, buflen, flag, srcaddr, ( socklen_t* )&addrlen );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPSocket::SendnTo( const ubyte_t* bufptr, size32_t buflen, const sockaddr* srcaddr, int addrlen, uint_t flag )
    {
        const ubyte_t* send_ptr = bufptr;
        size32_t       send_cnt = buflen;

        while( send_cnt > 0 )
        {
            int cnt = SendTo( send_ptr, send_cnt, srcaddr, addrlen, flag );

            if( cnt < 0 )
            {
                return -1;
            }

            send_ptr += cnt;
            send_cnt -= cnt;
        }

        return buflen;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPSocket::RecvnFrom( ubyte_t* bufptr, size32_t buflen, sockaddr* srcaddr, int& addrlen, uint_t flag )
    {
        ubyte_t* recv_ptr = bufptr;
        size32_t recv_cnt = buflen;

        while( recv_cnt > 0 )
        {
            int cnt = RecvFrom( recv_ptr, recv_cnt, srcaddr, addrlen, flag );

            if( cnt < 0 )
            {
                return -1;
            }

            recv_ptr += cnt;
            recv_cnt -= cnt;
        }

        return buflen;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPSocket::Broadcast( const ubyte_t* bufptr, size32_t buflen, const sockaddr* srcaddr, int addrlen )
    {
        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

        //int ret = 0;
        int flag = 1;  // enable broadcasting

        if( setsockopt( GetFD(), SOL_SOCKET, SO_BROADCAST, ( char* )&flag, sizeof( flag ) ) != SOCKET_ERROR )
        {
            int ret = SendTo( bufptr, buflen, srcaddr, addrlen );
            flag = 0;
            setsockopt( GetFD(), SOL_SOCKET, SO_BROADCAST, ( char* )&flag, sizeof( flag ) );
        }

        return 0;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPSocket::SetMuticast( ipa_t ipa, int timeout, int f )
    {
        //int fd, nbytes, addrlen;
        //char msgbuf[ MSGBUFSIZE ];
        u_int yes = 1;            // MODIFICATION TO ORIGINAL

        // MODIFICATION TO ORIGINAL
        // allow multiple sockets to use the same PORT number
        if( setsockopt( GetFD(), SOL_SOCKET, SO_REUSEADDR, ( const char* )&yes, sizeof( yes ) ) < 0 )
        {
            net::Socket::PrintError( "Reusing ADDR failed" );
            exit( 1 );
        }

        // END OF MODIFICATION TO ORIGINAL
        int level = IP_ADD_MEMBERSHIP;

        if( f != 1 )
        {
            level = IP_DROP_MEMBERSHIP;
        }

        //addrs_t srcaddr_storage;
        //sockaddr* srcaddr = GetPeerAddr( srcaddr_storage );
        //addrs_t addrstorage;
        //memset( &addrstorage, 0, sizeof( addrs_t ) );
        //
        //// set up destination address
        //if ( srcaddr->sa_family = AF_INET )
        //{
        //    //struct sockaddr_in* addr = ( sockaddr_in* ) &addrstorage;
        //    //addr->sin_family = srcaddr->sa_family;
        //    //addr->sin_addr.s_addr = htonl( INADDR_ANY ); /* N.B.: differs from sender */
        //    //addr->sin_port = htons( port );
        //    //
        //    //// bind to receive address
        //    //if ( bind( GetFD(), ( struct sockaddr* ) &addrstorage, sizeof( sockaddr_in ) ) < 0 )
        //    //{
        //    //    net::Socket::PrintError( "bind" );
        //    //    exit( 1 );
        //    //}
        //    struct ip_mreq mreq;
        //    mreq.imr_multiaddr.s_addr = inet_addr( ipa.c_str() );
        //    mreq.imr_interface.s_addr = htonl( INADDR_ANY );
        //    
        //    if ( setsockopt( GetFD(), IPPROTO_IP, level, ( const char* ) &mreq, sizeof( mreq ) ) < 0 )
        //    {
        //        net::Socket::PrintError( "setsockopt" );
        //        exit( 1 );
        //    }
        //    
        //    // !set IP timeout TTL
        //    setsockopt( GetFD(), IPPROTO_IP, IP_MULTICAST_TTL, ( char* ) &timeout, sizeof( int ) );
        //    int loop = 1;
        //    setsockopt( GetFD(), IPPROTO_IP, IP_MULTICAST_LOOP, ( char* ) &loop, sizeof( int ) );
        //}
        //else if ( srcaddr->sa_family = AF_INET6 )
        //{
        //    //struct sockaddr_in6* addr = ( sockaddr_in6* ) &addrstorage;
        //    //memset( &addr, 0, sizeof( addr ) );
        //    //addr->sin6_family = srcaddr->sa_family;
        //    //addr->sin6_addr = in6addr_any; /* N.B.: differs from sender */
        //    //addr->sin6_port = htons( port );
        //    //
        //    //// bind to receive address
        //    //if ( bind( GetFD(), ( struct sockaddr* ) &addrstorage, sizeof( sockaddr_in6 ) ) < 0 )
        //    //{
        //    //    net::Socket::PrintError( "bind" );
        //    //    exit( 1 );
        //    //}
        //    struct ipv6_mreq mreq;
        //    mreq.ipv6mr_multiaddr = net::Socket::IPA6ToN6( ipa.c_str() );
        //    /* Accept multicast from any interface */
        //    mreq.ipv6mr_interface = 0;
        //    
        //    if ( setsockopt( GetFD(), IPPROTO_IPV6, level, ( const char* ) &mreq, sizeof( mreq ) ) < 0 )
        //    {
        //        net::Socket::PrintError( "setsockopt" );
        //        exit( 1 );
        //    }
        //    
        //    // !set IP timeout TTL
        //    setsockopt( GetFD(), IPPROTO_IPV6, IP_MULTICAST_TTL, ( char* ) &timeout, sizeof( int ) );
        //    int loop = 1;
        //    setsockopt( GetFD(), IPPROTO_IPV6, IP_MULTICAST_LOOP, ( char* ) &loop, sizeof( int ) );
        //}

        return 1;
    }
};//namespace iocp


/*
#ifdef _UNIX
//=============================================================================
// NAME:
// DESC:
//=============================================================================
sighndlr_t* signal( int sig, sighndlr_t* hndlr )
{
    struct sigaction act;
    struct sigaction xact;
    act.sa_handler = hndlr;
    act.sa_flag = 0;
    sigemptyset( &act.sa_mask );

    if ( sigaction( sig, &act, &xact ) < 0 )
    { return SIG_ERR; }

    return xact.sa_handler;
}

//=============================================================================
// NAME: init()
// DESC:
//=============================================================================
void init( char** argv )
{
#if defined(_WINDOWS)
    WSADATA wsadata;
    ( program_name = strrchr( argv[0], '\\' ) ) ?
    program_name++ :
    ( program_name = argv[0] );
    WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
#endif
}
#endif



//=============================================================================
// NAME: isconnected()
// DESC:
//=============================================================================

int isconnected( socket_t sockfd, fd_set* rd, fd_set* wr, fd_set* ex )
{
    if ( sockfd == NULL_SOCKET )
    {
        return 0;
    }

#ifdef _LINUX
    int err;
    int buflen = sizeof( err );
    errno = 0; // assume no error

    if ( !FD_ISSET( sockfd, rd ) && !FD_ISSET( sockfd, wr ) )
    {
        return 0;
    }

    if ( getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &err, &buflen ) < 0 )
    {
        return 0;
    }

    //err;  // in case we're not connected
    return err == 0;
#endif
#if defined(_WINDOWS)
    WSASetLastError( 0 );

    if ( !FD_ISSET( sockfd, rd ) && !FD_ISSET( sockfd, wr ) )
    {
        return 0;
    }

    if ( FD_ISSET( sockfd, ex ) )
    {
        return 0;
    }

    return 1;
#endif
    return 0;
}







///////////////////////////////////////////////////////////////////////////////
//

// 2010 1 /20 ?叨?氐兜岑踱?威程繞簞峏X 繞繡?￡簧S繞?乒〣捌?穢?％?〩`??簧??
#define NTIMERS 25

typedef struct tevent_t tevent_t;
struct tevent_t
{
    tevent_t* next;
    struct timeval tv;
    void( *func )( void* );
    void*  arg;
    uint_t id;
};

static tevent_t* active = NULL; //active timers
static tevent_t* free_list = NULL; //inactive timers

//=============================================================================
// NAME: tselect()
// DESC:
//=============================================================================
uint_t timeout( void ( *handler ) ( void* ), void* arg, int ms );
void untimeout( uint_t timerid );
tevent_t* allocate_timer( void );

int tselect( int maxp1, fd_set* re, fd_set* we, fd_set* ee )
{
    fd_set rmask;
    fd_set wmask;
    fd_set emask;
    struct timeval now;
    struct timeval tv;
    struct timeval* tvp;
    tevent_t* tp;
    int n;

    if ( re )
    { rmask = *re; }

    if ( we )
    { wmask = *we; }

    if ( ee )
    { emask = *ee; }

    for ( ; ; )
    {
        if ( gettimeofday( &now, NULL ) < 0 )
        {
            printf( "tselect: gettimeofday failure" );
        }

        while ( active && !timercmp( &now, &active->tv, < ) )
        {
            tp = active;
            active = active->next;
            tp->next = free_list;
            free_list = tp;
            tp->func( tp->arg );
        }

        if ( active )
        {
            tv.tv_sec = active->tv.tv_sec - now.tv_sec;
            tv.tv_usec = active->tv.tv_usec - now.tv_usec;

            if ( tv.tv_usec < 0 )
            {
                tv.tv_sec += 1000000;
                tv.tv_sec--;
            }

            tvp = &tv;
        }
        else if ( ( re == NULL ) && ( we == NULL ) && ( ee == NULL ) )
        { return 0; }
        else
        { tvp = NULL; }

        n = select( maxp1, re, we, ee, tvp );

        if ( n < 0 )
        { return -1; }

        if ( n > 0 )
        { return n; }

        if ( re )
        { *re = rmask; }

        if ( we )
        { *we = wmask; }

        if ( ee )
        { *ee = emask; }
    }
}

//=============================================================================
// NAME: allocate_timer()
// DESC:
//=============================================================================
static tevent_t* allocate_timer( void )
{
    tevent_t* tp;

    if ( free_list == NULL ) // need new block of timers?
    {
        free_list = new tevent_t[NTIMERS];

        if ( free_list == NULL )
        { net::Socket::PrintError( "couldn't allocate timers\n" ); }

        for ( tp = free_list; tp < free_list + NTIMERS - 1; tp++ )
        { tp->next = tp + 1; }

        tp->next = NULL;
    }

    tp = free_list;  //allocate first free
    free_list = tp->next;  //and pop it off list
    return tp;
}

//=============================================================================
// NAME: timeout()
// DESC:
//=============================================================================
uint_t timeout( void ( *func ) ( void* ), void* arg, int ms )
{
    tevent_t* tp;
    tevent_t* tcur;
    tevent_t** tprev;
    static uint_t id = 1;  //timer ID
    tp = allocate_timer();
    tp-> func = func;

    if ( gettimeofday( &tp->tv, NULL ) < 0 )
    { printf( "timeout: gettimeofday failure" ); }

    tp->tv.tv_sec += ms * 1000;

    if ( tp->tv.tv_usec > 1000000 )
    {
        tp->tv.tv_sec += tp->tv.tv_usec / 1000000;
        tp->tv.tv_usec %= 1000000;
    }

    for ( tprev = &active, tcur = active;
            tcur && !timercmp( &tp->tv, &tcur->tv, < );
            tprev = &tcur->next, tcur->next )
    {
        ;
    }

    *tprev = tp;
    tp->next = tcur;
    tp->id = id++;
    return tp->id;
}

//=============================================================================
// NAME: untimeout()
// DESC:
//=============================================================================
void untimeout( uint_t id )
{
    tevent_t** tprev;
    tevent_t* tcur;

    for ( tprev = &active, tcur = active;
            tcur && id != tcur->id;
            tprev = &tcur->next, tcur = tcur->next )
    {
        ;
    }

    if ( tcur == NULL )
    {
        printf( "untimeout called for non-existent timer (%d)\n", id );
        return;
    }
}
*/