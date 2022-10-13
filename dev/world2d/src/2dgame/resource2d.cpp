// Intellect.cpp game element compute judege class
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
//                                               Copyright    (C)    1999    Pin
//

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-time.h>
#include <pf-timer.h>
#include <gadget\file_operator.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "resource2d.h"
#include <resourcemgr.h>


#if !defined(_WINDOWS)
    #include <pf-string.h>
#endif


namespace GSO
{
//#define ANIM_ID(a, d)           ((a * GetMaxAspect()) + d)

    //////////////////////////////////////////////////////////////////////////////////////////////
    // sprite class
    
    static ResourceMgr* gResource2dMgr = NULL;
    pftml::Timer             gResClock;
    
    //static std::vector<GSO::Resource2d*> gResource2dList;
    
    //=============================================================================
    // NAME:
    // DESC: constructor
    //=============================================================================
    Resource2d::Resource2d()
    {
        //m_ReferSum   = 0;
        //m_ResBuf=NULL;
        //m_ImageCounter = 0;
        //m_CloneConut = 0;
        m_TimeLive = 180000;          //清除已載入資源的時區
        //m_TimeWeight = 20;          //資源的時區加權值
        m_ReferSum = 0;               //目前使用資源的總數
        m_ReferCounter = 0;
        m_ResSum = 0;
        m_LoadedSum = 0;
        //m_ResBuf.release_element();
        m_ResBuf.clear();
    }
    
    
    //=============================================================================
    // NAME:
    // DESC: disturctor
    //=============================================================================
    Resource2d::~Resource2d()
    {
        //m_AnimTable.Release();
        //if(m_ReferSum == 0)
        //return;
        //if((*m_ReferSum) == 1)
        //ImageListRelease();
        //(*m_ReferSum)--;
        Resource2d::Destroy( this );
    }
    
    //=============================================================================
    // NAME: ()
    // DESC:
    //=============================================================================
    Resource2d* Resource2d::Create( const tchar_t* path )
    {
        Resource2d*    resource = NULL;
        
        if ( gResource2dMgr == NULL )
        { gResource2dMgr = new ResourceMgr; }
        
        if ( ( resource = ( Resource2d* )gResource2dMgr->Create( path ) ) == NULL )
        {
            FileSystem pf;
            
            if ( pf.open( path ) == NULL )
            {
                Pf_ERROR( "Resource2d::Create: allot image box file %s failure -_-b", ( tchar_t* ) path );
                return NULL;
            }
            
            resource = new Resource2d;
            resource->m_filename = path;
            resource->m_ResSum = pf.FileCount();
            gResource2dMgr->Insert( path, resource );
        }
        
        //else
        //resource->m_CloneConut++;
        resource->m_ReferSum = gResource2dMgr->GetReferSum( path );
        return resource;
        /*
        for(uint_t i = 0; i<gResource2dList.size(); i++)
        {
            if(strcmp(gResource2dList[i]->m_fname, path) == 0)
            {
                gResource2dList[i]->m_CloneConut++;
                return gResource2dList[i];
            }
        }
        
        FileOperator pf;
        if(pf.open(path) == NULL)
        {
            tchar_t msg[_MAX_MSG];
            sprintf(msg,
                     "allot image box file %s failure -_-b",
                     (char*) path);
            DebugMessage(msg, "Resource2d");
            return NULL;
        }
        
        Resource2d* resource = new Resource2d;
        resource->ReAllotBuffer(pf.FileCount());
        
        //int load_count  = PRE_LOAD_SEQUENCE_COUNT;
        //uint_t load_count = m_BaseLoadedCount;
        //load_count = min(m_AnimTable.SequenceCount(), load_count);
        
        //for(int c = 0;c < load_count;c++)
        //    SequenceInsert(c);
        
        //m_BaseLoadedCount = m_LoadedSum;
        resource->m_fname = path;
        gResource2dList.push_back(resource);
        
        return resource;
        */
    }
    
    //=============================================================================
    // NAME: Clear_Counter()
    // DESC:
    //=============================================================================
    bool Resource2d::Destroy( Resource2d* resource )
    {
        if ( resource == NULL )
        { return FALSE; }
        
        bool ret = false;
        
        if ( gResource2dMgr->Destroy( resource->m_filename.c_str() ) != NULL )
        {
            //if(!resource->m_ResBuf.empty())
            //{
            std::map<uint_t, ResUnit>::iterator iter = resource->m_ResBuf.begin();
            
            while ( iter != resource->m_ResBuf.end() )
            {
                //uint_t i=iter->first;
                //ResUnit res = iter->second;
                SAFE_DELETE( iter->second.Icon );
                iter++;
            }
            
            resource->m_ReferSum = 0;
            resource->m_ResBuf.clear();
            resource->m_LoadedSum = 0;
            SAFE_DELETE( resource );
            //}
            ret = true;
        }
        else
        { resource->m_ReferSum = gResource2dMgr->GetReferSum( resource->m_filename.c_str() ); }
        
        if ( gResource2dMgr )
        {
            if ( gResource2dMgr->Emtpy() )
            {
                SAFE_DELETE( gResource2dMgr );
                ret = true;
            }
        }
        
        return ret;
        /*
        if(resource->m_CloneConut == 0)
        {
            for(uint_t i = 0; i<gResource2dList.size(); i++)
            {
                if(gResource2dList[i] == resource)
                    gResource2dList.erase(gResource2dList.begin()+i);
            }
        
            if(!resource->m_ResBuf.empty())
            {
                resource->m_ResBuf.release_element();
                resource->m_ResBuf.clear();
                resource->m_LoadedSum = 0;
                resource->m_BaseLoadedCount = 0;
            }
            return TRUE;
        }
        else
            resource->m_CloneConut--;
        return FALSE;
        */
    }
    
    
    //=============================================================================
    // NAME: Clear_Counter()
    // DESC:
    //=============================================================================
    void Resource2d::Save( const tchar_t* path )
    {
        //tchar_t ILpath[_MAX_LPATH];
        //_makepath(ILpath, drive, dir, "I", "L");
        FileSystem pf;
        std::vector<const tchar_t*> Imagefpath;
        std::map<uint_t, GSO::Resource2d::ResUnit>::iterator iter = m_ResBuf.begin();
        
        while ( iter != m_ResBuf.end() )
        {
            Imagefpath.push_back( iter->second.Icon->GetFilename() );
        }
        
        if ( pf.open( path, _T( "wb" ) ) == NULL )
        {
            Pf_ERROR( "Resource2d::Save: save file %s failure !!!", path );
            return;
        }
        
        pf.Packagefiles( Imagefpath );
        pf.close();
    }
    
    
    //=============================================================================
    // NAME: empty()
    // DESC:
    //=============================================================================
    bool Resource2d::Empty( void )
    {
        if ( m_ResSum == 0 )
        { return true; }
        
        return false;
        //return m_ResBuf.empty();
    }
    
    //=============================================================================
    // NAME: ()
    // DESC:
    //=============================================================================
    uint_t Resource2d::GetResCount( void )
    {
        return m_ResSum;
    }
    
    //=============================================================================
    // NAME: ()
    // DESC:
    //=============================================================================
    uint_t Resource2d::GetLoadedCount( void )
    {
        return m_LoadedSum;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    CPIcon* Resource2d::GetRes( uint_t index )
    {
        std::map<uint_t, GSO::Resource2d::ResUnit>::iterator iter = m_ResBuf.find( index );
        
        //std::map<uint_t, ResUnit>::iterator iter = m_ResBuf.begin();
        //find resource
        if ( iter != m_ResBuf.end() )
        {
            //iter->second.DieTime += m_TimeWeight;
            iter->second.Weight++;
            iter->second.DieTime = pftml::Time::GetTickMSTime();
            return iter->second.Icon;
        }
        else
        {
            ResUnit res;
            tchar_t fname[_MAX_LPATH];
            tchar_t nobuf[10];
            //****************************************************** may be have wrong?????
            res.Icon = ::new CPIcon;
            sprintf( fname, _T( "%s|%s" ), m_filename.c_str(), itoa( index, nobuf, 10 ) );
            
            if ( res.Icon->Load( fname ) == 1 )
            {
                m_LoadedSum++;
                res.DieTime = pftml::Time::GetTickMSTime();
                res.Weight = 0;
                m_ResBuf.insert( std::map<uint_t, ResUnit>::value_type( index, res ) );
                res.Icon->Id = index;
                return res.Icon;
            }
            else  // if read image file failure write message to debug file
            {
                SAFE_DELETE( res.Icon );
                Pf_ERROR( " Resource2d::GetRes: read image file %s failure !!!", fname );
                
                if ( index != 0 )
                { return GetRes( 0 ); }
                else
                { return NULL; }
            }
        }
        
        return NULL;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Resource2d::Update( void )
    {
        int ret = 1;
        
        if ( m_ReferCounter == m_ReferSum )
        {
            mstimen_t nowtick = pftml::Time::GetTickMSTime();
            std::map<uint_t, GSO::Resource2d::ResUnit>::iterator iter = m_ResBuf.begin();
            
            while ( iter != m_ResBuf.end() )
            {
                //release resource
                if ( ( nowtick - ( iter->second.DieTime ) ) > m_TimeLive )
                {
                    SAFE_DELETE( iter->second.Icon );
                    m_LoadedSum--;
                    std::map<uint_t, ResUnit>::iterator diter = iter;
                    iter++;
                    m_ResBuf.erase( diter );
                }
                else
                {
                    iter++;
                }
                
                /*
                #ifdef _DEBUG
                                tchar_t s[50];
                                sprintf(s, "Resource Size = %d\n", m_ResBuf.size());
                                OutputDebugStringA(s);
                #endif
                */
            }
            
            m_ReferCounter = 0;
        }
        else
        {
            m_ReferCounter++;
        }
        
        return ret;
    }
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    void Resource2d::SetLive( dword_t s )
    {
        m_TimeLive = s;
        return;
    }
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    /*
    void Resource2d::SetWeight(dword_t s)
    {
        m_TimeWeight = s;
        return;
    }
    */
    
    
};//namespace GE


