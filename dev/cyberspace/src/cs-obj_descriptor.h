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


#define _CSOBJDESCRIPTOR_TYPE_NULL      0

namespace cyberspace
{
    //////////////////////////////////////////////////////////////////////////
    //
    typedef void( _CALLBACK *OnCsObjDescriptorReference )( uniqueid_t uid, void** pobj, int cnt );
    
    class __SLAPI ObjDescriptor
    {
        void*     headle_;
        
    public:
    
        ObjDescriptor();
        ObjDescriptor( _IN uniqueid_t objuniqueid, _IN unsigned short objtypeid, _IN void* pobj, _IN OnCsObjDescriptorReference pobjreferencefunc );
        ObjDescriptor( _IN const ObjDescriptor& objdescriptor );
        ~ObjDescriptor();
        
        operator uniqueid_t();
        operator void* ();
        
        ObjDescriptor& operator =( _IN const ObjDescriptor& objdescriptor );
        
        /*
        bool operator == (ulonglong_t &objuniqueid);
        bool operator != (ulonglong_t &objuniqueid);
        */
        
        uniqueid_t         GetUniqueId()      const;
        unsigned short     GetTypeId()        const;
        void*              GetObj()           const;
        mstimen_t          GetCreateMsTimer() const;
    };
}