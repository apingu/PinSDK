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
#include "WGameServer.h"
#include "WGameHeadle.h"

namespace WNE
{
    char ConfigureFilename[_MAX_FNAME] = "config.cfg";
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    GameService::GameService( char* port ) : TCPService( port )
    {
    };


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    GameService::~GameService()
    {
    };


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameService::OnInitiation( void )
    {
        char hostname[64];
        char hostaddr[64];
        printf( "Initiating server.......\n" );
        printf( "Name:%s    Ip:%s    Port:%d\n",
                GetHostName( hostname ),
                GetAddr( hostaddr ),
                GetPort() );

        StartUp( ConfigureFilename );

        printf( "server waiting........\n" );

        OnInitialing();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameService::OnShutdown( void )
    {
        printf( "Bye Bye  !!!!!" );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameService::OnAccept( WNE::IO::TCPSocket* socket )
    {
        printf( "add client on fd %d\n", *socket );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameService::OnKnockOut( WNE::IO::TCPSocket* socket )
    {
        printf( "removing client on fd %d\n", *socket );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int GameService::Recvmsg( WNE::IO::TCPSocket* socket,
                              char* bp,
                              size_t len,
                              unsigned int flag )
    {
        return socket->Recvn( bp, len );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameService::OnReceive( WNE::IO::TCPSocket* socket )
    {
        GameHeadle headle;
        if ( Recvmsg( socket, ( char * ) &headle, sizeof( GameHeadle ) ) ==
             SOCKET_ERROR )
        {
            return;
        }

        printf( "receive data on %d \n", *socket );

        char* buf = NULL;

        if ( headle.len != 0 )
        {
            buf = new char[headle.len];
            if ( socket->Recvn( buf, headle.len ) != SOCKET_ERROR )
            {
                delete [] buf;
                return;
            }
        }

        // service define function
        if ( headle.channel > MAX_MESSAGE_CHANNEL )
        {
            switch ( headle.channel )
            {
              case gcLOGIN:
                {
                    GameHLogin loginmsg;
                    loginmsg.usernamelen = *( DWORD * ) ( buf );
                    loginmsg.username = buf + 4;
                    loginmsg.passwordlen = *( DWORD * )
                                           ( buf + 4 + loginmsg.usernamelen );
                    loginmsg.password = buf + 4 + loginmsg.usernamelen + 4;
                    OnLogout( loginmsg.username, loginmsg.password );
                    break;
                }
            }
        }
        // user define function
        else
        {
            FuncMessageChannel func = m_MessageChannel[headle.channel];
            {
                if ( func != NULL )
                    func( ( const char * ) buf, headle.len );
            }
        }

        if ( buf != NULL )
            delete [] buf;





        /*
        DWORD bRecv=0;

        WSABUF wbuf[2];
        //wbuf.buf = (char*)buf;
        wbuf[0].buf = (char*)(&bRecv);
        wbuf[0].len = 4;
        wbuf[1].buf = buf;
        wbuf[1].len = 20;

        //wbuf[1].buf = buf;
        //wbuf[1].len = 20;
        DWORD dwIoSize =0;
        ULONG ulFlags = 0;
        DWORD dwErr=0;
        OVERLAPPED  m_ovIN;
        memset(&m_ovIN,0,sizeof(OVERLAPPED));

        //int ret = WSARecv( m_sockfd, wbuf, 2, &dwIoSize, 0, NULL, NULL );

        UINT iRel = WSARecv( socket->m_sockfd, 
        wbuf,
        2,
        &dwIoSize, 
        &ulFlags,
        &m_ovIN, 
        NULL);
        */

        //socket->Recvhm( bps, 2 );
        //DWORD* re = (DWORD*)(&bps[0]);



        //Message msg[2];
        //socket->Recvhm( msg, 255 );
        }

        /*
        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        int GameService::Sendmsg( const char* bp, size_t len, u_int channel )
        {
        GameHeadle headle;
        headle.channel = channel;
        headle.len     = len;

        Sendbuf gamemsg[2];
        //gamemsg[0].resize( 4 );
        //memcpy( &( gamemsg[0] )[0], &channel, 4 );
        gamemsg[0].buf = (char*)&headle;
        gamemsg[0].len = sizeof( GameHeadle );
        gamemsg[1].buf = (char*)bp;
        gamemsg[1].len = len;
        return Sendhm( gamemsg, 2 );
        }
        */

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        void GameService::RegisterChannel( u_int id, FuncMessageChannel func )
        {
            if ( id < MAX_MESSAGE_CHANNEL )
                m_MessageChannel[id] = func;
            //m_ChannelVec.insert( id, func );
        }


        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        void GameService::PublishEntity( Entity* entity, u_int channel, u_int zoneid )
            {
                //OnGetZoneId( entity->m_pData );
            }

            //=============================================================================
            //  Name :
            //  Desc :
            //=============================================================================
            bool GameService::StartUp( const char* configfile )
            {
                return true;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void GameService::ShutDown( void )
    {
        return;
    }
};//namespace WNE
