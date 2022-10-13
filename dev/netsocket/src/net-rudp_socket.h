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
#pragma once

#include <map>
#include "net-socket.h"

namespace net
{
    /////////////////////////////////////////////////////////////////////////////////////////
    // Reliable UDP

    class RUDPSocket: public Socket
    {
    public:

        typedef std::map<std::string, std::vector<uint64_t>>   acked_reg;

    private:

        T_Lock                  rudp_ut_lock_;
        stimen_t                stime_last_recv_;   //!
        stimen_t                stime_last_send_;   //!

        ustimen_t               rtt_last_;        // Last round trip time (RTT) for a message/ack in microseconds.
        int                     rtt_ave_;         // Approximate average of recent RTTs.
        int                     rtt_vary_;        // Approximate variation of recent RTTs.
        stimen_t                timeout_;         // Ideal timeout for next receive.
        int                     num_recv_;        // Number of packets attempted to receive.
        int                     num_send_;        // Number of packets attempted to send.
        int                     num_resend_;      // Number of resends.
        int                     num_fail_;        // Number of failures.
        uint64_t                last_uid_;        // Id number of last message sent.
        int                     max_retries_;     // Maximum number of retries per message.
        ubyte_t*                buffer_;
        size32_t                length_;

        mstimen_t               check_alive_;     // time for check alive
        acked_reg               acked_reg_;

        void                    TimedOut();
        int                     CalcTimeOut();
        void                    AddRoundTripTime( int time );
        bool                    GetOurAck( struct timeval* startTv );
        static void             Polling( void* data );

    public:

        RUDPSocket();
        ~RUDPSocket();

        virtual const ntchar_t* ObjectName() const { return _T( "RUDPSocket" ); }

        socket_t                Create( _IN int domain = AF_INET );
        //! 結束本 socket
        virtual int             Close( void );

        bool                    IsLive( void );

        // return read length
        int                     Waiting( _IN ustimen_t timeOut = 5000000 );

        // 現在 socket 收到的資料長度
        int                     GetReadLength();

        virtual int             SendTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN uint_t flag = 0 );

        virtual int             RecvFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN uint_t flag = 0, _IN ustimen_t timeOut = 0 );

        
        //virtual int             SendnTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN uint_t flag = 0 );

        //virtual int             RecvnFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN uint_t flag = 0, _IN ustimen_t timeOut = 0 );
    };    
}
    
    
    