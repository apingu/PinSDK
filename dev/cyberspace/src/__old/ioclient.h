//Client.h headle file of Client.cpp
//
//    client function class for network connect
//	Commented as doxygen format by Wood Woo on Mar 8, 2005
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

#ifndef IOCLIENT_H
#define IOCLIENT_H

#include "IOSocket.h"
#include <PThread.h>

namespace IO
{
	
	///////////////////////////////////////////////////////////////////////////////////
	//
	//! TCP 連線用 Client 基礎類別, 會開啟 thread 背景接收
    class TCPClient : public TCPSocket, public System::PThread
    {
    public:
        TCPClient();
        virtual ~TCPClient();

		const char*  SocketName() { return "TCPClinetSocket";}
		//! 嘗試連接到 host, port
        virtual bool Connect( char* host, unsigned short port ); //
		//! 單向切斷
		virtual void Shutdown(  int how = 2 );
		//! 強迫結束
		virtual void Close( void );
		//! 是否正在連線中
		bool         IsConnected( void );
		//! 當連線成功, 可以開始接收以後會被喚起的 callback
		virtual void OnConnect( void )  {};
		//! 主動斷線以後會被喚起的 callback
		virtual void OnKnockout( void ) {};
		//! 被斷線以後會被喚起的 callback
		virtual void OnDisconnection( void ){}
		//! 使用者接收訊息/處理訊息用 callback
		virtual void OnReceive( IO::TCPSocket* s ) {};

	protected:
		//! 中斷接收callback
		void         SetBlock( bool f );
		//! 啟動接收 thread
		void         BeginRecving( void );
		//! 讓系統停止接收 thread. 需要等一些時間
		void         EndRecving( void );

		//! NULL 表示接收 thread 沒在跑(可安心刪除)
		System::hThread  m_hListen;


    private:
	
		bool   m_bBlock;
		bool   m_bRecving;
		bool   m_bConnected;
		T_Lock m_TcpClinetLock;
		//! 接收的 thread
        static void __cdecl Receiving(void *);
    };



	///////////////////////////////////////////////////////////////////////////////////
	//
	class UDPClient : public UDPSocket, public System::PThread
	{
	public:

		UDPClient();
		~UDPClient();

		const char*  SocketName() { return "UDPClinetSocket";}
		virtual void Shutdown( void );

	protected:
		virtual void OnReceive( IO::UDPSocket* s ) {};

    private:
        static void __cdecl Receiving(void *);
		
	};
};

#endif
