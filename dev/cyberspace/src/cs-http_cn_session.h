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
#include "cs-http_server.h"

namespace http
{
    class Parser;
    class Writer;
}

namespace cyberspace
{

    class HttpCnSession: public CoreConnector
    {
        friend HttpServer;

        http::Parser*            v_http_parser_;
        http::Writer*            v_http_writer_;
        CsHttpContentType        content_type_type_;
        _cs_string               content_type_values_;
        CsHttpCodingType         context_coding_;
        CsHttpCodingType         accept_coding_;
        ubyte_t*                 zip_buffer_;
        int                      zip_length_;
        _cs_string               method_;

    public:

        HttpCnSession()
        {
            v_http_parser_ = NULL;
            v_http_writer_ = NULL;
            Clear();
        }

        ~HttpCnSession()
        {
            SAFE_DELETE( v_http_parser_ );
            SAFE_DELETE( v_http_writer_ );
        }

        const ntchar_t*  ObjectName() const { return ( "CsHttpCnSession" ); }

        void Clear()
        {
            SAFE_DELETE( v_http_parser_ );
            SAFE_DELETE( v_http_writer_ );
            v_http_parser_ = new http::Parser;
            v_http_writer_ = new http::Writer;
            context_coding_ = CsHttpEncType_Normal;
            accept_coding_ = CsHttpEncType_Normal;
            zip_buffer_ = NULL;
            zip_length_ = 0;
            content_type_type_ = CsHttpEncType_Unknown;
        }
    };
}

typedef cyberspace::HttpCnSession  CsHttpCnSession;
