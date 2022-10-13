//TCPServer.cpp network server function class
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
#else
#include "EPollDef.h"
#endif

#include "IOServer.h"
#include <time.h>
#include <assert.h>
#include <debug.h>



//////////////////////////////////////////////////////////////////////////////////////////////
//
//  server class
//


namespace IO
{


static T_Lock g_eraselock;
    int EraseSocketFromSeq( std::vector<IO::TCPSocket*>* seq, TCPSocket* s )
    {
		TS_Lock lock( &g_eraselock );
		
        for( UINT i=0; i< seq->size(); i++ )
        {
            if ( (*seq)[i] == s )
            {
            	seq->erase( seq->begin()+i );
				return 1;
            }
        }
        return -1;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    TCPServer::TCPServer()
    {
        m_LimitClient =0;
        m_RunningThreads=0;
#if defined (_WIN32IOCOMPLETIONPORT)     /* Windows platforms iocp*/
        m_hIOCP = NULL;
#endif
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    TCPServer::~TCPServer()
    {
        //Shutdown(2);
        Close();
        //client_clear();
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int TCPServer::StartUp( u_int allowplayers, unsigned short port )
    {

        if(m_hIOCP) return 1;  // already started, return

        //m_Style = STYLE_SERVICE;
        m_LimitClient = allowplayers;
        m_ListenPort = port;

        u_int cpusize   =0;
        u_int memorysize=512;
        m_PollingThreads=1;

#if defined (_WIN32IOCOMPLETIONPORT)     /* Windows platforms iocp*/
        
        // create IOCP and initance
        m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0);
        if( m_hIOCP == NULL )
        {
            printf( "CreateIoCompletionPort failed: %d\n", GetLastError());
            return -1;
        }

        SYSTEM_INFO systemInfo;//
        //計算一顆CPU給兩個 worker 
        GetSystemInfo( &systemInfo );
        cpusize = systemInfo.dwNumberOfProcessors;

        MEMORYSTATUS mstat;
        GlobalMemoryStatus( &mstat );

        memorysize = mstat.dwTotalPhys;

        //memorysize = systemInfo.dwAllocationGranularity;
        //nWorkerCnt=1;
        //HANDLE *hWorker= new HANDLE[nWorkerCnt];
#endif

#if defined(_LINUX)
        int i, maxi, listenfd, connfd, sockfd,epfd,nfds; 

        ssize_t n; 

        char line[MAXLINE]; 

        socklen_t clilen; 

        struct epoll_event ev,events[20]; 
		
		epfd=epoll_create(256); 


#endif

        //build listen thread
        m_PollingThreads = ( systemInfo.dwNumberOfProcessors * 2 )+2;
        //nWorkerCnt = 2;

		m_LivingThreads=0;
        for( unsigned int i=0; i<m_PollingThreads; i++ )
        {
            if( !BeginProcess( Polling, ( void * )this ) )
            {
                return -1;
            }
        }


        if( Create( AF_INET ) == NULL )
            return -1;
        
        const int on=1;
        if( SetOpt( SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) )
            SetErrorCode( 1, errno, "set reuseaddr failed!" );

		Set_tcp_buf_size( SO_RCVBUF, 4194304 );

        if( !BeginProcess( Listening, ( void * )this ) )
        {
            return -1;
        }

        sockaddr_in address;
        //struct sockaddr_in local;

        //set_address( hname, sname, &local, "tcp" );
        char buf[50];
        set_address( NULL, itoa( port, buf, 10 ), &address, "tcp" );

        //restart havn not problem in address already in use
        if ( bind( m_sockfd,
                   ( struct sockaddr * ) &address,
                   sizeof( address ) ) )
        {
            SetErrorCode( 1, errno, "bind failed" );
			//SetOPState( OP_BIND_FAILURE );
            return -1;
        }

        if ( listen( m_sockfd, NLISTEN ) )
        {
            SetErrorCode( 1, errno, "listen failed" );
			//SetOPState( OP_LISTEN_FAILURE );
            return -1;
        }

#if defined (_WIN32IOCOMPLETIONPORT)     /* Windows platforms iocp*/
        for( UINT s=0; s<allowplayers; s++ )
        {
			TCPSocket* sk=OnNewSocket();
			if( sk==NULL )
			{
				perror( "create socket failure, socket can't be NULL!" );
				exit(0);
			}

			SocketOV* ov = (SocketOV*)sk->m_Recvbuf;
			//sk->Create();
            if( ov->PostAccept( m_sockfd ) != 1 )
                OnDelSocket(sk);

            //link ov and sockfd

			m_SocketPool.push_back(sk);
			//m_ClientList.push_back( ov->GetSocket() );

            /*
            memset( ov, 0, sizeof(SocketOV) );//must do memset
            ov->s = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
            ov->bAssign = false;
            if ( ! AcceptEx( m_sockfd, ov->s, &(ov->buf[0]),
                             SocketOV::initialReceiveSize, ov->addrlen,
                             ov->addrlen, &(ov->n), ov ) )
            {
                int error = WSAGetLastError();
                if( error != ERROR_IO_PENDING )
                {
                    printf("AcceptEx error. code=%d socket=%d\n", error, ov->s);
                    return -1;
                }
            }
            OVVec.push_back(ov);
            */
        }

        HANDLE h = CreateIoCompletionPort( (HANDLE)m_sockfd,
                                           m_hIOCP,
                                           (DWORD)this,
                                           0 );
        if (h == NULL)
        {
            fprintf(stderr, "CreateIoCompletionPort failed: %d\n", GetLastError());
            return -1;
        }

#endif
        OnInitiation();

	
		sleep(1000);


        return 1;
        //BeginProcess( Listening, this ); 
        //BeginProcess( Select, this );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void TCPServer::Close( void )
    {
        if( m_sockfd==0 )
            return;

        OnShutDown();
        //EndAllProcess();
#if defined (_WIN32IOCOMPLETIONPORT)     /* Windows platforms iocp*/

		PostQueuedCompletionStatus( m_hIOCP, 0, KEY_TERMINATE, 0 );
//        for(int t=0; t < m_ProcessThread.size(); t++ )
//        {
//            
//        }

        // wait for thread terminations
        WaitForMultipleObjects( m_ProcessThread.size(), (HANDLE*)&(m_ProcessThread[0]), TRUE, 15000 );


        for( int o = 0; o < m_SocketPool.size(); o++ )
        {
			TCPSocket* s=m_SocketPool[o];
			KnockOut( s );
			s->Close();
            OnDelSocket( s );
            //delete OVVec[o];
			//SAFE_DELETE( OVVec[o]->sockfd->m_Sendbuf );
        }
        m_SocketPool.clear();
        CloseHandle( m_hIOCP );
        m_hIOCP = NULL;

#endif
		m_ClientList.clear();

		/*

        for( unsigned int i = 0; i < m_ClientList.size(); i++ )
        {
            //解構的時候會close
			KnockOut( m_ClientList[i] );
            OnDelSocket( m_ClientList[i] );
			m_ClientList.erase( m_ClientList.begin() + i );
        }
        m_ClientList.clear();

        //clear overlap buf
#if defined (_WIN32IOCOMPLETIONPORT)     // Windows platforms iocp
        for( int o = 0; o < OVVec.size(); o++ )
        {
			KnockOut( OVVec[o]->sockfd );
            OnDelSocket( OVVec[o]->sockfd );
            delete OVVec[o];
        }
        OVVec.clear();
        CloseHandle( m_hIOCP );
        m_hIOCP = NULL;
#endif
		*/
        TCPSocket::Close();
    }


    //=============================================================================
    // Name : GetLivingThreadSize()
    // Desc : 
    //=============================================================================
	long TCPServer::GetLivingThreadSize( void )
	{
		return m_LivingThreads;
	}

    //=============================================================================
    // Name : GetPort()
    // Desc : get local host name
    //=============================================================================
    unsigned short TCPServer::GetServicePort( void )
    {
		TS_Lock lock(&m_lServer);
        return m_ListenPort;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    unsigned int TCPServer::GetLimitConnectSize( void )
    {
		TS_Lock lock(&m_lServer);
        return m_LimitClient;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    unsigned int TCPServer::GetConnectedSize( void )
    {
		TS_Lock lock(&m_lServer);
        return m_ClientList.size();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    IO::TCPSocket* TCPServer::operator[]( u_int index )
    {
		TS_Lock lock(&m_lServer);
        return m_ClientList[index];
    }

    
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    IO::TCPSocket* TCPServer::FindSocket( socket_t fd )
    {
		TS_Lock lock(&m_lServer);
        for( UINT i=0; i< m_ClientList.size(); i++ )
        {
            if ( m_ClientList[i]->m_sockfd == fd )
                return m_ClientList[i];
        }
        return NULL;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool TCPServer::IsActive() //是否停止
    {
        return TRUE;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool TCPServer::IsAlive( TCPSocket* s )
    {
		if( s->m_sockfd==0 )
			return false;
		return true;
//        for( UINT i=0; i< m_ClientList.size(); i++ )
//        {
//            if ( m_ClientList[i] == s )
//                return i;
//        }
//        return -1;
    }

    //=============================================================================
    //  Name : Accept
    //  Desc :
    //=============================================================================
    bool TCPServer::Accept( TCPSocket& s )
    {
		TS_Lock lock(&m_lServer);
        //s.SetOPState( OP_ACCEPTING );
        BOOL bSuccess = TRUE;

#if defined(_WIN32IOCOMPLETIONPORT)     /* Windows platforms */
        //IO::TCPSocket* client_socket = new IO::TCPSocket;

        // need to update accept context
        setsockopt( s.m_sockfd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                      (char*)&m_sockfd, sizeof(SOCKET) );

        // turn on keep-alive monitoring
        unsigned long flag = 1;
        setsockopt( s.m_sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(flag) );

		OV* ov=(OV*)s.m_Recvbuf;

		int locallen=0, remotelen=0;
		sockaddr_in *plocal = NULL;
		sockaddr_in *premote = NULL;

	    GetAcceptExSockaddrs( &(ov->buf[0]), 0,	ACCEPT_ADDRLEN, ACCEPT_ADDRLEN, 
			                   (sockaddr **)(&plocal), &locallen,
			                   (sockaddr **)(&premote), &remotelen );
		

		s.m_Ip   = premote->sin_addr;
		s.m_Port = premote->sin_port;
		

        // 使用  CreateIoCompletionPort
        HANDLE h = CreateIoCompletionPort( (HANDLE)s.m_sockfd,
                                           m_hIOCP,
                                           (DWORD)&s,
                                           0 );

        if ( h != m_hIOCP )
        {
            return false;
        }

		//reset iocp buf
		( (SocketOV*)(s.m_Recvbuf) )->Reset( IO::OP_READING );
		( (SocketOV*)(s.m_Sendbuf) )->Reset( IO::OP_WRITING );
#else
		int          remotelen = 0;
		sockaddr_in* premote = NULL;
		s.m_sockfd = accept( m_sockfd, (sockaddr **)(&premote), &remotelen );

#endif
        return bSuccess;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
//	void TCPServer::ClientDisconnectCB( IO::TCPSocket* callfd, void* data )
//	{
//		TCPServer* pServer = (TCPServer*)data;
//		callfd->Close();
//	    EraseSocketFromSeq( &(pServer->m_ClientList), callfd );
//	    pServer->OnDisconnect( callfd );
//	}
//
//	void TCPServer::ClientKnockoutCB( IO::TCPSocket* callfd, void* data )
//	{
//		TCPServer* pServer = (TCPServer*)data;
//	    EraseSocketFromSeq( &(pServer->m_ClientList), callfd );
//	    pServer->OnKnockOut( callfd );
//		callfd->Close();
//	}


	T_Lock g_ListenThreadlock;
    void __cdecl TCPServer::Polling( void* argv )
    {
        //std::vector<IO::TCPSocket*>* servicelist = (std::vector<IO::TCPSocket*>*)argv;
		static T_Lock removelock;
        TCPServer* pServer = ( TCPServer* ) argv;

#if defined(_WIN32IOCOMPLETIONPORT)     // Windows platforms
        ULONG key;
        HANDLE       hCompletionPort = (HANDLE)pServer->m_hIOCP;
        DWORD        dwIoSize = 0;
        LPOVERLAPPED lpOverlapped = NULL;
        //PER_IO_DATA* PerIoData = NULL;
        //long RunningThreads=0;
        InterlockedIncrement( &pServer->m_RunningThreads );
        while ( 1 )
        {
            InterlockedDecrement( &pServer->m_RunningThreads );
            lpOverlapped = 0;
            dwIoSize = 0;
#ifdef _DEBUG
         	g_ListenThreadlock.Lock();
            pServer->m_LivingThreads++;
			g_ListenThreadlock.UnLock();
#endif
            BOOL bIORet = GetQueuedCompletionStatus( hCompletionPort,
                                                     &dwIoSize,
                                                     ( LPDWORD )&key,
                                                     &lpOverlapped,
                                                     INFINITE ); //INFINITE
#ifdef _DEBUG			
			g_ListenThreadlock.Lock();
			pServer->m_LivingThreads--;
			g_ListenThreadlock.UnLock();
#endif
			//TS_Lock lock(&g_Testlock);
            //PerIoData = CONTAINING_RECORD( ipOverlapped, PER_IO_DATA, *lpOverlapped );
            if( (unsigned long)key == KEY_TERMINATE )
				break; // exit thread

            //printf( "============= Call recv.......Client size %d====================\n", pServer->GetConnectionSize() );
			SocketOV* ov = (SocketOV*)lpOverlapped;
            IO::TCPSocket* callfd = (IO::TCPSocket*)key;

#ifdef _DEBUG
			//char cbbuff[512];
			//sprintf( cbbuff, "        IOCP callback, ovtype=%d, ovlen=%d, mark=%d, result=%d, recvlen=%d\n", ov->optype, ov->n, ov->m_mark, bIORet, dwIoSize );
			//printf( cbbuff );
			//OutputDebugString( cbbuff );
#endif
            InterlockedIncrement( &pServer->m_RunningThreads );
            
            if( bIORet )
            {
            	//===============================================================================
                // Read; Write
                if ( dwIoSize != 0 )
                {
					switch( ov->GetOPType() )
					{
					case IO::OP_READING:
						{
//#ifdef _DEBUG
//							UINT i=0;
//							if( dwIoSize >= _MAX_SOCKET_BUFF )
//							{
//								char buff[255]={"long package\n!"};
//								DebugMessage( buff, "IOServer" );								
//								OutputDebugString( buff );
//							}
//#endif
						    ov->end+=dwIoSize;
							while( ov->begin<ov->end )
							{
								pServer->OnReceive( callfd );
//#ifdef _DEBUG
//   								i++;
//								if( i>=500 )
//								{
//									char buff[512];
//									sprintf( buff, "%d too long, total %d = last %d\n!", callfd->m_sockfd, dwIoSize, ((ov->end)-(ov->begin)) );
//									DebugMessage( buff, "IOServer" );
//									OutputDebugString( buff );
//								}
//#endif
							}
							ov->PostRead();
						}
						continue;
					case IO::OP_WRITING:
						{
							//char uff[255];
//							sprintf( uff, "%d Do write message\n", callfd->m_sockfd);
//
//							DebugMessage( uff, "IOCP" );
						    ov->DoSend();
						}
						continue;
					}
                }
                
                //===============================================================================
                // Accept; Close
                else if(dwIoSize==0)
                {
                    if( pServer == callfd )
                    {
                        //accept new connect
#ifdef _DEBUG
                        printf( "[On accept : %d]....\n", ov->GetSocket() );
#endif
                        //must less than allow size
                        if( pServer->m_ClientList.size() < pServer->m_LimitClient )
                        {
                            if( pServer->Accept( *(ov->GetSocket()) ) )
							{
								sleep(0);
								if( ov->PostRead()>=0 )
								{
									sleep(0);
									pServer->m_ClientList.push_back( ov->GetSocket() );
									if( pServer->OnAccept( (ov->GetSocket()) ) )
									{
								        time( &((ov->GetSocket())->m_CreateTime) );
										continue;
									}
								}
							}
                        }
						pServer->KnockOut( ov->GetSocket() );
						
						//ov->PostAccept( pServer->m_sockfd );
                        continue;
                    }
                    // callfd call shutdown
                    else // callfd call shutdown
                    {
#ifdef _DEBUG
			            printf( "[On client disconnection : %d]....", callfd->m_sockfd );
//						// because WM_Destroy will call logout
				        printf( "Disconnection : exist and remove successful\n" );
#endif
						pServer->KnockOut( callfd );
					    continue;						
                    }

                }
            }
			else if( ( bIORet== ERROR_SUCCESS ) &&
				     ( callfd!=NULL ) )//&&
					//( dwIoSize==0 ) )
			{
				//printf( "[On client knockout by server : %d]....\n", callfd->m_sockfd );
				pServer->KnockOut( callfd );
				continue;
			}
            else
            {

                // something failed. Was it the call itself, or did we just eat
                // a failed socket I/O?
				DWORD lasterrorno=GetLastError();                

//#ifdef _DEBUG
//				int   wsalasterrorno=WSAGetLastError();
//				
//                printf( "============= the IOCP op failed!!! ====================\n" );
//				printf( "       System %d | Socket %d | Result %d\n",
//					    lasterrorno, wsalasterrorno, bIORet );
//#endif

				if( lasterrorno== WAIT_TIMEOUT )
				{
#ifdef _DEBUG
					printf( "Polling timeout\n" );
#endif
					continue;
				}

                if ( lpOverlapped != NULL ) // the IOCP op failed
                {
					// server close之後與 Server做連結的ov
					if( callfd == pServer )
					{
#ifdef _DEBUG
			            printf( "[On server close : %d]\n", callfd );
					    // because WM_Destroy will call logout
				        printf( "Close : exist and remove not used client successful\n" );
#endif
						pServer->Close();
					}
					else
					{
					
#ifdef _DEBUG
			            printf( "[On clinet crash : %d]\n", callfd->m_sockfd );
					    // because WM_Destroy will call logout
				        printf( "Crash : exist and remove successful\n" );
#endif
						pServer->KnockOut( callfd );
					}
					continue;
                }
                //callfd crash?
                else
                {
#ifdef _DEBUG
					printf( "============= Overlapped==NULL!!! ====================\n" );
#endif
                    perror( "Illegal call to GetQueuedCompletionStatus\n" );
                    // no key, no byte count, no overlapped info available!
                    // how do I handle this?
                    // by ignoring it. I don't even have a socket handle to close.
                    // You might want to abort instead, as something is seriously
                    // wrong if we get here.
                    continue;
				}

                continue;
            }
        }
#endif
         return;
     }

    //=============================================================================
    //  Name : Accept
    //  Desc :
    //=============================================================================
    int TCPServer::KnockOut( TCPSocket* s )
    {
		sleep(0);
        TS_Lock lock(&m_lClose);

		if( s==NULL )
			return 0;

#if defined(_WIN32IOCOMPLETIONPORT)     // Windows platforms
		SocketOV* cov = (SocketOV*)(s->m_Recvbuf);
		if( cov->GetOPType()==IO::OP_ACCEPTING )
			return 0;
#else
		if( s->m_sockfd==0 )
			return 0;
#endif


	    EraseSocketFromSeq( &(m_ClientList), s );
		struct linger li = { 1, 0 };
		setsockopt(s->m_sockfd, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li) );

		s->Close();
        OnDisconnect( s );
#if defined(_WIN32IOCOMPLETIONPORT)     // Windows platforms
        // 這個必須後作!!
		if( cov->PostAccept( m_sockfd )!=1 )
		    m_ListeningList.push_back( s );
#else
		
#endif
        return 1;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void __cdecl TCPServer::Listening( void* data )
    {
		char errmessage[256];
		int  failurecount=0;
		IO::TCPServer* pServer = (IO::TCPServer*)( data );
	    while( pServer->m_sockfd )
		{
			pServer->m_lClose.Lock();
		    if( !pServer->m_ListeningList.empty() )
			{
			    IO::TCPSocket* socket = pServer->m_ListeningList.front();
				SocketOV* cov = (SocketOV*)(socket->m_Recvbuf);
				if( cov->PostAccept(pServer->m_sockfd)==1 )
				{
					printf( "recycling one socket successful\n" );
				    pServer->m_ListeningList.erase( pServer->m_ListeningList.begin() );
					failurecount=0;
				}
				else
				{
					failurecount++;
					sprintf( errmessage, "failure create socket %d size=%d error %d\n", failurecount,
						                                           pServer->m_ListeningList.size(), socket->GetErrorNo()  );
					DebugMessage( errmessage, "Create socket faulare\n" );
				    
				}
			}
			pServer->m_lClose.UnLock();
			sleep(1000);
		}
		return;
//        while ( 1 )
//        {
//            if ( pServer->IsActive() == false )
//                break;
//
//            ////////printf( "waiting for connect........\n" );
//            if( pServer->m_ClientList.size() < pServer->m_LimitClient )
//            {
//                IO::TCPSocket* c = new IO::TCPSocket;
//                if( !pServer->Accept( *c ) )
//                    delete c;
//            }
            ////////printf( "add client on fd %d\n", (Psocket)(*client) );
        //}

        //return; // Normal Thread Exit Code...
    }

//extern int tselect( int maxp1, fd_set* re, fd_set* we, fd_set* ee );
    //=============================================================================
    //  Name : Idle
    //  Desc : server listening loop
    //=============================================================================
    void __cdecl TCPServer::Select( void* data ) //server wait for bind in win32 system
    {
        TCPServer* service = ( TCPServer* ) data;
        service->OnInitiation();
        //#ifndef _WIN32

        //Psocket sockfd, client_sockfd;
        //Psocket client_sockfd;

        //int    server_len;
        //struct sockaddr_in server_address;
        //int client_len;
        //struct sockaddr_in client_address;
        //int    result;
        fd_set readfds, testfds;

        //clear socket table

        //m_Client.clear();

        //Create and name a socket for the server.

        //if( tcp_server( NULL, port, domain, streamtype, protocol) < 0 )
        //    return -1;

        FD_ZERO( &readfds );
        FD_SET( service->m_sockfd, &readfds );

        //Now wait for clients and requests.
        //Since we have passed a null pointer as the timeout parameter, no timeout will occur.
        //The program will exit and report an error if select returns a value of less than 1.

        while ( 1 )
        {
            //int   fd;
            int nread = 0;

            testfds = readfds;

            //printf( "server waiting........\n" );
            select(FD_SETSIZE, &testfds, (fd_set *)0,
                (fd_set *)0, (struct timeval *) 0);

            /*
            if ( tselect( FD_SETSIZE,
                                   &testfds,
                                   ( fd_set * ) 0,
                                   ( fd_set * ) 0 ) < 1 )
            {
                perror( "server polling failure" );
                //exit(1);
            }
            */

            //Once we know we've got activity,
            //we find which descriptor it's on by checking each in turn using FD_ISSET.

            for ( int fd = 0; fd < FD_SETSIZE; fd++ )
            {
                if ( FD_ISSET( fd, &testfds ) )
                {
                    //If the activity is on server sockfd, it must be a request for a new connection
                    //and we add the associated client_sockfd to the descriptor set.

                    if ( fd == service->m_sockfd )
                    {
                        //client_len = sizeof( client_address );
                        IO::TCPSocket* client_sockfd = new IO::TCPSocket;
                        service->Accept( *client_sockfd );
                        ////service->OnConnect();
                        //service->AddServiceSocket( client_sockfd );

                        FD_SET( *client_sockfd, &readfds );

                        //client_add( client_sockfd, client_address );
                        ////////printf( "adding client on fd %d from  : %d\n", (*client_sockfd),
                        ///////client_sockfd->GetPort() );
                        ////////printf( "client size %d\n", service->m_ClientList.size() );
                    }

                    //If it isn't the server, it must be client activity.
                    //If close is received, the client has gone away and we remove it from the descriptor set.
                    //Otherwise, we 'serve' the client as in the previous examples.
                    else
                    {
#ifdef _LINUX
                        ioctl( fd, FIONREAD, &nread );
#endif
#ifdef _WIN32
                        ioctlsocket( fd, FIONREAD, ( u_long * ) &nread );
#endif

                        if ( nread == 0 )
                        {
                            //client_del( fd );
                            //close( fd );
                            /////service->OnDisconnect();

                            FD_CLR( fd, &readfds );
                            ////////printf( "removing client on fd %d\n", fd );
                            //printf( "client size %d\n", client_size() );
                        }
                        else
                        {
                            //char* recv_buffer;
                            //service->OnReceive()
                            char buffer[100];
                            int getlen = recv( fd, buffer, 100, 0 );
                            //int getlen = recvpk( fd, &recv_buffer, 0 );
                            /////printf( "get data  : %s\n", recv_buffer );
                            //////printf( "get length  : %d\n", getlen );
                        }
                    }
                }
            }
        }
        return;
    }









    ///////////////////////////////////////////////////////////////////////////////////
    //UDP server
    //
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    UDPService::UDPService()
    {
        m_shutdown = false;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    UDPService::~UDPService()
    {


    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int UDPService::StartUp( unsigned short port )
    {
        if ( m_sockfd == NULL )
            return -1;

        //struct sockaddr_in local;
        //char localaddr[255];

        struct sockaddr_in local;
        bzero( &local, sizeof(local) );
        local.sin_family = AF_INET;
        local.sin_port = htons( port );
        local.sin_addr = GetLocalAddress(0);

        if ( bind( m_sockfd, ( struct sockaddr * ) &local, sizeof( local ) ) )
        {
            SetErrorCode( 1, errno, "bind failed" );
			//SetOPState( OP_FAILURE );
            return -1;
        }

        //m_Style = STYLE_SERVICE;
        //開始接收訊息
        BeginProcess( Polling, ( void * )this );

        OnInitiation();

		//SetOPState( OP_LISTENING );
		return -1;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void UDPService::Close( void )
    {
        m_shutdown=true;
        sleep( 1000 );
        Socket::Close();
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void __cdecl UDPService::Polling( void* data )
    {
        UDPService* udpserver = (UDPService*)data;
        //struct sockaddr_in addr;
        //int addr_len = sizeof( struct sockaddr_in );
        //char buffer[256];
        int nread=0;

        while( !udpserver->m_shutdown )
        {
#ifdef _LINUX
            ioctl( udpserver->m_sockfd, FIONREAD, &nread );
#endif
#ifdef _WIN32
            ioctlsocket( udpserver->m_sockfd, FIONREAD, ( u_long * ) &nread );
#endif
            if( nread>0 )
                udpserver->OnReceive( udpserver );
                //recvfrom( m_sockfd, buffer, sizeof( buffer), 0, &addr, &addr_len );
        }
    }

};

