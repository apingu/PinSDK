//
//
//
//
//    用於 csprotocol 的介面
//    讓TCP 跟 UDP 抽象化
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

#include "cs-def.h"

namespace cyberspace
{
    class IfSocket
    {

    public:

        // 為了給 Protocol 使用
        virtual int             Send( _IN const ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag = 0 ) = 0;
        virtual int             Recv( _OUT ubyte_t* bp, _IN _cs_size_t len, _IN unsigned int flag = 0 ) = 0;
        //virtual macn_t   GetMac() = 0;
    };
}

typedef cyberspace::IfSocket CsIfSocket;
