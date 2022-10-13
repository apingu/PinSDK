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

#include "cs-def.h"

namespace cyberspace
{
    class __SLAPI AppSocket
    {

    public:

        virtual void              SetSessionUID( cs_session_uid_t sid ) = 0;
        virtual cs_session_uid_t  GetSessionUID() = 0;
    };
    typedef AppSocket IfSession;
}

typedef cyberspace::IfSession CsIfSession;
