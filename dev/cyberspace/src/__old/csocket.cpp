#include "Csocket.h"


/* socket class for server and client 
 * 
 *
 *                              second edition
 *
 *
 *   use for all stand server
 *
 *       
 *                                                  2001/9/6             Pin Wang  */


typedef struct tagCommand
{
    BYTE name[_MAX_COMMAND_NAME];
    BYTE argument[_MAX_COMMAND_ARG];
    char path[_MAX_COMMAND_PATH];
}Command;

std::vector<Command> CommandTable;


typedef struct tagUserSock
{
    bool Connected;            //連接埠使用開關  *
    char GetTime[255];         //上線時間  *
    char UserIP[255];          //UserIP  *
    char UserName[255];        //帳號  *
    int USockID;              //Server分給User的控制項編號  *
    BYTE* SendReady;           //傳送的資料  *
}UserSock;

std::vector<UserSock> UserSockTable;


//////////////////////////////////////////////////////////////////////////////////////////////
//
//  socket class
//
Csocket::Csocket()
{
}


Csocket::~Csocket()
{
}


UINT Csocket::win_create_socket( int domain, int streamtype, int protocol )
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
    SOCKET Socket = socket( domain, streamtype, protocol );
    if ( Socket == INVALID_SOCKET )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //MessageBox(NULL,"Create Socket failed"," Create Socket Failed", MB_OK);
        return -1;
    }
    return Socket;
#endif

    return -1;
}

int Csocket::unix_create_socket( int domain, int streamtype, int protocol )
{
#ifndef _WIN32

    int Socket = socket( domain, streamtype, protocol );
    if ( Socket < 0 )
    {
        sprintf( StatuMsg, "Create Socket failed ~_~b !!!" );
        //cout << "Create Socket failed ~_~b !!! \n";
        return -1;
    }
    return Socket;
#endif
    return 0;
}




//////////////////////////////////////////////////////////////////////////////////////////////
//
//  server class
//
Cserver::Cserver()
{
    ClearSockTable();
}


Cserver::~Cserver()
{
    for ( int i = 0; i < UserSockTable.size(); i++ )
        close( UserSockTable[i].USockID );
    ClearSockTable();
}

#ifndef _WIN32

int Csocket::unix_create_socket( int domain, int streamtype )
{
    return socket( AF_INET, streamtype, 0 );
}


int Cserver::unix_Idle( int portnumber,
                        int domain,
                        int streamtype,
                        int protocol ); //server wait for bind in win32 system
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int result;
    fd_set readfds, testfds;

    /* clear socket table */

    UserSockTable.clear();

    /*  Create and name a socket for the server.  */


    server_sockfd = socket( AF_INET, streamtype, protocol );

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl( INADDR_ANY );
    server_address.sin_port = htons( portnumber );
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
        char* getdata;
        int fd;
        int nread;

        testfds = readfds;

        printf( "server waiting\n" );
        result = select( FD_SETSIZE,
                         &testfds,
                         ( fd_set * ) 0,
                         ( fd_set * ) 0,
                         ( struct timeval * ) 0 );

        if ( result < 1 )
        {
            perror( "server5" );
            exit( 1 );
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
                                            ( socklen_t * ) &client_len );
                    FD_SET( client_sockfd, &readfds );
                    printf( "adding client on fd %d\n", client_sockfd );

                    time_t ltime;
                    time( &ltime );
                    UserSock USock;
                    USock.Connected = true;
                    strcpy( USock.GetTime, ctime( &ltime ) );
                    USock.Socket = client_sockfd;
                    UserSockTable.push_back( USock );
                }

                /*  If it isn't the server, it must be client activity.
                    If close is received, the client has gone away and we remove it from the descriptor set.
                    Otherwise, we 'serve' the client as in the previous examples.  */

                else
                {
                    ioctl( fd, FIONREAD, &nread );

                    if ( nread == 0 )
                    {
                        close( fd );
                        FD_CLR( fd, &readfds );
                        printf( "removing client on fd %d\n", fd );
                    }
                    else
                    {
                        //read(fd, getdata, 1);
                        recv( fd, getdata, 4, MSG_PEEK );
                        char* retdata = ( char ) Process( getdata );
                        sleep( 5 );
                        printf( "serving client on fd %d\n", fd );
                        //ch++;
                        //write(fd, retdata, 1);
                        send( fd, redata, 1, MSG_DONTROUTE );
                    }
                }
            }
        }
    }
}

#endif


int Cserver::win_Idle( int portnumber,
                       int domain,
                       int streamtype,
                       int protocol )
{
    return -1;
}

BYTE* Cserver::Process( BYTE* datastream )
{
    return 0;
}

int Cserver::ClearSockTable()
{
    for ( int i = 0; i < UserSockTable.size(); i++ )
    {
        if ( UserSockTable[i].SendReady != 0 )
        {
            delete UserSockTable[i].SendReady;
            UserSockTable[i].SendReady = 0;
        }
    }

    UserSockTable.clear();

    return 1;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
//    client class
//

Cclient::Cclient()
{
    Server_socket = 0;
    sockfd = 0;
}


Cclient::~Cclient()
{
    close( Server_socket );
    close( sockfd );
}



UINT Cclient::win_connect_to( char* IP_address, int portnumber )
{
    return 0;
}


int Cclient::unix_connect_to( char* IP_address, int portnumber )
{
    //int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char ch = 'A';

    /*  Create a socket for the client.  */

    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    /*  Name the socket, as agreed with the server.  */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr( IP_address );
    address.sin_port = portnumber;
    len = sizeof( address );

    /*  Now connect our socket to the server's socket.  */

    result = connect( sockfd, ( struct sockaddr * ) &address, len );

    if ( result == -1 )
    {
        perror( "oops: client2" );
        //exit(1);
        return -1;
    }

    return 1;

    /*  We can now read/write via sockfd.  */

    //write(sockfd, &ch, 1);
    //read(sockfd, &ch, 1);
    //printf("char from server = %c\n", ch);
    //close(sockfd);
    //exit(0);
}
