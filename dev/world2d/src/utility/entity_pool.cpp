//Surface.cpp base map class
//
//
// static -->> - (ID) - 2
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
#include "entity_pool.h"
#include <vector>
#include <string>
//#include <T_Lock.h>
//#include <pf-cstring.h>
//#include <file_operator.h>
//#include <fpstring.h>
//#include <Windows.h>





namespace GSO
{
    struct entity_t
    {
        EntityPool* Room;
        void* pData;
        std::string Name;
        DataIter    Iter;
    };


#define CHECK_FORMAT(e)      (((entity_t*)(e))->Room == this)? true: false
#define GET_ENTITYT(e)       (((entity_t*)e)->pData)
#define DELETE_ENTITY_T(e)   e->pData = NULL; e->Room = NULL; delete e //e->Iter.clear();


    ///////////////////////////////////////////////////////////////////////////////
    // surface class
    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //template <class void*>
    EntityPool::EntityPool()
    {
        m_pType = 0;
        m_pEntitySel = NULL;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //template <class void*>
    EntityPool::~EntityPool()
    {
        //Release();
        //EntityEraseAll();
        //EntityClear();
        EraseAllEntity();
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void EntityPool::SetFocusEntity( GENTITYDP fEntity )
    {
        TS_Lock lock( &s_EntityPoolLock );
        if ( fEntity == NULL )
        {
            return;
        }

        if ( !CHECK_FORMAT( fEntity ) )
            return;
        m_pEntitySel = fEntity;
        return;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void* EntityPool::GetFocusEntity( void )
    {
        TS_Lock lock( &s_EntityPoolLock );
        if ( m_pEntitySel != NULL )
            return GET_ENTITYT( m_pEntitySel );
        return NULL;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void* EntityPool::operator[]( unsigned int id )
    {
        TS_Lock lock( &s_EntityPoolLock );

        if ( id >= m_pEntitySeqs.size() )
            return NULL;
        //
        DataIter Iter = m_pEntitySeqs.begin();
        for ( unsigned int i = 0; i < id; i++ )
            Iter++;

        return GET_ENTITYT( ( *Iter ) );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void* EntityPool::GetEntityBuffer( GENTITYDP entity )
    {
        TS_Lock lock( &s_EntityPoolLock );
        if ( entity == NULL )
            return NULL;

        if ( !CHECK_FORMAT( entity ) )
            return NULL;

        return GET_ENTITYT( entity );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int EntityPool::TestEntity( void* pdata )
    {
        DataIter Iter = m_pEntitySeqs.begin();
        for ( unsigned int i = 0; i < m_pEntitySeqs.size(); i++ )
        {
            entity_t* e = (entity_t*)( *Iter );
            if ( pdata == e->pData )
                return i;
            Iter++;
        }
        return -1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void EntityPool::EntityClear( void )
    {
        TS_Lock lock( &s_EntityPoolLock );

        DataIter Iter = m_pEntitySeqs.begin();
        for ( unsigned int i = 0; i < m_pEntitySeqs.size(); i++ )
        {
            //entity_t* e = (entity_t*)(Iter.m_pNode->Value);
            entity_t* e = (entity_t*)( *Iter );
            DELETE_ENTITY_T( e );
            //++i;
            Iter++;
        }
        m_pEntitySeqs.clear();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    GENTITYDP EntityPool::InsertEntity( void* entity, const tchar_t* name )
    {
        TS_Lock lock( &s_EntityPoolLock );
        entity_t* e = new entity_t;

        e->pData = entity;
        if ( name != NULL )
        {
            e->Name = name;
        }
        e->Room = this;
        m_pEntitySeqs.push_back( (void*)e );
        e->Iter = m_pEntitySeqs.end();
        e->Iter--;

        return (GENTITYDP)e;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //template <class void*>
    void* EntityPool::RemoveEntityD( GENTITYDP& entity )
    {
        TS_Lock lock( &s_EntityPoolLock );
        if ( entity == NULL )
            return NULL;
        if ( !CHECK_FORMAT( entity ) )
            return NULL;

        entity_t* e = (entity_t*)entity;

        if ( m_pEntitySel == entity )
            m_pEntitySel = NULL;

        m_pEntitySeqs.erase( e->Iter );

        void* userdata = e->pData;
        DELETE_ENTITY_T( e );
        entity = NULL;

        return userdata;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //template <class void*>
    void* EntityPool::RemoveEntityU( unsigned int id )
    {
        TS_Lock lock( &s_EntityPoolLock );

        if ( id >= m_pEntitySeqs.size() )
            return NULL;

        DataIter Iter = m_pEntitySeqs.begin();
        for ( unsigned int i = 0; i < id; i++ )
            Iter++;

        entity_t* e = (entity_t*)( *Iter );
        void* userdata = e->pData;
        m_pEntitySeqs.erase( e->Iter );
        DELETE_ENTITY_T( e );

        return userdata;
    }


    /*
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void EntityPool::ForAllEntity(OnEntityBuffer usb)
    {
            for(uint_t i = 0; i < m_pEntitySeqs.size(); i++)
                    usb(m_pEntitySeqs[i].data.m_buf, m_pEntitySeqs[i].data.m_len);
            return;
    }
    */



    //=============================================================================
    // NAME: ForAllEntity
    // DESC: 對所有物件呼叫一個方法
    //=============================================================================
    void EntityPool::ForAllEntityBuffer( OnEntityBuffer udels, void* pData )//, void *pExcept)
    {
        TS_Lock lock( &s_EntityPoolLock );
        DataIter Iter = m_pEntitySeqs.begin();
        for ( unsigned int i = 0; i < m_pEntitySeqs.size(); i++ )
        {
            entity_t* e = (entity_t*)( *Iter );
            if ( !udels( i, e->pData, pData ) )
                break;
            Iter++;
        }
    }

    //=============================================================================
    // NAME: ForAllEntity
    // DESC: 對所有物件呼叫一個方法
    //=============================================================================
    void EntityPool::ForAllEntityDescriptor( OnEntityDescriptor udels, void* pData )
    {
        TS_Lock lock( &s_EntityPoolLock );
        DataIter Iter = m_pEntitySeqs.begin();

        for ( unsigned int i = 0; i < m_pEntitySeqs.size(); i++ )
        {
            entity_t* e = (entity_t*)( *Iter );
            if ( !udels( i, e, pData ) )
                break;
            Iter++;
        }
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void EntityPool::GetAllEntityBuffer( std::vector<void*>& retvec )
    {
        TS_Lock lock( &s_EntityPoolLock );
        DataIter Iter = m_pEntitySeqs.begin();
        while ( Iter != m_pEntitySeqs.end() )
        {
            entity_t* e = (entity_t*)( *Iter );
            retvec.push_back( e->pData );
            Iter++;
        }
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void EntityPool::GetAllEntityDescriptor( std::vector<GENTITYDP>& retvec )
    {
        TS_Lock lock( &s_EntityPoolLock );
        DataIter Iter = m_pEntitySeqs.begin();
        while ( Iter != m_pEntitySeqs.end() )
        {
            entity_t* e = (entity_t*)( *Iter );
            retvec.push_back( (GENTITYDP)e );
            Iter++;
        }
    }



    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void EntityPool::EraseAllEntity( OnEntityBuffer udelsb )
    {
        TS_Lock lock( &s_EntityPoolLock );
        DataIter Iter = m_pEntitySeqs.begin();
        for ( uint_t i = 0; i < m_pEntitySeqs.size(); ++i )
        {
            entity_t* e = (entity_t*)( *Iter );
            if ( udelsb != NULL )
            {
                udelsb( i, e->pData, NULL );
            }
            else
            {
                delete e->pData;
            }
            DELETE_ENTITY_T( e );
            Iter++;
        }

        m_pEntitySeqs.clear();
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    GENTITYDP EntityPool::FirstEntity( void )
    {
        TS_Lock lock( &s_EntityPoolLock );
        return m_pEntitySeqs.front();
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    GENTITYDP EntityPool::LastEntity( void )
    {
        TS_Lock lock( &s_EntityPoolLock );
        return m_pEntitySeqs.back();
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    /*
    void EntityPool::Drop(GENTITYDP entity)
    {
        TS_Lock lock(&s_EntityPoolLock);
        if(entity == NULL)
            return;
        if(!CHECK_FORMAT(entity))
            return;

        entity_t* e = (entity_t*)entity;
        //m_pEntitySeqs.drop(e->Iter);
    }
    */

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    /*
    void EntityPool::Push(GENTITYDP entity)
    {
        TS_Lock lock(&s_EntityPoolLock);
        if(entity == NULL)
            return;
        if(!CHECK_FORMAT(entity))
            return;

        entity_t* e = (entity_t*)entity;
        //m_pEntitySeqs.push(e->Iter);
    }
    */

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    /*
    void EntityPool::Swap(GENTITYDP a, GENTITYDP b)
    {
        TS_Lock lock(&s_EntityPoolLock);

        if((a == NULL) || (b == NULL))
            return;
        if(!CHECK_FORMAT(a))
            return;
        if(!CHECK_FORMAT(b))
            return;

        entity_t* ea = (entity_t*)a;
        entity_t* eb = (entity_t*)b;

        // by Karl 05/20/2004. to ensure the existence of a and b in m_pEntitySeqs////
        // We may test again and again until there are minimal errors and then change
        // back to the original code to improve performance
        //
        // original code
        //m_pEntitySeqs.swap(ea->Iter, eb->Iter);
        //
        // new codes
        /*
        if((m_pEntitySeqs.GetIndex(ea->Iter) != -1) &&
            (m_pEntitySeqs.GetIndex(eb->Iter) != -1)
            )
            m_pEntitySeqs.swap(ea->Iter, eb->Iter);
        else
            assert(0);
            */
            //////////////////////////////////////////////////////////////////////////////

            //return;
        //}


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
    void EntityPool::Move( GENTITYDP a, GENTITYDP b )
    {
        TS_Lock lock( &s_EntityPoolLock );

        if ( ( a == NULL ) || ( b == NULL ) )
            return;
        if ( !CHECK_FORMAT( a ) )
            return;
        if ( !CHECK_FORMAT( b ) )
            return;

        entity_t* ea = (entity_t*)a;
        entity_t* eb = (entity_t*)b;

        // by Karl 05/20/2004. to ensure the existence of a and b in m_pEntitySeqs////
        // We may test again and again until there are minimal errors and then change 
        // back to the original code to improve performance
        //
        // original code
        //m_pEntitySeqs.move(ea->Iter, eb->Iter);
        //
        // new codes
        /*
        if((m_pEntitySeqs.GetIndex(ea->Iter) != -1) &&
            (m_pEntitySeqs.GetIndex(eb->Iter) != -1)
            )
            m_pEntitySeqs.move(ea->Iter, eb->Iter);
        else
            assert(0);
        */

    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    EntityPool* __cdecl EntityPool::GetRoom( GENTITYDP e )
    {
        entity_t* entity = (entity_t*)e;
        return entity->Room;
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //template <class void*>
    /*
    void* EntityPool::operator[](uint_t i)
    {
            if(i == -1)
                    return m_pEntitySel;
            return (i < m_pEntitySeqs.size())? m_pEntitySeqs[i].data.m_buf : NULL;
    }
    */

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //template <class void*>
    /*
    void* EntityPool::Entity(int id, SearchRuleCallBack scb)
    {
            if(scb == NULL)
                    return operator[](id);

            for(uint_t i = 0; i < m_pEntitySeqs.size(); i++)
            {
                    if(scb(id, m_pEntitySeqs[i].data.m_buf))
                            return m_pEntitySeqs[i].data.m_buf;
            }
            return NULL;
    }
    */

    /*
    //=============================================================================
    // NAME: Flip_Scene()
    // DESC: exchange two scene
    //=============================================================================
    template <class void*> void EntityPool<void*>::FlipRoom(const char* fpath, int offset)
    {



    }
    */


    /*
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //template <class void*>
    void EntityPool::Link(const tchar_t* fname)
    {
        int ret = Surface_Compare_Name(fname);
        if(ret == -1)
            return;


            //Release();
            //EntityEraseAll();
         m_pEntitySeqs.clear();

        //Load(all_file_path_table[ret]);

         return;
    }
    */


    //==========================================================================================
    //
    //  save
    //
    //  =====================================
    //           BACKGROUND.PELM
    //  =====================================
    //           ELEMENTS.LIST
    //  =====================================
    //==========================================================================================
    //template <class void*>
    /*
    error_t EntityPool::Save(const tchar_t* filename, SaveDataCallBack savefunc)

    {
            //temp file for package element
            IOFilepath targetpath(filename);

            std::vector<const tchar_t*> merge_path;

            for(uint_t i = 0; i< m_pEntitySeqs.size(); i++)
            {
                    IOFilepath Entitypath;
                    //Entitypath.fmakepath(targetpath.dirve(), targetpath.dir(), (*Entityname)[i], "");
                    entity_t* e = (entity_t*)m_pEntitySeqs[i];
                    if(e->Name.empty())
                        Entitypath.fmakepath(targetpath.dirve(), targetpath.dir(), e->Name, "");
                    else
                    {
                            tchar_t Entityname[10];
                            sprintf(Entityname, "uint_t%d", i);
                            Entitypath.fmakepath(targetpath.dirve(), targetpath.dir(), Entityname, "");
                    }


                    if(savefunc != NULL)
                    {
                        FileOperator rf;
                        rf.open(Entitypath, "wb");
                        uint_t savelen = 0;
                        void* savedata = savefunc(savelen, e->pData);
                        if(savedata != NULL)
                        {
                            rf.write(savedata, savelen, 1);
                                delete savedata;
                        }
                        rf.close();
                    }

                    //else
                    //{
                    //    rf.write(e.data.m_buf, m_pEntitySeqs[i].data.m_len, 1);
                    //}
                    merge_path.push_back(Entitypath);
            }

            FileOperator tf;
            tf.open(filename, "wb");
            return tf.Packfiles(merge_path, "GS", _DELETE_SORUCE_FILE);

    }



    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    error_t EntityPool::Load(const tchar_t* filename, LoadDataCallBack loadfunc)
    {
        FileOperator loadf;
        loadf.open(filename);
        for(int i = 0; i<loadf.FileCount(); i++)
        {
            tchar_t name[_MAX_LFNAME];
            Pbyte *buf;
            //EntityBuffer Entitybuf;
            loadf.FileName(i, name, _MAX_LFNAME);
            int size = loadf.readfile(&buf, i);
            uint_t loadlen = 0;
            void* loaddata = loadfunc(loadlen, buf);
            InsertEntity(loaddata, name);
            if(loaddata != buf)
                delete [] buf;
        }
        return 0;
    }
    */


};//namespace GE

