//TCPServer.cpp network server function class
//
//
//
//
//
//
//
//               Apingu
//                  Sat Mar 16 2002

//01. == C 系統文件/C++ 系統文件
#if defined(_WINDOWS)
#include "iocp-def.h"
#include <time.h>
#endif

#if defined(_LINUX)
#include "epoll-def.h"
#include <sys/ioctl.h>
#include <signal.h>
#endif

#include <set>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-thread.h>
#include <pf-system.h>
#include <pf-stdlib.h>
#include <template/vmapl.inl>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "net-server.h"


//////////////////////////////////////////////////////////////////////////////////////////////
//
//  server class
//
namespace net
{

    typedef std::map<dword_t, TCPSocket*>    socket_bank_t;
    namespace
    {
#define _MAX_TCPLOCK_SIZE 10000
#define _MAX_TCPLOCK_TYPE 2
        pfstl::vmapl<net::TCPSocket*>  g_socket_cache( 1000 );
        T_Lock                         s_socekt_ut_lock[ 2 ][ _MAX_TCPLOCK_SIZE ];
        //std::vector<net::TCPSocket*> s_KickoutSocketCache
        // type = 0 used( 1 => 2)  else  free( 2 => 1)
        bool InitinalSocket( int fd )
        {
#if defined(_WIN32IOCOMPLETIONPORT)
            // 一定要呼叫
            setsockopt( fd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, ( ntchar_t* )&fd, sizeof( SOCKET ) );
            // turn on keep-alive monitoring
            ulong32_t flag = 1;
            setsockopt( fd, SOL_SOCKET, SO_KEEPALIVE, ( char* )&flag, sizeof( flag ) );
            //set socket no delay
            //ulong32_t flag = 1;
            setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, ( char* )&flag, sizeof( flag ) ); // no delay

            u_long mode = 1;  // 1 to enable non-blocking socket
            ioctlsocket( fd, FIONBIO, &mode );

#endif
#if defined(_LINUXEPOLL)
            int flags, s;
            flags = fcntl( fd, F_GETFL, 0 );
            if( flags == -1 )
            {
                net::Socket::PrintError( "fcntl" );
                return false;
            }
            flags |= O_NONBLOCK;
            s = fcntl( fd, F_SETFL, flags );
            if( s == -1 )
            {
                net::Socket::PrintError( "fcntl" );
                return false;
            }

            int iKeepAlive = 1;
            setsockopt( s, SOL_SOCKET, SO_KEEPALIVE, ( void * )&iKeepAlive, sizeof( iKeepAlive ) );

            //int bufflen = 256 * _1KB_;
            //setsockopt( fd, SOL_SOCKET, SO_SNDBUF, &bufflen, sizeof( bufflen ) );
            //setsockopt( fd, SOL_SOCKET, SO_RCVBUF, &bufflen, sizeof( bufflen ) );
#endif
            return true;
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    TCPServer::TCPServer()
    {
        max_client_ = 0;
        num_running_threads_ = 0;
        max_socket_banks_div_ = 0;
        //m_AcceptCnt = 0;
#if defined (_WIN32IOCOMPLETIONPORT)  /* Windows platforms iocp*/
        hIOCP_ = NULL;
#endif
        //g_socket_cache = new SocketCache;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    TCPServer::~TCPServer()
    {
        //Shutdown(2);
        //SocketCache* clinetlist = ( SocketCache* )g_socket_cache;
        //SAFE_DELETE( clinetlist );
        Shutdown();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t TCPServer::GetFD()
    {
        return tcpsocket_.GetFD();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t TCPServer::Create( int domain )
    {
        domain_ = domain;
        socket_t fd = tcpsocket_.Create( domain );

        if( fd != NULL_SOCKET )
        {
            const int on = 1;

            if( tcpsocket_.SetOpt( SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) ) )
            {
                tcpsocket_.SetErrorCode( 1, errno, "set reuseaddr failed!" );
            }

            tcpsocket_.SetTCPBufSize( SO_RCVBUF, 128 * _1KB_ );
        }

        return fd;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int TCPServer::StartUp( uint_t allowplayers, ushort_t port )
    {
        if( GetFD() == NULL_SOCKET )
        {
            return -1;
        }

#if defined (_WIN32IOCOMPLETIONPORT)  /* Windows platforms iocp*/

        if( hIOCP_ )
        {
            return 1;
        } // already started, return

#endif
        TS_Lock lock( &tcpserver_ut_lock_ );

        // set server as non block
        InitinalSocket( tcpsocket_.GetFD() );

        max_client_ = allowplayers;
        listen_port_ = port;
        num_polling_threads_ = ( pfkrl::GetNumberOfProcessors() * 2 ) + 2;
        sockaddr_in address;
        ntchar_t buf[ 50 ];
        net::Socket::SetAddress( NULL, itoa( port, buf, 10 ), &address, "tcp" );

        max_socket_banks_div_ = num_polling_threads_;
        sockets_lock_.resize( max_socket_banks_div_ );
        sockets_[ 0 ].resize( max_socket_banks_div_ );
        sockets_[ 1 ].resize( max_socket_banks_div_ );
        sockets_[ 2 ].resize( max_socket_banks_div_ );

        //restart havn not problem in address already in use
        if( tcpsocket_.Bind( ( struct sockaddr* ) &address, sizeof( address ) ) )
        {
            tcpsocket_.SetErrorCode( 1, errno, "bind failed" );
            //SetOPState(OP_BIND_FAILURE);
            return -1;
        }

        if( listen( tcpsocket_.GetFD(), NLISTEN ) )
        {
            tcpsocket_.SetErrorCode( 1, errno, "listen failed" );
            //SetOPState(OP_LISTEN_FAILURE);
            return -1;
        }

        {
            // 要再 Listening 跟 Kicking之前啟動 不然立刻就會離開了
            //TS_Lock lock(&m_lAlive);
            //m_bAlive=true;
        }

        //if ( !pfkrl::Thread::BeginProcess( Listening, ( void* )this ) )
        //{
        //    return -1;
        //}
        // 2009 09 23 先拿掉 kicklout 裏面值接
        // 2009 10  1 回覆 因為碰到 updata 跟 ondisconnect時候 資料 的問題
        // 2009 10 23 又打開 因為UPDATE的時候 會取到 沒有清除乾淨的資料
        //if ( !pfkrl::Thread::BeginProcess( Kicking, ( void* )this ) )
        //{
        //    return -1;
        //}

        for( uint_t si = 0; si < allowplayers; si++ )
        {
            //TCPSocket* sk = OnNewSocket();
            TCPSocket* sk = new TCPSocket;
            OnCreateSocket( sk );

            //TCPSocket* sk = new TCPSocket();
            if( sk == NULL )
            {
                net::Socket::PrintError( "create socket failure, socket can't be NULL!" );
                exit( 0 );
            }

#if defined (_WIN32IOCOMPLETIONPORT)  /* Windows platforms iocp*/
            SocketOV* cov = ( SocketOV* )( sk->recv_buffer_ );

            sk->Create();
            if( cov->PostAccept( tcpsocket_.GetFD() ) )
            {
                // 放入未被使用的駐列中
                //sockets_[ 0 ].insert( std::set<net::TCPSocket*>::value_type( sk ) );
                //sockets_[ 1 ].insert( std::set<net::TCPSocket*>::value_type( sk ) );
                sockets_[ 0 ][ sk->GetUID() % max_socket_banks_div_ ].insert( socket_bank_t::value_type( sk->GetUID(), sk ) );
                sockets_[ 1 ][ sk->GetUID() % max_socket_banks_div_ ].insert( socket_bank_t::value_type( sk->GetUID(), sk ) );
            }
            else
            {
                SAFE_DELETE( sk );
                sk--;
            }
#endif
#if defined(_LINUXEPOLL)

            //sockets_[ 0 ].insert( std::set<net::TCPSocket*>::value_type( sk ) );
            //sockets_[ 1 ].insert( std::set<net::TCPSocket*>::value_type( sk ) );
            sockets_[ 0 ][ sk->GetUID() % max_socket_banks_div_ ].insert( socket_bank_t::value_type( sk->GetUID(), sk ) );
            sockets_[ 1 ][ sk->GetUID() % max_socket_banks_div_ ].insert( socket_bank_t::value_type( sk->GetUID(), sk ) );
#endif
        }

#if defined (_WIN32IOCOMPLETIONPORT)  /* Windows platforms iocp*/
        // create IOCP and initance
        hIOCP_ = CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 );

        if( hIOCP_ == NULL )
        {
            printf( "CreateIoCompletionPort failed: %d\n", net::Socket::GetLastError() );
            return -1;
        }

        //計算一顆CPU給兩個 worker
        //SYSTEM_INFO systemInfo;
        //GetSystemInfo(&systemInfo);
        //cpusize = GetNumberOfProcessors();
        //MEMORYSTATUS mstat;
        //GlobalMemoryStatus(&mstat);
        //memorysize = mstat.dwTotalPhys;
        //build listen thread
        //num_polling_threads_ = (GetNumberOfProcessors() * 2) + 2;
        //tcpsocket_.m_SerialNumber = m_SocketLock.size();
        //m_SocketLock.push_back(new T_Lock);
        ULONG_PTR key = reinterpret_cast< ULONG_PTR >( &tcpsocket_ );
        HANDLE h = CreateIoCompletionPort( ( HANDLE )tcpsocket_.GetFD(), hIOCP_, key, 0 );

        if( h == NULL )
        {
            fprintf( stderr, "CreateIoCompletionPort failed: %d\n", net::Socket::GetLastError() );
            return -1;
        }

#endif
#if defined(_LINUXEPOLL)
        //int i, maxi, listenfd, connfd, sockfd,epfd,nfds;
        //size32_t n;
        //ntchar_t line[MAXLINE];
        //socklen_t clilen;
        int epoll_cnt = max_client_ + 1;
        hEPFD_ = epoll_create( epoll_cnt );  // 加上server的
        struct epoll_event ev;
        ev.data.fd = tcpsocket_.GetFD();
        ev.data.ptr = &tcpsocket_;
        // 多緒使用 EPOLLONESHOT 才不會出現 競爭的問題
        // //不能將監聽端口listenfd設為EPOLLONESHOT 否則會丟失連結
        ev.events = EPOLLIN | EPOLLET;
        int sk = epoll_ctl( hEPFD_, EPOLL_CTL_ADD, tcpsocket_.GetFD(), &ev );

        if( sk == -1 )
        {
            net::Socket::PrintError( "epoll_ctl" );
            return -1;
        }
        
        //  不被SIGPIPE信號殺死，那就需要忽略SIGPIPE信號：
        signal( SIGPIPE, SIG_IGN );
#endif
        // 因為 有時候開始時 LOCK會當掉 等一下看看
        pfstd::MSSleep( 3000 );
        // 靠2008一定得這樣做 甚麼鬼
        //for(long32_t i = 0; i < num_polling_threads_; i++)
        //{
        // m_LivingThreadsLock.push_back(new T_Lock);
        //}
        {
            TS_Lock plock( &num_processing_threads_lock_ );
            num_processing_threads_ = 0;
        }
        polling_threads_uid_.clear();
        //num_polling_threads_ = 1;

        for( uint_t i = 0; i < num_polling_threads_; i++ )
        {
            pfkrl::Thread::Handle tid = pfkrl::Thread::BeginProcess( Polling, ( void* )this, _T( "TCPServer::StartUp" ) );

            if( !tid )
            {
                return -1;
            }
            polling_threads_uid_.push_back( tid );
        }

        return 1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int TCPServer::Shutdown( void )
    {
        int ret = 0;
        TS_Lock lock( &tcpserver_ut_lock_ );

        if( tcpsocket_.GetFD() == 0 )
        {
            return -1;
        }

        //if ( sockets_[ 0 ].empty() )
        //{
        //    return -1;
        //}

        ret = tcpsocket_.Close();
        //std::vector<TCPSocket*> socketvec;
        //( ( SocketCache* )g_socket_cache )->GetAllSocket( socketvec );

        for( int pi = 0; pi < max_socket_banks_div_; pi++ )
        {
            for( socket_bank_t::iterator it = sockets_[ 0 ][ pi ].begin(); it != sockets_[ 0 ][ pi ].end(); ++it )
            {
                TCPSocket* sk = it->second;
                Kickout( sk );
                //sk->Close();
            }
        }

        //( ( SocketCache* )g_socket_cache )->clear();
        //m_KickoutSocketCache.clear();
        // 等待polling 都跳出來
#if defined (_WIN32IOCOMPLETIONPORT)  /* Windows platforms iocp*/

        // 這邊真的得 有幾個polling就得做幾次 不然會只跳出一個
        for( uint_t i = 0; i < polling_threads_uid_.size(); i++ )
        {
            PostQueuedCompletionStatus( hIOCP_, 0, KEY_TERMINATE, 0 );
        }
        if( !polling_threads_uid_.empty() )
            WaitForMultipleObjects( polling_threads_uid_.size(), ( HANDLE* )( &polling_threads_uid_[ 0 ] ), TRUE, 0 );
#endif

        //g_socket_cache

        // 等待polling 都跳出來 因為iocp每次都不靈光 乾脆自己做
        //for ( int i = 0; i < 100; i++ )
        //{
        //    pfstd::MSSleep( 1000 ); // just for testing
        //    TS_Lock plock( &num_processing_threads_lock_ );
        //
        //    if ( num_processing_threads_ == 0 )
        //    {
        //        break;
        //    }
        //}

        for( int pi = 0; pi < max_socket_banks_div_; pi++ )
        {
            for( socket_bank_t::iterator it = sockets_[ 0 ][ pi ].begin(); it != sockets_[ 0 ][ pi ].end(); ++it )
            {
                TCPSocket* sk = it->second;
                //OnDelSocket( sk );
                OnDestroySocket( sk );
                SAFE_DELETE( sk );
            }
            sockets_[ 0 ][ pi ].clear();
            sockets_[ 1 ][ pi ].clear();
            sockets_[ 2 ][ pi ].clear();
        }
        //sockets_.clear();
        //for(uint_t l = 0; l < m_SocketLock.size(); l++)
        //{
        // T_Lock* ll = m_SocketLock[l];
        // SAFE_DELETE(ll);
        //}
        //m_SocketLock.clear();
#if defined (_WIN32IOCOMPLETIONPORT)  /* Windows platforms iocp*/
        CloseHandle( hIOCP_ );
        hIOCP_ = NULL;
#endif
        return ret;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool TCPServer::IsActive() //是否停止
    {
        TS_Lock lock( &tcpserver_ut_lock_ );

        if( tcpsocket_.IsLive() == 0 )
        {
            return false;
        }

        return true;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void TCPServer::GetLocalIPAddress( _OUT std::vector<ipa_t>& ip4list, _OUT std::vector<ipa_t>& ip6list )
    //{
    //    ntchar_t hostname[ _MAX_HOST_NAME ];
    //    Socket::GetLocalHostname( hostname, _MAX_HOST_NAME );
    //    std::vector<sockaddr_storage> addr4info_list;
    //    Socket::GetAddrNameInfo( addr4info_list, hostname, NULL, AF_INET, SOCK_STREAM );
    //    std::vector<sockaddr_storage> addr6info_list;
    //    Socket::GetAddrNameInfo( addr6info_list, hostname, NULL, AF_INET6, SOCK_STREAM );
    //
    //    for ( int i = 0; i < addr4info_list.size(); i++ )
    //    {
    //        ip4list.push_back( Socket::GetSockAddrIPA( ( sockaddr* ) &addr4info_list[ i ] ) );
    //    }
    //
    //    for ( int i = 0; i < addr4info_list.size(); i++ )
    //    {
    //        ip6list.push_back( Socket::GetSockAddrIPA( ( sockaddr* ) &addr6info_list[ i ] ) );
    //    }
    //    return;
    //}


    //=============================================================================
    // NAME: GetLivingThreadSize()
    // DESC:
    //=============================================================================
    long32_t TCPServer::GetLivingThreadSize( void )
    {
        /*
        int cnt = 0;
        for(uint_t i = 0; i < m_LivingThreads.size(); i++)
        {
         TS_Lock lock(m_LivingThreadsLock[i]);
         cnt += m_LivingThreads[i];
        }
        return cnt;
        */
        return num_running_threads_;
    }

    //=============================================================================
    // NAME: GetPort()
    // DESC: get local host name
    //=============================================================================
    ushort_t TCPServer::GetServicePort( void )
    {
        TS_Lock lock( &tcpserver_ut_lock_ );
        return listen_port_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t TCPServer::GetMaxConnectSize( void )
    {
        TS_Lock lock( &tcpserver_ut_lock_ );
        return max_client_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t TCPServer::GetConnectedSize( void )
    {
        size_t ps = 0;
        for( int pi = 0; pi < max_socket_banks_div_; pi++ )
        {
            TS_Lock lock( &sockets_lock_[ pi ] );
            ps += sockets_[ 2 ][ pi ].size();
        }
        return ps;
        //return g_socket_cache.GetSocketSizeFromSeq();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    net::TCPSocket* TCPServer::operator[](uint_t index)
    {
     TS_Lock lock(&tcpserver_ut_lock_);
     //TS_Lock cllock(&g_lclientlist);
     //if(index >=((SocketCache*)g_socket_cache)->GetSocketSizeFromSeq())
      return NULL;
     //return g_socket_cache[index];
    }
    */


    void TCPServer::GetAllConnectedSocket( std::vector<net::TCPSocket*>& connectionlist )
    {
        for( int pi = 0; pi < max_socket_banks_div_; pi++ )
        {
            TS_Lock lock( &sockets_lock_[ pi ] );
            for( socket_bank_t::iterator it = sockets_[ 2 ][ pi ].begin(); it != sockets_[ 2 ][ pi ].end(); ++it )
            {
                connectionlist.push_back( it->second );
            }
        }
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    net::TCPSocket* TCPServer::FindConnectedSocket( socket_t fd )
    {
        //TS_Lock lock( &tcpserver_ut_lock_ );
        TS_Lock lock( &g_socket_cache.lock( fd ) );
        _vmapl_iterator( TCPSocket* ) iter = g_socket_cache.find( fd );
        if( g_socket_cache.miss( fd, iter ) )
        {
            return NULL;
        }
        return iter->second;
        //return g_socket_cache.FindSocketFromSeq( fd );
    }


    //=============================================================================
    // NAME: Accept
    // DESC:
    //=============================================================================
//    int TCPServer::Accept( TCPSocket& sk )
//    {
//        return NULL;
//#if defined(_WIN32IOCOMPLETIONPORT)  /* Windows platforms */
//        socket_t peerfd = sk.GetFD();
//        // need to update accept context
//        //socket_t sockett = GetFD();
//        setsockopt( peerfd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, ( ntchar_t* )&peerfd, sizeof( SOCKET ) );
//        // turn on keep-alive monitoring
//        ulong32_t flag = 1;
//        setsockopt( peerfd, SOL_SOCKET, SO_KEEPALIVE, ( char* )&flag, sizeof( flag ) );
//        //set socket no delay
//        //ulong32_t flag = 1;
//        setsockopt( peerfd, IPPROTO_TCP, TCP_NODELAY, ( char* )&flag, sizeof( flag ) ); // no delay
//        /*
//        // 預設socket 的屬性是
//        // 設定 未送完的資料 在 10的timeout 時間內送完 再close
//        struct linger li = { 1, 10 };
//        setsockopt(sk.GetFD(), SOL_SOCKET, SO_LINGER, (char*)&li, sizeof(li));
//        */
//#endif
//#if defined(_LINUX)  /* Linux platforms */
//       struct sockaddr_in peeraddr;
//       socklen_t          peerlen;
//       socket_t peerfd = accept( GetFD(), ( struct sockaddr* )&peeraddr, &peerlen );
//
//       if ( peerfd == -1 )
//       {
//           if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) )
//           {
//               /* We have processed all incoming
//               connections. */
//               return NULL;
//           }
//           else
//           {
//               net::Socket::PrintError( "accept" );
//               return NULL;
//           }
//       }
//
//       struct sockaddr info_addr;
//
//       socklen_t info_len;
//
//       int infd;
//
//       char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
//
//       int ret = getnameinfo( &info_addr, info_len, hbuf, sizeof( hbuf ), sbuf, sizeof( sbuf ), NI_NUMERICHOST | NI_NUMERICSERV );
//
//       if ( ret == 0 )
//       {
//           printf( "Accepted connection on descriptor %d ""(host=%sk, port=%sk)\n", infd, hbuf, sbuf );
//       }
//
//       /* Make the incoming socket non-blocking and add it to the list of fds to monitor. */
//       if ( !InitinalSocket( sk.GetFD() ) )
//       {
//           abort();
//       }
//
//       struct epoll_event event;
//
//       event.events = EPOLLIN | EPOLLET;
//
//       event.data.fd = sk.GetFD();
//
//       event.data.ptr = &sk;
//
//       int ss = epoll_ctl( hEPFD_, EPOLL_CTL_ADD, sk.GetFD(), &event );
//
//       if ( ss == -1 )
//       {
//           net::Socket::PrintError( "epoll_ctl" );
//           abort();
//       }
//
//#endif
//        //TCPSocket* sk = NULL;
//        // get unused socket
//        /*
//        {
//
//            TS_Lock lock(&tcpserver_ut_lock_);
//
//            // find can used socket
//            for (std::set<net::TCPSocket*>::iterator it = sockets_[1].begin(); it != sockets_[1].end(); ++it)
//            {
//                if ((*it)->GetFD() == 0)
//                {
//                    sk = (*it);
//                    sockets_[1].erase(it);
//                    break;
//                }
//            }
//
//            if (sk == NULL)
//            {
//                Pf_WARNING("unused socket not empty\n");
//                return NULL;
//            }
//
//            (*(Socket*)sk) = (peerfd);
//            //sk->Socket::operator=(peerfd);
//            sockets_[2].insert(std::set<net::TCPSocket*>::value_type(sk));
//        }
//        g_socket_cache.insert(peerfd, sk);
//        */
//        //bSuccess = g_socket_cache.InsertSocketIntoSeq();
//        //printf("[client accept : %d]....\n", sk.GetFD());
//        return sk;
//    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void TCPServer::Polling( void* argv )
    {
        TCPServer* p_server = ( TCPServer* )argv;
        int LivingId = -1;
        int LiveSize = 0;
        {
            TS_Lock plock( &p_server->num_processing_threads_lock_ );
            LivingId = p_server->num_processing_threads_;
            p_server->num_processing_threads_++;
            LiveSize = p_server->GetPollingCount();
        }

#if defined(_LINUXEPOLL)
        //struct epoll_event  event;
        struct epoll_event* events;
        /* Buffer where events are returned */
        int  eventlen = p_server->max_client_ + 1;
        events = ( epoll_event* )calloc( eventlen, sizeof( epoll_event ) );

        int timeout = 10000;
        /* The event loop */
        while( p_server->GetFD() != 0 )
        {
            int n = epoll_wait( p_server->hEPFD_, events, eventlen, timeout );
            if( n < 0 )
            {
                if( errno == EINTR )
                {
                    //printf( _T( "epoll_wait failure %d in\n" ), errno );
                }
                if( errno == EAGAIN )
                {
                    printf( _T( "epoll_wait failure %d in\n" ), errno );
                }
                pfstd::MSSleep( 10 );
                continue;
            }
            else if( n == 0 )  // timeout
            {
                continue;
            }
            //printf( _T( "ID %d in\n" ), LivingId );
            for( int i = 0; i < n; i++ )
            {
                //printf( _T( "00 - 01 \n" ) );
                //socket_t   callfd = events[ i ].data.fd;
                TCPSocket* call_skt = ( TCPSocket* )events[ i ].data.ptr;
                if( call_skt == NULL )
                {
                    continue;
                }

                // first lock
                TS_Lock lock( &s_socekt_ut_lock[ 0 ][ call_skt->GetUID() % _MAX_TCPLOCK_SIZE ] );
                //TS_Lock lock( &s_socekt_ut_lock[ 0 ][ call_skt->GetFD() % _MAX_TCPLOCK_SIZE ] );

                //TS_Lock lock( &call_skt->m_utLock );
                if( ( events[ i ].events & EPOLLERR ) || ( events[ i ].events & EPOLLHUP ) || ( !( events[ i ].events & EPOLLIN ) ) )
                {
                    /* An error has occured on this fd, or the socket is not ready for reading (why were we notified then?) */
                    Pf_ERROR( "epoll error\n" );
                    //close (events[i].data.fd);
                    p_server->Kickout( call_skt );
                    continue;
                }
                else
                {
                    if( p_server->GetFD() == call_skt->GetFD() )
                    {
                        /* We have a notification on the listening socket, which means one or more incoming connections. */
                        //TCPSocket* sk = p_server->OnNewSocket();
                        while( 1 )
                        {
                            TCPSocket* client_sk = NULL;
                            {
                                //TS_Lock lock( &s_socekt_ut_lock[ 1 ][ ( ( ulonglong_t ) ( *p_server->sockets_[ 1 ].begin() ) ) % _MAX_TCPLOCK_SIZE ] );
                                struct sockaddr_in peeraddr;
                                socklen_t          peerlen = sizeof( peeraddr );
                                socket_t clientfd = accept( p_server->GetFD(), ( struct sockaddr* )&peeraddr, &peerlen );
                                if( clientfd == -1 )
                                {
                                    if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) )
                                    {
                                        /* We have processed all incoming connections. */
                                        break;
                                    }
                                    else
                                    {
                                        Pf_ERROR( "accept" );
                                        goto _EXITPOLLING;
                                    }
                                }

                                {
                                    bool bfind = false;
                                    for( int pi = 0; pi < p_server->max_socket_banks_div_; pi++ )
                                    {
                                        TS_Lock poollock( &p_server->sockets_lock_[ pi ] );
                                        // find unused socket
                                        for( socket_bank_t::iterator it = p_server->sockets_[ 1 ][ pi ].begin(); it != p_server->sockets_[ 1 ][ pi ].end(); ++it )
                                        {
                                            if( it->second->GetFD() == 0 )
                                            {
                                                client_sk = it->second;
                                                p_server->sockets_[ 1 ][ pi ].erase( it );
                                                p_server->sockets_[ 2 ][ pi ].insert( socket_bank_t::value_type( client_sk->GetUID(), client_sk ) );
                                                {
                                                    TS_Lock lock( &g_socket_cache.lock( clientfd ) );
                                                    g_socket_cache.insert( clientfd, client_sk );
                                                }

                                                // set client tcp socket fd
                                                ( *( Socket* )client_sk ) = ( clientfd );
                                                bfind = true;
                                                break;
                                            }
                                        }
                                        if( bfind )
                                            break;

                                    }
                                    //pfstd::MSSleep( 100 );
                                }

                                // cant find
                                if( client_sk == NULL )
                                {
                                    Pf_ERROR( "unused socket not empty\n" );
                                    close( clientfd );
                                    break;
                                }

                                TS_Lock lock( &s_socekt_ut_lock[ 0 ][ client_sk->GetUID() % _MAX_TCPLOCK_SIZE ] );
                                //TS_Lock clock( &s_socekt_ut_lock[ 0 ][ client_sk->GetFD() % _MAX_TCPLOCK_SIZE ] );
                                InitinalSocket( clientfd );

                                struct epoll_event ev;
                                ev.data.fd  = clientfd;
                                ev.data.ptr = ( void* )client_sk;
                                //ev->data.fd = clientfd;
                                // 多緒中 使用 EPOLLONESHOT
                                ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                                p_server->OnAccept( client_sk );
                                int ss = epoll_ctl( p_server->hEPFD_, EPOLL_CTL_ADD, clientfd, &ev );
                                if( ss == -1 )
                                {
                                    Pf_ERROR( "epoll_ctl" );
                                    p_server->Kickout( client_sk );
                                }
                                //printf( "accept client %d\n", client_sk->GetFD() );
                            }
                        }
                    }
                    else
                    {
                        /* We have data on the fd waiting to be read. Read and
                        display it. We must read whatever data is available
                        completely, as we are running in edge-triggered mode
                        and won't get a notification again for the same
                        data. */
                        int blive = 1;
                        //int nread = 0;
                        //int count = 0;

                        //printf( "recvice client %d\n", call_skt->GetFD() );
                        //pfstd::MSSleep( 1000 );
                        for( int loops = 0; loops < 20; loops++ )
                        {
                            int nread = 0;
                            // 
                            //count = ioctl( call_skt->GetFD(), FIONREAD, &nread );
                            int count = ioctl( call_skt->GetFD(), FIONREAD, &nread );
                            if( nread > 0 )
                            {
                                //printf( "recvice client %d\n", call_skt->GetFD() );
                                //printf( _T( "00 - 02 \n" ) );
                                p_server->OnReceive( call_skt );
                            }
                            else
                            {
                                //printf( "errno %d\n", errno );
                                //int h = errno;
                                //if( errno != EAGAIN )
                                //{
                                blive = 0;
                                //}                                
                                break;
                            }
                        }

                        if( blive == 0 )
                        {
#ifdef _DEBUG
                            Pf_INFO( "Closed connection on descriptor %d\n", call_skt->GetFD() );
#endif
                            /* Closing the descriptor will make epoll remove it
                            from the set of descriptors which are monitored. */
                            p_server->Kickout( call_skt );
                        }
                    }
                }
            }
            //printf( _T( "ID %d out\n" ), LivingId );
        }
    _EXITPOLLING:
        free( events );
#endif
#if defined(_WIN32IOCOMPLETIONPORT)  // Windows platforms
        ULONG_PTR  key;
        HANDLE hCompletionPort = ( HANDLE )p_server->hIOCP_;
        DWORD  dwIoSize = 0;
        LPOVERLAPPED lpOverlapped = NULL;
        //PER_IO_DATA* PerIoData = NULL;
        //long32_t RunningThreads=0;
        InterlockedIncrement( &p_server->num_running_threads_ );

        while( 1 )
        {
            InterlockedDecrement( &p_server->num_running_threads_ );
            lpOverlapped = 0;
            dwIoSize = 0;
            //{
            // TS_Lock Livel(p_server->m_LivingThreadsLock[LivingId]);
            // p_server->m_LivingThreads[LivingId]++;
            //}
            BOOL bIORet = GetQueuedCompletionStatus( hCompletionPort, &dwIoSize, ( PULONG_PTR )&key, &lpOverlapped, INFINITE ); //INFINITE
            //{
            // TS_Lock Livel(p_server->m_LivingThreadsLock[LivingId]);
            // p_server->m_LivingThreads[LivingId]--;
            //}
            InterlockedIncrement( &p_server->num_running_threads_ );

            //TS_Lock lock(&g_Testlock);
            //PerIoData = CONTAINING_RECORD(ipOverlapped, PER_IO_DATA, *lpOverlapped);
            if( ( ulong32_t )key == KEY_TERMINATE )
            {
                break;
            } // exit thread

            //printf("============= Call recv.......Client size %d====================\n", p_server->GetConnectionSize());
            SocketOV* ov = ( SocketOV* )lpOverlapped;
            net::TCPSocket* call_skt = ( net::TCPSocket* )key;
            //Pf_INFO( "TCPServer::Polling: [wakeup1 : %d]\n", ov->GetSocket()->GetFD() );
            TS_Lock lock( &s_socekt_ut_lock[ 0 ][ call_skt->GetUID() % _MAX_TCPLOCK_SIZE ] );
            //Pf_INFO( "TCPServer::Polling: [wakkuup2 : %d]\n", ov->GetSocket()->GetFD() );
            //TS_Lock lock(&call_skt->m_TcpPollLock);
            //TS_Lock lock( &call_skt->m_utLock );

            if( ( bIORet ) && ( ov != NULL ) )
            {
                net::TCPSocket* ov_sk = ov->GetSocket();

                //===============================================================================
                // Read; Write
                if( dwIoSize != 0 )
                {
                    switch( ov->GetOPType() )
                    {
                    case net::OP_READING:
                        {
                            // 減掉上次處理過的
                            int readret = dwIoSize - ov->addlen;
                            ov->end += readret;
                            ov->addlen = 0;  //清空
                            while( readret > 0 )
                            {
                                while( ov->begin < ov->end )
                                {
                                    if( ( ov->GetOPType() == net::OP_READING ) && ( ov_sk->GetFD() != NULL_SOCKET ) )
                                    {
                                        p_server->OnReceive( call_skt );
                                    }
                                }
                                readret = ov->PostRead();

                                //if(readret==SOCKET_ERROR)
                                if( readret < 0 )
                                {
                                    //char buff[256];
                                    //_stprintf( buff, "iocp %d PostRead error", ov->GetSocket()->GetFD() );
#ifdef _DEBUG
                                    Pf_ERROR( "TCPServer::Polling: iocp %d PostRead error", ov->GetSocket()->GetFD() );
#endif
                                    p_server->Kickout( ov->GetSocket() );
                                    //if ( ov->GetSocket()->GetFD() != NULL_SOCKET )
                                    //if ( ov->GetSocket()->IsLive() )
                                    //{
                                    //    p_server->Kickout( ov->GetSocket() );
                                    //}
                                }
                                // 紀錄 即刻收到已經處理過的了 因為 如果 這邊 再放入 iocp中
                                // GetQueuedCompletionStatus 還是會因為 這些已經被處理過的資料
                                // 在被 叫起來 在polling 下一次 還會被叫起 但 buffer 裡面卻沒有資料
                                else if( readret > 0 )
                                {
                                    ov->end += readret;
                                    ov->addlen += readret;
                                }
                                else // =0 返回正確 且沒有 立即的資料要處理
                                {
                                }
                            }
                        }
                    }
                }
                //===============================================================================
                // Accept; Close
                else if( dwIoSize == 0 )
                {
                    if( ( &p_server->tcpsocket_ ) == call_skt )
                    {
                        // 因為現在被喚醒的是 server 的 socket
                        // 所以 得鎖住 要接收的socket 以免有client端在 connect之後 就傳送資料
                        // 但server端還沒有跑完 onaccept
                        net::TCPSocket* client_sk = ov_sk;

                        if( client_sk != call_skt )
                        {
#ifdef _DEBUG
                            Pf_INFO( "TCPServer::Polling: [iocp clinet accept : %d]\n", ov->GetSocket()->GetFD() );
#endif
                            {
                                int div = client_sk->GetUID() % p_server->max_socket_banks_div_;
                                TS_Lock poollock( &p_server->sockets_lock_[ div ] );
                                socket_bank_t::iterator it = p_server->sockets_[ 2 ][ div ].find( client_sk->GetUID() );
                                // 已經被放入使用中
                                if( it != p_server->sockets_[ 2 ][ div ].end() )
                                {
                                    Pf_WARNING( "TCPServer::Polling: [iocp clinet accept : %d already on used]\n", ov->GetSocket()->GetFD() );
                                    continue;
                                }
                                p_server->sockets_[ 1 ][ div ].erase( client_sk->GetUID() );
                                p_server->sockets_[ 2 ][ div ].insert( socket_bank_t::value_type( client_sk->GetUID(), client_sk ) );
                                {
                                    TS_Lock lock( &g_socket_cache.lock( client_sk->GetFD() ) );
                                    g_socket_cache.insert( client_sk->GetFD(), client_sk );
                                }
                                ov->DoAccept();
                            }

                            InitinalSocket( client_sk->GetFD() );
                            // 使用  CreateIoCompletionPort
                            ULONG_PTR key = reinterpret_cast< ULONG_PTR >( client_sk );
                            HANDLE hHandle = CreateIoCompletionPort( ( HANDLE )client_sk->GetFD(), p_server->hIOCP_, key, 0 );
                            if( hHandle != p_server->hIOCP_ )
                            {
                                Pf_WARNING( "TCPServer::Polling: [iocp clinet accept : %d createipcompletionport failure]\n", ov->GetSocket()->GetFD() );
                                p_server->Kickout( client_sk );
                                continue;
                            }

                            //ov->Reset( net::OP_READING );
                            if( ov->PostRead() < 0 )
                            {
                                Pf_WARNING( "TCPServer::Polling: [iocp clinet accept : %d post read failure]\n", ov->GetSocket()->GetFD() );
                                p_server->Kickout( client_sk );
                                //client_sk->Close();
                                continue;
                            }
                            p_server->OnAccept( client_sk );
                        }

                        //ov->PostAccept(p_server->socket_fd_);
                        continue;
                    }
                    // call_skt call shutdown
                    // 0得時候 就算是已經關掉了 還是RECV還是會回傳正確?
                    else
                    {
#ifdef _DEBUG
                        //ntchar_t buff[256];
                        //_stprintf( buff, "iocp %d PostRead0 error", ov->GetSocket()->GetFD() );
                        Pf_ERROR( "TCPServer::Polling: iocp %d PostRead0 error", ov->GetSocket()->GetFD() );
#endif
                        p_server->Kickout( call_skt );
                        //if ( call_skt->GetFD() != NULL_SOCKET )
                        //if ( call_skt->IsLive() )
                        //{
                        //    p_server->Kickout( call_skt );
                        //}
                    }
                }
            }
            else
            {
                dword_t lasterrorno = net::Socket::GetLastError();
#ifdef _DEBUG

                switch( lasterrorno )
                {
                case WAIT_TIMEOUT:
                    {
                        Pf_ERROR( "TCPServer::Polling: Polling timeout\n" );
                    }
                    break;

                case ERROR_SEM_TIMEOUT:
                    {
                        Pf_ERROR( "TCPServer::Polling: Polling sem timeout\n" );
                    }
                    break;

                case ERROR_NETNAME_DELETED:
                    {
                        Pf_ERROR( "TCPServer::Polling: Polling netname deleted\n" );
                    }
                    break;

                case ERROR_OPERATION_ABORTED:
                    {
                        Pf_ERROR( "TCPServer::Polling: Polling operation aborted\n" );
                    }
                    break;

                default:
                    {
                        Pf_ERROR( "TCPServer::Polling: Polling error\n" );
                    }
                    break;
                }

                //printf("Polling error %d\n", lasterrorno);
#endif

                if( ( ( &p_server->tcpsocket_ ) != call_skt ) && ( call_skt != NULL ) && ( ov != NULL ) )
                {
                    if( p_server->Kickout( call_skt ) == 1 )
                    {
                    }
                }
                else
                {
                    //p_server->Shutdown();
                    goto _EXITPOLLING;
                }
            }
        }
    _EXITPOLLING:

#endif

        //SAFE_DELETE(p_server->m_LivingThreadsLock[LivingId]);
        //ntchar_t exibuff[256];
        //_stprintf( exibuff, "polling %d exit\n", LivingId );
        Pf_INFO( "polling %d/%d exit\n", LivingId, LiveSize );
        {
            TS_Lock plock( &p_server->num_processing_threads_lock_ );
            p_server->num_processing_threads_--;
        }
        return;
    }

    //=============================================================================
    // NAME: Accept
    // DESC:
    //=============================================================================
    int TCPServer::Kickout( TCPSocket* sk )
    {
        //pfstd::MSSleep( 0 );
        //TS_Lock lock( &tcpserver_ut_lock_ );
        if( sk == NULL )
        {
            return 0;
        }

        // 可能被呼叫close?
        //if ( sk->GetFD() == NULL_SOCKET )
        //{
        //    return 0;
        //}

        // Server can't kickout self, call shutdown
        if( sk == ( &this->tcpsocket_ ) )
        {
            return 0;
        }

        {
            //ntchar_t buff[256];
            //_stprintf( buff, "[clinet Kickout : %d]\n", sk->GetFD() );
            int div = sk->GetUID() % max_socket_banks_div_;
            TS_Lock poollock( &sockets_lock_[ div ] );

            // 如果 socket 的 close 被直接使用 會被 polling 呼叫 但 socket == 0
            //if ( sk->GetFD() == 0 )
            //{
            //    return 0;
            //}
            socket_bank_t::iterator it = sockets_[ 1 ][ div ].find( sk->GetUID() );

            // 已經被放入可再利用了
            if( it != sockets_[ 1 ][ div ].end() )
            {
                return 0;
            }

            sockets_[ 2 ][ div ].erase( sk->GetUID() );
            sockets_[ 1 ][ div ].insert( socket_bank_t::value_type( sk->GetUID(), sk ) ); // 放入可以 再回收利用的 列
            {
                TS_Lock lock( &g_socket_cache.lock( sk->GetFD() ) );
                g_socket_cache.erase( sk->GetFD() );
            }
#if defined(_LINUXEPOLL)  // Windows platforms
            epoll_ctl( hEPFD_, EPOLL_CTL_DEL, sk->GetFD(), NULL );
#endif
        }

        OnKickout( sk );
        sk->Close();
        // get unused socket
        //net::Socket::PrintError( buff );
#if defined(_WIN32IOCOMPLETIONPORT)  // Windows platforms
        SocketOV* cov = ( SocketOV* )( sk->recv_buffer_ );
        sk->Create();
        cov->PostAccept( tcpsocket_.GetFD() );
#endif
        //printf("[clinet Kickout : %d]\n", sk->GetFD());
        //( ( SocketCache* )g_socket_cache )->EraseSocketFromSeq( sk );
        // 設定 未送完的資料 在 10的timeout 時間內送完 再close
        //struct linger li = { 1, 1 };
        //struct linger li = { 1, 10 };
        //setsockopt( sk->GetFD(), SOL_SOCKET, SO_LINGER, ( char* )&li, sizeof( li ) );
        // 先設為禁收 然後再另一個序裡面close 避免 deadlock
        //sk->Close();
        // server 要關閉了
        //m_KickoutSocketCache.push_back( sk );
        return 1;
        // 2009 1030  : 似乎擺在另一個序中在移除 比較有意義 因為
        // 1.如果 同一個 id 一個 再被踢除中  一個在重新加入中 應該 也不會是 同一個socket 所以 用 同一個socket的utlock鎖住 並沒有意義
        // 2.在Ondisconnet 中 和 update 整個鎖住 應該安全性也會較高
        // 所以該段無使用
        /*
        OnDisconnect( sk );
        {
        #if defined(_WIN32IOCOMPLETIONPORT)  // Windows platforms
            TS_Lock lock( &tcpserver_ut_lock_ );
            // 這個必須後作!!
            SocketOV* cov = ( SocketOV* )( sk->recv_buffer_ );

            if ( cov->PostAccept( tcpsocket_.GetFD() ) != 1 )
            {
                // 立刻回收沒成功，丟到LIST 等一下再回收吧
                //m_ListeningList.push_back( sk );
                //ntchar_t buff[256];
                //_stprintf( buff, "Kickout:: accept socket failure total count = %d, push on listen list and accept later\n", m_AcceptCnt );
                Pf_WARNING( "TCPServer::Kickout: accept socket failure total count = %d, push on listen list and accept later\n", m_AcceptCnt );
            }
            else
            {
                m_AcceptCnt++;
            }

        #endif
        }
        return 1;
        */
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool TCPServer::IsActive( TCPSocket* sk )
    {
        int div = sk->GetUID() % max_socket_banks_div_;
        TS_Lock poollock( &sockets_lock_[ div ] );
        socket_bank_t::iterator it = sockets_[ 1 ][ div ].find( sk->GetUID() );
        // 已經被放入可再利用了
        if( it != sockets_[ 1 ][ div ].end() )
        {
            return false;
        }
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC: 因為有時候 在關閉後 PostAccept 無法立即回收該SOCKET
    //   所已先將牠放在一個VECTOR中 然後再持續試試看可不可以回收
    //   如果成功回收，就將它重回收VECTOR中移除，否則等待下一次的回收
    //=============================================================================
//    void TCPServer::Listening( void* data )
//    {
//#ifdef _WINDOWS
//        ntchar_t errmessage[256];
//        int  failurecount = 0;
//        net::TCPServer* p_server = ( net::TCPServer* )( data );
//
//        while ( p_server->IsActive() )
//        {
//            {
//                TS_Lock( &p_server->tcpserver_ut_lock_ );
//
//                if ( !p_server->m_ListeningList.empty() )
//                {
//                    net::TCPSocket* socket = p_server->m_ListeningList.front();
//                    SocketOV* cov = ( SocketOV* )( socket->recv_buffer_ );
//
//                    if ( cov->PostAccept( p_server->tcpsocket_.GetFD() ) == 1 )
//                    {
//                        p_server->m_AcceptCnt++;
//                        p_server->m_ListeningList.erase( p_server->m_ListeningList.begin() );
//                        failurecount = 0;
//                        printf( "recycling one socket successful\n" );
//                        Pf_INFO( "TCPServer::Listening: Listening:: accept socket size = %d\n", p_server->m_AcceptCnt );
//                    }
//                    else
//                    {
//                        failurecount++;
//                        _stprintf( errmessage, "failure create socket %d size=%d error %d\n", failurecount,
//                                 p_server->m_ListeningList.size(), net::Socket::GetLastError() );
//                        Pf_ERROR( "TCPServer::Listening: Create socket faulare\n" );
//                    }
//                }
//            }
//            pfstd::MSSleep( 1000 );
//        }
//
//#endif
//        return;
//    }

    //=============================================================================
    // NAME: Kicking
    // DESC: 為了保有多續的安全性  所以這樣做
    //=============================================================================
//    void TCPServer::Kicking( void* data )
//    {
//        TCPServer* p_server = ( TCPServer* )data;
//
//        while ( p_server->IsActive() )
//        {
//            std::vector<TCPSocket*> safekicklist;
//            {
//                TS_Lock lock( &p_server->tcpserver_ut_lock_ );
//                uint_t kicklistsize = p_server->m_KickoutSocketCache.size();
//
//                for ( uint_t i = 0; i < kicklistsize; i++ )
//                {
//                    safekicklist.push_back( p_server->m_KickoutSocketCache[i] );
//                }
//
//                p_server->m_KickoutSocketCache.clear();
//            }
//
//            for ( uint_t j = 0; j < safekicklist.size(); j++ )
//            {
//                net::TCPSocket* sk = safekicklist[j];
//                //TS_Lock lock(&sk->m_TcpPollLock);
//                TS_Lock lock( &sk->m_utLock );
//                p_server->OnDisconnect( sk );
//#if defined(_WIN32IOCOMPLETIONPORT)  // Windows platforms
//                // 這個必須後作!!
//                SocketOV* cov = ( SocketOV* )( sk->recv_buffer_ );
//
//                if ( cov->PostAccept( p_server->tcpsocket_.GetFD() ) != 1 )
//                {
//                    TS_Lock lock( &p_server->tcpserver_ut_lock_ );
//                    p_server->m_ListeningList.push_back( sk );
//                }
//#else
//#endif
//            }
//
//            safekicklist.clear();
//            pfstd::MSSleep( 1000 );
//        }
//    }

    /*
    舊式的做法 現在用不到 乾脆MARK起來
    //extern int tselect(int maxp1, fd_set* re, fd_set* we, fd_set* ee);
    //=============================================================================
    // NAME: Idle
    // DESC: server listening loop
    //=============================================================================
    void __cdecl TCPServer::Select(void* data) //server wait for bind in win32 system
    {
     TCPServer* service = (TCPServer*) data;
     service->OnInitiation();
     //#ifndef _WINDOWS

     //Psocket sockfd, client_sockfd;
     //Psocket client_sockfd;

     //int server_len;
     //struct sockaddr_in server_address;
     //int client_len;
     //struct sockaddr_in client_address;
     //int result;
     fd_set readfds, testfds;

     //clear socket table

     //m_Client.clear();

     //Create and name a socket for the server.

     //if(tcp_server(NULL, port, domain, streamtype, protocol) < 0)
     // return -1;

     FD_ZERO(&readfds);
     FD_SET(service->socket_fd_, &readfds);

     //Now wait for clients and requests.
     //Since we have passed a null pointer as the timeout parameter, no timeout will occur.
     //The program will exit and report an error if select returns a value of less than 1.

     while (1)
     {
      //int   fd;
      int nread = 0;

      testfds = readfds;

      //printf("server waiting........\n");
      select(FD_SETSIZE, &testfds, (fd_set *)0,
       (fd_set *)0, (struct timeval *) 0);


    //    if (tselect(FD_SETSIZE,
    //         &testfds,
    //         (fd_set *) 0,
    //         (fd_set *) 0) < 1)
    //    {
    //     net::Socket::PrintError("server polling failure");
    //     //exit(1);
    //    }

      //Once we know we've got activity,
      //we find which descriptor it'sk on by checking each in turn using FD_ISSET.

      for (int fd = 0; fd < FD_SETSIZE; fd++)
      {
       if (FD_ISSET(fd, &testfds))
       {
        //If the activity is on server sockfd, it must be a request for a new connection
        //and we add the associated client_sockfd to the descriptor set.

        if (fd == service->socket_fd_)
        {
         //client_len = sizeof(client_address);
         net::TCPSocket* client_sockfd = new net::TCPSocket;
         service->Accept(*client_sockfd);
         ////service->OnConnect();
         //service->AddServiceSocket(client_sockfd);

         FD_SET(*client_sockfd, &readfds);

         //client_add(client_sockfd, client_address);
         ////////printf("adding client on fd %d from  : %d\n", (*client_sockfd),
         ///////client_sockfd->GetPort());
         ////////printf("client size %d\n", service->g_socket_cache.size());
        }

        //If it isn't the server, it must be client activity.
        //If close is received, the client has gone away and we remove it from the descriptor set.
        //Otherwise, we 'serve' the client as in the previous examples.
        else
        {
    #ifdef _LINUX
         ioctl(fd, FIONREAD, &nread);
    #endif
    #ifdef _WINDOWS
         ioctlsocket(fd, FIONREAD, (u_long*) &nread);
    #endif

         if (nread == 0)
         {
          //client_del(fd);
          //close(fd);
          /////service->OnDisconnect();

          FD_CLR(fd, &readfds);
          ////////printf("removing client on fd %d\n", fd);
          //printf("client size %d\n", client_size());
         }
         else
         {
          //ntchar_t* recv_buffer;
          //service->OnReceive()
          ntchar_t buffer[100];
          int getlen = recv(fd, buffer, 100, 0);
          //int getlen = recvpk(fd, &recv_buffer, 0);
          /////printf("get data  : %sk\n", recv_buffer);
          //////printf("get length  : %d\n", getlen);
         }
        }
       }
      }
     }
     return;
    }
    */


};


