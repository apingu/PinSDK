//
//
//
//
//
//  http connection(TCP) socket
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

#include <pf-def.h>
#include <net/net-socket.h>

#include "cs-def.h"
#include "cs-if_session.h"

namespace http
{
    class Parser;
    class Writer;
}

namespace cyberspace
{
    class HttpCnSocket: public net::TCPSocket, public CsIfSession
    {
        typedef net::TCPSocket  BaseClass;

        http::Parser*     v_http_parser_;
        http::Writer*     v_http_writer_;
        CsHttpCodingType  context_coding_;
        CsHttpCodingType  accept_coding_;
        ubyte_t*          zip_buffer_;
        int               zip_length_;

    public:

        HttpCnSocket();
        ~HttpCnSocket();

        const ntchar_t*          ObjectName() const { return _T( "CsHttpCnSocket" ); }

        void                     Clear();

        virtual void             SetSessionUid( cs_session_uid_t sid );
        virtual cs_session_uid_t GetSessionUid();

        int                      RecvPacket( _OUT CsHttpEncType& enctype, _OUT _mstring& actstr, _OUT const ubyte_t** bp, _OUT cs_size_t& comlen );
        int                      SendPacket( _IN const _binbuf& response_str );
    };
}

typedef cyberspace::HttpCnSocket  CsHttpCnSocket;
