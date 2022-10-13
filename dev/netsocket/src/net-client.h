//Client.h headle file of Client.cpp
//
//    client function class for network connect
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

#ifndef NETCLIENT_H
#define NETCLIENT_H

#include "net-socket.h"

namespace net
{
    ///////////////////////////////////////////////////////////////////////////////////
    //
    //! TCP 連線用 Client 基礎類別, 會開啟 thread 背景接收
    class TCPClient//, public System::PfThread
    {
        TCPSocket*       tcp_socket_v_;
        T_Lock           tcpclient_ut_lock_;
        int              flag_;
        static bool      s_is_tcp_client_recving_;
        
    public:
    
        TCPClient( _IN net::TCPSocket* tcpsocket = NULL );
        virtual ~TCPClient();
        
        const ntchar_t*     ObjectName() { return tcp_socket_v_->ObjectName(); };
        
        socket_t            GetFD();
        
    protected:
    
        operator            TCPSocket* () { return tcp_socket_v_; }
        void                SetTCPBufSize( _IN int buf_type, _IN int size = 32168 ) { return tcp_socket_v_->SetTCPBufSize( buf_type, size ); }
        //! 使用者接收訊息/處理訊息用 callback
        //virtual void        OnReceive( _IN net::TCPSocket* s ) = 0;
        virtual void        OnReceive() = 0;
                        
    public:
    
        virtual socket_t    Create( _IN int domain = AF_INET );
   
        // 2022 08 17 為什麼要 bind? 忘了 先註解起來
        // void                Bind( _IN ipa_t lname, _IN ushort_t lport );
   
        //! 嘗試連接到 host, port 
        virtual bool        Connect( _IN ipa_t hname, _IN ushort_t hport );

           //! 強迫結束
        virtual int         Close( void );

        //! 發送訊息
        virtual int         Send( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 ) { return tcp_socket_v_->Send( bufptr, buflen, flag ); }
        //! 接收訊息
        virtual int         Recv( _OUT ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 ) { return tcp_socket_v_->Recv( bufptr, buflen, flag ); }
        //! 發送緊急(OOB)訊息
        virtual int         Sendn( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 ) { return tcp_socket_v_->Sendn( bufptr, buflen, flag ); }
        //! 接收緊急(OOB)訊息
        virtual int         Recvn( _OUT ubyte_t* bufptr, _IN size32_t buflen, _IN uint_t flag = 0 ) { return tcp_socket_v_->Recvn( bufptr, buflen, flag ); }
        //! 發送訊息(不同資料區段)
        virtual int         Sendb( _IN const Databuf* bps, _IN size32_t lens, _IN uint_t flag = 0 ) { return tcp_socket_v_->Sendb( bps, lens, flag ); }
        //! 接收訊息(不同資料區段)
        virtual int         Recvb( _OUT Databuf* bps, _IN size32_t lens, _IN uint_t flag = 0 ) { return tcp_socket_v_->Recvb( bps, lens, flag ); }
        
        //! 單向切斷
        //virtual int             Shutdown(int how = 2);
        //! 是否正在連線中
        bool                IsConnected( void );
        
        //! 使用block的模式 要再connect之前呼叫
        void                SetBlock( _IN bool f );
        
        //static void         GetLocalIPAddress( _OUT std::vector<ipa_t>& ip4list, _OUT std::vector<ipa_t>& ip6list );
        stimen_t&           GetLastCreateTime( void ) { return tcp_socket_v_->GetLastCreateTime(); }
        
    private:

        //! 接收的 thread
        static void         Polling( _IN void* );
    };
};

#endif
