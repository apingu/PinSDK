#include "CsRegion.h"
#include "CsEntityManage.h"
#include <assert.h>



namespace Cyberspace
{
	using namespace GSO;
    //	namespace GSO
	//	{
	//	    BOOL DeleteAllEntity( unsigned int order, void* pEntity, void* pData )
	//		{
	//		    CsEntity* e = (CsEntity*)pEntity;
	//		    SAFE_DELETE( e );
	//			return true;
	//		}
	//	};

	//===============================================================
	//
	//
	//===============================================================
	CsRegion::CsRegion( RegionId_t id )
	{
		Id.SetValue(id);
		//Id = id;
		InsertNeighbor(id);
		//m_Neighbors.push_back(id);
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
		return Id.GetValue();
	}


	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::InsertNeighbor( RegionId_t rid )
	{
		//m_Neighbors.insert( std::set<RegionId_t>::value_type(rid) );
		m_Neighbors.push_back(rid);
	}


	//===============================================================
	//
	//
	//===============================================================
	bool CsRegion::IsNeighborExist( RegionId_t rid )
	{
		//		std::set<RegionId_t>::iterator iter = m_Neighbors.find( rid );
		//		if( iter!=m_Neighbors.end() )
		//			return true;
		//		return false;
		for( UINT i=0; i<m_Neighbors.size(); i++ )
		{
			if( rid==m_Neighbors[i] )
				return true;
		}
		return false;
	}


	T_Lock g_lInsertEntity;
	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::InsertEntity( CsEntityDescriptor edp )
	{
		TS_Lock elock(&g_lInsertEntity);
		//CsEntity* entity = (CsEntity*)g_EntityManage.GetCsEntityByDesp( edp );
		CsRegion* region = g_EntityManage.GetCsEntityRegion(edp);
		if( region !=NULL )
		{
			if( region->GetId() != GetId() )
			{
				region->RemoveEntity( edp );
			}
		}
		TS_Lock lock( &m_Lock );
		m_Entities.insert( std::map<entityid_t, CsEntityDescriptor>::value_type( g_EntityManage.GetEntityId(edp), edp ) );
		g_EntityManage.SetCsEntityRegion(edp, this);
		return;
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::RemoveEntity( CsEntityDescriptor edp )
	{
		//		CsEntity* entity = g_EntityManage.GetCsEntityByDesp( edp );
		//
		//		if( entity==NULL )
		//		{
		//			assert( 0 && "RemoveEntity CsEntity not exist" );
		//			return;
		//		}

		CsRegion* region = g_EntityManage.GetCsEntityRegion(edp);

		if( region==NULL )
			return;

		if( region->GetId()!=GetId() )
		{
			region->RemoveEntity( edp );
			OutputDebugString( "RemoveEntity region not the same" );
			return;
		}

		TS_Lock lock( &m_Lock );
		std::map<entityid_t, CsEntityDescriptor>::iterator iter = region->m_Entities.find( g_EntityManage.GetEntityId(edp) );
		if( iter != region->m_Entities.end() )
		{
			region->m_Entities.erase( iter );
		}
		else
		{
			assert( 0 && "RemoveEntity not exist" );
			char buff[255];
			sprintf( buff, "************RemoveEntity %lld not exist in %d**************\n", g_EntityManage.GetEntityId(edp), region->GetId() );
		}
		g_EntityManage.SetCsEntityRegion(edp, NULL);
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::ForAllEntityBuffer( CsEntityBufferCallBack udels, void* pData )
	{
		TS_Lock lock(&m_Lock);

		UINT i=0;
		std::map<entityid_t, CsEntityDescriptor>::iterator iter = m_Entities.begin();
		while( iter != m_Entities.end() )
		{
			//udels( i, GET_ENTITY(iter->second)->GetPropbuf(), pData );
			//size_t len;
			udels( i,_GetResBufPtr( g_EntityManage.GetCsEntityPropbuf(iter->second )), pData );
			iter++;
			i++;
		}
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsRegion::ForAllEntityDescriptor( CsEntityDescriptorCallBack udels, void *data )
	{
		TS_Lock lock(&m_Lock);

		UINT i=0;
		std::map<entityid_t, CsEntityDescriptor>::iterator iter = m_Entities.begin();
		while( iter != m_Entities.end() )
		{
			udels( i, iter->second, data );
			iter++;
			i++;
		}
	}

	//===============================================================
	//
	//
	//===============================================================
	size_t CsRegion::GetEntityCount( void )
	{
		TS_Lock lock( &m_Lock );
		return m_Entities.size();
	}

};
