//Server.h headle file of server.cpp
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
//               Apingu
//                  Sat Mar 16 2002

#ifndef NETSERVER_H
#define NETSERVER_H

#pragma warning (disable : 4786)

#include <vector>
#include <map>
#include "net-socket.h"


namespace net
{
    //#define _MAX_POOL_DIV   16
        ////////////////////////////////////////////////////////////////////////
        // TCPServer
        //! 使用 TCP 的一般 Server 類別. 運用 Thread 可以進行非同步接收
    class TCPServer //, public System::__MSVC__
    {
        T_Lock                                      tcpserver_ut_lock_;
                                                    
        TCPSocket                                   tcpsocket_;
        int                                         domain_;
        //! 本 Server 監聽的 port                    
        ushort_t                                    listen_port_;
        //! 本 Server 最大接受連線數                 
        uint_t                                      max_client_;

        // Polling 處理被喚起時 +1 處理完 -1　
        uint_t                                      num_running_threads_;   /**< Number of current running threads. */
        //! 本 建立了幾個Server Polling的 thread 數目
        uint_t                                      num_polling_threads_;   /**< Number of current running threads. */

        //! 連線列表
        int                                         max_socket_banks_div_;
        std::vector<T_Lock>                         sockets_lock_;
        std::vector<std::map<dword_t, TCPSocket*>>  sockets_[ 3 ];    // 0 預先建立好的 1 未使用 2 使用中

        std::vector<ulong64_t>                      polling_threads_uid_;

        //! 還活著的 thread(debug用)
        // 檢查polling 有沒有離開用  polling 進入 後 +1  離開 -1
        T_Lock                                      num_processing_threads_lock_;
        long32_t                                    num_processing_threads_;          // 有幾個Polling 正在處理中

#if defined(_WINDOWS)  /* Windows platforms */
        //! for win32 Netcp
        void*                          hIOCP_;
        // 沒有立即回收成功的 IOCP SOCKET
        //std::list<net::TCPSocket*>  m_ListeningList;
#endif
#if defined(_LINUX)
        int                            hEPFD_;
#endif
    public:

        TCPServer();
        ~TCPServer();

        const ntchar_t*         ObjectName() const { return ( "TCPServer" ); }

        socket_t                GetFD();
        //! 是否停止
        bool                    IsActive( void );

        //! 查詢 socket sk 是否有效(不一定在連線中)?
        bool                    IsActive( _IN TCPSocket* sk );

        virtual socket_t        Create( _IN int domain = AF_INET );

        //! start server
        virtual int             StartUp( _IN uint_t allowplayers, _IN ushort_t port );

        //! close server if no error, return 0
        virtual int             Shutdown( void );

        //! kickout one client
        virtual int             Kickout( TCPSocket* sk ) final;

        stimen_t                GetLastCreateTime( void ) { return tcpsocket_.GetLastCreateTime(); }
        //static void             GetLocalIPAddress( _OUT std::vector<ipa_t>& ip4list, _OUT std::vector<ipa_t>& ip6list );
        const ntchar_t*         GetLocalHostname( _OUT ntchar_t* buf, _IN size32_t len ) { return tcpsocket_.GetLocalHostname( buf, len ); }
        long32_t                GetLivingThreadSize( void );

        //! 取得監聽的 port 號碼
        ushort_t                GetServicePort( void );

        //! how much client allow to be connect
        uint_t                  GetMaxConnectSize( void );

        //! get connected client size
        uint_t                  GetConnectedSize( void );

        //! get connected client by index order
        //net::TCPSocket* operator[](uint_t index);
        void                    GetAllConnectedSocket( _OUT std::vector<net::TCPSocket*>& connectionlist );

        //
        UINT                    GetPollingCount( void ) { return num_polling_threads_; }

        //! get connected client by index order
        net::TCPSocket*         FindConnectedSocket( _IN socket_t fd );

    protected:
        //! accept one client
        //int                     Accept( TCPSocket& sk );

        //! 當需要產生新連線時呼叫
        //virtual TCPSocket*      OnNewSocket( void ) { return new net::TCPSocket; }
        //! 當停用一個連線時呼叫 visual distruct 所以不用
        virtual void            OnCreateSocket( _IN TCPSocket* sk ){};

        //virtual void            OnDelSocket( net::TCPSocket* sk ) { SAFE_DELETE( sk ); }        
        virtual void            OnDestroySocket( _IN TCPSocket* sk ){};

        //! call back when server accept one connect
        virtual void            OnAccept( _IN TCPSocket* sk ) = 0;

        //! call back when client cutoff connect
        virtual void            OnKickout( _IN TCPSocket* sk ) = 0;

        //! call back when sock receive message
        virtual void            OnReceive( _IN TCPSocket* sk ) = 0;

    private:

        // IOCP中為回收 沒有立即回收成功的SOCKET
        //static void             Listening( void* data );
        //! 內部用來接收訊息的 thread
        static void             Polling( void* data );
        // IOCP中拿來 刪除該SOCKET用(為避免死結)
        //static void             Kicking( void* data );

        //server wait for bind in unix  systme
        //static void __cdecl Select   (void* data);
    };
};

#endif  //SERVER_H
