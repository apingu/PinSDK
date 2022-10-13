//
//
//
//
//
//
//    包裝UDP協定下 衍伸的 點對點 傳送 ( like server/client ) 故使用 peer ( 接收/傳送點 都是 peer 沒有 server/client的概念)
//
//
//
#pragma once

#include "net-socket.h"

namespace net
{
    /////////////////////////////////////////////////////////////////////////////////////////
    //
    class UDPPeer
    {
        net::UDPSocket*        v_udpsocket_;
        
    public:
    
        UDPPeer();
        ~UDPPeer();
        
        const tchar_t*          ObjectName() { return _T( "UDPPeer" );    }
        
        socket_t                GetFD();
        
        virtual socket_t        Create( _IN int domain = AF_INET );
        
        //指定本地的ip與port
        // lname = "" => 指定本地IP
        virtual int             Bind(_IN ushort_t hport );
        
        virtual int             Close( void );
        
        bool                    IsLive( void );
        
        int                     SendnTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN uint_t flag = 0 );
        int                     SendnTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const ipa_t to_addr, _IN ushort_t to_port, _IN uint_t flag = 0 );
        
        int                     RecvnFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN uint_t flag = 0, ustimen_t timeOut = 0 );
        int                     RecvnFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT ipa_t& form_addr, _OUT ushort_t& form_port, _IN uint_t flag = 0, ustimen_t timeOut = 0 );
        
        // 224.0.0.0 ~ 239.255.255.255 , f = 1 join  f = 0 exit
        virtual int             SetMuticast( _IN ipa_t ipa, _IN int timeout, _IN int f );
        
    protected:
    
        //virtual void            OnReceive( _IN net::UDPPeer* s ) = 0;
        
    private:
    
        //static void             Polling( void* data );
    };
    
    
    /////////////////////////////////////////////////////////////////////////////////////////
    //
    class RUDPSocket;
    class RUDPPeer
    {
        net::RUDPSocket*        v_rudpsocket_;
        
    public:
    
        RUDPPeer();
        ~RUDPPeer();
        
        const tchar_t*          ObjectName() { return _T( "RUDPPeer" );}
        
        socket_t                GetFD();
        
        virtual socket_t        Create( _IN int domain = AF_INET );
        
        //指定本地的ip與port
        // lname = "" => 指定本地IP
        virtual int             Bind( _IN ushort_t hport );

        ushort_t                GetPort();
        
        virtual int             Close( void );
        
        bool                    IsLive( void );

        int                     Waiting( _IN ustimen_t timeOut = 0 );

        int                     GetReadLength();

        int                     SendnTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN uint_t flag = 0 );
        int                     SendnTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const ipa_t to_addr, _IN ushort_t to_port, _IN uint_t flag = 0 );
        
        int                     RecvnFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN uint_t flag = 0, int timeOut = 0 );
        int                     RecvnFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT ipa_t& form_addr, _OUT ushort_t& form_port, _IN uint_t flag = 0, int timeOut = 0 );

        // 檢查對方是否還活著
        void                    Ping( _IN const sockaddr* srcaddr, _IN int addrlen );

        
    protected:

    
        //virtual void            OnReceive( _IN net::RUDPPeer* s ) = 0;
        
    private:
    
        //static void             Polling( void* data );
    };
}