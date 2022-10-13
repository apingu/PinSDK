//Socket.h headle file of Socket.cpp
//    IOSocket network object
//     for cross platform
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
//                                                         Apingu
//                                                               Sat Mar 16 2002
#ifndef IOSOCKET_H
#define IOSOCKET_H

#include "unp.h"
#include <T_Lock.h>

extern char* program_name;


///////////////////////////////////////////////////////////////////////////////
//  socket status

struct  Databuf
{
    size_t len;
    char* buf;
};


namespace IO
{
    ///////////////////////////////////////////////////////////////////////////////
    //  class pcsocket
    //! Generic Socket interface
    class Socket
    {
    public:

        //enum STYLE { STYLE_SERVICE, STYLE_CLIENT };

    public:
        //! constructor 
        Socket();
        //! destructor
        virtual ~Socket();

        virtual const char* SocketName() const = 0 { return ""; }

        //! 轉換成 socket number, 可被一般 socket function 使用
        operator socket_t() { return m_sockfd; }

        //! 初始化
        virtual socket_t     Create( int domain = AF_INET,
                                     int streamtype = SOCK_STREAM,
                                     int protocol = 0 );                //create socket in win32 system

        //! 邏輯上的單向斷線(不會真正關閉連線)
        virtual void         Shutdown( int how = 2 );
        //! 結束本 socket
        virtual void         Close( void );
		//void         ClearBuff( void );


        /** Get the socket number. */
        socket_t     GetSocketNumber() const { return m_sockfd; }
        /** Get the local IP address. If the socket is closed, it'll return 0. */
        static char* GetLocalHostname( char* buf,size_t len );
        static ipn_t GetLocalAddress( unsigned int i=0 );
        /** Get the remote IP address. If the socket is closed, it'll return 0. */
        ipn_t        GetPeerAddress( void );
        //! 取得錯誤碼(errno 或是 WSAGetLastError())
        int          GetErrorNo( void );
        //! 印出錯誤狀態, 如果 status !=0 則結束
        static void  SetErrorCode( int status,int err,char* format,... );
        //! 設定連線選項(參考 setsockopt)
        int          SetOpt( int level,int optname,const void* optval,socklen_t optlen );
        //! 呼叫 ioctl()
        int          Ioctl( long cmd,unsigned long* argp );
        //! 由錯誤碼印出錯誤訊息
        void         PrintErrorCode( int code );
        //! 取得 Mac Address(Ethernet 有效)
        MacAddress_t GetMacAddress( void );
        //! 呼叫 Create() 的時間
        time_t       GetCreateTime( void );

//        //! Key為上層使用的資料, 設定連結傳回舊的key 值
//        void         SetKey( void* key );
//        //! Key為上層使用的資料, 取得連結
//        void*        GetKey( void );

    protected:

        virtual void OnCreate( void )   {};
        virtual void OnClose( void )    {};
        virtual void OnShutdown( void ) {};

	public:
        //! Socket number
        socket_t m_sockfd;
		ipn_t    m_Ip;
		short    m_Port;

        //! 建立連線時間, 可以作為上線時間計算用
        time_t   m_CreateTime;


 	
    protected:
        
        //! block the background recving, so can recving by block mode
        bool   m_bBlock;

	private:

		//! 狀態
        //OPSTATE  m_OPState;

        //! Critical section lock
        T_Lock m_Lock;

		//! Critical section for use key lock
		//T_Lock m_lKey;
		
		//! extern data buffer
		//void*  m_Blockbuf;


        // 這一部份網路底層系統不用, 給上層自由運用的變數(上層自己負責管理,per-socket state)
        // By Wood 20040910 (Hint: Cyberspace 會用到,請看 CsConnect.h)
        //char user_var[32];

#if defined(_WIN32) || defined(_WINDOWS)
#endif
    };

	

    /////////////////////////////////////////////////////////////////////////////////////////
    // tcp protocol
    //! TCP 連線用 socket 介面

    class TCPSocket: public Socket
    {
    public:

        TCPSocket();
        virtual ~TCPSocket();

        virtual const char* SocketName() const { return "TCPSocket"; }


        socket_t     Create( int domain = AF_INET );

        virtual void Shutdown( int how = 2 );
        //! 結束本 socket
        virtual void Close( void );
        //! 發送訊息
        virtual int  Send( char* bp,size_t len,unsigned int flag = 0 );
        //! 接收訊息
        virtual int  Recv( char* bp,size_t len,unsigned int flag = 0 );
        //! 發送訊息(另一種)
        virtual int  Send( Databuf* bps,
                           unsigned int lens,
                           unsigned int flags = 0 );
        //! 接收訊息(另一種)
        virtual int  Recv( Databuf* bps,
                           unsigned int lens,
                           unsigned int flags = 0 );
        //! 發送緊急(OOB)訊息
        virtual int  Sendn( char* bp,size_t len );
        //! 接收緊急(OOB)訊息
        virtual int  Recvn( char* bp,size_t len );
        //! 設定內部緩衝區. 對 Server 來說是SO_RCVBUF, 對 Client 是SO_ENDBUF
        void         Set_tcp_buf_size( int buf_type,int size = 32168 );

    protected:
        //! 讀取資料
        int         Readn( char* bp,size_t len );
        int         Readvrec( char* bp,size_t len );
        //! 讀取一行資料(文字模式)
        int         Readline( char* bp,size_t len );

	private:

		T_Lock m_lTcp;

	public:

		//! use in windows iocp platform
        void*  m_Recvbuf;
		void*  m_Sendbuf;
    };



    /////////////////////////////////////////////////////////////////////////////////////////
    // udp protocol
    class UDPSocket:public Socket
    {
    public:

        UDPSocket();
        ~UDPSocket();

        virtual const char* SocketName() const = 0 { return "UDPSocket"; }

        socket_t            Create( int domain );

        int                 Service( char* host,char* port ); //server wait for bind in unix  systme
        int                 Connect( char* host,char* port,struct sockaddr_in* sap ); //    

        virtual int         Sendto( unsigned long ipaddr,
                                    Databuf* bps,
                                    unsigned int lens,
                                    unsigned int flags = 0 );

        virtual int         Recvfrom( unsigned long& ipaddr,
                                      Databuf* bps,
                                      unsigned int lens,
                                      unsigned int flags = 0 );

        virtual int         Sendto( unsigned long  ipaddr,
                                    char* bp,
                                    unsigned int len,
                                    unsigned int flags = 0 );

        virtual int         Recvfrom( unsigned long& ipaddr,
                                      char* bp,
                                      unsigned int len,
                                      unsigned int flags = 0 );

        virtual int         Broadcast( unsigned long ipaddr,
                                       const char* bp,
                                       unsigned int len );
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    // Reliable UDP
    class RUDPSocket:public UDPSocket
    {
    public:
        RUDPSocket();
        ~RUDPSocket();

        virtual int Sendhm( unsigned long  ipaddr,
                            Databuf* bps,
                            unsigned int lens,
                            unsigned int flags = 0 );
        virtual int Recvhm( unsigned long& ipaddr,
                            Databuf* bps,
                            unsigned int lens,
                            unsigned int flags = 0 );

        virtual int Sendto( unsigned long  ipaddr,char* bp,unsigned int len );
        virtual int Recvfrom( unsigned long& ipaddr,char* bp,unsigned int len );

        virtual int Broadcast( unsigned long ipaddr,
                               const char* bp,
                               unsigned int len );

    private:
        BYTE m_RttTime[32];
        BYTE m_Sendhdr[16];
        BYTE m_Recvhdr[16];
    };
};//namespace iocp

int   inet_aton( const char* cp,struct in_addr* pin );
void  set_address( const char* ipaddr,
                   const char* portnum,
                   struct sockaddr_in* sap,
                   const char* protocol );

char* get_ipa( char* hname );
int   get_ipn( char* hname );

ipn_t get_ipaton( ipa_t host );
ipa_t get_ipntoa( ipn_t host );

//int     ipv4aton( const char* host );
//char*   ipv4ntoa( int ipn, const char* host );




#endif  //SOCKET_H
