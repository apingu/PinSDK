#include <CstRegion.h>

namespace Cyberspace
{
    //===============================================================
	//
	//
	//===============================================================
	CsRegion::CsRegion( RegionId_t id )
	{
		Id = id;
		m_Neighbors.push_back(id);
		printf( "Create Region Id: %d\n", id );
	}

	//===============================================================
	//
	//
	//===============================================================
	CsRegion::~CsRegion()
	{

	}

	//===============================================================
	//
	//
	//===============================================================
	RegionId_t CsRegion::GetId( void )
	{
		return Id;
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::InsertEntity( GENTITY edp )
	{
		TS_Lock lock( &m_UtLock );
		CsEntity* entity = (CsEntity*)EntityRoom::GetRoom( edp )->GetEntity( edp );
		m_Entities.insert( std::map<EntityId_t, GENTITY>::value_type( entity->GetId(), edp ) );
		entity->m_Region=this;
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::RemoveEntity( GENTITY edp )
	{
		TS_Lock lock( &m_UtLock );
		CsEntity* entity = (CsEntity*)EntityRoom::GetRoom( edp )->GetEntity( edp );
		std::map<EntityId_t, GENTITY>::iterator iter = m_Entities.find( entity->GetId() );
		if( iter != m_Entities.end() )
		{
			m_Entities.erase( iter );
		}
		else
		{
			assert( 0 && "RemoveEntity not exist" );
		}
		entity->m_Region = NULL;
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::ForAllEntity( EntityCallBack udels, void* pData )
	{
		TS_Lock lock(&m_UtLock);

		UINT i = 0;
		std::map<EntityId_t, GENTITY>::iterator iter = m_Entities.begin();
		while (iter != m_Entities.end())
		{
			udels( i, (void*)iter->second, pData );
			i++;
			iter++;
		}
	}

	//===============================================================
	//
	//
	//===============================================================
	size_t CsRegion::GetEntityCount( void )
	{
		return m_Entities.size();
	}
};

