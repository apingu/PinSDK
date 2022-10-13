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
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <net/net-server.h>

#include "cs-def.h"
#include "cs-database_service.h"

//#include <map>


namespace cyberspace
{
    class HttpServer : public net::TCPServer
    {

        std::string         content_type_;

        vclass_t            v_object_[ 3 ];

    public:

        HttpServer();
        ~HttpServer();

        virtual int         StartUp( unsigned int allowplayers, unsigned short port );

        int                 Shutdown( void );

        // return content type
        void                SetContentType( const char* buf );

        // 驗證
        void                SetKeepAliveMstime( mstimen_t mst );
        int                 GetInspectedSize();
        

    protected:

        // 用這個來取代原本的 callback
        virtual void        OnAcceptSocket( _IN net::TCPSocket* s ) = 0;
        virtual void        OnKickoutSocket( _IN net::TCPSocket* s ) = 0;
        virtual _csbinbuf   OnReceiveSocket( _IN net::TCPSocket* s, _IN CsHttpEncType enctype, _IN const char_t* actstr, _IN const ubyte_t* comstr, _IN uint_t comlen ) = 0;

        // 請掛載
        virtual void        Update();

    private:

        net::TCPSocket*     OnNewSocket( void );
        void                OnDelSocket( net::TCPSocket* sk );

        void                OnAccept( _IN net::TCPSocket* s );
        //! call back when client cutoff connect
        void                OnKickout( _IN net::TCPSocket* s );
        //! call back when sock receive message
        void                OnReceive( _IN net::TCPSocket* s );

        //=============================================================================
        std::string         OnRecvOutputDebug( _IN  net::TCPSocket* s, _IN CsHttpEncType enctype, _IN const char* comstr, _IN int comlen );
    };
}