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

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-thread.h>
#include <pf-debug.h>
#include <pf-string.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "net-peer.h"
#include "net-rudp_socket.h"


namespace net
{
    /////////////////////////////////////////////////////////////////////////////////////////
    //
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    UDPPeer::UDPPeer()
    {
        v_udpsocket_ =  new UDPSocket;

    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    UDPPeer::~UDPPeer()
    {
        SAFE_DELETE( v_udpsocket_ );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t UDPPeer::GetFD()
    {
        return v_udpsocket_->GetFD();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t UDPPeer::Create( int domain )
    {
        return v_udpsocket_->Create( domain );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPPeer::Bind( ushort_t hport )
    {
        if ( v_udpsocket_->GetFD() == 0 )
        {
            //Create();
            Pf_ERROR( "UDPPeer::StartUp:　create socket first...failure!\n" );
            return -1;
        }
        
        addrs_t address;
        ntchar_t buf[ 50 ];
        net::Socket::SetAddress( (const ntchar_t*)NULL, ( const ntchar_t* )itoa( hport, buf, 10 ), (sockaddr*)&address, 0, SOCK_DGRAM );
        
        if ( v_udpsocket_->Bind( ( struct sockaddr* ) &address, sizeof( address ) ) )
        {
            v_udpsocket_->SetErrorCode( 1, errno, "bind failed" );
            //SetOPState(OP_BIND_FAILURE);
            return -1;
        }
        
        //pfkrl::Thread::BeginProcess( Polling, this, _T( "UDPPeer::Bind" ) );
        return 1;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPPeer::Close()
    {
        return v_udpsocket_->Close();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool UDPPeer::IsLive( void )
    {
        return v_udpsocket_->IsLive();
    }
   
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPPeer::SendnTo( const ubyte_t* bufptr, size32_t buflen, const sockaddr* srcaddr, int addrlen, uint_t flag )
    {
        return v_udpsocket_->SendTo( bufptr, buflen, srcaddr, addrlen, flag );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPPeer::SendnTo( const ubyte_t* bufptr, size32_t buflen, const ipa_t to_addr, ushort_t to_port, uint_t flag )
    {
        sockaddr_in ser;
        ser.sin_family = AF_INET;
        ser.sin_port = htons( to_port );
        ser.sin_addr.s_addr = inet_addr( to_addr.c_str() );
        return v_udpsocket_->SendTo( bufptr, buflen, ( sockaddr* ) &ser, sizeof( ser ), flag );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPPeer::RecvnFrom( ubyte_t* bufptr, size32_t buflen, sockaddr* srcaddr, int& addrlen, uint_t flag, ustimen_t timeOut )
    {
        return v_udpsocket_->RecvFrom( bufptr, buflen, srcaddr, addrlen, flag );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPPeer::RecvnFrom( ubyte_t* bufptr, size32_t buflen, ipa_t& form_addr, ushort_t& form_port, uint_t flag, ustimen_t timeOut )
    {
        sockaddr_storage cli;
        int nRecLen = sizeof( cli );
        int ret = v_udpsocket_->RecvFrom( bufptr, buflen, ( sockaddr* ) &cli, nRecLen, flag );
        form_addr = net::Socket::GetSockAddrIPA( ( sockaddr* ) &cli );
        form_port = v_udpsocket_->GetSockAddrPort( ( sockaddr* ) &cli );
        return ret;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int UDPPeer::SetMuticast( ipa_t ipa, int timeout, int f )
    {
        return v_udpsocket_->SetMuticast( ipa, timeout, f );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    void UDPPeer::Polling( void* data )
    {
        net::UDPPeer* pRUDPPeer = ( net::UDPPeer* )data;
        fd_set rfd;
        //逾時
        timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        
        while ( pRUDPPeer->IsLive() )
        {
            FD_ZERO( &rfd );         // 清空
            // 開始使用select
            FD_SET( pRUDPPeer->GetFD(), &rfd ); // 把socka放入要測試的描述符集中
            int nRet = select( 0, &rfd, NULL, NULL, &timeout );
            
            if ( nRet == SOCKET_ERROR )
            {
                //printf( "select()/n" );
                return;
            }
            else if ( nRet == 0 ) // 超時
            {
                //printf( "timeout/n" );
                //closesocket( socka );
                continue;
            }
            else                  // 檢測到到有套接口可讀
            {
                if ( FD_ISSET( pRUDPPeer->GetFD(), &rfd ) )
                {
                    pRUDPPeer->OnReceive( pRUDPPeer );
                    //int nRecEcho = recvfrom( socka, recv_buf, sizeof( recv_buf ), 0, ( sockaddr* ) &cli, &nRecLen );
                    //if ( nRecEcho == INVALID_SOCKET )
                    //{
                    //    printf( "recvfrom()/n" );
                    //    break;
                    //}
                    //printf( "data to port 11111: %s/n", recv_buf );
                }
            }
        }
    }
    */
    
    /////////////////////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    RUDPPeer::RUDPPeer()
    {
        v_rudpsocket_ = new RUDPSocket;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    RUDPPeer::~RUDPPeer()
    {
        SAFE_DELETE( v_rudpsocket_ );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t RUDPPeer::GetFD()
    {
        return v_rudpsocket_->GetFD();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t RUDPPeer::Create( int domain )
    {
        return v_rudpsocket_->Create( domain );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //int RUDPPeer::Bind( ipa_t hname, ushort_t hport )
    int RUDPPeer::Bind( ushort_t hport )
    {
        if ( v_rudpsocket_->GetFD() == 0 )
        {
            //Create();
            Pf_ERROR( "RUDPPeer::StartUp:　create socket first...failure!\n" );
            return -1;
        }
        
        //ntchar_t buf[ 256 ];
        //std::vector<sockaddr_storage> addrinfo_list;
        //net::Socket::GetAddrNameInfo( addrinfo_list, hname.c_str(), itoa( hport, buf, 10 ), AF_UNSPEC, SOCK_STREAM );

        struct sockaddr_in address;
        //ntchar_t buf[50];
        //net::Socket::SetAddress( NULL, itoa( hport, buf, 10 ), &address, "udp" );

        address.sin_family = AF_INET;
        address.sin_port = htons( hport );
        /* Use any local address */
        address.sin_addr.s_addr = htonl( INADDR_ANY );
        /* Socket will be bind to any random port if port number is 0 */
        //if ( ( bind( rsock->sockfd, ( struct sockaddr* )&address, sizeof( address ) ) ) < 0 )
        //{

        
        //if ( v_rudpsocket_->Bind( ( struct sockaddr* ) & ( addrinfo_list[ 0 ] ), sizeof( sockaddr ) ) == SOCKET_ERROR )
        if ( v_rudpsocket_->Bind( ( struct sockaddr* ) &address, sizeof( address ) ) == SOCKET_ERROR )
        {
            return -1;
        }
        
        //pfkrl::Thread::BeginProcess( Polling, this, _T("UDPPeer::Bind") );
        return 1;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    ushort_t RUDPPeer::GetPort()
    {
        sockaddr_storage addrs;
        int addrlen = sizeof( sockaddr );
        v_rudpsocket_->GetAddress( (sockaddr*)&addrs, addrlen );
        return net::Socket::GetSockAddrPort( ( sockaddr* )&addrs );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPPeer::Close( )
    {
        return v_rudpsocket_->Close();
    }    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool RUDPPeer::IsLive( void )
    {
        return v_rudpsocket_->IsLive();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPPeer::Waiting( ustimen_t timeOut )
    {
        return v_rudpsocket_->Waiting( timeOut );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPPeer::GetReadLength()
    {
        return v_rudpsocket_->GetReadLength();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPPeer::SendnTo( const ubyte_t* bufptr, size32_t buflen, const sockaddr* srcaddr, int addrlen, uint_t flag )
    {
        return v_rudpsocket_->SendTo( bufptr, buflen, srcaddr, addrlen, flag );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPPeer::SendnTo( const ubyte_t* bufptr, size32_t buflen, const ipa_t to_addr, ushort_t to_port, uint_t flag )
    {
        sockaddr_in ser;
        ser.sin_family = AF_INET;
        ser.sin_port = htons( to_port );
        ser.sin_addr.s_addr = inet_addr( to_addr.c_str() );
        return v_rudpsocket_->SendTo( bufptr, buflen, ( sockaddr* )&ser, sizeof( ser ), flag );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPPeer::RecvnFrom( ubyte_t* bufptr, size32_t buflen, sockaddr* srcaddr, int& addrlen, uint_t flag, int timeOut )
    {
        return v_rudpsocket_->RecvFrom( bufptr, buflen, srcaddr, addrlen, flag, timeOut );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPPeer::RecvnFrom( ubyte_t* bufptr, size32_t buflen, ipa_t& form_addr, ushort_t& form_port, uint_t flag, int timeOut )
    {
        sockaddr_storage cli;
        int nRecLen = sizeof( cli );
        int ret = v_rudpsocket_->RecvFrom( bufptr, buflen, ( sockaddr* ) &cli, nRecLen, flag, timeOut );
        form_addr = net::Socket::GetSockAddrIPA( ( sockaddr* ) &cli );
        form_port = net::Socket::GetSockAddrPort( ( sockaddr* ) &cli );
        return ret;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void RUDPPeer::Ping( _IN const sockaddr* srcaddr, _IN int addrlen )
    {

    }
    
    //=============================================================================
    // NAME:
    // DESC: 2018-01-03 因為會有收到重送封包的問題 所以 乾脆拿掉
    //=============================================================================
    /*
    void RUDPPeer::Polling( void* data )
    {
        net::RUDPPeer* pRUDPPeer = ( net::RUDPPeer* )data;
        fd_set rfd;
        unsigned long nread = 0;
        //逾時
        //timeval timeout;
        //timeout.tv_sec = 3;
        //timeout.tv_usec = 0;

        // wait until either socket has data ready to be recv()d( timeout 3.5 secs )
        timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 500000;

        while ( pRUDPPeer->IsLive() )
        {
            FD_ZERO( &rfd );         // 清空
            // 開始使用select
            FD_SET( pRUDPPeer->GetFD(), &rfd ); // 把socka放入要測試的描述符集中
            int nRet = select( pRUDPPeer->GetFD() + 1, &rfd, NULL, NULL, &timeout );
            
            if ( nRet == SOCKET_ERROR )
            {
                //printf( "select()/n" );
                return;
            }
            else if ( nRet == 0 ) // 超時
            {
                //printf( "timeout/n" );
                //closesocket( socka );
                continue;
            }
            else                  // 檢測到到有套接口可讀
            {
                int recvret = ioctl( pRUDPPeer->GetFD(), FIONREAD, &nread );
                if ( FD_ISSET( pRUDPPeer->GetFD(), &rfd ) )
                {
                    pRUDPPeer->OnReceive( pRUDPPeer );
                    //int nRecEcho = recvfrom( socka, recv_buf, sizeof( recv_buf ), 0, ( sockaddr* ) &cli, &nRecLen );
                    //if ( nRecEcho == INVALID_SOCKET )
                    //{
                    //    printf( "recvfrom()/n" );
                    //    break;
                    //}
                    //printf( "data to port 11111: %s/n", recv_buf );
                }
            }
        }
    }
    */
}