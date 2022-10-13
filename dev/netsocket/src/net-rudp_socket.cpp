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
#include <pf-stdlib.h>
#include <pf-thread.h>
//
#include <memio.inl>

//03. == 其他 使用 函式庫的.h 文件


//04. == 此專案內.h 文件
#include "net-rudp_socket.h"



namespace net
{
    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////
        // Reliable UDP
        struct hdr
        {
            uint_t seq;
            uint_t ts;
        } sendhdr, recvhdr;

        enum rudpType
        {
            rudpAck  = 199,          // Acknowledge message.
            rudpData = 200,          // Message with some data.
        };

        // The header to a rudp message.
        struct rudpHeader
        {
            qword_t  id;          // Message id.  Returned with ack.
            dword_t  sendSec;      // Time sent in seconds.  Returned with ack.
            dword_t  sendMicro;      // Time sent microseconds. Returned with ack.
            uint_t   resendcnt;   //
            uint_t   sendLen;     //
            stimen_t creattime;
            ubyte_t  type;          // One of rudpType above.
            ubyte_t  reserved1;      // Reserved, always zero for now.
            ubyte_t  reserved2;      // Reserved, always zero for now.
            ubyte_t  reserved3;      // Reserved, always zero for now.
        };

        typedef int rudpHost;     // The IP address (in host order) of another computer. */

#define udpEthMaxSize 1444
                                  // Max data size that will fit into a single ethernet packet after UDP and IP
                                  // headers.  Things are faster & more reliable if you stay below this

#define rudpMaxSize (udpEthMaxSize - sizeof(struct rudpHeader)  )

#define MAX_TIME_OUT 999999

#if defined(_WINDOWS)
        int   g_bInitWinsocket = 0;
#endif

        //=============================================================================
        // NAME:
        // DESC: Return difference in microseconds between t1 and t2.  t2 must be
        //        later than t1 (but less than 50 minutes later).
        //=============================================================================
        static int timeDiff( struct timeval* t1, struct timeval* t2 )
        {
            int secDiff = t2->tv_sec - t1->tv_sec;
            int microDiff = 0;

            if ( secDiff != 0 )
            {
                microDiff = secDiff * 1000000;
            }

            microDiff += ( t2->tv_usec - t1->tv_usec );

            if ( microDiff < 0 )
            {
                // Note, this case actually happens, currently particularly on
                // kkr2u62 and kkr8u19.  I think this is just a bug in their clock
                // hardware/software.  However in general it _could_ happen very
                // rarely on normal machines when the clock is reset by the
                // network time protocol thingie.
                //warn( "t1 %u.%u, t2 %u.%u.  t1 > t2 but later?!", ( unsigned ) t1->tv_sec,( unsigned ) t1->tv_usec, ( unsigned ) t2->tv_sec, ( unsigned ) t2->tv_usec );
                microDiff = 0;
            }

            return microDiff;
        }

        //=============================================================================
        // NAME:
        // DESC: Wait for descriptor to have some data to read, up to
        //        given number of microseconds.
        //=============================================================================
        static bool readReadyWait( int sd, ustimen_t microseconds )
        {
            struct timeval tv;
            fd_set set;
            int readyCount;

            for ( ;; )
            {
                if ( microseconds > 1000000 )
                {
                    tv.tv_sec = microseconds / 1000000;
                    tv.tv_usec = microseconds % 1000000;
                }
                else
                {
                    tv.tv_sec = 0;
                    tv.tv_usec = microseconds;
                }

                FD_ZERO( &set );
                FD_SET( sd, &set );
                readyCount = select( sd + 1, &set, NULL, NULL, &tv );

                if ( readyCount < 0 )
                {
                    if ( errno == EINTR )    /* Select interrupted, not timed out. */
                    {
                        continue;
                    }
                    else
                    {
                        //warn( "select failure in rudp: %s", strerror( errno ) );
                    }
                }
                else
                {
                    return readyCount > 0;    /* Zero readyCount indicates time out */
                }
            }
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        inline static std::string MakeKey( _IN sockaddr* addr, _IN rudpHeader* header )
        {
            sockaddr_in* addrin = ( sockaddr_in*)addr;
            std::string key;
            pfstd::strprintf( key, ( "%u:%d:lld" ), addrin->sin_addr.s_addr, addrin->sin_port, header->creattime );
            return key;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //inline static void SplitKey( _IN const std::string* key, _OUT sockaddr* addr, _OUT rudpHeader* header )
        //{
        //    sockaddr_in* addrin = ( sockaddr_in* )addr;
        //    sscanf( key->c_str(), "%u : %d : %lld", &addrin->sin_addr.s_addr, &addrin->sin_port, &header->creattime );
        //}

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        static void InsertAck( _OUT RUDPSocket::acked_reg* ackmap, _IN const std::string* key, _IN qword_t val )
        {
            //std::string key;
            //pfstd::strprintf( key, ( "%u:%d:lld" ), net::Socket::GetSockAddrIPN4( addr ).s_addr, net::Socket::GetSockAddrPort( addr ), header->creattime );
            std::vector<uint64_t> vals;
            vals.push_back( pftml::Time::GetTickMSTime() );
            vals.push_back( val );
            ackmap->insert( RUDPSocket::acked_reg::value_type( (*key), vals ) );
            return;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        static uint64_t SearchAckId( _OUT RUDPSocket::acked_reg* ackmap, _IN const std::string* key )
        {
            RUDPSocket::acked_reg::iterator it = ackmap->find( (*key) );
            if ( it != ackmap->end() )
            {
                return it->second[1];
            }
            return 0;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        static void RemoveAck( _OUT RUDPSocket::acked_reg* ackmap, _IN const std::string* key )
        {
            ackmap->erase( (*key) );
        }
    }

    //using namespace RUDPSocketS;
  
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    RUDPSocket::RUDPSocket()
    {
        //RTT::rtt_init((RTT::rtt_info *) &m_RttTime);
        //memset( &m_Sendhdr, 0, sizeof( m_Sendhdr ) );
        //memset( &m_Recvhdr, 0, sizeof( m_Recvhdr ) );
        stime_last_recv_ = 0;
        stime_last_send_ = 0;
        rtt_last_ = 0;
        rtt_ave_ = 0;
        rtt_vary_ = 0;
        timeout_ = 0;
        num_recv_ = 0;
        num_send_ = 0;
        num_resend_ = 0;
        num_fail_ = 0;
        last_uid_ = 0;
        max_retries_ = 0;
        length_ = udpEthMaxSize;
        buffer_ = new ubyte_t[ length_ ];
       //check_alive_ = 600000;// 10 
        check_alive_ = 1000;
#if defined(_WINDOWS)
        if ( g_bInitWinsocket == 0 )
        {
            WORD wVersionRequested;
            WSADATA wsaData;
            int err;
            wVersionRequested = MAKEWORD( 2, 2 );
            /*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
            err = WSAStartup( wVersionRequested, &wsaData );

            //count++;
            if ( err != 0 )
            {
                /* Tell the user that we could not find a usable */
                /* WinSock DLL. */
                net::Socket::PrintError( "ERROR: Start up winsock dll failed ~_~b !!!" );
                //MessageBox(NULL,"Start up failed"," Socket Failed", MB_OK);
            }
        }

        g_bInitWinsocket++;
#endif
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    RUDPSocket::~RUDPSocket()
    {
        length_ = 0;
        SAFE_DELETE_ARRAY( buffer_ );
    }
    
    //=============================================================================
    // NAME:
    // DESC: Tell system about a time-out.
    //=============================================================================
    void RUDPSocket::TimedOut()
    {
        timeout_ <<= 1;   /* Back off exponentially. */
        
        if ( timeout_ >= MAX_TIME_OUT )
        {
            timeout_ = MAX_TIME_OUT;
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC: Return calculation of time out based on current data.
    //=============================================================================
    int RUDPSocket::CalcTimeOut()
    {
        int timeOut = rtt_ave_ + ( rtt_vary_ << 2 );
        
        if ( timeOut > MAX_TIME_OUT )
        {
            timeOut = MAX_TIME_OUT;
        }// No more than a second.
        
        if ( timeOut < 10000 )
        {
            timeOut = 10000;
        }// No less than 1/100th second
        
        return timeOut;
    }
    
    //=============================================================================
    // NAME:
    // DESC: Add info about round trip time and based on this recalculate time outs.
    //=============================================================================
    void RUDPSocket::AddRoundTripTime( int time )
    {
        int delta;
        rtt_last_ = time;
        delta = time - rtt_ave_;
        rtt_ave_ += ( delta >> 3 );              /* g = 1/8 */
        
        if ( delta < 0 )
        {
            delta = -delta;
        }
        
        rtt_vary_ += ( ( delta - rtt_vary_ ) >> 2 );  /* h = 1/4 */
        timeout_ = CalcTimeOut();
    }
    
    //=============================================================================
    // NAME:
    // DESC:  Wait for acknowledgement to the message we just sent.
    //         The set should be zeroed out. Only wait for up to
    //         ru->timeOut microseconds.   Prints a message and returns FALSE
    //         if there's a problem.
    //=============================================================================
    bool RUDPSocket::GetOurAck( struct timeval* startTv )
    {
        struct rudpHeader head;
        int readSize;
        stimen_t timeOut = timeout_;
        
        for ( ;; )
        {
            // Set up select with our time out.
            int dt;
            struct timeval tv;
            
            if ( readReadyWait( GetFD(), timeOut ) )
            {
                /* Read message and if it's our ack return true.   */
                readSize = recvfrom( GetFD(), ( char* ) &head, sizeof( head ), 0, NULL, NULL );
                
                if ( readSize >= sizeof( head ) && head.type == rudpAck && head.id == last_uid_ )
                {
                    gettimeofday( &tv, NULL );
                    dt = timeDiff( startTv, &tv );
                    AddRoundTripTime( dt );
                    return TRUE;
                }
            }
            
            // If we got to here then we did get a message, but it's not our
            // ack.  We ignore the message and loop around again,  but update
            // our timeout so that we won't keep getting other people's messages
            // forever.
            gettimeofday( &tv, NULL );
            timeOut = timeout_ - timeDiff( startTv, &tv );
            
            if ( timeOut <= 0 )
            {
                return FALSE;
            }
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    socket_t RUDPSocket::Create( _IN int domain )
    {
        TS_Lock lock( &rudp_ut_lock_ );
        socket_t sd = Socket::Create( domain, SOCK_DGRAM, IPPROTO_UDP );
        
        if ( sd < 0 )
        {
            net::Socket::PrintError( "Couldn't open socket in rudpOpen %s" );
            return NULL;
        }
        
        rtt_vary_ = 250;            // Initial variance 250 microseconds.
        timeout_ = CalcTimeOut();
        max_retries_ = 7;
        // check alive
        pfkrl::Thread::BeginProcess( Polling, this, _T( "RUDPSocket::Polling" ) );
        return sd;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPSocket::Close( void )
    {
        return Socket::Close();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool RUDPSocket::IsLive( void )
    {
        return ( GetFD() == 0 ) ? false : true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPSocket::Waiting( ustimen_t timeOut )
    {
        if ( timeOut != 0 )
        {
            if ( !readReadyWait( GetFD(), timeOut ) )
            {
                //warn( "rudpReceive timed out\n" );
                errno = ETIMEDOUT;
                return -1;
            }
        }
        return GetReadLength();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPSocket::GetReadLength()
    {
        return Socket::GetReadLength() - sizeof( rudpHeader );
    }
        
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int RUDPSocket::SendTo( const ubyte_t* bufptr, size32_t buflen, const sockaddr* srcaddr, int addrlen, uint_t flag )
    {
        TS_Lock lock( &rudp_ut_lock_ );

        struct timeval sendTv;    // Current time.
        if ( length_ < sizeof( rudpHeader ) + buflen )
        {
            SAFE_DELETE_ARRAY( buffer_ );
            length_ = sizeof( rudpHeader ) + ( buflen * 2 );
            buffer_ = new ubyte_t[ length_ ];
        }
        
        //char outBuf[ udpEthMaxSize ];
        struct rudpHeader* head;
        int fullSize = buflen + sizeof( *head );
        int i, err = 0, maxRetry = max_retries_;
        // Make buffer with header in front of message.
        // At some point we might replace this with a scatter/gather
        // iovector.+= 1;
        //assert( buflen <= rudpMaxSize );
        head = ( struct rudpHeader* )buffer_;
        if( ( bufptr != NULL ) && ( buflen > 0 ) )
        {
            memcpy( head + 1, bufptr, buflen );
        }
        if ( last_uid_ >= _MAX_ULLONG )
            last_uid_ = 0;
        head->id = ++last_uid_;
        head->resendcnt = 0;
        head->type = rudpData;
        head->sendLen = buflen;
        head->creattime = GetLastCreateTime();
        
        // Go into send/wait for ack/retry loop.
        for ( i = 0; i < maxRetry; ++i )
        {
            gettimeofday( &sendTv, NULL );
            head->sendSec = sendTv.tv_sec;
            head->sendMicro = sendTv.tv_usec;
            err = sendto( GetFD(), ( char* ) buffer_, fullSize, flag, ( struct sockaddr* )srcaddr, addrlen );
            
            if ( err < 0 )
            {
                // Warn, wait, and retry.
                struct timeval tv;
                //warn( " sendto problem %s", strerror( errno ) );
                tv.tv_sec = 0;
                tv.tv_usec = timeout_;
                select( 0, NULL, NULL, NULL, &tv );
                num_resend_ += 1;
                TimedOut();
                continue;
            }
            
            if ( GetOurAck( &sendTv ) )
            {
                return err -= sizeof( *head );
            }
            
            TimedOut();
            num_resend_ += 1;
            head->resendcnt++;
        }
        
        if ( err >= 0 )
        {
            err = ETIMEDOUT;
            //warn( "rudpSend timed out" );
        }
        
        num_fail_ += 1;
        return -1;
    }
    
    //=============================================================================
    // NAME:
    // DESC: Read message into buffer of given size.  Returns actual size read on
    //        success. On failure prints a warning, sets errno, and returns -1.
    //        Also returns ip address of message source. If timeOut is nonzero,
    //        it represents the timeout in milliseconds.  It will set errno to
    //        ETIMEDOUT in this case.
    //=============================================================================
    int RUDPSocket::RecvFrom( ubyte_t* bufptr, size32_t buflen, sockaddr* srcaddr, int& addrlen, uint_t flag, ustimen_t timeOut )
    {
        if ( GetFD() == NULL_SOCKET )
        {
            return -1;
        }
        
        if ( srcaddr == NULL )
        {
            return -1;
        }
        
        //char inBuf[ udpEthMaxSize ];
        struct rudpHeader* head = ( struct rudpHeader* )buffer_;
        struct rudpHeader  ackHead;
        //struct sockaddr_in sai;
        //int saiSize = sizeof( sockaddr_in );
        int readSize, err;
        //assert( bufSize <= rudpMaxSize );
        num_recv_ += 1;
        
        for ( ;; )
        {
            if ( timeOut != 0 )
            {
                if ( !readReadyWait( GetFD(), timeOut ) )
                {
                    //warn( "rudpReceive timed out\n" );
                    errno = ETIMEDOUT;
                    return -1;
                }
            }

            TS_Lock lock( &rudp_ut_lock_ );

            if ( length_ < sizeof( rudpHeader ) + buflen )
            {
                SAFE_DELETE_ARRAY( buffer_ );
                length_ = sizeof( rudpHeader ) + ( buflen * 2 );
                buffer_ = new ubyte_t[length_];
            }

            readSize = recvfrom( GetFD(), ( char* ) buffer_, length_, flag, ( struct sockaddr* )srcaddr, ( socklen_t* ) &addrlen );
            
            if ( readSize < 0 )
            {
                if ( errno == EINTR )
                {
                    continue;
                }
                
                //warn( "recvfrom error: %s", strerror( errno ) );
                num_fail_ += 1;
                return readSize;
            }
            
            if ( readSize < sizeof( *head ) )
            {
                //warn( "rudpRecieve truncated message" );
                continue;
            }

            if( readSize != ( head->sendLen + sizeof( *head ) ) )
            {
                //warn( "rudpRecieve truncated message" );
                continue;
            }

            if( ( head->type != rudpData ) )
            {
                if ( head->type != rudpAck )
                {
                    //warn( "skipping non-data message %d in rudpReceive", head->type );
                }

                continue;
            }

            std::string key = MakeKey( ( struct sockaddr* )srcaddr, &ackHead );
            // todo
            // 可能是另一頭timeout 重送的 需先判斷 這邊有沒有收過
            if ( head->resendcnt > 0 )
            {
                // 沒收到ack重送的資料 忽略它
                if ( head->id <= SearchAckId( &acked_reg_, &key ) )
                {
                    return -1;
                }
            }

            ackHead = *head;
            ackHead.type = rudpAck;
            InsertAck( &acked_reg_, &key, head->id );
            err = sendto( GetFD(), ( char* ) &ackHead, sizeof( ackHead ), 0, ( struct sockaddr* )srcaddr, ( addrlen ) );
            
            if ( err < 0 )
            {
                //warn( "problem sending ack in rudpRecieve: %s", strerror( errno ) );
            }
            
            readSize -= sizeof( *head );
            
            if ( readSize > buflen )
            {
                //warn( "read more bytes than have room for in rudpReceive" );
                readSize = buflen;
            }
            
            __pfmemcpy( bufptr, head + 1, readSize );
            break;
        }
        
        return readSize;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //int RUDPSocket::SendnTo( const ubyte_t* bufptr, size32_t buflen, const sockaddr* srcaddr, int addrlen, uint_t flag )
    //{
    //    const ubyte_t* send_ptr = bufptr;
    //    size32_t       send_cnt = buflen;
    //    
    //    while ( send_cnt > 0 )
    //    {
    //        int cnt = SendTo( send_ptr, send_cnt, srcaddr, addrlen, flag );
    //        
    //        if ( cnt < 0 )
    //        {
    //            return -1;
    //        }
    //        
    //        send_ptr += cnt;
    //        send_cnt -= cnt;
    //    }
    //    
    //    return buflen;
    //}
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //int RUDPSocket::RecvnFrom( ubyte_t* bufptr, size32_t buflen, sockaddr* srcaddr, int& addrlen, uint_t flag, ustimen_t timeOut )
    //{
    //    ubyte_t* recv_ptr = bufptr;
    //    size32_t recv_cnt = buflen;
    //    
    //    while ( recv_cnt > 0 )
    //    {
    //        int cnt = RecvFrom( recv_ptr, recv_cnt, srcaddr, addrlen, flag, timeOut );
    //        
    //        if ( cnt < 0 )
    //        {
    //            return -1;
    //        }
    //        
    //        recv_ptr += cnt;
    //        recv_cnt -= cnt;
    //    }
    //    
    //    return buflen;
    //}


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void RUDPSocket::Polling( void* data )
    {
        net::RUDPSocket* pRUDPFd = ( net::RUDPSocket* )data;
        // wait until either socket has data ready to be recv()d( timeout 3.5 secs )
        while ( pRUDPFd->IsLive() )
        {
            pfstd::MSSleep( pRUDPFd->check_alive_ );

            // copy
            RUDPSocket::acked_reg acked_uid = pRUDPFd->acked_reg_;
            mstimen_t currtime = pftml::Time::GetTickMSTime();
            for ( RUDPSocket::acked_reg::iterator it = acked_uid.begin(); it != acked_uid.end(); it++ )
            {
                if ( ( currtime - it->second[0] ) > pRUDPFd->check_alive_ )
                {
                    TS_Lock lock( &pRUDPFd->rudp_ut_lock_ );
                    RemoveAck( &pRUDPFd->acked_reg_, &it->first );
                }
            }
            pfstd::MSSleep( 1000 );
        }
    }    
}