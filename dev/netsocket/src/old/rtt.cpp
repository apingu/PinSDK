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
#include "unprtt.h"
//#include <PSTimer.h>

namespace RTT
{
    int rtt_d_flag = 0;  //debug flag; can be set nonzero by caller
    
#define RTT_RTOCALC(ptr)   ((ptr)->rtt_srtt + (4.0*(ptr)->rtt_rttvar))
    
    
    static float rtt_minmax( float rto )
    {
        if ( rto < RTT_RXTMIN )
        {
            rto = RTT_RXTMIN;
        }
        else if ( rto > RTT_RXTMAX )
        {
            rto = RTT_RXTMAX;
        }
        
        return rto;
    }
    
    void rtt_init( struct rtt_info* ptr )
    {
        struct timeval tv;
        gettimeofday( &tv, NULL );
        ptr->rtt_base = tv.tv_sec;// sec since 1/1/1970 at start
        ptr->rtt_rtt = 0;
        ptr->rtt_srtt = 0;
        ptr->rtt_rttvar = 0.75;
        ptr->rtt_rto = rtt_minmax( ( float )RTT_RTOCALC( ptr ) );
        //first RTO at (srtt+4*rttvar)) = 3 seconds
    }
    
    unsigned int rtt_ts( struct rtt_info* ptr )
    {
        unsigned int ts;
        struct timeval tv;
        gettimeofday( &tv, NULL );
        ts = ( ( tv.tv_sec - ptr->rtt_base ) * 1000 ) + ( tv.tv_usec / 1000 );
        return ts;
    }
    
    void rtt_newpack( struct rtt_info* ptr )
    {
        ptr->rtt_nrexmt = 0;
    }
    
    int rtt_start( struct rtt_info* ptr )
    {
        return ( ( int )( ptr->rtt_rto + 0.5 ) ); //round float to int
        //return value can be used as: alarm(rtt_start(&foo))
    }
    
    void rtt_stop( struct rtt_info* ptr, UINT ms )
    {
        double delta;
        ptr->rtt_rtt = ms / 1000.0; //measured RTT in seconds
        delta = ptr->rtt_rtt - ptr->rtt_srtt;
        ptr->rtt_srtt += delta / 8;
        
        if ( delta < 0.0 )
        {
            delta = -delta;
        }
        
        ptr->rtt_rttvar += ( delta - ptr->rtt_rttvar ) / 4;
        ptr->rtt_rto = rtt_minmax( RTT_RTOCALC( ptr ) );
    };
    
    int rtt_timeout( struct rtt_info* ptr )
    {
        ptr->rtt_rto *= 2;
        
        if ( ++ptr->rtt_nrexmt > RTT_MAXNREXMT )
        {
            return -1;
        }
        
        return 0;
    }
};//namespace RTT



