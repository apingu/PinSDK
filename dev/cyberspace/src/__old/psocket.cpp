#include "PSOCKET.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//        Socket Class
//
//
//                                  Apingu
//
////////////////////////////////////////////////////////////////////////////////////////////////


PSOCKET::PSOCKET()
{
    sockfd = -1;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr( "255.255.255.255" );
    address.sin_port = 0;
    Max_Data_Size = 1024;
}

////////////////////////////////////////////////////////////////////////////////////////////////

PSOCKET::PSOCKET( int Data_Size )
{
    sockfd = -1;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr( "255.255.255.255" );
    address.sin_port = 0;
    Max_Data_Size = Data_Size;
}

////////////////////////////////////////////////////////////////////////////////////////////////

PSOCKET::~PSOCKET()
{
    close( sockfd );
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// be a server class
//
void PSOCKET::ServerInit( short SocketType )
{
#ifdef _WIN32
    sockfd = Create_WIN_Socket( SocketType );
#else
    sockfd = Create_UNIX_Socket( SocketType );
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// be a client class
//
void PSOCKET::ClientInit( unsigned short PortNumber,
                          char* IP_Addr,
                          short SocketType )
{
#ifdef _WIN32
    sockfd = this->Create_WIN_Socket( SocketType );
#else
    sockfd = Create_UNIX_Socket( SocketType );
#endif

    ConnectTo( PortNumber, IP_Addr );
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// get IP for Host_Name
//

char* PSOCKET::GetHostAddr( char* szHost )
{
    LPHOSTENT lpstHost;
    unsigned long lAddr = INADDR_ANY;

    /*check that we have a string*/
    if ( *szHost )
    {
        /* check for a dotted_IP address strubg */
        lAddr = inet_addr( szHost );

        /* if not an address, then try to resolve it as a host name */
        if ( ( lAddr == INADDR_NONE ) &&
             ( strcmp( szHost, "255.255.255.255" ) ) )
        {
            lpstHost = gethostbyname( szHost );
            if ( lpstHost )
            {
                //lAddr = *((unsigned long FAR *)(lpstHost->h_addr));
                return inet_ntoa( *( struct in_addr * ) *lpstHost->h_addr_list );

                //sprintf(msg,"%s", inet_ntoa(*(struct in_addr *)*lpstHost->h_addr_list));
                //MessageBox (NULL, msg , "IP_ADDR", 0) ;
            }
            else
            {
                lAddr = INADDR_ANY;
                return ( char * ) lAddr;
            }
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// get IP for Host_Name
//

char* PSOCKET::GetHostName( const void* addr, int type )
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// server process get data from client
//

BYTE* PSOCKET::DataProcess( BYTE* Data, int len )
{
    //MessageBox(NULL, (char *)Data, "test !!!",MB_OK);
    if ( !strcmp( ( const char * ) Data, "bye" ) )
    {
        SocketClose( &sockfd );
        exit( 0 );
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Server recv data and push into queue
//

bool PSOCKET::RecvData( void )
{
    char* GetBuf;              /*收取封包的空間*/
    char PSize[4];                          /*封包的前4個BYTES紀錄封包的長度*/
    /*取前4 BYTES得此封包的長度*/
    recv( sockfd, PSize, sizeof( PSize ), MSG_PEEK );
    int getlen = atoi( PSize ) + 4;

    /*配置封包長度的記憶體空間*/
    GetBuf = new char[getlen];
    recv( sockfd, GetBuf, getlen, MSG_PEEK );

    Sleep( 100L );

    /*配置去除前4 BYTES的實際資料*/
    char* GetData;

    GetData = new char[atoi( PSize )];
    strcpy( GetData, GetBuf + 4 );
    delete [] GetBuf;

    //PDATA data(GetData);

    while ( DataQueue.size() > Max_Data_Size )
    {
        sprintf( StatuMsg, "Queue Over Max size ~_~b !!!" );
        DataQueue.pop();
    }

    DataQueue.push( ( BYTE * ) GetData );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Server recv data and pop from queue
//

bool PSOCKET::SendData( char* Data, int len )
{
    char* SendData = new char[len + 4];

    sprintf( SendData, "%d%s", len, Data );
    send( sockfd, SendData, len + 4, MSG_DONTROUTE );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// close carries in socket
//

bool PSOCKET::SocketClose( NSocket* S )
{
#ifdef _WIN32
    if ( *S != INVALID_SOCKET )
    {
        closesocket( *S );
        return true;
    }
#else
    if ( *S >= 0 )
    {
        closesocket( *S );
        return true;
    }
#endif

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Get server port by server name
//

unsigned short PSOCKET::GetPort( char* szService, char* proto )
{
    unsigned short  nPort = 0;    /* port 0 is invalid */
    struct servent* lpServent;
    char C;

    C = *szService;
    if ( ( C >= '1' ) && ( C <= '9' ) )
    {
        /* convert ASCII to integer , and put in network order */
        nPort = htons( ( unsigned short ) atoi( szService ) );
    }
    else
    {
        /* resolve service name to a port number */
        lpServent = getservbyname( szService, proto );
        if ( !lpServent )
        {
            sprintf( StatuMsg, "Cant't get port ~_~b !!!" );
            //perror(WSAGetLastError(), "getservbyname()");
        }
        else
        {
            nPort = lpServent->s_port;
        }
    }
    return nPort;
}/* end GetPort */


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Client Class
//



////////////////////////////////////////////////////////////////////////////////////////////////
//
// Client connect
//

bool PSOCKET::ConnectTo( unsigned short PortNumber, char* IP_Addr )
{
    int len;
    int result;

    /*  Name the socket, as agreed with the server.  */
    address.sin_family = AF_INET           ;
    address.sin_addr.s_addr = inet_addr( IP_Addr );
    address.sin_port = PortNumber        ;
    len = sizeof( address )   ;

    /*  Now connect our socket to the server's socket.  */
    result = connect( sockfd, ( struct sockaddr * ) &address, len );

    if ( result == -1 )
    {
        sprintf( StatuMsg, " Connect failed ~_~ b !!!! " );
        exit( 1 );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// record accept client IP
//
void PSOCKET::ConnectHistory( char* IP )
{
    FILE* ConnectClient;

    if ( ( ConnectClient = fopen( ".\\history", "ab" ) ) == NULL )//開檔如果-1則失敗，傳回-1
    {
        sprintf( StatuMsg, "write file %s failure !!!", "history" );
        MessageBox( NULL, StatuMsg, "Write Failed", MB_OK );
        return ;
    }

    fwrite( IP, strlen( IP ), 1, ConnectClient );


    char* DateTime;
#ifdef _WIN32
    time_t ltime;
    time( &ltime );
    DateTime = ctime( &ltime );
#else
    struct timeval tv;
    struct timezone tz;
    time_t now;

    gettimeofday( &tv, &tz );
    now = tv.tv_sec;
    DateTime = ctime( &now );
#endif
    fwrite( "    ", strlen( "    " ), 1, ConnectClient );
    fwrite( DateTime, strlen( DateTime ), 1, ConnectClient );
    fwrite( "\n", strlen( "\n" ), 1, ConnectClient );

    fclose( ConnectClient );
}





////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//    For Win32 System
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////

NSocket PSOCKET::Create_WIN_Socket( short SocketType )
{
#ifdef _WIN32

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 2 );
    /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/ 
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL. */

        sprintf( StatuMsg, "Start up failed ~_~b !!!" );
        //MessageBox(NULL,"Start up failed"," Socket Failed", MB_OK);
        return 0;
    }
    NSocket pSocket = socket( AF_INET, SocketType, 0 );
    if ( pSocket == INVALID_SOCKET )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //MessageBox(NULL,"Create Socket failed"," Create Socket Failed", MB_OK);
        return -1;
    }
    return pSocket;
#endif

    return -1;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                         |
//  Muilt thread Server    |
//                         |
//==========================
//    *must #include <process.h>
//    
//    Project -> Setting -> C/C++ ->GateGory ->Gode Gener... -> Use RunTime Lib , must change to 
//                         @Multithreaded@  
//
//    ?????Project -> Setting -> Link     LIBCMT.LIB MSVCRT.LIB
//                                          must have -> Ws2_32.lib
//
//                              Apingu
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////



int PSOCKET::WIN_Listen( unsigned short PortNumber )
{
#ifdef _WIN32

    SOCKET client_sockfd;   
    int server_len, client_len;
    struct sockaddr_in client_address;
    struct sockaddr_in server_address;

    /*  Name the socket.  */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl( INADDR_ANY );
    server_address.sin_port = PortNumber;
    server_len = sizeof( server_address );
    bind( sockfd, ( struct sockaddr * ) &server_address, server_len );

    /*  Create a connection queue and wait for clients.  */
    listen( sockfd, 5 );

    while ( 1 )
    {
        //printf("server waiting\n");

        /*  Accept a connection.  */
        if ( Close_Listen )
            break;

        client_len = sizeof( client_address );
        client_sockfd = accept( sockfd,
                                ( struct sockaddr * ) &client_address,
                                &client_len );

        //將連結的Client IP位置紀錄
        ConnectHistory( inet_ntoa( client_address.sin_addr ) );


        /*  We can now read/write to client on client_sockfd.  */
        /*  Create a thread. */
        if ( _beginthread( PacketProcess, 0, ( void * ) ( &client_sockfd ) ) <
             0 )
        {
            close( client_sockfd );
        }
        /* Wait one second between loops. */
        Sleep( 500L );
    }

    _endthread();

    return 1;

#endif
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void PSOCKET::PacketProcess( void* Any )
{
    PSOCKET PS;

    unsigned int client_sockfd = *( unsigned int * ) ( Any );

    PS.RecvData( client_sockfd );                         // 儲存收到的資料

    //PS.DataProcess((BYTE *)GetData, atoi(PSize));   // 處理收到的資料

    char RecvOK = 'O';
    send( client_sockfd, &RecvOK, 1, MSG_DONTROUTE );
}

//#endif













#ifndef _WIN32

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  
//    For Unix systme
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////

NSocket PSOCKET::Create_UNIX_Socket( short SocketType )
{
    int pSocket = socket( AF_INET, SocketType, 0 );
    if ( pSocket < 0 )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //cout << "Create Socket failed ~_~b !!! \n";
        return -1;
    }
    return pSocket;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// unix system wait for query from client 
//

int PSOCKET::UNIX_Listen( unsigned short PortNumber )
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int result;
    fd_set readfds, testfds;

    int ReturnValue = 1;             /*Server 端接收回傳的訊息*/  

    /*  Create and name a socket for the server.  */

    server_sockfd = Socket;

    /*  Name the socket.  */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl( INADDR_ANY );
    server_address.sin_port = Server.address.sin_port;
    server_len = sizeof( server_address );

    bind( server_sockfd, ( struct sockaddr * ) &server_address, server_len );

    /*  Create a connection queue and initialize readfds to handle input from server_sockfd.  */

    listen( server_sockfd, 5 );

    FD_ZERO( &readfds );
    FD_SET( server_sockfd, &readfds );

    /*  Now wait for clients and requests.
      Since we have passed a null pointer as the timeout parameter, no timeout will occur.
      The program will exit and report an error if select returns a value of less than 1.  */

    while ( 1 )
    {
        if ( Close_Listen )
        {
            break;
        }

        char* ByteRecv = 0;
        int fd;
        unsigned long nread;

        testfds = readfds;

        printf( "server waiting\n" );
        result = select( FD_SETSIZE,
                         &testfds,
                         ( fd_set * ) 0,
                         ( fd_set * ) 0,
                         ( struct timeval * ) 0 );
        if ( result < 1 )
        {
            sprintf( Message, "Create Socket failed ~_~b !!!" );
            //cout << "Create Socket failed ~_~b !!! \n";
            return -1;
        }

        /*  Once we know we've got activity,
          we find which descriptor it's on by checking each in turn using FD_ISSET.  */

        for ( fd = 0; fd < FD_SETSIZE; fd++ )
        {
            if ( FD_ISSET( fd, &testfds ) )
            {
                /*  If the activity is on server_sockfd, it must be a request for a new connection
                    and we add the associated client_sockfd to the descriptor set.  */

                if ( fd == server_sockfd )
                {
                    client_len = sizeof( client_address );
                    client_sockfd = accept( server_sockfd,
                                            ( struct sockaddr * ) &client_address,
                                            &client_len );

                    //將連結的Client IP位置紀錄
                    ConnectHistory( inet_ntoa( client_address.sin_addr ) );


                    FD_SET( client_sockfd, &readfds );
                    sprintf( Message, "adding client on fd %d", client_sockfd );
                    //cout << "adding client on fd %d\n" << client_sockfd;
                }

                /*  If it isn't the server, it must be client activity.
                    If close is received, the client has gone away and we remove it from the descriptor set.
                    Otherwise, we 'serve' the client as in the previous examples.  */
                else
                {
                    ioctlsocket( fd, FIONREAD, &nread );

                    if ( nread == 0 )
                    {
                        close( fd );
                        FD_CLR( fd, &readfds );
                        sprintf( Message, "removing client on fd %d", fd );
                        //cout << "removing client on fd %d\n" << fd;
                    }
                    else
                    {
                        DataRecv( fd );                               // 儲存收到的資料
                        //DataProcess((BYTE *)GetData, atoi(PSize));  // 處理收到的資料

                        char RecvOK = 'O';
                        send( fd, &RecvOK, 1, MSG_DONTROUTE );
                        close( fd );
                        sleep( 5 );
                    }
                }
            }
        }
    }

    return 0;
}

#endif