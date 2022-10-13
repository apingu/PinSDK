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

//01.== C 系統文件/C++ 系統文件
#include <assert.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-time.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-obj_descriptor.h"


namespace cyberspace
{
    struct ObjDescriptorHeadle
    {
        uniqueid_t                      obj_uniqueid;
        unsigned short                  obj_typeid;
        mstimen_t                       create_mstime;
        void*                           obj;
        OnCsObjDescriptorReference      reference_func;
    };

    //=============================================================================
    //
    //
    //=============================================================================
    ObjDescriptor::ObjDescriptor()
    {
        ObjDescriptorHeadle* pHeadle = new ObjDescriptorHeadle;
        pHeadle->obj_uniqueid = CsINVALID_ID;
        pHeadle->obj_typeid = _CSOBJDESCRIPTOR_TYPE_NULL;
        pHeadle->obj = NULL;
        pHeadle->create_mstime = pftml::Time::GetTickMSTime();
        pHeadle->reference_func = NULL;

        if( pHeadle->reference_func )
        {
            pHeadle->reference_func( pHeadle->obj_uniqueid, &pHeadle->obj, 1 );
        }

        headle_ = pHeadle;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ObjDescriptor::ObjDescriptor( uniqueid_t objuniqueid, unsigned short objtypeid, void* pobj, OnCsObjDescriptorReference pobjreferencefunc )
    {
        //SAFE_DELETE(headle_);
        ObjDescriptorHeadle* pHeadle = new ObjDescriptorHeadle;
        pHeadle->obj_uniqueid = objuniqueid;
        pHeadle->obj_typeid = objtypeid;
        pHeadle->obj = pobj;
        pHeadle->create_mstime = pftml::Time::GetTickMSTime();
        pHeadle->reference_func = pobjreferencefunc;

        if( pHeadle->reference_func )
        {
            pHeadle->reference_func( pHeadle->obj_uniqueid, &pHeadle->obj, 1 );
        }

        headle_ = pHeadle;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ObjDescriptor::ObjDescriptor( const ObjDescriptor& objdescriptor )
    {
        headle_ = NULL;
        operator =( objdescriptor );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ObjDescriptor::~ObjDescriptor()
    {
        if( headle_ )
        {
            ObjDescriptorHeadle* pHeadle = ( ObjDescriptorHeadle* )headle_;

            if( pHeadle->reference_func )
            {
                pHeadle->reference_func( pHeadle->obj_uniqueid, &pHeadle->obj, -1 );
            }

            SAFE_DELETE( pHeadle );
        }
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ObjDescriptor::operator uniqueid_t()
    {
        if( headle_ )
        {
            return ( ( ObjDescriptorHeadle* )headle_ )->obj_uniqueid;
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ObjDescriptor::operator void* ( )
    {
        if( headle_ )
        {
            return ( ( ObjDescriptorHeadle* )headle_ )->obj;
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ObjDescriptor& ObjDescriptor::operator = ( const ObjDescriptor& objdescriptor )
    {
        ObjDescriptorHeadle* pHeadle = ( ObjDescriptorHeadle* )headle_;
        ObjDescriptorHeadle* pobjHeadle = ( ObjDescriptorHeadle* )objdescriptor.headle_;

        if( headle_ == NULL )
        {
            pHeadle = new ObjDescriptorHeadle;
            headle_ = pHeadle;
        }
        else
        {
            pHeadle = ( ObjDescriptorHeadle* )headle_;

            // 不是同類或是未使用 不能設定
            if( ( pHeadle->obj_typeid != _CSOBJDESCRIPTOR_TYPE_NULL ) && ( pHeadle->obj_typeid != pobjHeadle->obj_typeid ) )
            {
                return ( *this );
            }

            if( pHeadle->reference_func )
            {
                pHeadle->reference_func( pHeadle->obj_uniqueid, &pHeadle->obj, -1 );
            } //原先使用的先-1
        }

        pHeadle->obj_uniqueid = pobjHeadle->obj_uniqueid;
        pHeadle->obj_typeid = pobjHeadle->obj_typeid;
        pHeadle->obj = pobjHeadle->obj;
        pHeadle->create_mstime = pobjHeadle->create_mstime;
        pHeadle->reference_func = pobjHeadle->reference_func;

        if( pHeadle->reference_func )
        {
            pHeadle->reference_func( pHeadle->obj_uniqueid, &pHeadle->obj, 1 );
        }

        return ( *this );
    }

    /*
    bool ObjDescriptor::operator == (ulonglong_t& objuniqueid)
    {
     if(objuniqueid==NULL)
     {
      if(headle_==NULL)
       return true;
      else if(((ObjDescriptorHeadle*)headle_)->obj_typeid==_CSOBJDESCRIPTOR_TYPE_NONE)
       return true;
     }

       if(headle_)
       {
      return ((ObjDescriptorHeadle*)headle_)->obj_uniqueid==objuniqueid;
       }
       return false;
    }

    bool ObjDescriptor::operator != (ulonglong_t& objuniqueid)
    {
     if(objuniqueid==NULL)
     {
      if(headle_==NULL)
       return false;
      else if(((ObjDescriptorHeadle*)headle_)->obj_typeid==_CSOBJDESCRIPTOR_TYPE_NONE)
       return false;
     }

       if(headle_)
       {
        return ((ObjDescriptorHeadle*)headle_)->obj_uniqueid!=objuniqueid;
       }
       return false;
    }
    */

    //=============================================================================
    //
    //
    //=============================================================================
    uniqueid_t ObjDescriptor::GetUniqueId() const
    {
        if( headle_ )
        {
            return ( ( ObjDescriptorHeadle* )headle_ )->obj_uniqueid;
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    unsigned short ObjDescriptor::GetTypeId() const
    {
        if( headle_ )
        {
            return ( ( ObjDescriptorHeadle* )headle_ )->obj_typeid;
        }

        return 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void* ObjDescriptor::GetObj() const
    {
        if( headle_ )
        {
            return ( ( ObjDescriptorHeadle* )headle_ )->obj;
        }

        return NULL;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    mstimen_t ObjDescriptor::GetCreateMsTimer() const
    {
        if( headle_ )
        {
            return ( ( ObjDescriptorHeadle* )headle_ )->create_mstime;
        }

        return 0;
    }
};