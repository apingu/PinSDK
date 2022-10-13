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
//
//
//
//
#include "WSocket.h"
#include "WGameClient.h"
#include "WGameHeadle.h"


namespace WNE
{
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    GameClient::GameClient( char* ipaddr, char* port ) : TCPClient( ipaddr,
                                                                    port )
    {
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    GameClient::~GameClient()
    {
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameClient::RegisterChannel( u_int id, FuncMessageChannel func )
    {
        if ( id < MAX_MESSAGE_CHANNEL )
            m_MessageChannel[id] = func;
        return;
        //m_ChannelVec.insert( id, func );
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameClient::OnKnockOut()
    {
        printf( "Broken...." );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int GameClient::Sendmsg( const char* bp, size_t len, u_int channel )
    {
        GameHeadle headle;
        headle.channel = channel;
        headle.len = len;

        Sendbuf gamemsg[2];
        gamemsg[0].buf = ( char * ) &headle;
        gamemsg[0].len = sizeof( GameHeadle );
        gamemsg[1].buf = ( char * ) bp;
        gamemsg[1].len = len;
        return TCPSocket::Sendhm( gamemsg, 2 );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameClient::OnReceive()
    {
        GameHeadle headle;

        if ( Recv( ( char * ) &headle, sizeof( GameHeadle ) ) == SOCKET_ERROR )
            return;
        char* buf = new char[headle.len];

        FuncMessageChannel func = m_MessageChannel[headle.channel];
        if ( func != NULL )
            func( ( const char * ) buf, headle.len );

        delete [] buf;
    }
};