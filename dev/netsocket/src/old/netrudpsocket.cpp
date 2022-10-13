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
#include "NetRUDPSocket.h"
#include <PfStdlib.h>
#include <memio.inl>


namespace Net
{
    /////////////////////////////////////////////////////////////////////////////////////////
    // Reliable UDP
    struct hdr
    {
        unsigned int seq;
        unsigned int ts;
    } sendhdr, recvhdr;
    
    enum rudpType
    {
        rudpAck = 199,	      // Acknowledge message.
        rudpData = 200,	      // Message with some data.
    };
    
    // The header to a rudp message.
    struct rudpHeader
    {
        dword_t  id;		  // Message id.  Returned with ack.
        dword_t  sendSec;	  // Time sent in seconds.  Returned with ack.
        dword_t  sendMicro;	  // Time sent microseconds. Returned with ack.
        ubyte_t  type;		  // One of rudpType above.
        ubyte_t  reserved1;	  // Reserved, always zero for now.
        ubyte_t  reserved2;	  // Reserved, always zero for now.
        ubyte_t  reserved3;	  // Reserved, always zero for now.
    };
    
    typedef int rudpHost;     // The IP address (in host order) of another computer. */
    
#define udpEthMaxSize 1444
    // Max data size that will fit into a single ethernet packet after UDP and IP
    // headers.  Things are faster & more reliable if you stay below this
    
#define rudpMaxSize (udpEthMaxSize - sizeof(struct rudpHeader)  )
    
#define MAX_TIME_OUT 999999
    
    
    //=============================================================================
    // Name :
    // Desc : Return difference in microseconds between t1 and t2.  t2 must be
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
    // Name :
    // Desc : Wait for descriptor to have some data to read, up to
    //        given number of microseconds.
    //=============================================================================
    static bool readReadyWait( int sd, int microseconds )
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
                if ( errno == EINTR )	/* Select interrupted, not timed out. */
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
                return readyCount > 0;	/* Zero readyCount indicates time out */
            }
        }
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    RUDPSocket::RUDPSocket()
    {
        //RTT::rtt_init((RTT::rtt_info *) &m_RttTime);
        //memset( &m_Sendhdr, 0, sizeof( m_Sendhdr ) );
        //memset( &m_Recvhdr, 0, sizeof( m_Recvhdr ) );
        m_length = udpEthMaxSize;
        m_buffer = new ubyte_t[ m_length ];
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    RUDPSocket::~RUDPSocket()
    {
        m_length = 0;
        SAFE_DELETE_ARRAY( m_buffer );
    }
    
    //=============================================================================
    // Name :
    // Desc : Tell system about a time-out.
    //=============================================================================
    void RUDPSocket::TimedOut()
    {
        m_timeOut <<= 1;   /* Back off exponentially. */
        
        if ( m_timeOut >= MAX_TIME_OUT )
        {
            m_timeOut = MAX_TIME_OUT;
        }
    }
    
    //=============================================================================
    // Name :
    // Desc : Return calculation of time out based on current data.
    //=============================================================================
    int RUDPSocket::CalcTimeOut()
    {
        int timeOut = m_rttAve + ( m_rttVary << 2 );
        
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
    // Name :
    // Desc : Add info about round trip time and based on this recalculate time outs.
    //=============================================================================
    void RUDPSocket::AddRoundTripTime( int time )
    {
        int delta;
        m_rttLast = time;
        delta = time - m_rttAve;
        m_rttAve += ( delta >> 3 );		      /* g = 1/8 */
        
        if ( delta < 0 )
        {
            delta = -delta;
        }
        
        m_rttVary += ( ( delta - m_rttVary ) >> 2 );  /* h = 1/4 */
        m_timeOut = CalcTimeOut();
    }
    
    //=============================================================================
    // Name :
    // Desc :  Wait for acknowledgement to the message we just sent.
    //         The set should be zeroed out. Only wait for up to
    //         ru->timeOut microseconds.   Prints a message and returns FALSE
    //         if there's a problem.
    //=============================================================================
    bool RUDPSocket::GetOurAck( struct timeval* startTv )
    {
        struct rudpHeader head;
        int readSize;
        int timeOut = m_timeOut;
        
        for ( ;; )
        {
            // Set up select with our time out.
            int dt;
            struct timeval tv;
            
            if ( readReadyWait( GetFD(), timeOut ) )
            {
                /* Read message and if it's our ack return true.   */
                readSize = recvfrom( GetFD(), ( char* ) &head, sizeof( head ), 0, NULL, NULL );
                
                if ( readSize >= sizeof( head ) && head.type == rudpAck && head.id == m_lastId )
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
            timeOut = m_timeOut - timeDiff( startTv, &tv );
            
            if ( timeOut <= 0 )
            {
                return FALSE;
            }
        }
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    socket_t RUDPSocket::Create( _IN int domain )
    {
        socket_t sd = Socket::Create( domain, SOCK_DGRAM, IPPROTO_UDP );
        
        if ( sd < 0 )
        {
            perror( "Couldn't open socket in rudpOpen %s" );
            return NULL;
        }
        
        m_rttVary = 250;	        // Initial variance 250 microseconds.
        m_timeOut = CalcTimeOut();
        m_maxRetries = 7;
        return sd;
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::Close( void )
    {
        return Socket::Close();
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    bool RUDPSocket::IsLive( void )
    {
        return ( GetFD() == 0 ) ? false : true;
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::SendTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN unsigned int flag )
    {
        struct timeval sendTv;	// Current time.
        
        if ( m_length < sizeof( rudpHeader ) + addrlen )
        {
            SAFE_DELETE_ARRAY( m_buffer );
            m_length = sizeof( rudpHeader ) + ( addrlen * 2 );
            m_buffer = new ubyte_t[ m_length ];
        }
        
        //char outBuf[ udpEthMaxSize ];
        struct rudpHeader* head;
        int fullSize = buflen + sizeof( *head );
        int i, err = 0, maxRetry = m_maxRetries;
        // Make buffer with header in front of message.
        // At some point we might replace this with a scatter/gather
        // iovector.
        m_sendCount += 1;
        //assert( buflen <= rudpMaxSize );
        head = ( struct rudpHeader* )m_buffer;
        memcpy( head + 1, bufptr, buflen );
        head->id = ++m_lastId;
        head->type = rudpData;
        
        // Go into send/wait for ack/retry loop.
        for ( i = 0; i < maxRetry; ++i )
        {
            gettimeofday( &sendTv, NULL );
            head->sendSec = sendTv.tv_sec;
            head->sendMicro = sendTv.tv_usec;
            err = sendto( GetFD(), ( char* ) m_buffer, fullSize, flag, ( struct sockaddr* )srcaddr, addrlen );
            
            if ( err < 0 )
            {
                // Warn, wait, and retry.
                struct timeval tv;
                //warn( " sendto problem %s", strerror( errno ) );
                tv.tv_sec = 0;
                tv.tv_usec = m_timeOut;
                select( 0, NULL, NULL, NULL, &tv );
                m_resendCount += 1;
                TimedOut();
                continue;
            }
            
            if ( GetOurAck( &sendTv ) )
            {
                return err;
            }
            
            TimedOut();
            m_resendCount += 1;
        }
        
        if ( err >= 0 )
        {
            err = ETIMEDOUT;
            //warn( "rudpSend timed out" );
        }
        
        m_failCount += 1;
        return -1;
    }
    
    //=============================================================================
    // Name :
    // Desc : Read message into buffer of given size.  Returns actual size read on
    //        success. On failure prints a warning, sets errno, and returns -1.
    //        Also returns ip address of message source. If timeOut is nonzero,
    //        it represents the timeout in milliseconds.  It will set errno to
    //        ETIMEDOUT in this case.
    //=============================================================================
    int RUDPSocket::RecvFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN unsigned int flag, int timeOut )
    {
        if ( GetFD() == NULL_SOCKET )
        {
            return -1;
        }
        
        if ( srcaddr == NULL )
        {
            return -1;
        }
        
        if ( m_length < sizeof( rudpHeader ) + buflen )
        {
            SAFE_DELETE_ARRAY( m_buffer );
            m_length = sizeof( rudpHeader ) + ( buflen * 2 );
            m_buffer = new ubyte_t[ m_length ];
        }
        
        //char inBuf[ udpEthMaxSize ];
        struct rudpHeader* head = ( struct rudpHeader* )m_buffer;
        struct rudpHeader  ackHead;
        //struct sockaddr_in sai;
        //int saiSize = sizeof( sockaddr_in );
        int readSize, err;
        //assert( bufSize <= rudpMaxSize );
        m_receiveCount += 1;
        
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
            
            readSize = recvfrom( GetFD(), ( char* ) m_buffer, m_length, flag, ( struct sockaddr* )srcaddr, ( socklen_t* ) &addrlen );
            
            if ( readSize < 0 )
            {
                if ( errno == EINTR )
                {
                    continue;
                }
                
                //warn( "recvfrom error: %s", strerror( errno ) );
                m_failCount += 1;
                return readSize;
            }
            
            if ( readSize < sizeof( *head ) )
            {
                //warn( "rudpRecieve truncated message" );
                continue;
            }
            
            if ( head->type != rudpData )
            {
                if ( head->type != rudpAck )
                {
                    //warn( "skipping non-data message %d in rudpReceive", head->type );
                }
                
                continue;
            }
            
            ackHead = *head;
            ackHead.type = rudpAck;
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
            
            IOMEMCPY( bufptr, head + 1, readSize );
            break;
        }
        
        return readSize;
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::SendnTo( const ubyte_t* bufptr, size32_t buflen, const sockaddr* srcaddr, int addrlen, unsigned int flag )
    {
        const ubyte_t* send_ptr = bufptr;
        size32_t       send_cnt = buflen;
        
        while ( send_cnt > 0 )
        {
            int cnt = SendTo( send_ptr, send_cnt, srcaddr, addrlen, flag );
            
            if ( cnt < 0 )
            {
                return -1;
            }
            
            send_ptr += cnt;
            send_cnt -= cnt;
        }
        
        return buflen;
    }
    
    //=============================================================================
    // Name :
    // Desc :
    //=============================================================================
    int RUDPSocket::RecvnFrom( ubyte_t* bufptr, size32_t buflen, sockaddr* srcaddr, int& addrlen, unsigned int flag, int timeOut )
    {
        ubyte_t* recv_ptr = bufptr;
        size32_t recv_cnt = buflen;
        
        while ( recv_cnt > 0 )
        {
            int cnt = RecvFrom( recv_ptr, recv_cnt, srcaddr, addrlen, flag, timeOut );
            
            if ( cnt < 0 )
            {
                return -1;
            }
            
            recv_ptr += cnt;
            recv_cnt -= cnt;
        }
        
        return buflen;
    }
    
}