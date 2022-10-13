/* ===========================================================================================
 *
 * Net Socket Class
 *
 *
 *
 *
 *
 * =========================================================================================*/

#ifndef _PScoket_H_
#define _PScoket_H_

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


const int Internet = 0;
const int LAN = 1;

typedef struct tagMessage
{
    int len;
    BYTE* DataBuf;
}Message;



class PSOCKET
{
private:

    int Max_Data_Size;     //定義MessageQueue的最大值

    /* WIN32 */
    NSocket Create_WIN_Socket( short SocketType = SOCK_STREAM ) ;                                  //建立WIN  system 的 Socket , arguments 1 arries [SOCK_STREAM] or [SOCK_DGRAM]
    void WIN_broadcast( BYTE* SendBuf, int len )   ;
    int WIN_Listen( unsigned short PortNumber );
    static void PacketProcess( void* Any )                ;

    /* UNIX */
    NSocket Create_UNIX_Socket( short SocketType = SOCK_STREAM ) ;                                  //建立UNIX system 的 Socket , arguments 1 carries [SOCK_STREAM] or [SOCK_DGRAM]
    void UNIX_broadcast( BYTE* SendBuf, int len )   ;
    int UNIX_Listen( unsigned short PortNumber );

    void ConnectHistory( char* IP )                 ;

protected:

    char StatuMsg[50];

    NSocket sockfd      ;
    struct sockaddr_in address     ;

public:


    PSOCKET();
    PSOCKET( int Data_Size );
    ~PSOCKET();

    std::queue<BYTE*> DataQueue    ;     //儲存Message


    int NetType                                                 ;

    bool Close_Listen                                            ;                                                                  //Close Server listen;
    bool ConnectTo( unsigned short PortNumber, char* IP_Addr );

    void ServerInit( short SocketType = SOCK_STREAM )          ;
    void ClientInit( unsigned short PortNumber,
                     char* IP_Addr,
                     short SocketType = SOCK_STREAM )          ;

    char* GetHostAddr( char* szHost )                            ;                 //取得傳入名字主機的IP
    char* GetHostName( const void* addr, int type )              ;
    unsigned short GetPort( char* szService, char* proto )            ;            //arguments 2 carries ["tcp"] or ["udp"]

    bool RecvData( void )                                    ;                                           //接收資料
    bool SendData( char* Data, int len )                     ;                   //傳送資料

    virtual BYTE* DataProcess( BYTE* Data, int len )                     ;             //處理接收到的資料
    bool SocketClose( NSocket* S )                              ;
};


#endif