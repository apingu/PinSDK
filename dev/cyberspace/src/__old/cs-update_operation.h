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

#include <pf-timer.h>


namespace cyberspace
{

    class Scheduler
    {
        friend class CoreServer;
        
        pftml::Timer  timer_;
        void*         value_;

    public:
    
        Scheduler( _IN void* value );
        virtual ~Scheduler();
        
        void   BeginUpdate( _IN mstimen_t framecycle );
        float  TestUpdate( void );
        void*  GetValue( void ) { return value_; }       
    };
    
};