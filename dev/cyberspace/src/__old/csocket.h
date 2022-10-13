#ifndef _Csocket_H_
#define _Csocket_H_

#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <time.h>


using namespace std;

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <memory.h>
#include <io.h>
#include <process.h>    /* _beginthread, _endthread */

typedef unsigned int NSocket;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

typedef BYTE  unsigned char
typedef DWORD unsigned long

typedef int NSocket;
typedef hostent* LPHOSTENT;

#endif
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <time.h>


using namespace std;

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <memory.h>
#include <io.h>
#include <process.h>    /* _beginthread, _endthread */

typedef unsigned int NSocket;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

typedef BYTE  unsigned char
typedef DWORD unsigned long

typedef int NSocket;
typedef hostent* LPHOSTENT;

#endif
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <time.h>


using namespace std;

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <memory.h>
#include <io.h>
#include <process.h>    /* _beginthread, _endthread */

typedef unsigned int NSocket;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

typedef BYTE  unsigned char
typedef DWORD unsigned long

typedef int NSocket;
typedef hostent* LPHOSTENT;

#endif


#define _MAX_COMMAND_NAME 256
#define _MAX_COMMAND_ARG  1024
#define _MAX_COMMAND_PATH 256



class Csocket
{
protected:


public:

    char StatuMsg[255];

    Csocket( void ); 
    ~Csocket( void );

    UINT win_create_socket( int domain = AF_INET,
                            int streamtype = SOCK_STREAM,
                            int protocol = 0 );                           //create socket in win32 system
    int unix_create_socket( int domain = AF_INET,
                            int streamtype = SOCK_STREAM,
                            int protocol = 0 );                           //create socket in unix  system


    int rectdata( void );
    int senddata( void );
};


class Cserver : public Csocket
{
public:

    Cserver();
    ~Cserver();

    int win_Idle( int portnumber,
                  int domain = AF_INET,
                  int streamtype = SOCK_STREAM,
                  int protocol = 0 ); //server wait for bind in win32 system

    int unix_Idle( int portnumber,
                   int domain = AF_INET,
                   int streamtype = SOCK_STREAM,
                   int protocol = 0 ); //server wait for bind in unix  systme

    int ClearSockTable( void );
    int LoadTable( char* filepath );
    BYTE* Process( BYTE* datastream );
};


class Cclient : public Csocket
{
#ifdef _WIN32
    UINT Server_socket;
    UINT sockfd;
#else
    int        Server_socket
    int sockfd;
#endif

public:

    Cclient();
    ~Cclient();

    UINT win_connect_to( char* IP_adderess, int portnumber );
    int unix_connect_to( char* IP_adderess, int portnumber );
};

#endif