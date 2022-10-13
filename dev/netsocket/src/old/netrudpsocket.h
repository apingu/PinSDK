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

#include "NetSocket.h"

namespace Net
{
    /////////////////////////////////////////////////////////////////////////////////////////
    // Reliable UDP
    class RUDPSocket: public Socket
    {
    private:
        T_Lock                  m_RUDPLock;
        stimen_t                m_LastRecvTime;   //!
        stimen_t                m_LastSendTime;   //!

        int                     m_rttLast;        // Last round trip time (RTT) for a message/ack in microseconds.
        int                     m_rttAve;         // Approximate average of recent RTTs.
        int                     m_rttVary;        // Approximate variation of recent RTTs.
        int                     m_timeOut;        // Ideal timeout for next receive.
        int                     m_receiveCount;   // Number of packets attempted to receive.
        int                     m_sendCount;      // Number of packets attempted to send.
        int                     m_resendCount;    // Number of resends.
        int                     m_failCount;      // Number of failures.
        uint32_t                m_lastId;         // Id number of last message sent.
        int                     m_maxRetries;     // Maximum number of retries per message.
        ubyte_t*                m_buffer;
        size32_t                m_length;


        void                    TimedOut();
        int                     CalcTimeOut();
        void                    AddRoundTripTime( int time );
        bool                    GetOurAck( struct timeval* startTv );

    public:

        RUDPSocket();
        ~RUDPSocket();

        virtual const tchar_t*  ObjectName() const { return _T( "RUDPSocket" ); }

        socket_t                Create( _IN int domain = AF_INET );
        //! 結束本 socket
        virtual int             Close( void );

        bool                    IsLive( void );

        virtual int             SendTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN unsigned int flag = 0 );

        virtual int             RecvFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN unsigned int flag = 0, _IN int timeOut = 0 );

        virtual int             SendnTo( _IN const ubyte_t* bufptr, _IN size32_t buflen, _IN const sockaddr* srcaddr, _IN int addrlen, _IN unsigned int flag = 0 );

        virtual int             RecvnFrom( _OUT ubyte_t* bufptr, _IN size32_t buflen, _OUT sockaddr* srcaddr, _INOUT int& addrlen, _IN unsigned int flag = 0, _IN int timeOut = 0 );
    };    
}
    
    
    