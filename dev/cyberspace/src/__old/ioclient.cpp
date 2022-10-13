//Client.cpp network client function class
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
//                                                         Apingu 
//                                                               Sat Mar 16 2002

#if defined(_WIN32) || defined(_WINDOWS)
#include "IOCPDef.h"
#endif
#include <Debug.h>

#include "IOClient.h"


namespace IO
{
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    TCPClient::TCPClient()
	:m_hListen(NULL)
    {
		m_bRecving   = true;
		m_bConnected = false;
		m_bBlock     = false;
		//m_Style = STYLE_CLIENT;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    TCPClient::~TCPClient()
    {
		m_bRecving=false;
		Close();
        //shutdown( m_sockfd, 1 );// my socket will not send any data, but still can receive data
        //m_OPState = SOCKETOP_UNCREATE;
    }

    //=============================================================================
    // Name : tcp_client()
    // Desc : Create one client sock link with peer host and port( so input server and)
    //=============================================================================
    bool TCPClient::Connect( char* hname, unsigned short port )
    {
		TS_Lock lock( &m_TcpClinetLock );

#ifdef _WIN32IOCOMPLETIONPORT
		OV* rov=(OV*)m_Recvbuf;
		SAFE_DELETE( rov );
		m_Recvbuf=NULL;
		OV* sov=(OV*)m_Sendbuf;
		SAFE_DELETE( sov );
		m_Sendbuf=NULL;
#endif
		
        if( Create() == 0 )
		{
			DebugMessage( "connect create socket failure!\n", "Connect failure" );
			//SetOPState( OP_CREATE_FAILURE );
			return false;
		}

		//SetOPState( OP_CONNECTING );
		
        struct sockaddr_in peer;

        char buf[255];

        set_address( hname, itoa( port, buf, 10 ), &peer, "tcp" );

        if( connect( m_sockfd, ( struct sockaddr * ) &peer, sizeof( peer ) ) )
        {
			char buff[256];
			sprintf( buff, "connect servrer failure! error no=%d\n", GetErrorNo() );
			DebugMessage( buff, "Connect failure" );
			SetErrorCode( 0, 0, "connect failed" );
			TCPSocket::Close();
			//SetOPState( OP_CONNECT_FAILURE );
            return false;
        }
		else
		{
		    BeginRecving();
		}
		m_bConnected=true;
		OnConnect();
		//SetOPState( OP_CONNECTED );
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	void TCPClient::Shutdown( int how )
	{
		TS_Lock lock( &m_TcpClinetLock );
		TCPSocket::Shutdown( how );
		m_bConnected = false;
		//EndAllProcess();
		//sleep( 3000 );
	}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	void TCPClient::Close( void )
	{
		TS_Lock lock( &m_TcpClinetLock );
		//sleep( 5000 );

		struct linger li = { 1, 5000 };
        //setsockopt( m_sockfd, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li) );
		SetOpt( SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li) );
		
		EndRecving();
		TCPSocket::Close();
		//TCPSocket::Shutdown(1);
		//到所有的thread 都被關閉
		while( GetThreadSize()!=0 )
		{
			sleep( 500 );
		}
		m_bConnected = false;
		
		//EndAllProcess();
	}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	bool TCPClient::IsConnected( void )
	{
		return m_bConnected;
	}


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	void TCPClient::SetBlock( bool f )//中斷接收callback
	{
		m_bBlock = f;
	}


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	void TCPClient::BeginRecving( void )
	{
		m_bRecving = true;
		m_hListen = BeginProcess( Receiving, this) ;
	}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	void TCPClient::EndRecving( void )
	{
		m_bRecving = false;
	}


	//void TCPClient::OpenRecving( bool b )


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void __cdecl TCPClient::Receiving(void* data)
    {
        TCPClient* client = ( TCPClient* ) data;
		socket_t fd = client->m_sockfd;

		int ret = 0;
		int nread = 0;
        fd_set readfds;//, testfds;
		

        //Now wait for clients and requests.
        //Since we have passed a null pointer as the timeout parameter, no timeout will occur.
        //The program will exit and report an error if select returns a value of less than 1.
		/*
		int ii=0;
		while ( 1 )
		{
			printf( "on process........%d\n", ii );
			Sleep(100);
			ii++;
		}
		*/


	    timeval waittime;
		waittime.tv_sec  = 0;
		waittime.tv_usec = 2000;

        while ( client->m_bRecving )
        {
			//printf( "OK\n" );

			FD_ZERO( &readfds );
            FD_SET( fd, &readfds );

            //testfds = readfds;
			//if( fd )
				//break;
            if( ( ret = select( 0, &readfds, (fd_set *)0,
				                //(fd_set *)0, (struct timeval *)0) )== SOCKET_ERROR )
                                (fd_set *)0, (struct timeval *)&waittime) )== SOCKET_ERROR )
			{
				client->PrintErrorCode( client->GetErrorNo() );
				break;
				//error
			}

			// timeout
			if( ret==0 )
				continue;

			/*
			client->m_Lock.Wait();
			if( fd==0 )
			{
				break;
			}

			client->m_Lock.Lock();
			*/
		    //printf( "Socket 0: %d\n", fd );
			//printf( "Socket 1: %d\n", fd );

            if ( FD_ISSET( fd, &readfds ) )
            {
				//Sleep( 500 );
				//printf( "Socket 2: %d\n", fd );
				//Sleep( 500 );
//#ifdef _LINUX
                int recvret = ioctl( fd, FIONREAD, ( u_long * )&nread );
//#endif
//#ifdef _WIN32
                //int recvret = ioctlsocket( fd, FIONREAD, ( u_long * ) &nread );
//#endif
				//maybe socket had be close
				if( recvret == SOCKET_ERROR )
				{
					//client->PrintErrorCode( client->GetErrorNo() );
					FD_CLR( fd, &readfds );
					client->Socket::Close();
					break;
				}
				//Sleep( 500 );
				//printf( "Socket 3: %d\n", fd );
				//Sleep( 500 );
				//printf( "Socket 4: %d\n", fd );
				//Sleep( 500 );
                if( ( nread > 0 ) && ( !client->m_bBlock ) )
                {
//#ifdef _DEBUG
					
					//OutputDebugString( "Client receive message..\n");
//#endif
				    client->OnReceive(client);
					//client->m_Lock.UnLock();
					continue;
                }
				else
				{
					//client->m_Lock.UnLock();
					if ( nread == 0 )
					{
#ifdef _DEBUG
				        printf( "server knockout..\n");
#endif
						//server 主動斷線先CLOSE
						client->Socket::Close();
						client->OnKnockout();
						FD_CLR( fd, &readfds );
						break;
					}
					else if( nread == SOCKET_ERROR  )
					{
#ifdef _DEBUG
					    printf( "clinet disconnection..\n");
#endif

						client->PrintErrorCode( client->GetErrorNo() );
						FD_CLR( fd, &readfds );
						client->Socket::Close();
						client->OnDisconnection();
						//client->Socket::Close();
						break;
					}
					else// socket already closed. (Shutdown called)
					{
						printf( "Socket EXIT: %d\n", fd);
						//client->OnKnockOut();
						//client->Close();
						//FD_CLR( fd, &readfds );
						break;
					}
				}
            }
			//client->m_Lock.UnLock();
        }
		
		client->m_bRecving=false;
		
		/*
        bool bRecv;
		int ret=0;

        while( 1 )
        {
			
			
#ifdef _LINUX
            //ioctl( fd, FIONREAD, &nread );
#endif
#ifdef _WIN32
			//ioctlsocket( fd, FIONBIO, ( u_long * ) &nread );
            //ioctlsocket( fd, FIONREAD, ( u_long * ) &nread );
#endif
			ret = client->Recv( (char*)&bRecv, 1, MSG_PEEK );
			sleep(1000);

            if ( ret > 0 )
            {
                client->OnReceive(client);
			}
			else if ( ret == 0 )
            {
                client->OnKnockOut();
				client->Shutdown();
				break;
            }
			else if ( ret == -1 )
			{
				//server crash
                client->OnKnockOut();
				client->Shutdown();
				break;
			}
			else		// socket already closed. (Shutdown called)
				break;
			
//			printf("Thread %d running...\n",client->m_HT);
        }
		*/

		//printf("Thread %d exit\n",client->m_HT);
		//client->EndProcess(client->m_hListen);
#ifdef _WIN32
		//_endthread();
#endif
#ifdef _DEBUG
		client->Socket::Close();
		printf( "BYE BYE\n" );
#endif
    }








	

	///////////////////////////////////////////////////////////////////////////////////
	//

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	UDPClient::UDPClient()
	{

	}


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	UDPClient::~UDPClient()
	{

	}


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	void UDPClient::Shutdown( void )
	{

	}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void __cdecl UDPClient::Receiving(void *)
	{

	}

};
