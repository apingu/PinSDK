//Socket.h headle file of Socket.cpp
// IOSocket network object
//  for cross platform
//
// Commented(in Doxygen format) by Wood Woo Mar 8, 2005
//
//
//
//
//
//
//
//
//               Apingu
//                  Sat Mar 16 2002
#pragma once

#include "unp.h"
#include <t_lock.h>
#include <vector>
#include <map>

//extern ntchar_t* program_name;

///////////////////////////////////////////////////////////////////////////////
//  socket status
//  IPV4    : sockaddr_in         16    ||    in_addr      4
//  IPV6    : sockaddr_in6        25    ||    in6_addr     16
//  Common1 : sockaddr            16
//  Common2 : sockaddr_storage    128
// http://www.cnblogs.com/tanghuimin0713/p/3425936.html

namespace net
{
    ///////////////////////////////////////////////////////////////////////////////
    //  class pcsocket
    //! Generic Socket interface
    class Socket
    {
    private:
        socket_t                    socket_fd_;
        dword_t                     socket_uid_;
        stimen_t                    stime_last_create_; //! 建立連線時間, 可以作為上線時間計算用

        T_Lock                      socket_ut_lock_;    // for user_key
        std::map< _mstring, void* > user_key_;          // 擺放自訂資料
        
    protected:
    
        //! 初始化
        virtual socket_t        Create( _IN int domain = AF_INET, _IN int streamtype = SOCK_STREAM, _IN int protocol = 0 ); //create socket in win32 system
        virtual int             Close( void );
        
    public:
    
        //! constructor
        Socket();
        //! destructor
        virtual ~Socket();
        virtual const ntchar_t* ObjectName() const { return ( "Socket" ); }
        
        // 等同create
        virtual socket_t&       operator= ( const socket_t& sk );
        //! 轉換成 socket number, 可被一般 socket function 使用
        operator socket_t& ( );
        socket_t                GetFD();
        dword_t                 GetUID();

        // v NULL for remove
        bool                    SetKey( _IN const ntchar_t* k, _IN void* v );
        void*                   GetKey( _IN const ntchar_t* k );
        
        int                     Bind( _IN const sockaddr* addrptr, _IN int addrlen );

        int                     GetAddress( _OUT sockaddr* addrptr, _IN int addrlen );
        //! 呼叫 ioctl()
        int                     Ioctl( _IN long32_t cmd, _INOUT ulong32_t* argp );
        //! 設定連線選項(參考 setsockopt)
        int                     SetOpt( _IN int level, _IN int optname, _IN const void* optval, _IN socklen_t optlen );
        //! 設定連線選項(參考 getsockopt)
        int                     GetOpt( _IN int level, _IN int optname, _OUT void* optval, _INOUT int* optlen );
        //! 呼叫 Create() 的時間
        stimen_t&               GetLastCreateTime( void );
       
        static int              GetAddrNameInfo( _IN const struct sockaddr* sa, _OUT char* host, _IN dword_t hostlen, _OUT char* serv, _IN dword_t servlen, _IN int flags );

        // sockettype = SOCK_xxx
        // family     = AF_UNSPEC(不指定) / AF_INET(IP4) / AF_INET6(IP6)
        // 連線前 取得正確的 連線資訊  connect bind
        // hostname = "www.example.com" or ip  |  service = "http" or "80"
        static void             GetAddrNameInfo( _OUT std::vector<addrs_t>& addrlist, _IN const ntchar_t* hostname, const _IN ntchar_t* service = NULL, _IN int family = AF_UNSPEC, _IN int socktype = SOCK_STREAM );

        // old funciton
        static void             SetAddress( _IN const ntchar_t* ipaddr, _IN const ntchar_t* portnum, _OUT struct sockaddr_in* sap, _IN const ntchar_t* sockettype );
        // new funciton 取代上面的
        static void             SetAddress( _IN const ntchar_t* ipaddr, _IN const ntchar_t* service, _OUT struct sockaddr* sap, _IN int family = AF_UNSPEC, _IN int socktype = SOCK_STREAM );


        //////////////////////////////////////////////////////////////////////////
        //
        /** Get the local IP address. If the socket is closed, it'll return 0. */
        static ntchar_t*        GetLocalHostname( _OUT ntchar_t* buf, _OUT size32_t len );
        // Get Enternet inteface information
        static void             GetEthIFInfo( _OUT std::vector<EthIF>& ethif_list );
        //macn_t                  GetMACAddress( void );
        //static macn_t           GetLocalMACAddress( _IN uint_t ethif = 0 );
        static ntchar_t*        GetLocalPortService( _IN ushort_t port, _IN const ntchar_t* proto );
                
        //////////////////////////////////////////////////////////////////////////
        //
        static sockaddr_in*     GetSockAddrIN4( _IN const sockaddr* addr );
        static sockaddr_in6*    GetSockAddrIN6( _IN const sockaddr* addr );
        // if family = AF_INT return a4 string, AF_INT6 return a6 string
        static ipa_t            GetSockAddrIPA( _IN const sockaddr* addr );
        //
        static ipn4_t           GetSockAddrIPN4( _IN const sockaddr* addr );
        static ipn6_t           GetSockAddrIPN6( _IN const sockaddr* addr );
        static ushort_t         GetSockAddrPort( _IN const sockaddr* addr );       

        //////////////////////////////////////////////////////////////////////////
        //
        static ipa_t            IPN6ToA4( _IN ipn6_t ipn );
        static ipa_t            IPN6ToA6( _IN ipn6_t ipn );
        static ipn4_t           IPN6ToN4( _IN ipn6_t ipn );

        static ipa_t            IPN4ToA4( _IN ipn4_t ipn );
        static ipn6_t           IPN4ToN6( _IN ipn4_t ipn );

        static ipa_t            IPA4ToA6( _IN ipa_t ipa );
        static ipa_t            IPA6ToA4( _IN ipa_t ipa );
        static ipn4_t           IPA4ToN4( _IN ipa_t ipa );
        static ipn6_t           IPA6ToN6( _IN ipa_t ipa );
        
        static macn_t           MACAToN( _IN maca_t maca );
        static maca_t           MACNToA( _IN macn_t& macn );
        // block socket
        bool                    SetBlock( _IN bool isBlk );

        void                    SetKeepAlive( _IN bool isKeep );

        bool                    IsLive( void );

        // 現在 socket 收到的資料長度
        int                     GetReadLength();

        //! 取得錯誤碼(errno 或是 WSAGetLastError())
        static int              GetLastError( void );
        //! 印出錯誤狀態, 如果 status !=0 則結束
        static void             SetErrorCode( _IN int status, _IN int err, _IN const ntchar_t* format, ... );
        //! 由錯誤碼印出錯誤訊息
        static void             PrintErrorCode( _IN int code );
        static int              PrintError( _IN const ntchar_t* errstr );
    };
    
    
    /////////////////////////////////////////////////////////////////////////////////////////
    // tcp protocol
    //! TCP 連線用 socket 介面
    class TCPSocket: public Socket
    {
    private:
        T_Lock                  tcpsocket_ut_lock_;
        stimen_t                stime_last_recv_;   //!
        stimen_t                stime_last_send_;   //!
        
    public:
    
        //! use in windows iocp platform
        void*                   recv_buffer_;
        
    public:
    
        TCPSocket();
        virtual ~TCPSocket();
        
        virtual const ntchar_t* ObjectName() const { return ( "TCPSocket" ); }
        
        virtual socket_t        Create( _IN int domain = AF_INET );
        //! 結束本 socket
        virtual int             Close( void );

        /** Get the remote IP address and port*/
        /** Get the sockaddr_in and port*/
        sockaddr*               GetPeerAddr( _OUT addrs_t& addr );        
       
        //! 設定內部緩衝區. 對 Server 來說是SO_RCVBUF, 對 Client 是SO_ENDBUF
        void                    SetTCPBufSize( _IN int buf_type, _IN int size = 32168 );
        //! 設定內部緩衝區. 對 Server 來說是SO_RCVBUF, 對 Client 是SO_ENDBUF
        void                    GetTCPBufSize( _IN int buf_type, _OUT int& size );
        int                     Ioctl( _IN long32_t cmd, _INOUT ulong32_t* argp );
        stimen_t                GetLastSendTime();
        stimen_t                GetLastRecvTime();

        int                     GetReadLength();

        //! 發送訊息
        virtual int             Send( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 );
        //! 接收訊息
        virtual int             Recv( _OUT ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 );
        //! 發送緊急(OOB)訊息
        virtual int             Sendn( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 );
        //! 接收緊急(OOB)訊息
        virtual int             Recvn( _OUT ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 );
        //! 發送訊息(不同資料區段)
        virtual int             Sendb( _IN const Databuf* bps, _IN size32_t lens, _IN uint_t flag = 0 );
        //! 接收訊息(不同資料區段)
        virtual int             Recvb( _OUT Databuf* bps, _IN size32_t lens, _IN uint_t flag = 0 );
        
    protected:
    
        //! 讀取資料
        int                     Readvrec( _OUT ubyte_t* bufptr, _IN size32_t buflen );
        //! 讀取一行資料(文字模式)
        int                     Readline( _OUT ubyte_t* bufptr, _IN size32_t buflen );
        
    };
    
    
    /////////////////////////////////////////////////////////////////////////////////////////
    // udp protocol
    class UDPSocket: public Socket
    {
        T_Lock                  udpsocket_ut_lock_;
        
    public:
    
        UDPSocket();
        ~UDPSocket();
        
        virtual const ntchar_t* ObjectName() const { return ( "UDPSocket" ); }
        
        socket_t                Create( _IN int domain = AF_INET );
        //! 結束本 socket
        virtual int             Close( void );
        
        bool                    IsLive( void );
        
        virtual int             SendTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN uint_t flag = 0 );
        
        virtual int             RecvFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN uint_t flag = 0 );
        
        virtual int             SendnTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN uint_t flag = 0 );
        
        virtual int             RecvnFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN uint_t flag = 0 );
        
        virtual int             Broadcast( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen );
        // 224.0.0.0 ~ 239.255.255.255 , f = 1 join  f = 0 exit
        virtual int             SetMuticast( _IN ipa_t ipa, _IN int timeout, _IN int f );
    };
    
};//namespace net

//int  ipv4aton(const ntchar_t* host);
//ntchar_t*   ipv4ntoa(int ipn, const ntchar_t* host);