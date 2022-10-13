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
#include "EntityRoom.h"
#include <vector>
//#include <tString.h>
//#include <PFile.h>
//#include <fpstring.h>
//#include <Windows.h>



namespace GSO
{
	struct entity_t
	{
		EntityRoom* Room;
		void*       pData;
		std::string Name;
		DataIter    Iter;
		
	};

    #define CHECK_FORMAT( e )      ( ((entity_t*)(e))->Room == this )? true: false
	#define GET_ENTITYT( e )       ( ( (entity_t*)e )->pData )
	#define DELETE_ENTITY_T( e )   e->pData=NULL; e->Room=NULL; delete e //e->Iter.clear();


	///////////////////////////////////////////////////////////////////////////////
	// surface class
	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	//template <class void*>
	EntityRoom::EntityRoom()
	{
		m_pType=0;
		m_pEntitySel=NULL;
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	//template <class void*>
	EntityRoom::~EntityRoom()
	{
		//Release();
		//EntityEraseAll();
		//EntityClear();
		EraseAllEntity();
	}


	//=============================================================================
	//  Name :
	//  Desc :
	//=============================================================================
	void EntityRoom::SetFocusEntity( GENTITY fEntity )
	{
		TS_Lock lock(&m_Lock);
		if( fEntity == NULL )
		{
			m_pEntitySel = NULL;
			return;
		}
		if( !CHECK_FORMAT( fEntity ) )
			return;
	    m_pEntitySel = fEntity;
		return;
	}

	//=============================================================================
	//  Name :
	//  Desc :
	//=============================================================================
	Pdata EntityRoom::GetFocusEntity( void )
	{
		TS_Lock lock(&m_Lock);
		if( m_pEntitySel != NULL )
			return GET_ENTITYT( m_pEntitySel );
		return NULL;
	}

	//=============================================================================
	//  Name :
	//  Desc :
	//=============================================================================
	void* EntityRoom::operator[]( unsigned int id )
	{
		TS_Lock lock(&m_Lock);
				
		if( id >= m_pEntitySeqs.size() )
			return NULL;
		//
		DataIter Iter = m_pEntitySeqs.begin();
		for( unsigned int i=0; i<id; i++ )
			Iter++;

		return GET_ENTITYT( (*Iter) );
	}

	//=============================================================================
	//  Name :
	//  Desc :
	//=============================================================================
	void* EntityRoom::GetEntity( GENTITY entity )
	{
		TS_Lock lock(&m_Lock);
		if( entity == NULL )
			return NULL;

		if( !CHECK_FORMAT( entity ) )
			return NULL;

		return GET_ENTITYT( entity );
	}

	//=============================================================================
	//  Name :
	//  Desc :
	//=============================================================================
	int EntityRoom::TestEntity( void* pdata )
	{
	    DataIter Iter = m_pEntitySeqs.begin();
		for( unsigned int i=0; i<m_pEntitySeqs.size(); i++ )
		{
			entity_t* e = (entity_t*)(*Iter);
			if( pdata == e->pData )
				return i;
			Iter++;
		}
		return -1;
	}

	//=============================================================================
	//  Name :
	//  Desc :
	//=============================================================================
	void EntityRoom::EntityClear( void )
	{
		TS_Lock lock(&m_Lock);

	    DataIter Iter = m_pEntitySeqs.begin();
		for( unsigned int i=0; i<m_pEntitySeqs.size(); i++ )
		{
			//entity_t* e = (entity_t*)(Iter.m_pNode->Value);
			entity_t* e = (entity_t*)(*Iter);
			DELETE_ENTITY_T( e );
			//++i;
			Iter++;
		}
		m_pEntitySeqs.clear();
	}

	//=============================================================================
	//  Name :
	//  Desc :
	//=============================================================================
	GENTITY EntityRoom::InsertEntity( void* entity, const char* name )
	{
		TS_Lock lock(&m_Lock);
		entity_t* e = new entity_t;
        
		e->pData = entity;
		if( name != NULL )
		{
			e->Name = name;//new char[ strlen( name ) ];
			//strcpy( e->Name, name );
		}
		e->Room = this;
		m_pEntitySeqs.push_back( (void*)e );
		e->Iter=m_pEntitySeqs.end();
		e->Iter--;

		return (long)e;
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	//template <class void*>
	Pdata EntityRoom::RemoveEntityD( GENTITY& entity )
	{
		TS_Lock lock(&m_Lock);
		if( entity == NULL )
			return NULL;
		if( !CHECK_FORMAT( entity ) )
			return NULL;
		
		entity_t* e = (entity_t*)entity;
		
		if( m_pEntitySel == entity )
			m_pEntitySel=NULL;

		m_pEntitySeqs.erase( e->Iter );
		
		void* userdata = e->pData;
		DELETE_ENTITY_T( e );
		entity=NULL;

		return userdata;
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	//template <class void*>
	Pdata EntityRoom::RemoveEntityU( unsigned int id )
	{
		TS_Lock lock(&m_Lock);
		
		if( id >= m_pEntitySeqs.size() )
			return NULL;

		DataIter Iter = m_pEntitySeqs.begin();
		for( unsigned int i=0; i<id; i++ )
			Iter++;

		entity_t* e = (entity_t*)(*Iter);
		void* userdata = e->pData;
		m_pEntitySeqs.erase( e->Iter );
		DELETE_ENTITY_T( e );
	       
		return userdata;
	}


	/*
	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void EntityRoom::ForAllEntity( EntityBufferCallBack usb )
	{
			for( UINT i=0; i < m_pEntitySeqs.size(); i++ )
					usb( m_pEntitySeqs[i].data.m_buf, m_pEntitySeqs[i].data.m_len );
			return;
	}
	*/

	//=============================================================================
	//  Name : ForAllEntity
	//  Desc : 對所有物件呼叫一個方法
	//=============================================================================
	void EntityRoom::ForAllEntity( EntityBufferCallBack udels, void* pData )//, void *pExcept)
	{
		unsigned int i=0;
		TS_Lock lock(&m_Lock);
	    DataIter Iter = m_pEntitySeqs.begin();
		//for( unsigned int i=0; i<m_pEntitySeqs.size(); i++ )
		while( Iter!=m_pEntitySeqs.end() )
		{
			//entity_t* e = (entity_t*)(Iter.m_pNode->Value);
			entity_t* e = (entity_t*)(*Iter);
			//if(pExcept!=e->pData)
			//{
			if( !udels( i, e->pData, pData ) )
				break;
			//}
			//++i;
			Iter++;
			i++;
		}
		
		/*
		for( unsigned int i=0; i<m_pEntitySeqs.size(); i++ )
		{
			entity_t* e = (entity_t*)m_pEntitySeqs[i];
			if(pExcept!=e->pData)
				udels( i , e->pData, pData );
		}
		*/
	}

	//=============================================================================
	//  Name : ForAllEntity
	//  Desc : 對所有物件呼叫一個方法
	//=============================================================================
	void EntityRoom::ForAllEntityDescriptor( EntityDescriptorCallBack udels, void* pData )
	{
		TS_Lock lock(&m_Lock);
	    DataIter Iter = m_pEntitySeqs.begin();
		for( unsigned int i=0; i<m_pEntitySeqs.size(); i++ )
		{
			//entity_t* e = (entity_t*)(Iter.m_pNode->Value);
			entity_t* e = (entity_t*)(*Iter);
			//if(pExcept!=e->pData)
			//{
			if( !udels( i, (long)e, pData ) )
				break;
			//}
			//++i;
			Iter++;
		}
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void EntityRoom::EraseAllEntity( EntityBufferCallBack udelsb )
	{
		TS_Lock lock(&m_Lock);
		DataIter Iter = m_pEntitySeqs.begin();
		for( UINT i=0; i < m_pEntitySeqs.size(); ++i )
		{
			entity_t* e = (entity_t*)(*Iter);
			if( udelsb!=NULL )
			{
				udelsb( i , e->pData, NULL );
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
	//  Name : 
	//  Desc :
	//=============================================================================
	GENTITY EntityRoom::FirstEntity( void )
	{
		TS_Lock lock(&m_Lock);
		return (long)m_pEntitySeqs.front();
	}

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	GENTITY EntityRoom::LastEntity( void )
	{
		TS_Lock lock(&m_Lock);
		return (long)m_pEntitySeqs.back();
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
    /*
	void EntityRoom::Drop( GENTITY entity )
	{
		TS_Lock lock(&m_Lock);
		if( entity == NULL )
			return;
		if( !CHECK_FORMAT( entity ) )
			return;

		entity_t* e = (entity_t*)entity;
		//m_pEntitySeqs.drop( e->Iter );
	}
	*/

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	/*
	void EntityRoom::Push( GENTITY entity )
	{
		TS_Lock lock(&m_Lock);
		if( entity == NULL )
			return;
		if( !CHECK_FORMAT( entity ) )
			return;

		entity_t* e = (entity_t*)entity;
		//m_pEntitySeqs.push( e->Iter );
	}
	*/

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	/*
	void EntityRoom::Swap( GENTITY a, GENTITY b )
	{
		TS_Lock lock(&m_Lock);

		if( ( a == NULL ) || (b == NULL ) )
			return;
		if( !CHECK_FORMAT( a ) )
			return;
		if( !CHECK_FORMAT( b ) )
			return;

		entity_t* ea = (entity_t*)a;
		entity_t* eb = (entity_t*)b;

		// by Karl 05/20/2004. to ensure the existence of a and b in m_pEntitySeqs////
		// We may test again and again until there are minimal errors and then change 
		// back to the original code to improve performance
		//
		// original code
		//m_pEntitySeqs.swap( ea->Iter, eb->Iter );
		//
		// new codes
		/*
		if( (m_pEntitySeqs.GetIndex(ea->Iter) != -1) &&
			(m_pEntitySeqs.GetIndex(eb->Iter) != -1)
			)
			m_pEntitySeqs.swap( ea->Iter, eb->Iter );
		else
			assert(0);
			*/
		//////////////////////////////////////////////////////////////////////////////

		//return;
	//}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void EntityRoom::Move( GENTITY a, GENTITY b )
	{
		TS_Lock lock(&m_Lock);

		if( ( a == NULL ) || (b == NULL ) )
			return;
		if( !CHECK_FORMAT( a ) )
			return;
		if( !CHECK_FORMAT( b ) )
			return;

		entity_t* ea = (entity_t*)a;
		entity_t* eb = (entity_t*)b;

		// by Karl 05/20/2004. to ensure the existence of a and b in m_pEntitySeqs////
		// We may test again and again until there are minimal errors and then change 
		// back to the original code to improve performance
		//
		// original code
		//m_pEntitySeqs.move( ea->Iter, eb->Iter );
		//
		// new codes
		/*
		if( (m_pEntitySeqs.GetIndex(ea->Iter) != -1) &&
			(m_pEntitySeqs.GetIndex(eb->Iter) != -1)
			)
			m_pEntitySeqs.move( ea->Iter, eb->Iter );
		else
			assert(0);
		*/

	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	EntityRoom* __cdecl EntityRoom::GetRoom( GENTITY e )
	{
		entity_t* entity = (entity_t*)e;
		return entity->Room;
	}

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	//template <class void*>
	/*
	void* EntityRoom::operator[]( UINT i )
	{
			if( i == -1 )
					return m_pEntitySel;
			return ( i < m_pEntitySeqs.size() )? m_pEntitySeqs[i].data.m_buf : NULL;
	}
	*/

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	//template <class void*>
	/*
	void* EntityRoom::Entity( int id, SearchRuleCallBack scb )
	{ 
			if( scb==NULL )
					return operator[]( id );

			for( UINT i = 0; i < m_pEntitySeqs.size(); i++ )
			{
					if( scb( id, m_pEntitySeqs[i].data.m_buf ) )
							return m_pEntitySeqs[i].data.m_buf;
			}
			return NULL;
	}
	*/

	/*
	//=============================================================================
	//  Name : Flip_Scene()
	//  Desc : exchange two scene
	//=============================================================================
	template <class void*> void EntityRoom<void*>::FlipRoom( const char* fpath, int offset )
	{



	}
	*/


	/*
	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	//template <class void*>
	void EntityRoom::Link( const char* fname )
	{
		int ret = Surface_Compare_Name(fname);
		if( ret == -1 )
			return;

        
			//Release();
			//EntityEraseAll();
		 m_pEntitySeqs.clear();

		//Load( all_file_path_table[ret] );

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
	error_t EntityRoom::Save( const char* filename, SaveDataCallBack savefunc )
                                                           
	{
			//temp file for package element
			IOFilepath targetpath( filename );

			std::vector<const char*> merge_path;

			for( UINT i=0; i< m_pEntitySeqs.size(); i++)
			{
					IOFilepath Entitypath;
					//Entitypath.fmakepath( targetpath.dirve(), targetpath.dir(), (*Entityname)[i], "" );
					entity_t* e = (entity_t*)m_pEntitySeqs[i];
					if( e->Name.empty() )
						Entitypath.fmakepath( targetpath.dirve(), targetpath.dir(), e->Name, "" );
					else
					{
							char Entityname[10];
							sprintf( Entityname, "UINT%d", i );
							Entitypath.fmakepath( targetpath.dirve(), targetpath.dir(), Entityname, "" );
					}


					if( savefunc != NULL )
					{
						IO::PFile rf;
						rf.open( Entitypath, "wb" );
						UINT savelen = 0;
						void* savedata = savefunc( savelen, e->pData );
						if( savedata != NULL )
						{
							rf.write( savedata, savelen, 1 );
								delete savedata;
						}
						rf.close();
					}

					//else
					//{
					//    rf.write( e.data.m_buf, m_pEntitySeqs[i].data.m_len, 1 );
					//}
					merge_path.push_back( Entitypath );
			}

			IO::PFile tf;
			tf.open( filename, "wb" );
			return tf.Packfiles( merge_path, "GS", _DELETE_SORUCE_FILE );

	}



	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	error_t EntityRoom::Load( const char* filename, LoadDataCallBack loadfunc )
	{
		IO::PFile loadf;
		loadf.open( filename );
		for( int i=0; i<loadf.FileCount(); i++ )
		{
			char name[_MAX_FNAME];
			Pbyte *buf;
			//EntityBuffer Entitybuf;
			loadf.FileName( i, name, _MAX_FNAME );
			int size = loadf.readfile( &buf, i );
			UINT loadlen = 0;
			void* loaddata = loadfunc( loadlen, buf );
			InsertEntity( loaddata, name );
			if( loaddata != buf )
				delete [] buf;
		}
		return 0;
	}
	*/


};//namespace GE

