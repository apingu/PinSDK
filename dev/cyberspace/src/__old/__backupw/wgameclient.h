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
#ifndef WGAMECLIENT_H
#define WGAMECLIENT_H

#include <WClient.h>
#include "WGameDefine.h"

namespace WNE
{
    class GameClient : public TCPClient
    {
    public:

        GameClient( char* ipaddr, char* port );
        ~GameClient();

        bool Login( const char* username, const char* password );
        bool Logout( void );

        int Sendmsg( const char* bp, size_t len, u_int channel );


        //call back function entry
        void RegisterChannel( u_int id, FuncMessageChannel func );
        virtual void OnEnterEntity( u_int id, void* pProp, size_t len )
        {
        };
        virtual void OnLeaveEntity( u_int id, void* pProp, size_t len )
        {
        };


    private:

        void OnKnockOut();
        void OnReceive();

        FuncMessageChannel m_MessageChannel[MAX_MESSAGE_CHANNEL];
    };//GameClient
};//namespace WNE



#endif
