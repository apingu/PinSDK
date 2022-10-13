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

#include <PfDef.h>
#include <NetSocket.h>
#include "CsDef.h"
#include "CsIfSocket.h"


namespace Cyberspace
{
    class HttpCSocket: public Net::TCPSocket
    {
    public:
        const ntchar_t* ObjectName() const { return _T( "HttpCSocket" ); }
    };


}