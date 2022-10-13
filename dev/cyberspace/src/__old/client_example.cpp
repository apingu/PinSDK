#include "PSocket.h"

void main( void )
{
    PSOCKET Client;
    char data[50];
    sprintf( data, "Test instruct ........" );

    Client.DoClient( 9274, "127.0.0.1" );
    Client.SendData( data, 50 );
    //Client.RecvData();
}