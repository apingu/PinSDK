//Client.cpp network client function class
//
//
//
//
//
//
//
//                                                         Apingu
//                                                               Sat Mar 16 2002

//01. == C 系統文件/C++ 系統文件
#include <map>
#include <set>
#if defined(_WINDOWS)
#include "iocp-def.h"
#else
#include <stdlib.h>
#include "netinet/tcp.h"
#include <pf-string.h>         // 為了要有 itoa
#endif

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-thread.h>
#include <pf-stdlib.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "net-client.h"


namespace net
{
//#define _pTCPLClientSingletonLock() (&g_create_client_lock)
    //#define _pTCPLClientSingletonProc() (g_BeginClientProcess)
    
    typedef std::map<socket_t, net::TCPClient*>     net_clientvec_t;
    typedef std::set<socket_t>                      net_socketvec_t;
    
    namespace
    {
        T_Lock                              g_create_client_lock;  //
        net_clientvec_t                     g_create_client_cntr;  //Container
        pfkrl::Thread::Handle               g_client_thread_hid = 0;
        
        class Pooling
        {
            static T_Lock                              pooling_socket_lock_;  //
            static net_socketvec_t                     pooling_socket_cntr_; //Container
            
        public:
        
            static void InsertPooling( net::TCPClient* c )
            {
                TS_Lock lock( &pooling_socket_lock_ );
                
                if ( c->GetFD() != 0 )
                {
                    pooling_socket_cntr_.insert( net_socketvec_t::value_type( c->GetFD() ) );
                }
            }
            
            static void RemovePooling( net::TCPClient* c )
            {
                TS_Lock lock( &pooling_socket_lock_ );
                
                if ( c->GetFD() != 0 )
                {
                    pooling_socket_cntr_.erase( c->GetFD() );
                }
            }
            
            static int UpdatePolling( fd_set& readfds, std::vector<socket_t>& clientfds )
            {
                //std::vector<socket_t> clientfds;
                int maxfd = 0;
                FD_ZERO( &readfds );
                {
                    TS_Lock lock( &pooling_socket_lock_ );
                    net_socketvec_t::iterator iter = pooling_socket_cntr_.begin();
                    
                    while ( iter != pooling_socket_cntr_.end() )
                    {
                        FD_SET( *iter, &readfds );
                        clientfds.push_back( *iter );
                        maxfd = __max( *iter, maxfd );
                        iter++;
                    }
                }
                return maxfd;
            }
        };
        
        net::TCPClient* FindClient( socket_t f )
        {
            TS_Lock lock( &g_create_client_lock );
            net_clientvec_t::iterator iter = g_create_client_cntr.find( f );
            
            if ( iter != g_create_client_cntr.end() )
            {
                return iter->second;
            }
            
            return NULL;
        }

        
        T_Lock          Pooling::pooling_socket_lock_;//
        net_socketvec_t Pooling::pooling_socket_cntr_;
    }
    
#define _pTCPLClientSingletonList() (&g_create_client_cntr)
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    TCPClient::TCPClient( _IN net::TCPSocket* tcpsocket )
    //:m_hListen(NULL)
    {
        if( tcpsocket == NULL )
        {
            tcpsocket = new net::TCPSocket;
        }
        tcp_socket_v_ = tcpsocket;
        flag_   = 0;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    TCPClient::~TCPClient()
    {
        Close();
        SAFE_DELETE( tcp_socket_v_ );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t TCPClient::GetFD()
    {
        return tcp_socket_v_->GetFD();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t TCPClient::Create( _IN int domain )
    {
        //{
        //    TS_Lock lock( &g_lClinetCount );
        //    g_nClientCount++;
        //
        //    if ( !g_bClientInit )
        //    {
        //        g_bClientInit = true;
        //        //Pooling::Init(Receiving);
        //    }
        //
        //    if ( g_nClientCount == 1 )
        //    {
        //        g_bRecving = true;
        //        g_client_thread_hid = pfkrl::Thread::BeginProcess( Receiving, _pTCPLClientSingletonList() ) ;
        //    }
        //}
        TS_Lock lock( &tcpclient_ut_lock_ );
#ifdef _WIN32IOCOMPLETIONPORT
        SocketOV* rov = ( SocketOV* )tcp_socket_v_->recv_buffer_;
        SAFE_DELETE( rov );
        tcp_socket_v_->recv_buffer_ = NULL;
        //SocketOV* sov = ( SocketOV* )tcp_socket_v_->m_Sendbuf;
        //SAFE_DELETE( sov );
        //tcp_socket_v_->m_Sendbuf = NULL;
#endif
        socket_t fd = tcp_socket_v_->Create( domain );
        
        if ( fd != NULL_SOCKET )
        {
            /*
            // turn on keep-alive monitoring
            ulong32_t lflag = 1;
            setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&lflag, sizeof(lflag));
            
            // 預設socket 的屬性是
            // 設定 未送完的資料 在 10的timeout 時間內送完 再close
            struct linger li = { 1, 10 };
            setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&li, sizeof(li));
            */
            //set socket no delay
            ulong32_t flag = 1;
            setsockopt( fd,  IPPROTO_TCP, TCP_NODELAY, ( const char* )&flag, sizeof( flag ) ); // no delay
            // no block
            ulong32_t block = 0;
            tcp_socket_v_->Ioctl( FIONBIO, &block );
        }
        
        {
            TS_Lock lock( &g_create_client_lock );
            if( ( _pTCPLClientSingletonList()->empty() ) && ( g_client_thread_hid == 0 ) )
            {
                 g_client_thread_hid = pfkrl::Thread::BeginProcess( Polling, NULL, _T("TCPClient::Create") );
            }
            _pTCPLClientSingletonList()->insert( net_clientvec_t::iterator::value_type( fd, this ) );
        }
        
        return fd;
    }
    
    //=============================================================================
    // NAME: Bind
    // DESC:
    //=============================================================================
    //void TCPClient::Bind( _IN ipa_t lname, _IN ushort_t lport )
    //{
    //    TS_Lock lock( &tcpclient_ut_lock_ );
    //    
    //    if ( tcp_socket_v_->GetFD() == 0 )
    //    {
    //        //Create();
    //        Pf_ERROR( "TCPClient::Bind: socket can't be 0!\n" );
    //        return;
    //    }
    //    
    //    struct sockaddr_in locala;
    //    
    //    char fportbuf[ 128 ];
    //    
    //    itoa( lport, fportbuf, 10 );
    //    
    //    net::Socket::SetAddress( lname.c_str(), fportbuf, &locala, _T( "tcp" ) );
    //    
    //    tcp_socket_v_->Bind( ( struct sockaddr* ) &locala, sizeof( locala ) );
    //}
    
    //=============================================================================
    // NAME: Connect
    // DESC:
    //=============================================================================
    bool TCPClient::Connect( _IN ipa_t hname, _IN ushort_t hport )
    {
        int ret = 0;
        {
            TS_Lock lock( &tcpclient_ut_lock_ );
            
            if ( tcp_socket_v_->GetFD() == 0 )
            {
                //Create();
                Pf_ERROR( "TCPClient::Connect:  socket can't be 0!\n" );
                return false;
            }
            
            ntchar_t buf[255];
            //struct sockaddr_in peer;
            //net::Socket::SetAddress( hname.c_str(), buf, &peer, _T("tcp") );
            std::vector<sockaddr_storage> addrinfo_list;
            net::Socket::GetAddrNameInfo( addrinfo_list, hname.c_str(), itoa( hport, buf, 10 ), AF_UNSPEC, SOCK_STREAM );
            //ntchar_t dbgmsg[256]; _stprintf(dbgmsg, "Begin connect to %s %d..\n", hname, hport); OutputDebugString(dbgmsg);
            Pf_INFO( "Begin connect to %s %d..\n", hname.c_str(), hport );
            
            //if ( ( ret = connect( tcp_socket_v_->GetFD(), ( struct sockaddr* ) &peer, sizeof( peer ) ) ) == 0 )
            if ( addrinfo_list.empty() )
            {
                Pf_ERROR( "TCPClient::Connect: connect servrer failure! server not find\n" );
                net::Socket::SetErrorCode( 0, 0, "connect failed" );
                tcp_socket_v_->Close();
                return false;
            }
            
            if ( ( ret = connect( tcp_socket_v_->GetFD(), ( struct sockaddr* ) & ( addrinfo_list[0] ), sizeof( sockaddr ) ) ) == 0 )
            {
                //ntchar_t dbgmsg[256]; _stprintf(dbgmsg, "connect to %s %d successful\n", hname, hport); OutputDebugString(dbgmsg);
                Pf_INFO( "connect to %s %d successful\n", hname.c_str(), hport );
                //SET_BIT_FLAG( flag_, net::s_fConnected );
                // 建立的時候加入 離開的時候移除 以免會有 沒有呼叫 close 便離開成是 照成 receive無法離開
                // 而 tcpclinet 等步道 receive 結束 而無法解構
                //Pooling::InsertClientObject( this );
                SetBlock( false );
            }
            else
            {
                //ntchar_t buff[256];
                //_stprintf(buff, "connect servrer failure! error no = %d\n", net::Socket::GetLastError());
                Pf_ERROR( "TCPClient::Connect: connect servrer failure! error no = %d\n", net::Socket::GetLastError() );
                net::Socket::SetErrorCode( 0, 0, "connect failed" );
                tcp_socket_v_->Close();
                return false;
            }
        }
        return true;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int TCPClient::Close( void )
    {
        SetBlock( true );
        socket_t fd = tcp_socket_v_->GetFD();
        {
            TS_Lock lock( &g_create_client_lock );
            _pTCPLClientSingletonList()->erase( fd );
        }
        int ret = 0;
        {
            TS_Lock lock( &tcpclient_ut_lock_ );
            ret = tcp_socket_v_->Close();
        }
        return ret;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool TCPClient::IsConnected( void )
    {
        TS_Lock lock( &tcpclient_ut_lock_ );
        return ( tcp_socket_v_->GetFD() == 0 ) ? false : true;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void TCPClient::SetBlock( _IN bool f )
    {
        //TS_Lock lock( &tcpclient_ut_lock_ );
        if ( !f )
        {
            //SET_BIT_FLAG( flag_, net::s_fBlock );
            Pooling::InsertPooling( this );
        }
        else
        {
            //CANCEL_BIT_FLAG( flag_, net::s_fBlock );
            Pooling::RemovePooling( this );
        }
        
        //m_bBlock = f;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void TCPClient::GetLocalIPAddress( _OUT std::vector<ipa_t>& ip4list, _OUT std::vector<ipa_t>& ip6list )
    //{
    //    ntchar_t hostname[ _MAX_HOST_NAME ];
    //    Socket::GetLocalHostname( hostname, _MAX_HOST_NAME );
    //
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
    // NAME:
    // DESC:
    //=============================================================================
    void TCPClient::Polling( _IN void* param )
    {
        int ret = 0;

        unsigned long nread = 0;

        fd_set readfds;//, testfds;
        
        //while ( g_bRecving )
        while ( 1 )
        {
            {
                TS_Lock lock( &g_create_client_lock );
                if ( _pTCPLClientSingletonList()->empty() )
                {
                    g_client_thread_hid = 0;
                    break;
                }
            }

            timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 2000;
            // close remove clinet if it exist
            /*
            for(int i = 0; i < closeclient.size(); i++)
            {
                socket_t fd = closeclient[i];
                TS_Lock lock(_pTCPLClientSingletonLock());
                net_clientvec_t::iterator iter = _pTCPLClientSingletonList()->find(fd);
                if(iter!=_pTCPLClientSingletonList()->end())
                {
                    iter->second->Close();
                    //FD_CLR(fd, &readfds);
                }
            }
            closeclient.clear();
            */
            // get which socket was weakup
            std::vector<socket_t> clientfds;
            int maxfd = Pooling::UpdatePolling( readfds, clientfds );
            
            if ( clientfds.empty() )
            {
                pfstd::MSSleep( 100 );
                continue;
            }
            
            //int maxfd = FD_SETSIZE;
            if ( ( ret = select( maxfd + 1, &readfds, ( fd_set* )0, ( fd_set* )0, ( struct timeval* )&timeout ) ) == SOCKET_ERROR )
            {
                int lasterrorno = net::Socket::GetLastError();
                net::Socket::PrintErrorCode( lasterrorno );
                Pf_ERROR( "TCPClient::Receiving: select socket failure %d\n", lasterrorno );
                continue;
            }
            
            // timeout
            if ( ret == 0 )
            {
                //pfstd::MSSleep(10);
                continue;
            }
            
            //for ( int i = 0; i < readfds.fd_count; i++ )
            //for ( int fd = 0; fd < maxfd; fd++ )
            
            for ( size_t ci = 0; ci < clientfds.size(); ci++ )
            {
                socket_t fd = clientfds[ ci ];
                
                if ( FD_ISSET( fd, &readfds ) )
                {
                    int recvret = ioctl( fd, FIONREAD, &nread );
                    
                    if ( recvret >= 0 )
                    {
                        if ( nread == 0 )
                        {
                            net::TCPClient* client = FindClient( fd );
                            if ( client )
                            {
                                //client->OnReceive( &client->tcp_socket_v_ );
                                FD_CLR( fd, &readfds );
                                client->Close();
                            }
                        }
                        else
                        {
                            while ( nread > 0 )
                            {
                                //TS_Lock lock( _pTCPLClientSingletonLock() );
                                net::TCPClient* client = FindClient( fd );
                                if ( client )
                                {
                                    //client->OnReceive( &client->tcp_socket_v_ );
                                    client->OnReceive();
                                }
                                recvret = ioctl( fd, FIONREAD, &nread );
                            }                            
                        }
                        //pfstd::MSSleep( 10 );                        
                        continue;
                    }
                    else
                    {
                        int lasterrorno = net::Socket::GetLastError();
                        //net::Socket::PrintErrorCode( lasterrorno );
#if defined(_WINDOWS)
                        
                        if ( ( lasterrorno == WAIT_TIMEOUT ) || ( lasterrorno == ERROR_SEM_TIMEOUT ) )
                        {
                            Pf_ERROR( "TCPClient::Receiving: Polling timeout\n" );
                        }
                        
#endif
                        if ( recvret == SOCKET_ERROR )
                        {
                            Pf_ERROR( "TCPClient::Receiving: select ioctl == SOCKET_ERROR: socket error %d recvret %d nread %d\n", lasterrorno, recvret, nread );
                            //berase=true;
                            //bclose=true;
                        }
                        else
                        {
                            if ( nread == 0 )
                            {
                                // 範例這樣做的 回傳0 是錯誤的 lasterrorno==0 代表server 呼叫了 close 這個socket
                                Pf_ERROR( "TCPClient::Receiving: select : server kickout.. socket error %d recvret %d nread %d\n", lasterrorno, recvret, nread );
                            }
                            else if ( nread == SOCKET_ERROR )
                            {
                                Pf_ERROR( "TCPClient::Receiving: select : client disconnection.. socket error %d recvret %d nread %d\n", lasterrorno, recvret, nread );
                                //berase=true;
                                //bclose=true;
                            }
                            else// socket already closed. (Shutdown called)
                            {
                                Pf_ERROR( "TCPClient::Receiving: select : socket exit.. socket error %d recvret %d nread %d\n", lasterrorno, recvret, nread );
                                //berase=true;
                                //bclose=true;
                            }
                        }

                        //pfstd::MSSleep( 10 );
                        continue;
                    }
                }
            }
            
            //client->m_Lock.UnLock();
        }
        
        //_NETEXITCLIENT:
#ifdef _DEBUG
        Pf_INFO( "TCPClient::Receiving: BYE BYE\n" );
        //DebugOutPutf("BYE BYE", "NetClient");
#endif
        return;
    }
};
