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
#ifndef UNPRTT_H
#define UNPRTT_H

#include "unp.h"

extern int gettimeofday( struct timeval* tvp, struct timezone* tzp );

namespace RTT
{
    struct rtt_info
    {
        float        rtt_rtt;     //most recnt measured RTT, second
        float        rtt_srtt;    //smoothed RTT estimator,  second
        float        rtt_rttvar;  //smoothed mean deviation, second
        float        rtt_rto;     //current RTO to use, second
        int          rtt_nrexmt;  //times retransmitted: 0, 1, 2, ....
        unsigned int rtt_base;    //sec since 1/1/1970 at strat
    };
    
#define RTT_RXTMIN            2   //min retransmit timeout value, seconds 
#define RTT_RXTMAX           60   //min retransmit timeout value, seconds 
#define RTT_MAXNREXMT         3   //max #times to retransmit
    
    void         rtt_debug  ( struct rtt_info* );
    void         rtt_init   ( struct rtt_info* );
    void         rtt_newpack( struct rtt_info* );
    int          rtt_start  ( struct rtt_info* );
    void         rtt_stop   ( struct rtt_info* );
    int          rtt_timeout( struct rtt_info* );
    unsigned int rtt_ts     ( struct rtt_info* );
    
    extern int   rtt_d_flag;
};//namespace RTT


#endif//UNPRTT_H