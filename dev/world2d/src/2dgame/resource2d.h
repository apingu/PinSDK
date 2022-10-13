//Intellect.h header file of Intellect.cpp
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
//        $(PIN_HOME),$(PIN_HOME)\Base,$(PIN_HOME)\Platform\Win32,$(PIN_HOME)\2DEngine,$(PIN_HOME)\Game,$(PIN_HOME)\2DGame
//
//                                               Copyright    (C)    1999    Pin
#ifndef RESOURCE2D_H
#define RESOURCE2D_H

#pragma warning (disable : 4786)

#include <map>
#include <pf-cstring.h>
#include <graphic\icon.h>

namespace GSO
{
    class Resource2d
    {
        typedef struct
        {
            dword_t   Weight;
            mstimen_t  DieTime;
            CPIcon*   Icon;
        } ResUnit;
        
        _tstring                m_filename;
        ushort_t                  m_LoadedSum;              //目前載入的圖片數
        
        uint_t                    m_ResSum;
        uint_t                    m_ReferSum;               //目前使用資源的總數
        uint_t                    m_ReferCounter;
        
        mstimen_t                 m_TimeLive;               //清除已載入資源的時區
        
        std::map<uint_t, GSO::Resource2d::ResUnit> m_ResBuf;
        
    public:
    
        Resource2d();
        ~Resource2d();
        
        static  Resource2d* Create( const tchar_t* path );
        static  bool        Destroy( Resource2d* resource );
        virtual void        Save( const tchar_t* path );
        
        const char*         GetFilename() {return m_filename.c_str();}
        
        bool                Empty( void );
        
        uint_t              GetResCount( void );
        uint_t              GetLoadedCount( void );
        
        CPIcon*             GetRes( uint_t index );
        
        int                 Update( void );
        
        void                SetLive( dword_t s );
        
    };
};//namespace GSO

#endif
