#include <windows.h>
#include "D:\My Documents\Programs\C\CClass\NET\PSOCKET\Csocket.h"

Cserver Socket;

int  WINAPI WinMain( HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow )
{
    //Socket.ServerInit();
    //Socket.Listen(9274);

    Socket.win_create_socket();
    Socket.win_Idle( 9000 );
    return 1;
}

