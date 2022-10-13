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
#ifndef WGAMESERVER_H
#define WGAMESERVER_H

#pragma warning (disable : 4786)

#include <WServer.h>
#include <map>
#include <vector>
#include "WGameDefine.h"
//#include "LogFile.h"
#include "WZone.h"




namespace WNE
{
    class GameService : private TCPService
    {
        std::vector<Zone*> m_ZoneVec;
        FuncMessageChannel m_ChannelVec[MAX_MESSAGE_CHANNEL];

    public:

        GameService( char* port );
        ~GameService();

    public:


        /** publish message to one zone  */
        void PublishEntity( Entity* entity, u_int channel, u_int zoneid );


        /** Register message passed channel function */
        void RegisterChannel( u_int id, FuncMessageChannel func );

        /** deal message function */
        virtual void OnInitialing( void )
        {
        };
        virtual void OnLogin( const char* username, const char* password )
        {
        };
        virtual void OnLogout( const char* username, const char* password )
        {
        };
        virtual void OnEnterEntity( void* prop, u_int size )
        {
        };
        virtual void OnLeaveEntity( void* prop, u_int size )
        {
        };

        /** call back query function */
        virtual int OnGetZoneId( void* entity )
        {
            return -1;
        }
        virtual unsigned int OnGetZoneCount( int& zoneid )
        {
            return 0;
        }
        virtual unsigned int OnGetZoneNeightborhood( u_int zoneid,
                                                     std::vector<u_int>& zonesid )
        {
            return 0;
        }

    private:

        /** Start-up server. */
        bool StartUp( const char* configfile );

        /** Shutdown server. */
        void ShutDown();

        bool InitDB( const char* configfile );
        bool InitDSC( const char* configfile );
        bool InitWorld( const char* configfile );

        // inherit mumber function
        void OnInitiation( void );
        void OnShutdown( void );
        void OnAccept( WNE::IO::TCPSocket* socket );
        void OnKnockOut( WNE::IO::TCPSocket* socket );
        void OnReceive( WNE::IO::TCPSocket* socket );

        int Recvmsg( WNE::IO::TCPSocket* socket,
                     char* bp,
                     size_t len,
                     unsigned int flag = 0 );

    private:

        FuncMessageChannel m_MessageChannel[MAX_MESSAGE_CHANNEL];
    };
}





#endif //WGAMESERVER_H