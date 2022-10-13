//surface.h header file of surface.cpp
//
// surface
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
#ifndef ENTITYPOOL_H
#define ENTITYPOOL_H

#include <pf-def.h>
#include <data_def.h>
#include <vector>
#include <T_Lock.h>


namespace GSO
{
    typedef void* GENTITYDP;
    // 這個傳回 TRUE 繼續, FALSE 結束
    // 得到放入的buffer
    typedef bool ( _CALLBACK *OnEntityBuffer )( unsigned int order, void* pEntity, void* pData );
    typedef bool ( _CALLBACK *OnEntityDescriptor )( unsigned int order, GENTITYDP pEntity, void* pData );
    //typedef Pbool (*SearchRuleCallBack)(int id, void* pData);

    class EntityPool
    {
        uint_t             m_pType;
        GENTITYDP          m_pEntitySel;
        T_Lock             s_EntityPoolLock;


    protected:

        DataList           m_pEntitySeqs;

        void               SetFocusEntity( GENTITYDP fEntity );
        void*              GetFocusEntity( void );

    public:

        EntityPool();
        ~EntityPool();

        void*              operator[]         ( unsigned int id );
        void*              GetEntityBuffer( GENTITYDP entity );
        int                TestEntity( void* pEntity );
        bool               IsEntityEmpty( void ) { return m_pEntitySeqs.empty(); }
        uint_t             EntityCount( void ) { return m_pEntitySeqs.size(); }
        GENTITYDP          InsertEntity( void* entity, const tchar_t* name = NULL );
        void*              RemoveEntityD( GENTITYDP &Entity );
        void*              RemoveEntityU( unsigned int i );
        void               EraseAllEntity( OnEntityBuffer udels = NULL );
        // 對所有的 instance 呼叫 user function
        void               ForAllEntityBuffer( OnEntityBuffer udels, void* pData );//, void *pExcept=NULL);
        void               ForAllEntityDescriptor( OnEntityDescriptor udels, void* pData );//, void *pExcept=NULL);
        void               GetAllEntityBuffer( std::vector<void*>& retvec );
        void               GetAllEntityDescriptor( std::vector<GENTITYDP>& retvec );


        GENTITYDP          FirstEntity( void );
        GENTITYDP          LastEntity( void );

        void               Move( GENTITYDP a, GENTITYDP b );

        /*
        void               Swap               (GENTITYDP a, GENTITYDP b);
        void               Drop               (GENTITYDP entity);
        void               Push               (GENTITYDP entity);
        */

        void               EntityClear( void );

        static EntityPool* __cdecl GetRoom( GENTITYDP a );

        //        error_t  Save               (const char* filename, SaveDataCallBack savefunc);
        //virtual error_t  Load               (const char* filename, LoadDataCallBack loadfunc);

    };

};


#endif
