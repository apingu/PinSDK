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
#ifndef ENTITYROOM_H
#define ENTITYROOM_H


#include <basedef.h>
#include <datadef.h>
#include <T_Lock.h>


namespace GSO
{

	typedef long GENTITY;
	// 這個傳回 TRUE 繼續, FALSE 結束
	// 得到放入的buffer
    typedef BOOL (*EntityBufferCallBack)( unsigned int order, void* pEntity, void* pData );
	typedef BOOL (*EntityDescriptorCallBack)( unsigned int order, GENTITY pEntity, void* pData );
	//typedef Pbool (*SearchRuleCallBack)( int id, void* pData );

	class EntityRoom
	{
		UINT               m_pType;
		GENTITY            m_pEntitySel;
		T_Lock             m_Lock;
		
		//bool               m_bLock;

	protected:

		DataList           m_pEntitySeqs;
		
		void               SetFocusEntity   ( GENTITY fEntity );
		void*              GetFocusEntity   ( void );

	public:

		EntityRoom();
		~EntityRoom();

		//void*              GetEntityByID      ( int id );
		void*              operator[]         ( unsigned int id );
		void*              GetEntity          ( GENTITY entity );
		int                TestEntity         ( void* pdata );
		BOOL               IsEntityEmpty      ( void ) { return m_pEntitySeqs.empty(); }
		UINT               EntityCount        ( void ) { return m_pEntitySeqs.size(); }
		GENTITY            InsertEntity       ( void* entity, const char* name=NULL );
		void*              RemoveEntityD      ( GENTITY& Entity );   
   		void*              RemoveEntityU      ( unsigned int i );
		void               EraseAllEntity     ( EntityBufferCallBack udels=NULL );
		// 對所有的 instance 呼叫 user function(但是可以有一個除外)
		void               ForAllEntity          ( EntityBufferCallBack udels, void* pData );//, void *pExcept=NULL );
		void               ForAllEntityDescriptor( EntityDescriptorCallBack udels, void* pData );//, void *pExcept=NULL );

		GENTITY            FirstEntity        ( void );
		GENTITY            LastEntity         ( void );
	
		void               Move               ( GENTITY a, GENTITY b );

		/*
		void               Swap               ( GENTITY a, GENTITY b );
		void               Drop               ( GENTITY entity );
		void               Push               ( GENTITY entity );
		*/
			
		void               EntityClear        ( void );
		
		static EntityRoom* __cdecl GetRoom    ( GENTITY a );

		//        error_t  Save               ( const char* filename, SaveDataCallBack savefunc );
		//virtual error_t  Load               ( const char* filename, LoadDataCallBack loadfunc );

	};

};


#endif
