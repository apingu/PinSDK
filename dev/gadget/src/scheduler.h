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

#include <map>
#include <pf-timer.h>


namespace cyberspace
{
    namespace scheduler
    {
        typedef void( _CALLBACK *OnTask)( float frame );

        bool         BeginTask( _IN mstimen_t framecycle, _IN OnTask op_func );
        void         EndTask( _IN  OnTask op_func );
    };    
};