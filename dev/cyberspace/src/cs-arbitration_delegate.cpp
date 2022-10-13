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

#include "cs-entity_def.h"

namespace cyberspace
{
    class ArbitrationDelegate
    {
    public:
        
        //ArbitrationDelegate();
        //~ArbitrationDelegate();
            
        virtual int   OnJoinSection( _IN CsSectionEntity* node ) = 0;     // 收到server 通知有一台section 加入
        virtual int   OnExitSection( _IN cs_section_uid_t sid ) = 0;      // 收到server 通知有一台section 離開 
        virtual void  OnSyncSection( _OUT CsSectionEntity& hnode ) = 0;
        //virtual bool  IsActive() = 0;            
    };
}