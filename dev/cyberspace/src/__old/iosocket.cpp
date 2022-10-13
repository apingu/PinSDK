//Socket.cpp base socket class
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
//   use for all stand server
//                                                         Apingu
//                                                               Sat Mar 16 2002
#if defined(_WIN32) || defined(_WINDOWS)
//#include <Iphlpapi.h>
//#pragma comment ( lib, "Iphlpapi.lib" )   /* link with Win32 lib */ 
#include "IOCPDef.h"
//#include <windows.h>
#include <wincon.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <time.h>
//#include <lm.h>//for nic address
#pragma comment(lib, "Netapi32.lib")//for nic address
//#include <Nb30.h>

#else

#include "EPollDef.h"


#endif //win32



void GetGetAdaptersMacAddress( void );

#include <Debug.h>
#include <assert.h>
#include "IOSocket.h"
#include "unprtt.h"



char* program_name;

#ifdef _LINUX

#define MAXLINE 10 
#define OPEN_MAX 100 
#define LISTENQ 20 
#define SERV_PORT 5555 
#define INFTIM 1000 
sighndlr_t* signal( int sig,sighndlr_t* hndlr );
void        init( char** argv );

#endif


//u_int g_tcpbuffsize=32168;
//////////////////////////////////////////////////////////////////////////////////////////////
//
//  socket class
//
namespace IO
{

    //int Socket::count=0;
    //=============================================================================
    // Name :
    // Desc : Constructor
    //=============================================================================
    Socket::Socket()
    {
        m_sockfd = 0;
        //m_domain = AF_INET;
        //m_streamtype = SOCK_STREAM;
        //m_protocol = 0;
        m_bBlock = false;
        //m_Blockbuf = NULL;
		m_Port=0;
		m_CreateTime=0;
		memset( &m_Ip, 0, sizeof(ipn_t) );
        //m_RunningThreads = 0;
        //m_OPState = OP_INVALID;
		//m_Keycount=0;
		//m_bSetKey=false;


#if defined(_WIN32) || defined(_WINDOWS)
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 2 );
        /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
        err = WSAStartup( wVersionRequested, &wsaData );
        //count++;
        if ( err != 0 )
        {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL. */

            perror( "ERROR: Start up winsock dll failed ~_~b !!!" );
            //MessageBox(NULL,"Start up failed"," Socket Failed", MB_OK);
            return ;
        }
#endif
        //memset( &m_address, 0, sizeof( m_address ) );
        //memset( user_var, 0, sizeof( user_var ) );
    }

    //=============================================================================
    // Name :
    // Desc : Distructor
    //=============================================================================
    Socket::~Socket()
    {
        Close();
        //m_Blockbuf = NULL;
        //m_OPState = OP_INVALID;

#if defined(_WIN32) || defined(_WINDOWS)
        WSACleanup();
        //if(--count==0)
#endif
    }

    //=============================================================================
    // Name : create_socket()
    // Desc : Create one sock
    //=============================================================================
    socket_t Socket::Create( int domain,int streamtype,int protocol )
    {
		m_Lock.Lock();
        if ( m_sockfd != 0 )
        {
            perror( "ERROR: socket already create ~_~b:" );
            return 0;
        }

#ifdef _LINUX
        //socket_t Socket = socket( domain, streamtype, protocol );
        m_sockfd = socket( domain, streamtype, protocol );
        //if( Socket < 0 )
        if ( !isvalidsock( m_sockfd ) )
        {
            //sprintf(StatuMsg,"Create Socket failed ~_~b !!!");
            perror( "ERROR: Unix socket call failed ~_~b:" );

            //cout << "Create Socket failed ~_~b !!! \n";
            return 0;
        }
#endif

#if defined(_WIN32) || defined(_WINDOWS)
        //socket_t Socket = socket( domain, streamtype, protocol );
        m_sockfd = WSASocket( domain,
                              streamtype,
                              protocol,
                              NULL,
                              0,
                              WSA_FLAG_OVERLAPPED );

        if ( m_sockfd == INVALID_SOCKET )
        {
            //sprintf(StatuMsg,"Create Socket failed ~_~b !!!");
            perror( "ERROR: Create win32 socket fail ~_~b:" );
//			switch( WSAGetLastError() )
//			{
//			case WSANOTINITIALISED:    DebugOutPutf("WSANOTINITIALISED\n", "socket"); assert( 0&&"=WSANOTINITIALISED=\n" ); break;
//			case WSAENETDOWN:          DebugOutPutf("WSAENETDOWN\n", "socket");assert( 0&&"=WSAENETDOWN=\n"); break;
//			case WSAEAFNOSUPPORT:      DebugOutPutf("WSAEAFNOSUPPORT\n", "socket");assert( 0&&"=WSAEAFNOSUPPORT=\n"); break;
//			case WSAEINPROGRESS:       DebugOutPutf("WSAEINPROGRESS\n", "socket");assert( 0&&"=WSAEINPROGRESS=\n"); break;
//			case WSAEMFILE:            DebugOutPutf("WSAEMFILE\n", "socket");assert( 0&&"=WSAEMFILE=\n"); break;
//			case WSAENOBUFS: 
//				{
//					if( g_tcpbuffsize< 32168000 )
//					{
//				      g_tcpbuffsize<<1;
//				      Set_tcp_buf_size( SO_RCVBUF|SO_SNDBUF,g_tcpbuffsize );
//				      DebugOutPutf("WSAENOBUFS\n", "socket");assert( 0&&"=WSAENOBUFS=\n");
//					  return Create( domain,streamtype,protocol );
//					}
//					else
//					{
//						return 0;
//						
//					}
//				}
//				break;
//
//			case WSAEPROTONOSUPPORT:   DebugOutPutf("WSAEPROTONOSUPPORT\n", "socket");assert( 0&&"=WSAEPROTONOSUPPORT=\n"); break;
//			case WSAEPROTOTYPE:        DebugOutPutf("WSAEPROTOTYPE\n", "socket");assert( 0&&"=WSAEPROTOTYPE=\n"); break;
//            case WSAESOCKTNOSUPPORT:   DebugOutPutf("WSAESOCKTNOSUPPORT\n", "socket");assert( 0&&"=WSAESOCKTNOSUPPORT=\n"); break;
//			case WSAEINVAL:            DebugOutPutf("WSAEINVAL\n", "socket");assert( 0&&"=WSAEINVAL=\n"); break;
//			case WSAEFAULT:            DebugOutPutf("WSAEFAULT\n", "socket");assert( 0&&"=WSAEFAULT=\n"); break;
//			default:                   DebugOutPutf("undefine\n", "socket");assert( 0&&"=undefine error=\n" ); break;
//			}

            //MessageBox(NULL,"Create Socket failed"," Create Socket Failed", MB_OK);
            //m_OPState = OP_CREATE_FAILURE;
            return 0;
        }
#endif

		time( &m_CreateTime );
		m_Lock.UnLock();

        OnCreate();
        return m_sockfd;
    }


    //=============================================================================
    // Name : close()
    // Desc : close this socket
    //=============================================================================
    void Socket::Close( void )
    {
		
		OnClose();
        TS_Lock lock( &m_Lock );
        if ( m_sockfd != 0 )
        {
            //避免執行緒出問題 所以要先設0
            socket_t closefd = m_sockfd;
            m_sockfd = 0;
			memset( &m_Ip, 0, sizeof(ipn_t) );
			m_Port=0;
			//m_CreateTime=0;
			
#if defined(_WIN32) || defined(_WINDOWS)
            closesocket( closefd );
#else
            close( closefd );
#endif
        }
        sleep( 1 );
    }


    //=============================================================================
    // Name : Shutdown()
    // Desc : Shutdown
    //=============================================================================
    void Socket::Shutdown( int how )
    {
        OnShutdown();
        TS_Lock lock( &m_Lock );

        if ( m_sockfd != 0 )
        {
            ::shutdown( m_sockfd, how );
            m_sockfd = 0;
        }
		//SetKey( NULL );
    }


    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    time_t Socket::GetCreateTime( void )
    {
        return m_CreateTime;
    }


    //=============================================================================
    // Name : GetNicAddress()
    // Desc : Get Nic Address
    //=============================================================================
    /*
    static void PrintMACaddress(unsigned char MACData[])
    {
        printf("MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n", 
            MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
    }
    */
typedef struct _ASTAT_
{

    ADAPTER_STATUS adapt;
    NAME_BUFFER    NameBuff [30];

}ASTAT, * PASTAT;


    MacAddress_t Socket::GetMacAddress( void )
    {
        MacAddress_t MACData;
		MACData.clear();
#if defined(_WIN32) || defined(_WINDOWS)

		ASTAT Adapter;
		NCB Ncb;
        UCHAR uRetCode;
        //char NetName[50];
        LANA_ENUM   lenum;
        int      i;

        memset( &Ncb, 0, sizeof(Ncb) );
        Ncb.ncb_command = NCBENUM;
        Ncb.ncb_buffer = (UCHAR *)&lenum;
        Ncb.ncb_length = sizeof(lenum);
        uRetCode = Netbios( &Ncb );
        //printf( "The NCBENUM return code is: 0x%x \n", uRetCode );

        for(i=0; i < lenum.length ;i++)
		{
            memset( &Ncb, 0, sizeof(Ncb) );
            Ncb.ncb_command = NCBRESET;
            Ncb.ncb_lana_num = lenum.lana[i];

            uRetCode = Netbios( &Ncb );
            //printf( "The NCBRESET on LANA %d return code is: 0x%x \n",
                  //lenum.lana[i], uRetCode );

            memset( &Ncb, 0, sizeof (Ncb) );
            Ncb.ncb_command = NCBASTAT;
            Ncb.ncb_lana_num = lenum.lana[i];

            strcpy( ((char*)Ncb.ncb_callname),  "*               " );
            Ncb.ncb_buffer = (unsigned char *) &Adapter;
            Ncb.ncb_length = sizeof(Adapter);

            uRetCode = Netbios( &Ncb );
            //printf( "The NCBASTAT on LANA %d return code is: 0x%x \n",
                    //lenum.lana[i], uRetCode );
            if ( uRetCode == 0 )
			{
              //printf( "The Ethernet Number on LANA %d is:
                       //%02x%02x%02x%02x%02x%02x\n",
                  //lenum.lana[i],
                  MACData[0]=Adapter.adapt.adapter_address[0];
                  MACData[1]=Adapter.adapt.adapter_address[1];
                  MACData[2]=Adapter.adapt.adapter_address[2];
                  MACData[3]=Adapter.adapt.adapter_address[3];
                  MACData[4]=Adapter.adapt.adapter_address[4];
                  MACData[5]=Adapter.adapt.adapter_address[5];
          }
       }

#endif
        return MACData;
    }

    //=============================================================================
    // Name : GetIP()
    // Desc :
    //=============================================================================
    ipn_t Socket::GetLocalAddress( unsigned int i )
    {
		char buff[128];
        hostent* lpHost;

        in_addr ipn;
		ipn.S_un.S_addr=0;

		gethostname( buff, 128 );
        if ( ( lpHost = gethostbyname( buff ) ) == NULL )
        {
            return ipn;
        }
		if(lpHost->h_addr_list[i]==NULL)
			return ipn;
        return *( in_addr* ) ( lpHost->h_addr_list[i] );
    }

    //=============================================================================
    // Name : GetHostName()
    // Desc : get local host name
    //=============================================================================
    char* Socket::GetLocalHostname( char* buf,size_t len )
    {
        if ( gethostname( buf, len ) != 0 )
        {
            return NULL;
        }
        return buf;
    }

    //=============================================================================
    // Name : GetPeerAddress
    // Desc : get remote address
    //=============================================================================
    ipn_t Socket::GetPeerAddress( void )
    {
        //unsigned char * cp;
//#ifdef _WIN32IOCOMPLETIONPORT
//        SOCKADDR_IN* local, * remote;
//        int llocal, lremote;
//        SocketOV* ov = ( SocketOV* ) m_Blockbuf;
//        GetAcceptExSockaddrs( &( ov->buf[0] ),
//                              INITIALRECEIVESIZE,
//                              ACCEPT_ADDRLEN,
//                              ACCEPT_ADDRLEN,
//                              ( LPSOCKADDR * ) &local,
//                              &llocal,
//                              ( LPSOCKADDR * ) &remote,
//                              &lremote );
//        cp = ( unsigned char * ) &remote->sin_addr;
//#else
		return m_Ip;
//        sockaddr_in remote;
//        int len = sizeof( remote );
//        if ( getsockname( m_sockfd, ( sockaddr * ) &remote, (int*)&len ) ==
//             SOCKET_ERROR )
//        {
//			ipn_t ipn;
//			ipn.S_un.S_addr=0;
//            return ipn;
//        }
//        else
//            return remote.sin_addr;
//#endif
        //sprintf( buf, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3] );
        //return buf;
    }
    //=============================================================================
    // Name : OIControl()
    // Desc :
    //=============================================================================
    int Socket::Ioctl( long cmd,unsigned long* argp )
    {
#ifdef _WIN32
        return ioctlsocket( m_sockfd, cmd, argp );
#endif
#ifdef _LINUX
		return ioctl( m_sockfd, cmd, argp );
#endif
    }

    //=============================================================================
    // Name : SetOpt()
    // Desc :
    //=============================================================================
    int Socket::SetOpt( int level,
                        int optname,
                        const void* optval,
                        socklen_t optlen )
    {
        return setsockopt( m_sockfd,
                           level,
                           optname,
                           ( const char * ) optval,
                           optlen );
    }


	/*
    //=============================================================================
    // Name : SetKey()
    // Desc :
    //=============================================================================
    void Socket::SetKey( void* key )
    {
		TS_Lock lock( &m_lKey );
//#ifdef _WIN32IOCOMPLETIONPORT
//		//oldkey=( ( SocketOV * ) m_Blockbuf )->key;
//		if( m_Blockbuf!=NULL )
//		{
//            ( ( SocketOV * ) m_Blockbuf )->key = key;
//		}
//        //return;
//#else
        m_Blockbuf = key;
		//return;
//#endif

//		if( key !=NULL )
//		{
//			m_bSetKey=true;
//			m_Keycount++;
//			m_BackupKey=m_Blockbuf;
//			if( m_Keycount!=1 )
//			{
//				assert( 0&&"set key!=0");
//			}
//			
//		}
//		else
//		{
//			m_bSetKey=false;
//			m_Keycount=0;
//			m_BackupKey=NULL;
//		}

//		m_BackupKey=NULL;
		
		return;

    }


    //=============================================================================
    // Name : OIControl()
    // Desc :
    //=============================================================================
    void* Socket::GetKey( void )
    {
		TS_Lock lock( &m_lKey );
		//void* oldkey=NULL;
//#ifdef _WIN32IOCOMPLETIONPORT
//        if ( !m_Blockbuf )
//            return NULL;
//        else
//		{
//			oldkey=( ( SocketOV * ) m_Blockbuf )->key;
//			//( ( SocketOV * ) m_Blockbuf )->key=NULL;
//		}
//#else
		//oldkey=m_Blockbuf;
		//m_Blockbuf=NULL;
//#endif
//		m_BackupKey=oldkey;
//		if( oldkey !=NULL )
//		{
//		    m_Keycount--;
//			if( m_Keycount!=0 )
//			{
//				assert( 0&&"get key!=0");
//			}
//		}

        return m_Blockbuf;
    }
	*/

    //=============================================================================
    // Name : GetErrorNo()
    // Desc :
    //=============================================================================
    int Socket::GetErrorNo( void )
    {
#ifdef _LINUX
        return errno;
#endif
#if defined(_WIN32) || defined(_WINDOWS)
        return WSAGetLastError();
#endif
    }


    //=============================================================================
    // Name : error()
    // Desc : print error status
    //=============================================================================
    void Socket::SetErrorCode( int status,int err,char* fmt,... )
    {
        va_list ap;
        va_start( ap, fmt );
        if ( program_name )
        {
            fprintf( stderr, "%s: ", program_name );
            vfprintf( stderr, fmt, ap );
        }
        va_end( ap );
        if ( err )
        {
            fprintf( stderr, ": %s (%d)\n", strerror( err ), err );
        }
        if ( status )
            EXIT( status );

        return;
    }

//    //=============================================================================
//    //  Name :
//    //  Desc :
//    //=============================================================================
//	Socket::OPSTATE Socket::GetOPState( void ) 
//	{
//		TS_Lock lock(&m_Lock);
//		return m_OPState;
//	}
//
//
//    //=============================================================================
//    //  Name :
//    //  Desc :
//    //=============================================================================
//	void Socket::SetOPState( Socket::OPSTATE op )
//	{
//		TS_Lock lock(&m_Lock);
//		m_OPState=op;
//		return;
//	}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Socket::PrintErrorCode( int code )
    {
        switch ( GetLastError() )
        {
#if defined(_WIN32) || defined(_WINDOWS)
          case ERROR_NETNAME_DELETED:
            printf( "The specified network name is no longer available.\n" );
            break;

          case WSANOTINITIALISED:
            perror( "ERROR Shutdown: A successful WSAStartup call must occur before using this function\n" ); 
            break;
          case WSAENETDOWN:
            perror( "ERROR Shutdown: The network subsystem has failed\n" );
            break;
          case WSAEINVAL:
            perror( "ERROR Shutdown: The how parameter is not valid, or is not consistent with the socket optype. For example, SD_SEND is used with a UNI_RECV socket optype\n" ); 
            break;
          case WSAEINPROGRESS:
            perror( "ERROR Shutdown: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function\n" ); 
            break;
          case WSAENOTCONN:
            perror( "ERROR Shutdown: The socket is not connected (connection-oriented sockets only)\n" );
            break;
          case WSAENOTSOCK:
            perror( "ERROR Shutdown: The descriptor is not a socket\n" );
            break;

#else

#endif
        }
    }


    ///////////////////////////////////////////////////////////////////////////////
    // TCP socket

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    TCPSocket::TCPSocket()
    {
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* recvov=new SocketOV( this );
		m_Recvbuf=recvov;

        SocketOV* sendov=new SocketOV( this );
        m_Sendbuf=sendov;
#endif
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    TCPSocket::~TCPSocket()
    {
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* rov=(SocketOV*)m_Recvbuf;
		SocketOV* sov=(SocketOV*)m_Sendbuf;
		SAFE_DELETE( rov );
		SAFE_DELETE( sov );
#endif
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    socket_t TCPSocket::Create( int domain )
    {
		
		m_lTcp.Lock();
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* recvov=(SocketOV*)m_Recvbuf;
		if( recvov!=0 )
		{
			recvov->Reset( OP_ACCEPTING );
		}

        SocketOV* sendov=(SocketOV*)m_Sendbuf;
		if( sendov!=NULL )
		{
			sendov->Reset( OP_ACCEPTING );
		}
#endif		
		m_lTcp.UnLock();
        return Socket::Create( domain, SOCK_STREAM, IPPROTO_TCP );
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void TCPSocket::Shutdown( int how )
	{
     	TS_Lock lock(&m_lTcp);
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* recvov=(SocketOV*)m_Recvbuf;
		if( recvov!=0 )
		{
			recvov->Reset( OP_CLOSED );
		}

        SocketOV* sendov=(SocketOV*)m_Sendbuf;
		if( sendov!=NULL )
		{
			sendov->Reset( OP_CLOSED );
		}
#endif
		Socket::Shutdown( how );
		return;
	}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void TCPSocket::Close( void )
	{
		TS_Lock lock(&m_lTcp);
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* recvov=(SocketOV*)m_Recvbuf;
		if( recvov!=0 )
		{
			recvov->Reset( OP_CLOSED );
		}

        SocketOV* sendov=(SocketOV*)m_Sendbuf;
		if( sendov!=NULL )
		{
			sendov->Reset( OP_CLOSED );
		}
#endif
		Socket::Close();
		return;
	}

    //=============================================================================
    //  Name : set_tcp_buf_size()
    //  Desc : server : SO_RCVBUF client : SO_SENDBUF
    //=============================================================================
    void TCPSocket::Set_tcp_buf_size( int buf_type,int size )
    {
		TS_Lock lock(&m_lTcp);

        if ( m_sockfd == NULL )
            return;

        //if ( GetOPState() == OP_LISTENING )
            //DebugMessage( " call before build !! ", "error" );

        if ( setsockopt( m_sockfd,
                         SOL_SOCKET,
                         buf_type,
                         ( char * ) &size,
                         sizeof( size ) ) )
            SetErrorCode( 1, errno, "setsockopt buffer size failed" );
    }


    //=============================================================================
    //  Name : sendpk() & recvpk()
    //  Desc :
    //=============================================================================
    int TCPSocket::Send( char* bp,size_t len,unsigned int flag )
    {
		TS_Lock lock(&m_lTcp);

        if ( m_sockfd == NULL )
            return -1;

        return send( m_sockfd, bp, len, flag );
    }

    //=============================================================================
    //  Name : sendpk() & recvpk()
    //  Desc :
    //=============================================================================
    int TCPSocket::Recv( char* bp,size_t len,unsigned int flag )
    {
		TS_Lock lock(&m_lTcp);

#ifdef _WIN32IOCOMPLETIONPORT
//        // not ov
		//暫時這樣做
        if ( m_Recvbuf == NULL )
        {
	        if ( m_sockfd == NULL )
                return -1;
            /*  Databuf dbuf;
                dbuf.buf = bp;
                dbuf.len = len;
                return Recv( &dbuf, 1 );  */
            return recv( m_sockfd, bp, len, flag );
        }

        SocketOV* ov = ( SocketOV* )m_Recvbuf;
        return ov->DoRead( bp, len, flag );
#else
        return recv( m_sockfd, bp, len, flag );
#endif
    }

    //=============================================================================
    //  Name : sendpk() & recvpk()
    //  Desc :
    //=============================================================================
	int TCPSocket::Send( Databuf* bps, unsigned int lens, unsigned int flags )
    {
		TS_Lock lock(&m_lTcp);

        if ( m_sockfd == NULL )
            return -1;
	
        //SetOPState( OP_WRITING );
        int ret = -1;

#ifdef _LINUX
        struct iovec* iov = new iovec[lens];
        for ( UINT i = 0; i < lens; i++ )
        {
            iov[i].iov_base = bps[i].buf;
            iov[i].iov_len = bps[i].len;
        }
        //n = htonl( iov[1].iov_len );
        ret = writev( m_sockfd, iov, lens );
        delete [] iov;

#endif
#if defined(_WIN32) || defined(_WINDOWS)

		//暫時這樣做但cleint 也有可能被blocking，記得要改
		if ( m_Sendbuf == NULL )
		{
			WSABUF* wbuf = new WSABUF[lens];
			for ( UINT i = 0; i < lens; i++ )
			{
				wbuf[i].buf = bps[i].buf;
				wbuf[i].len = bps[i].len;
			}

			// ul=1;
			//ioctlsocket( m_sockfd, FIONBIO, &ul );
			unsigned long sent=0;
			ret = WSASend( m_sockfd, wbuf, lens, &sent, flags, NULL, NULL );
			delete [] wbuf;
			if( ret==0 )
				ret = sent;
		}
		else
		{
			SocketOV* ov=( SocketOV* )m_Sendbuf;
		    ov->PostSend( bps, lens, flags );
	        ret=ov->DoSend();
		    //PostRead( (SocketOV*)m_Recvbuf );
		}
#endif
        return ret;
    }

    //=============================================================================
    //  Name : sendpk() & recvpk()
    //  Desc :
    //=============================================================================
    int TCPSocket::Recv( Databuf* bps,unsigned int lens,unsigned int flags )
    {
		TS_Lock lock(&m_lTcp);

        if ( m_sockfd == NULL )
            return -1;

        //SetOPState( OP_READING );

#ifdef _LINUX
        struct iovec* iov = new iovec[lens];
        for ( UINT i = 0; i < lens; i++ )
        {
            iov[i].iov_base = bps[i].buf;
            iov[i].iov_len = bps[i].len;
        }
        //n = htonl( iov[1].iov_len );
        int ret = Readv( m_sockfd, iov, lens );
        delete [] iov;
        return ret;
#endif
#if defined(_WIN32) || defined(_WINDOWS)
        WSABUF* wbuf = new WSABUF[lens];
        //DWORD recvlen;
        for ( UINT i = 0; i < lens; i++ )
        {
            wbuf[i].buf = bps[i].buf;
            wbuf[i].len = bps[i].len;
        }

        DWORD dwIoSize = 0;
        ULONG ulFlags = 0;
        DWORD dwErr = 0;
        OVERLAPPED m_ovIN;
        memset( &m_ovIN, 0, sizeof( OVERLAPPED ) );

        //int ret = WSARecv( m_sockfd, wbuf, 2, &dwIoSize, 0, NULL, NULL );

        UINT iRel = WSARecv( m_sockfd,
                             wbuf,
                             lens,
                             &dwIoSize,
                             &ulFlags,
                             &m_ovIN,
                             NULL );

        memset( wbuf, 0, sizeof( wbuf ) * lens );
        delete [] wbuf;
		if( iRel==0 )
			iRel=dwIoSize;
        return iRel;
#endif
    }

    //=============================================================================
    //  Name : Sendn()
    //  Desc :
    //=============================================================================
    int TCPSocket::Sendn( char* bp,size_t len )
    {
		TS_Lock lock(&m_lTcp);

        if ( m_sockfd == NULL )
            return -1;

        //SetOPState( OP_WRITING );

        int ret = send( m_sockfd, bp, len, MSG_OOB );
        return ret;
    }

    //=============================================================================
    //  Name : Recvn()
    //  Desc :
    //=============================================================================
    int TCPSocket::Recvn( char* bp,size_t len )
    {
        return Readn( bp, len );
    }


    //=============================================================================
    //  Name : readn()
    //  Desc :
    //=============================================================================
    int TCPSocket::Readn( char* bp,size_t len )
    {
		TS_Lock lock(&m_lTcp);

        if ( m_sockfd == NULL )
            return -1;

        //SetOPState( OP_READING );

        int cnt;
        int rc;
        cnt = len;
        while ( cnt > 0 )
        {
            rc = recv( m_sockfd, bp, cnt, 0 );
            if ( rc < 0 )  /* read error? */
            {
                if ( errno == EINTR )  /* interrupted? */
                    continue;  /* restart the read */
                return -1;
            }
            if ( rc == 0 )  /* EOF? */
                return len - cnt;  /* return short count */
            bp += rc;
            cnt -= rc;
        }

        return len;
    }


    //=============================================================================
    //  Name : readvrec
    //  Desc :
    //=============================================================================
    int TCPSocket::Readvrec( char* bp,size_t len )
    {
        if ( m_sockfd == NULL )
            return -1;
		

        u_int32_t reclen;
      

        /* Retrieve the length of the record */

        int rc = Readn( ( char* ) &reclen, sizeof( u_int32_t ) );
        if ( rc != sizeof( u_int32_t ) )
            return rc < 0 ? -1 : 0;
        reclen = ntohl( reclen );
        if ( reclen > len )
        {
            /* not enough room for the record discard it and return an error */
            while ( reclen > 0 )
            {
                rc = Readn( bp, len );
                if ( rc != static_cast<int>( len ) )
                    return rc < 0 ? -1 : 0;
                reclen -= len;
                if ( reclen < len )
                    len = reclen;
            }
            set_errno( EMSGSIZE );
            return -1;
        }

        /* Retrieve the record itself */
        rc = Readn( bp, reclen );
        if ( rc != static_cast<int>( reclen ) )
            return rc < 0 ? -1 : 0;
        return rc;
    }

    //=============================================================================
    //  Name : readline
    //  Desc : read one line and return number of bytes read or -1 on error
    //=============================================================================
    int TCPSocket::Readline( char* bufptr,size_t len )
    {
		TS_Lock lock(&m_lTcp);

        if ( m_sockfd == NULL )
            return -1;
		
        //SetOPState( OP_READING );

        char* bufx = bufptr;
        static char* bp;
        static int cnt = 0;
        static char b[1500];
        char c;

        while ( --len > 0 )
        {
            if ( --cnt <= 0 )
            {
                cnt = recv( m_sockfd, b, sizeof( b ), 0 );
                if ( cnt < 0 )
                {
                    if ( errno == EINTR )
                    {
                        len++;
                        continue;
                    }
                    return -1;
                }
                if ( cnt == 0 )
                    return 0;
                bp = b;
            }

            c = *bp++;
            *bufptr++ = c;
            if ( c == '\n' )
            {
                *bufptr = '\0';
                return bufptr - bufx;
            }
        }
        set_errno( EMSGSIZE );
        return -1;
    }



    ///////////////////////////////////////////////////////////////////////////////
    // UDP socket

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    UDPSocket::UDPSocket()
    {
        //Create( AF_INET, SOCK_DGRAM, 0 );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    UDPSocket::~UDPSocket()
    {
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    socket_t UDPSocket::Create( int domain )
    {
        return Socket::Create( domain, SOCK_DGRAM, IPPROTO_UDP );
    }

    //=============================================================================
    // Name : udp_server()
    // Desc :
    //=============================================================================
    int UDPSocket::Service( char* hname,char* sname )
    {
        if ( m_sockfd == NULL )
            return -1;

        //char localaddr[255];

        struct sockaddr_in local;

        set_address( inet_ntoa( GetLocalAddress(0) ), sname, &local, "udp" );

        if ( bind( m_sockfd, ( struct sockaddr * ) &local, sizeof( local ) ) )
        {
            SetErrorCode( 1, errno, "bind failed" );
			//SetOPState( OP_FAILURE );
			return -1;
        }

        //m_Style = STYLE_SERVICE;
		//SetOPState( OP_LISTENING );
		return -1;
    }


    //=============================================================================
    // Name : udp_client()
    // Desc :
    //=============================================================================
    int UDPSocket::Connect( char* hname,char* sname,struct sockaddr_in* sap )
    {
        set_address( hname, sname, sap, "udp" );
        //m_Style = STYLE_CLIENT;
		//SetOPState( OP_CONNECTED ); 
		return 1;
    }

    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int UDPSocket::Broadcast( unsigned long ipaddr,
                              const char* bp,
                              unsigned int len )
    {
        /*
              if ( m_sockfd == NULL )
                  return m_OPState = SOCKETOP_UNCREATE;
              m_OPState = OP_WRITING;
              int flag = 1;        // enable broadcasting
            address address;
              if ( setsockopt( m_sockfd,
                               SOL_SOCKET,
                               SO_BROADCAST,
                               ( char * ) &flag,
                               sizeof( flag ) ) != SOCKET_ERROR )
              {
                  struct sockaddr_in sa;
                  sa.sin_family = AF_INET;
                  sa.sin_port = m_address.sin_port;
                  sa.sin_addr.s_addr = ipaddr;
                  return sendto( m_sockfd,
                                 bp,
                                 len,
                                 0,
                                 ( struct sockaddr * ) &sa,
                                 sizeof( sa ) );
              }
            */

        return 0;
    }

    //=============================================================================
    // Name : udp_client()
    // Desc :
    //=============================================================================
    int UDPSocket::Sendto( unsigned long ipaddr,
                           char* bp,
                           unsigned int len,
                           unsigned int flags )
    {
        /*
              if ( m_sockfd == NULL )
                  return m_OPState = SOCKETOP_UNCREATE;
              m_OPState = OP_WRITING;
              struct sockaddr_in sa;
              sa.sin_family = AF_INET;
              sa.sin_port = htons( m_address.sin_port );
              sa.sin_addr.s_addr = ipaddr;
              return ::sendto( m_sockfd,
                               bp,
                               len,
                               flags,
                               ( struct sockaddr * ) &sa,
                               sizeof( sa ) );
                             */
        return 0;
    }


    //=============================================================================
    // Name : udp_client()
    // Desc :
    //=============================================================================
    int UDPSocket::Recvfrom( unsigned long& ipaddr,
                             char* bp,
                             unsigned int len,
                             unsigned int flags )
    {
        if ( m_sockfd == NULL )
            return -1;

		//SetOPState( OP_READING );

        struct sockaddr_in sa;
        int salen = sizeof( sa );
        int ret = ::recvfrom( m_sockfd,
                              bp,
                              len,
                              flags,
                              ( struct sockaddr* ) &sa,
                              &salen );
        ipaddr = sa.sin_addr.s_addr;
        return ret;
    }


    //=============================================================================
    // Name : udp_client()
    // Desc :
    //=============================================================================
    int UDPSocket::Sendto( unsigned long ipaddr,
                           Databuf* bps,
                           unsigned int lens,
                           unsigned int flags )
    {
        /*
              if ( m_sockfd == NULL )
                  return m_OPState = SOCKETOP_UNCREATE;
              m_OPState = OP_WRITING;
        #ifdef _LINUX
              struct msghdr msgsend;
              struct sockaddr_in sa;
              sa.sin_family = AF_INET;
              sa.sin_port = htons( m_address.sin_port );
              sa.sin_addr.s_addr = ipaddr;
              iovec* iov = new iovec[lens];
              msgsend.msg_name = &sa;
              msgsend.msg_len = sizeof( sa );
              msgsend.msg_iov = iov;
              msgsend.msg_iovlen = lens;
              for ( UINT i = 0; i < lens; i++ )
              {
                  iov[i].iov_base = Databuf[i].buf;
                  iov[i].iov_len = Databuf[i].len;
              }
              int ret = Sendmsg( m_sockfd, msgsend, flags );
              delete [] iov;
              return ret;
        #endif
        #if defined(_WIN32) || defined(_WINDOWS)
              //int n;
              struct sockaddr_in sa;
              sa.sin_family = AF_INET;
              sa.sin_port = htons( m_address.sin_port );
              sa.sin_addr.s_addr = ipaddr;
              WSABUF* wbuf = new WSABUF[lens];
              DWORD sent;
              for ( UINT i = 0; i < lens; i++ )
              {
                  wbuf[i].buf = bps[i].buf;
                  wbuf[i].len = bps[i].len;
              }
              int ret = WSASendTo( m_sockfd,
                                   wbuf,
                                   lens,
                                   &sent,
                                   flags,
                                   ( struct sockaddr* ) &sa,
                                   sizeof( sa ),
                                   NULL,
                                   NULL );
              delete [] wbuf;
              return ret;
        #endif
            */
        return 0;
    }


    //=============================================================================
    // Name : Recvhm()
    // Desc :
    //=============================================================================
    int UDPSocket::Recvfrom( unsigned long& ipaddr,
                             Databuf* bps,
                             unsigned int lens,
                             unsigned int flags )
    {
        /*
              if ( m_sockfd == NULL )
                  return m_OPState = SOCKETOP_UNCREATE;
              m_OPState = OP_READING;
        #ifdef _LINUX
        #endif
        #if defined(_WIN32) || defined(_WINDOWS)
              struct sockaddr_in sa;
              int salen = sizeof( sa );
              sa.sin_family = AF_INET;
              sa.sin_port = htons( m_address.sin_port );
              sa.sin_addr.s_addr = ipaddr;
              WSABUF* wbuf = new WSABUF[lens];
              for ( UINT i = 0; i < lens; i++ )
              {
                  wbuf[i].buf = bps[i].buf;
                  wbuf[i].len = bps[i].len;
              }
              DWORD sent;
              int ret = WSARecvFrom( m_sockfd,
                                     wbuf,
                                     lens,
                                     &sent,
                                     ( ( unsigned long* ) &flags ),
                                     ( struct sockaddr* ) &sa,
                                     &salen,
                                     NULL,
                                     NULL );
              ipaddr = sa.sin_addr.s_addr;
              delete [] wbuf;
              return ret;
        #endif
        */
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // Reliable UDP
    struct hdr
    {
        unsigned int seq;
        unsigned int ts;
    }sendhdr, recvhdr;
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    RUDPSocket::RUDPSocket()
    {
        RTT::rtt_init( ( RTT::rtt_info * ) &m_RttTime );
        memset( &m_Sendhdr, 0, sizeof( m_Sendhdr ) );
        memset( &m_Recvhdr, 0, sizeof( m_Recvhdr ) );
    }

    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    RUDPSocket::~RUDPSocket()
    {
    }


    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::Sendhm( unsigned long ipaddr,
                            Databuf* bps,
                            unsigned int lens,
                            unsigned int flags )
    {
        return 0;
    }

    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::Recvhm( unsigned long& ipaddr,
                            Databuf* bps,
                            unsigned int lens,
                            unsigned int flags )
    {
        return 0;
    }

    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::Sendto( unsigned long ipaddr,char* bp,unsigned int len )
    {
        /*
              if ( m_sockfd == NULL )
                  return m_OPState = SOCKETOP_UNCREATE;
              m_OPState = OP_WRITING;
              ( ( hdr * ) &m_Sendhdr )->seq++;
        #ifdef _LINUX
              struct msghdr msgsend;
              struct sockaddr_in sa;
              sa.sin_family = AF_INET;
              sa.sin_port = htons( m_address.sin_port );
              sa.sin_addr.s_addr = ipaddr;
              iovec iov[2];
              msgsend.msg_name = &sa;
              msgsend.msg_len = sizeof( sa );
              msgsend.msg_iov = iov;
              msgsend.msg_iovlen = 2;
              iov[0].iov_base = ( char * ) m_Sendhdr;
              iov[0].iov_len = sizeof( hdr );
              iov[1].iov_base = bp;
              iov[1].iov_len = len;
              Signal( SIGALRM, sig_alrm );
              RTT::rtt_newpack( ( RTT::rtt_info * ) &m_RttTime );
              sendagain:
              ( ( hdr * ) &m_Sendhdr )->ts = rtt_ts( ( RTT::rtt_info * ) &m_RttTime );
              Sendmag( m_sockfd, &msgsend, 0 );
              alarm( rtt_start( (RTT::rtt_info*)&m_RttTime );
              if( sinsetjmp( jmpbuf, 1 )1= 0 )    
        #endif
        #if defined(_WIN32) || defined(_WINDOWS)
              //int n;
              struct sockaddr_in sa;
              sa.sin_family = AF_INET;
              sa.sin_port = htons( m_address.sin_port );
              sa.sin_addr.s_addr = ipaddr;
              WSABUF wbuf[2];
              DWORD sent;
              wbuf[0].buf = ( char * ) m_Sendhdr;
              wbuf[0].len = sizeof( hdr );
              wbuf[1].buf = bp;
              wbuf[1].len = len;
              int ret = WSASendTo( m_sockfd,
                                   wbuf,
                                   2,
                                   &sent,
                                   0,
                                   ( struct sockaddr* ) &sa,
                                   sizeof( sa ),
                                   NULL,
                                   NULL );
        #endif
        */

        return 0;
    }

    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::Recvfrom( unsigned long& ipaddr,char* bp,unsigned int len )
    {
        if ( m_sockfd == NULL )
            return -1;

		//SetOPState( OP_READING );

        return 0;
    }

    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::Broadcast( unsigned long ipaddr,
                               const char* bp,
                               unsigned int len )
    {
        if ( m_sockfd == NULL )
            return -1;

		//SetOPState( OP_WRITING );

        return 0;
    }
};//namespace iocp


#ifdef _LINUX
//=============================================================================
//  Name :
//  Desc :
//=============================================================================
sighndlr_t* signal( int sig,sighndlr_t* hndlr )
{
    struct sigaction act;
    struct sigaction xact;
    act.sa_handler = hndlr;
    act.sa_flags = 0;
    sigemptyset( &act.sa_mask );
    if ( sigaction( sig, &act, &xact ) < 0 )
        return SIG_ERR;
    return xact.sa_handler;
}

//=============================================================================
//  Name : init() 
//  Desc :
//=============================================================================
void init( char** argv )
{
#if defined(_WIN32) || defined(_WINDOWS)
    WSADATA wsadata;

    ( program_name = strrchr( argv[0], '\\' ) ) ?
    program_name++ :
    ( program_name = argv[0] );
    WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
#endif
}


#endif

//=============================================================================
// Name : set_address()
// Desc : set one sockaddr_in value
//=============================================================================
void set_address( const char* ipaddr,
                  const char* portnum,
                  struct sockaddr_in* sap,
                  const char* protocol )
{
    struct servent* sp;
    struct hostent* hp;
    char* endptr;
    short port;

    memset( sap, 0, sizeof( *sap ) );
    sap->sin_family = AF_INET;
    if ( ipaddr != NULL )
    {
        if ( !inet_aton( ipaddr, &sap->sin_addr ) )
        {
            hp = gethostbyname( ipaddr );
            if ( hp == NULL )
            {
                char msg[255];
                sprintf( msg, "unknow host: %s\n", ipaddr );
                DebugMessage( msg, "set address " );
            }
            else
                sap->sin_addr = *( struct in_addr * ) hp->h_addr;
        }
    }
    else
        sap->sin_addr.s_addr = htonl( INADDR_ANY );

    port = strtol( portnum, &endptr, 0 );
    if ( *endptr == '\0' )
        sap->sin_port = htons( port );
    else
    {
        sp = getservbyname( portnum, protocol );
        if ( sp == NULL )
        {
            char msg[255];
            sprintf( msg, "unknow service: %s\n", portnum );
            DebugMessage( msg, "set address " );
            //error( 1, 0, "unknow service: %s\n", portnum );
        }
        sap->sin_port = sp->s_port;
    }
    //memcpy( &m_address, sap, sizeof(m_address) );
    return;
}

//=============================================================================
// Name : isconnected()
// Desc : 
//=============================================================================
int isconnected( socket_t sockfd,fd_set* rd,fd_set* wr,fd_set* ex )
{
    if ( sockfd == NULL )
        return 0;

#ifdef _LINUX
    int err;
    int len = sizeof( err );

    errno = 0;    // assume no error
    if ( !FD_ISSET( sockfd, rd ) && !FD_ISSET( sockfd, wr ) )
        return 0;
    if ( getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &err, &len ) < 0 )
        return 0;
    //err;  // in case we're not connected
    return err == 0;

#endif
#if defined(_WIN32) || defined(_WINDOWS)
    WSASetLastError( 0 );
    if ( !FD_ISSET( sockfd, rd ) && !FD_ISSET( sockfd, wr ) )
        return 0;
    if ( FD_ISSET( sockfd, ex ) )
        return 0;
    return 1;

#endif

    return 0;
}



//=============================================================================
//  Name : inet_aton
//  Desc :
//=============================================================================
int inet_aton( const char* cp,struct in_addr* pin )
{
    int rc = inet_addr( cp );
    if ( rc == -1 && strcmp( cp, "255.255.255.255" ) )
        return 0;
    pin->s_addr = rc;
    return 1;
}

//=============================================================================
//  Name : get_ipa
//  Desc : get ipaddr with char by host name
//=============================================================================
char* get_ipa( char* hname )
{
    struct in_addr in;
    in.s_addr = get_ipn( hname );
    return inet_ntoa( in );
}

//=============================================================================
//  Name : get_ipn
//  Desc : get ipaddr with int by host name
//=============================================================================
int get_ipn( char* hname )
{
    struct sockaddr_in local_addr;
    struct hostent* ht = gethostbyname( hname );
    memcpy( &local_addr.sin_addr.s_addr, ht->h_addr, ht->h_length );//change char* to int in right format
    return local_addr.sin_addr.s_addr;
}

ipn_t get_ipaton( ipa_t host )
{
    in_addr addr;
    inet_aton( host, &addr );
    return addr;
}


ipa_t get_ipntoa( ipn_t host )
{
    in_addr addr;
    addr = host;
    return inet_ntoa( addr );
}





///////////////////////////////////////////////////////////////////////////////
//
#define NTIMERS 25

typedef struct tevent_t tevent_t;
struct tevent_t
{
    tevent_t* next;
    struct timeval tv;
    void( *func )( void* );
    void* arg;
    Puint id;
};

static tevent_t* active = NULL;    //active timers
static tevent_t* free_list = NULL;    //inactive timers

//=============================================================================
//  Name : tselect()
//  Desc :
//=============================================================================
unsigned int timeout( void ( *handler ) ( void* ),void* arg,int ms );
void untimeout( Puint timerid );
tevent_t* allocate_timer( void );

int tselect( int maxp1,fd_set* re,fd_set* we,fd_set* ee )
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
        rmask = *re;
    if ( we )
        wmask = *we;
    if ( ee )
        emask = *ee;

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
        else if ( re == NULL && we == NULL && ee == NULL )
            return 0;
        else
            tvp = NULL;
        n = select( maxp1, re, we, ee, tvp );
        if ( n < 0 )
            return -1;
        if ( n > 0 )
            return n;
        if ( re )
            *re = rmask;
        if ( we )
            *we = wmask;
        if ( ee )
            *ee = emask;
    }
}

//=============================================================================
//  Name : allocate_timer()
//  Desc :
//=============================================================================
static tevent_t* allocate_timer( void )
{
    tevent_t* tp;

    if ( free_list == NULL ) // need new block of timers?
    {
        free_list = new tevent_t[NTIMERS];
        if ( free_list == NULL )
            perror( "couldn't allocate timers\n" );
        for ( tp = free_list; tp < free_list + NTIMERS - 1; tp++ )
            tp->next = tp + 1;
        tp->next = NULL;
    }
    tp = free_list;        //allocate first free
    free_list = tp->next;  //and pop it off list
    return tp;
}

//=============================================================================
//  Name : timeout()
//  Desc :
//=============================================================================
Puint timeout( void ( *func ) ( void* ),void* arg,int ms )
{
    tevent_t* tp;
    tevent_t* tcur;
    tevent_t** tprev;
    static Puint id = 1;  //timer ID

    tp = allocate_timer();
    tp-> func = func;

    if ( gettimeofday( &tp->tv, NULL ) < 0 )
        printf( "timeout: gettimeofday failure" );
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
        ;}

    *tprev = tp;
    tp->next = tcur;
    tp->id = id++;
    return tp->id;
}

//=============================================================================
//  Name : untimeout()
//  Desc :
//=============================================================================
void untimeout( Puint id )
{
    tevent_t** tprev;
    tevent_t* tcur;

    for ( tprev = &active, tcur = active;
          tcur && id != tcur->id;
          tprev = &tcur->next, tcur = tcur->next )
    {
        ;}
    if ( tcur == NULL )
    {
        printf( "untimeout called for non-existent timer (%d)\n", id );
        return;
    }
}

