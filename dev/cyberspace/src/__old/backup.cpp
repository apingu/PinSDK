#include "PSOCKET.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
// send data
//

bool PSOCKET::SendData( char IP_Addr[], int port, BYTE* SendBuf, int len )
{
    struct sockaddr_in address;
    int result;

#ifdef _WIN32

    SOCKET sockfd;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;


    wVersionRequested = MAKEWORD( 2, 2 );

    /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        sprintf( StatuMsg, "could not find a usable WinSock DLL ~_~b !!!" );
        return false;
    }
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd == INVALID_SOCKET )
    {
        sprintf( StatuMsg, "Create connect socket failed  ~_~b !!!" );
        //MessageBox(NULL, "Create Socket failed !!!", "Socket failed !!", MB_OK);
        return false;
    }

#else
    int sockfd;
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd < 0 )
    {
        sprintf( StatuMsg, "Create connect socket failed ~_~b !!!\n" );
        //cout << "Create Socket failed ~_~b !!!\n";
        return false;
    }

#endif

    /*  Name the socket, as agreed with the server.  */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr( IP_Addr );
    address.sin_port = port;
    len = sizeof( address );

    /*  Now connect our socket to the server's socket.  */
    result = connect( sockfd, ( struct sockaddr * ) &address, len );

    if ( result == -1 )
    {
        sprintf( StatuMsg, "Socket connect failed ~_~b !!!\n" );
        return false;
    }

    /*  We can now read/write via sockfd.  */


    char* SendData;
    int iPacketlen = len + 4;

    SendData = new char[iPacketlen];

    sprintf( SendData, "%d%s", iPacketlen, SendBuf );
    send( sockfd, SendData, iPacketlen, MSG_DONTROUTE );

    char RecvResult = 'F';
    recv( sockfd, &RecvResult, 1, MSG_PEEK );

    close( sockfd );

    if ( RecvResult == 'F' )
    {
        sprintf( StatuMsg, "Send Data failed ~_~b !!!\n" );
        return false;
    }
    return true;
}
//=======================================================================
bool PSOCKET::SendData( DWORD IP_Addr, int port, BYTE* SendBuf, int len )
{
    struct sockaddr_in address;
    int result;

#ifdef _WIN32

    SOCKET sockfd;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;


    wVersionRequested = MAKEWORD( 2, 2 );

    /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        sprintf( StatuMsg, "could not find a usable WinSock DLL ~_~b !!!" );
        return false;
    }
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd == INVALID_SOCKET )
    {
        sprintf( StatuMsg, "Create Socket failed !! ~_~b !!!" );
        //MessageBox(NULL, "Create Socket failed !!!", "Socket failed !!", MB_OK);
        return false;
    }

#else
    int sockfd;
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd < 0 )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!\n" );
        //cout << "Create Socket failed ~_~b !!!\n";
        return false;
    }

#endif

    /*  Name the socket, as agreed with the server.  */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl( IP_Addr );
    address.sin_port = port;
    len = sizeof( address );

    /*  Now connect our socket to the server's socket.  */
    result = connect( sockfd, ( struct sockaddr * ) &address, len );

    if ( result == -1 )
    {
        sprintf( StatuMsg, "Socket connect failed ~_~b !!!\n" );
        return false;
    }

    /*  We can now read/write via sockfd.  */


    char* SendData;
    int iPacketlen = len + 4;

    SendData = new char[iPacketlen];

    sprintf( SendData, "%d%s", iPacketlen, SendBuf );
    send( sockfd, SendData, iPacketlen, MSG_DONTROUTE );

    char RecvResult = 'F';
    recv( sockfd, &RecvResult, 1, MSG_PEEK );

    close( sockfd );

    if ( RecvResult == 'F' )
    {
        sprintf( StatuMsg, "Send Data failed ~_~b !!!\n" );
        return false;
    }
    return true;
}
//=======================================================================
bool PSOCKET::SendData( NSocket Sockfd, BYTE* SendBuf, int len )
{
    char* SendData;
    int iPacketlen = len + 4;

    SendData = new char[iPacketlen];

    sprintf( SendData, "%d%s", iPacketlen, SendBuf );
    send( Sockfd, SendData, iPacketlen, MSG_DONTROUTE );

    char RecvResult = 'F';
    recv( Sockfd, &RecvResult, 1, MSG_PEEK );

    close( Sockfd );

    if ( RecvResult == 'F' )
    {
        sprintf( StatuMsg, "Send data failed ~_~b !!!" );
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// recevies data
//

bool PSOCKET::RecvData( char IP_Addr[], int port, BYTE* RecvBuf, int len )
{
    struct sockaddr_in address;
    int result;

#ifdef _WIN32

    SOCKET sockfd;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;


    wVersionRequested = MAKEWORD( 2, 2 );

    /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        sprintf( StatuMsg, "could not find a usable WinSock DLL ~_~b !!!" );
        return false;
    }
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd == INVALID_SOCKET )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //MessageBox(NULL, "Create Socket failed !!!", "Socket failed !!", MB_OK);
        return false;
    }

#else
    int sockfd;
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd < 0 )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //cout << " Socket create failed ~_~b !!!\n";
        return false;
    }

#endif

    /*  Name the socket, as agreed with the server.  */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr( IP_Addr );
    address.sin_port = port;
    len = sizeof( address );

    /*  Now connect our socket to the server's socket.  */
    result = connect( sockfd, ( struct sockaddr * ) &address, len );

    if ( result == -1 )
    {
        sprintf( StatuMsg, "Socket connect failed ~_~b !!!" );
        return false;
    }

    /*  We can now read/write via sockfd.  */

    char pLen[4];                              /*封包的前4個BYTES紀錄封包的長度*/
    /*取前4 BYTES得此封包的長度*/
    recv( sockfd, pLen, sizeof( pLen ), MSG_PEEK );
    int getlen = atoi( pLen ) + 4;

    char* GetData;   //完整的封包空間 
    GetData = new char[getlen];
    recv( sockfd, GetData, getlen, MSG_PEEK );

#ifdef _WIN32
    Sleep( 100L );
#else
    sleep( 5 );   
#endif

    if ( len < atoi( pLen ) )
    {
        RecvBuf = new BYTE[atoi( pLen )];
    }
    else
    {
        RecvBuf = new BYTE[len];
    }

    strcpy( ( char * ) RecvBuf, GetData + 4 );
    close( sockfd );

    return true;
}
//=======================================================================
bool PSOCKET::RecvData( DWORD IP_Addr, int port, BYTE* RecvBuf, int len )
{
    struct sockaddr_in address;
    int result;

#ifdef _WIN32

    SOCKET sockfd;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;


    wVersionRequested = MAKEWORD( 2, 2 );

    /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        sprintf( StatuMsg, "could not find a usable WinSock DLL ~_~b !!!" );
        return false;
    }
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd == INVALID_SOCKET )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //MessageBox(NULL, "Create Socket failed !!!", "Socket failed !!", MB_OK);
        return false;
    }

#else
    int sockfd;
    /*  Create a socket for the client.  */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sockfd < 0 )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //cout << " Socket create failed ~_~b !!!\n";
        return false;
    }

#endif

    /*  Name the socket, as agreed with the server.  */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl( IP_Addr );
    address.sin_port = port;
    len = sizeof( address );

    /*  Now connect our socket to the server's socket.  */
    result = connect( sockfd, ( struct sockaddr * ) &address, len );

    if ( result == -1 )
    {
        sprintf( StatuMsg, "Socket connect failed ~_~b !!!" );
        return false;
    }

    /*  We can now read/write via sockfd.  */

    char pLen[4];                              /*封包的前4個BYTES紀錄封包的長度*/
    /*取前4 BYTES得此封包的長度*/
    recv( sockfd, pLen, sizeof( pLen ), MSG_PEEK );
    int getlen = atoi( pLen ) + 4;

    char* GetData;   //完整的封包空間 
    GetData = new char[getlen];
    recv( sockfd, GetData, getlen, MSG_PEEK );

#ifdef _WIN32
    Sleep( 100L );
#else
    sleep( 5 );   
#endif

    if ( len < atoi( pLen ) )
    {
        RecvBuf = new BYTE[atoi( pLen )];
    }
    else
    {
        RecvBuf = new BYTE[len];
    }

    strcpy( ( char * ) RecvBuf, GetData + 4 );
    close( sockfd );

    return true;
}
//=======================================================================
bool PSOCKET::RecvData( NSocket Sockfd, BYTE* RecvBuf, int len )
{
    char pLen[4];                              /*封包的前4個BYTES紀錄封包的長度*/
    /*取前4 BYTES得此封包的長度*/
    recv( Sockfd, pLen, sizeof( pLen ), MSG_PEEK );
    int getlen = atoi( pLen ) + 4;

    char* GetData;   //完整的封包空間 
    GetData = new char[getlen];
    recv( Sockfd, GetData, getlen, MSG_PEEK );

#ifdef _WIN32
    Sleep( 100L );
#else
    sleep( 5 );   
#endif

    if ( len < atoi( pLen ) )
    {
        RecvBuf = new BYTE[atoi( pLen )];
    }
    else
    {
        RecvBuf = new BYTE[len];
    }

    strcpy( ( char * ) RecvBuf, GetData + 4 );
    close( Sockfd );

    return true;
}