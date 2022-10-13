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
//
//
//
#ifndef IOCPDEF_H
#define IOCPDEF_H

////#include <assert.h>
#include "unp.h"
#include "net-socket.h"
//#include "net-server.h"
#include <memio.inl>
#include <pf-debug.h>

#if defined(_WINDOWS)  /* Windows platforms iocp*/
#define _WIN32IOCOMPLETIONPORT
#define KEY_TERMINATE      0xFFFFFFFF
#define ACCEPT_ADDRLEN     (sizeof(sockaddr_in) + 16)
#define INITIALRECEIVESIZE 0
#endif

namespace net
{
    //typedef void _CALLBACK( *CloseCallback )( net::TCPSocket* callfd, void* data );
#if defined (_WIN32IOCOMPLETIONPORT)  /* Windows platforms iocp*/

    //! Socket state enumerate
    enum OPSTATE
    {
        //OP_CLOSED,
        //OP_CLOSING,
        OP_RESET,
        OP_ACCEPTING,
        OP_ACCEPTED,
        OP_READING,
        OP_WRITING
    };

    const int IOCPBUFSIZE = _MAX_SOCKET_BUFF; //80K
    const int IOCPSAFESIZE = _MAX_SOCKET_BUFF; //80K
    struct OV: public OVERLAPPED
    {
        //static const int addrlen;
        //static const int initialReceiveSize;
        DWORD    n; // number of bytes received or sent
        DWORD    begin;
        DWORD    end;
        DWORD    addlen;
        ubyte_t* buf; // receive buffer
        //tchar_t buf[IOCPBUFSIZE]; // receive buffer
        DWORD    buflen;
        DWORD    Flags;
        OPSTATE  optype;
    };


    class SocketOV: public OV
    {
        //T_Lock          m_lOv;
        net::TCPSocket* v_tcp_socket_;

    public:

        SocketOV( net::TCPSocket* s )
        {
            v_tcp_socket_ = s;
            buflen = IOCPBUFSIZE;
            buf = new ubyte_t[ buflen ];
            Reset();
        }

        ~SocketOV()
        {
            Reset();
            SAFE_DELETE_ARRAY( buf );
        }

        inline net::TCPSocket* GetSocket( void )
        {
            //TS_Lock lock( &( m_lOv ) );
            return v_tcp_socket_;
        }

        inline OPSTATE GetOPType( void )
        {
            //TS_Lock lock( &( m_lOv ) );
            return optype;
        }

        inline size32_t GetBufsize( void )
        {
            return buflen - end;
        }

        inline size32_t GetUsedsize( void )
        {
            return end - begin;
        }

        inline void Reset( /*OPSTATE op = OP_RESET*/ )
        {
            //TS_Lock lock( &( m_lOv ) );
            memset( this, 0, sizeof( OVERLAPPED ) ); //must do memset
            memset( buf, 0, buflen ); //must do memset
            begin = 0;
            end = 0;
            optype = OP_RESET;
            n = 0;
            addlen = 0;
            //Flags = MSG_PEEK;
            Flags = 0;
        }

        inline int PostAccept( socket_t listen )
        {
            //TS_Lock lock( &( m_lOv ) );
            //if ( optype == net::OP_ACCEPTING )
            //{
            //    return 0;
            //}

            if( listen == 0 )
            {
                return 0;
            }

            optype = net::OP_ACCEPTING;

            //v_tcp_socket_->Close();
            if( v_tcp_socket_->GetFD() == 0 )
            {
                if( !v_tcp_socket_->Create() )
                {
                    return -1;
                }
            }

            if( !AcceptEx( listen, v_tcp_socket_->GetFD(), &( buf[ 0 ] ), INITIALRECEIVESIZE, ACCEPT_ADDRLEN, ACCEPT_ADDRLEN, &( n ), this ) )
            {
                int error = WSAGetLastError();

                if( error != ERROR_IO_PENDING )
                {
                    printf( "AcceptEx error. code = %d socket = %lld\n", error, ( int64_t )v_tcp_socket_->GetFD() );
                    v_tcp_socket_->Close();
                    //closesocket(ov->s);
                    //delete ov;
                    return -1;
                }

                //pfstd::MSSleep( 0 );
            }

            //OVVec.push_back(ov);
            return 1;
        }

        inline void DoAccept()
        {
            optype = net::OP_ACCEPTED;
        }
        //inline void DoAccept( sockaddr_in& LocalSockaddr, int& LocalSockaddrLength,
        //                      sockaddr_in& RemoteSockaddr, int& RemoteSockaddrLength )
        //
        //{
        //    int locallen = 0, remotelen = 0;
        //    sockaddr_in* plocal = NULL;
        //    sockaddr_in* premote = NULL;
        //    GetAcceptExSockaddrs( &( buf[0] ), 0, ACCEPT_ADDRLEN, ACCEPT_ADDRLEN,
        //                          ( sockaddr** )( &plocal ), &locallen,
        //                          ( sockaddr** )( &premote ), &remotelen );
        //    memcpy( &( LocalSockaddr ), plocal, locallen );
        //    memcpy( &( RemoteSockaddr ), premote, remotelen );
        //    LocalSockaddrLength = locallen;
        //    RemoteSockaddrLength = remotelen;
        //}


        inline int PostClose( void )
        {
            //TS_Lock lock( &( m_lOv ) );
            v_tcp_socket_->Close();
            return 1;
        }


        //change iocp receive buf
        inline int PostRead( void )
        {
            if( optype == net::OP_ACCEPTING )
            {
                return 0;
            }

            optype = net::OP_READING;

            if( end >= buflen )
            {
                end = 0;
            }

            //if all data had be read
            if( begin >= end )
            {
                end = 0;
                begin = 0;
                if( buflen > IOCPBUFSIZE )
                {
                    SAFE_DELETE_ARRAY( buf );
                    buflen = IOCPBUFSIZE;
                    buf = new ubyte_t[ buflen ];
                }
            }

            //剩下可接收的長度
            n = buflen - end;
#ifdef _DEBUG
            if( n == 0 )
            {
                Pf_ERROR( "SocketOV::PostRead: buf may not be zero!! " );
                return -1;
            }
#endif
            WSABUF wsbuf;
            wsbuf.len = n;                            // 不可以為0  當被呼叫起時會長度為0 
            wsbuf.buf = ( CHAR* )( &( buf[ end ] ) ); //下次底層就從這邊開始放資料
            //memset(wsbuf.buf, 0, wsbuf.buflen);
            DWORD recv_len = 0;
            int   recv_ret = WSARecv( v_tcp_socket_->GetFD(), &wsbuf, /* number of WSABUFs */ 1, &recv_len, &Flags, this, /* completion routine address */ 0 );
            //char tempbuf[512];
            //memcpy(tempbuf, wsbuf.buf, min(512, n));

            // 因為返回 正確 一定是 0
            if( recv_ret != 0 )
            {
                if( recv_ret == SOCKET_ERROR )
                {
                    int errco = WSAGetLastError();
                    if( errco != WSA_IO_PENDING )
                    {
                        //printf("  Post recv %d : mark %d failure\n", ret, m_mark);
                        return SOCKET_ERROR;
                    }
                    //n = 0;
                    //printf("  Post recv %d : mark %d later\n", ret, m_mark);
                }
                else
                {
                    return recv_ret;
                }
            }
            //end += recv_len;
            //addlen += recv_len;
            return 0;
        }

        // 傳回收到的字元數
        inline int DoRead( ubyte_t* bufptr, size32_t buflen, uint_t flag )
        {
            //TS_Lock lock( &( m_lOv ) );
            if( optype != net::OP_READING )
            {
                return -1;
            }

            size32_t recvlen = end - begin;
            //超出可接收的範圍
            if( recvlen < buflen )
            {
                __pfmemcpy( bufptr, &( buf[ begin ] ), recvlen );
                begin += recvlen;
                buflen = recvlen;
            }
            else
            {
                recvlen = buflen;
                __pfmemcpy( bufptr, &( buf[ begin ] ), recvlen );

                if( flag != MSG_PEEK )
                {
                    begin += buflen;
                }
            }
            return buflen;
        }

        /*
        inline int PostSend( const ubyte_t* bufptr, size32_t buflen, uint_t flags )
        {
            //TS_Lock lock( &( m_lOv ) );

            if ( optype != net::OP_WRITING )
            {
                return -1;
            }

            //          DWORD l=0;
            //
            //          for (UINT i = 0; i < lens; i++)
            //          {
            //              l+=bps[i].buflen;
            //          }

            //          if(l>=IOCPBUFSIZE)
            //          {
            //              return -1;
            //          }

            // buffer overflew
            if ( ( end + buflen ) >= buflen )
            {
                ubyte_t* tempbuf = new ubyte_t[end + buflen];
                __pfmemcpy( tempbuf, buf, end );
                __pfmemcpy( tempbuf + end, bufptr, buflen );
                SAFE_DELETE_ARRAY( buf );
                buf = tempbuf;
                buflen = end + buflen;
            }
            else
            {
                __pfmemcpy( &( buf[end] ), bufptr, buflen );
            }

            //          else
            //          {
            //              begin = end;
            //          }
            //          for(UINT b=0; b<lens; b++)
            //          {
            end += buflen;
            //}
            return 0;
        }

        inline int DoSend( void )
        {
            //TS_Lock lock( &( m_lOv ) );

            if ( optype != net::OP_WRITING )
            {
                return -1;
            }

            WSABUF wsbuf;
            wsbuf.buf = &( buf[begin] );
            wsbuf.buflen = n = ( end - begin );
            DWORD sent = 0;

            if ( wsbuf.buflen == 0 )
            {
                return 0;
            }

            // I think it no overlapped operation. If that case, different mechanism is needed.
            int ret = WSASend( v_tcp_socket_->GetFD(), &wsbuf, 1, &sent, Flags, this, NULL );
            //LARGE_INTEGER PerformanceCount;
            //QueryPerformanceCounter(&PerformanceCount);
            //m_mark=GetTickCount();
            n -= sent;
            begin += sent;

            if ( wsbuf.buflen == 0 )
            {
                begin = 0;
                end = 0;
                //return 0;
            }

            if ( begin >= end )
            {
                begin = 0;
                end = 0;
            }

            if ( ret == SOCKET_ERROR )
            {
                if ( WSAGetLastError() != WSA_IO_PENDING )
                {
                    //printf("  Post send %d : mark %d failure, buflen=%d\n", ret, m_mark, sent);
                    //optype=net::OP_CLOSED;
                    //v_tcp_socket_->Close();
                    return SOCKET_ERROR;
                }

                //printf("  Post send %d later : mark %d, buflen=%d\n", ret, m_mark, sent);
            }

            return sent;
        }
        */
    };

#endif //_WINDOWS

};

#endif
