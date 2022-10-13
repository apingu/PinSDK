//Server.h headle file of server.cpp
//
// Commented as doxygen format by Wood on Mar 8, 2005
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

#ifndef IOSERVER_H
#define IOSERVER_H

#pragma warning (disable : 4786)
#include "IOSocket.h"
#include <PThread.h>
#include <map>
#include <list>




namespace IO
{
    ////////////////////////////////////////////////////////////////////////
    // TCPServer
	//! 使用 TCP 的一般 Server 類別. 運用 Thread 可以進行非同步接收
    class TCPServer : protected TCPSocket, public System::PThread
    {
    public:

        TCPServer();
        ~TCPServer();

		const char*    SocketName() const { return "TCPServerSocket";}
		//! 是否停止
		bool           IsActive( void );

		//! start server
        int            StartUp( u_int allowplayers, unsigned short port );

		//! close server
		void           Close( void );

		//! accept one client
        bool           Accept( TCPSocket& s );

		//! knockout one client
		virtual int    KnockOut( TCPSocket* s );

		time_t         GetCreateTime( void ){ return TCPSocket::GetCreateTime(); }
		
		ipn_t          GetLocalAddress( unsigned int i ){ return TCPSocket::GetLocalAddress( i ); }

		const char*    GetLocalHostname( char* buf, size_t len ){ return TCPSocket::GetLocalHostname( buf, len ); }
		long           GetLivingThreadSize( void );

        //! 取得監聽的 port 號碼
        unsigned short GetServicePort( void );

		//! how much client allow to be connect
        unsigned int   GetLimitConnectSize( void );

		//! get connected client size
		unsigned int   GetConnectedSize( void );
		
		//! get connected client by index order
		IO::TCPSocket* operator[]( u_int index );

		//
		UINT           GetPollingCount( void ){ return m_PollingThreads; }

		//! get connected client by index order
		IO::TCPSocket* FindSocket( socket_t fd );

		//! 查詢 socket s 是否正在連線中?
        bool           IsAlive( TCPSocket* s );


		//! call back when server initiation
		virtual void   OnInitiation( void ){}

		//! call back when server shutdown
        virtual void   OnShutDown( void ){ return; }

		//! call back when server accept one connection
        virtual bool   OnAccept( TCPSocket* s ){ return true; }

		//! call back when server knock one connection
        //virtual void   OnKnockOut( TCPSocket* s ){}

		//! call back when client cutoff connection
        virtual void   OnDisconnect( TCPSocket* s ){}

		//! call back when sock receive message
        virtual void   OnReceive( TCPSocket* s ) {}

	

	protected:
		//! 當需要產生新連線時呼叫
		virtual TCPSocket* OnNewSocket( void ){ return new IO::TCPSocket; }
		//! 當停用一個連線時呼叫
		virtual void       OnDelSocket( IO::TCPSocket* s ){ SAFE_DELETE(s); }

		
    private:

        static void __cdecl Listening( void* data );
        static void __cdecl Select( void* data ); //server wait for bind in unix  systme
		//! 內部用來接收訊息的 thread
        static void __cdecl Polling( void* data );

#if defined(_WIN32) || defined(_WINDOWS)     /* Windows platforms */
        //! for win32 iocp
		void*  m_hIOCP;
		std::list<IO::TCPSocket*>  m_ListeningList;
//		static void ClientDisconnectCB( IO::TCPSocket* callfd, void* data );
//		static void ClientKnockoutCB( IO::TCPSocket* callfd, void* data );
#endif
		T_Lock                      m_lServer;
		T_Lock                      m_lClose;
		//! 本 Server 監聽的 port
        unsigned short              m_ListenPort;
		//! 本 Server 最大接受連線數
        unsigned int                m_LimitClient;
		//! 本 Server 開啟的 thread 數目
		long                        m_RunningThreads;   /**< Number of current running threads. */

		//! 本 Server Polling的 thread 數目
		long                        m_PollingThreads;   /**< Number of current running threads. */
		long                        m_LivingThreads;

		//! 連線列表
		std::vector<IO::TCPSocket*> m_ClientList;
		std::vector<IO::TCPSocket*> m_SocketPool;
		//std::map< socket_t, IO::TCPSocket* > m_ConnectionList;
    };


    /////////// /////////////////////////////////////////////////////////////
    // TCPServer
    class UDPService : public UDPSocket, public System::PThread
    {
    public:
      UDPService();
      ~UDPService();

	  const char*  SocketName() { return "UDPServerSocket";}

	  int           StartUp( unsigned short port );
	  void          Close( void );

	protected:

  		virtual void   OnInitiation( void ){};
        virtual void   OnShutDown( void )  {};
        virtual void   OnReceive( UDPSocket* s ) {};
	  
	private:

	  bool m_shutdown;

	  static void __cdecl Polling( void* data );
	  /*
          int       MainLoop       ( void ); //server wait for bind in unix  systme
          void      BroadCast      ( void );
          int       LoadTable      ( char *filepath );    //read command table
          void      client_add     ( int fd, struct sockaddr_in client_address );
          void      client_del     ( int fd );
          void      client_clear   ( void );
          int       client_size    ( void );
          void      ShowOnlineUser ( int fd = 1 );
		  */
    };

};

#endif  //SERVER_H
