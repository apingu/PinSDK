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
//

//#include "CsSConnection.h"
#include "CstGameAccount.h"
#include "CsEntityManage.h"

#include <assert.h>

namespace Cyberspace
{

	using namespace GSO;

    //===============================================================================//
    //
    //
    //===============================================================================//
	CsGameObject::CsGameObject( connectionid_t id, CsConnectionDescriptor s )
		//: CsIdentity( id )
	{
		m_CId = id;
		m_Usedcount=0;
		//TS_Lock lock( &m_lOwnfd );
		m_Ownfd = s;
		//m_CreateTime = s->GetLastCreateTime();
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	CsGameObject::~CsGameObject()
	{
		Clear();
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void CsGameObject::Clear( void )
	{
		TS_Lock lock( &m_lLive );
		m_Usedcount=0;
		//CsIdentity::Clear();
		std::map< entityid_t, _cstLPENTITYDESCRIPTOR >::iterator iter = m_Entities.begin();
		while( iter!=m_Entities.end() )
		{
			CsEntityManage::DeleteEntity( iter->second );
			iter++;
		}
		m_Entities.clear();
		//m_JoinEntity.clear();
		return;
	}


//    //===============================================================================//
//    //
//    //
//    //===============================================================================//
//	void CsGameObject::Release( void )
//	{
//		TS_Lock lock( &m_lOwnfd );
//	    m_Ownfd=NULL;
//	}


    //===============================================================================//
    //
    //
    //===============================================================================//
//	void CsGameObject::Used( void )
//	{
//		m_JoinLock.Lock();
//			
//
//	}
//
//	void CsGameObject::Free( void )
//	{
//		m_JoinLock.Unlock();
//	}



    //===============================================================================//
    //建立一個新的ENTITY
    //
    //===============================================================================//	
	_cstLPENTITYDESCRIPTOR CsGameObject::CreateEntity( entityid_t eid, const char* name, const byte_t* propbuf, size_t proplen )
	{
		if( IsCsInvalidId(eid) )
			return 0;

		TS_Lock lock( &m_lLive );
		std::map< entityid_t, _cstLPENTITYDESCRIPTOR >::iterator iter = m_Entities.find( eid );
		if( iter!= m_Entities.end() )
		{
			Beep( 1000, 1000 );
			//assert( 0 && "Entity already in this account!!" );
			printf( "Entity %lld already in account %lld!\n", eid, m_CId );
			return 0;
		}

		//u_int s= g_EntityManage.GetEntityCount();


		_cstLPENTITYDESCRIPTOR e=CsEntityManage::GetInstance(eid).GetEntityDescriptor( eid );
		if( e!=NULL )
		{
			//assert( 0 && "Entity already exist!!" );
			printf( "Entity %lld already exist!\n", eid );
			Beep( 1000, 1000 );
			//m_Entities.insert( std::map< connectionid_t, GENTITYDP >::value_type( eid, e ) );
			return 0;
		}
		
		e = CsEntityManage::GetInstance(eid).CreateEntity( eid, name, propbuf, proplen, m_CId, m_Ownfd, CsEF_MASTER );
		//entity->m_AId=m_CId;
		//entity->m_Ownfd=m_Ownfd;
		
		m_Entities.insert( std::map< entityid_t, _cstLPENTITYDESCRIPTOR >::value_type( eid, e ) );
		return e;

	}
		
										 
										 
    //===============================================================================//
   	//刪除一個ENTITY
    //
    //===============================================================================//
	void CsGameObject::DeleteEntity( _cstLPENTITYDESCRIPTOR& e )
	{
		TS_Lock lock( &m_lLive );
		assert( e!=0 && "entity dispitcher can't be 0 " );

		//RemoveENTITYDP(e);
//		CsEntity* entity = g_EntityManage.GetCsEntityByDesp( e );
//		if( entity == NULL )
//		{
//			assert( e!=0 && "entity dispitcher can't be find " );
//			return;
//		}
		
		connectionid_t eid = CsEntityManage::GetEntityId(e);
		m_Entities.erase( eid );
		CsEntityManage::DeleteEntity( e );
	}




    //===============================================================================//
    //
    //
    //===============================================================================//
	_cstLPENTITYDESCRIPTOR CsGameObject::GetEntity( entityid_t eid )
	{
		TS_Lock lock( &m_lLive );
		std::map< entityid_t, _cstLPENTITYDESCRIPTOR >::iterator iter = m_Entities.find( eid );
		//find resource
		if( iter != m_Entities.end() )
		{
			return iter->second;
		}

		return NULL;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void CsGameObject::ForAllEntity( CsEntityDescriptorCallBack udels, void* pData )
	{
		TS_Lock lock( &m_lLive );

		uint_t i = 0;
		std::map<entityid_t, _cstLPENTITYDESCRIPTOR>::iterator iter = m_Entities.begin();
		while (iter != m_Entities.end())
		{
			udels( i, iter->second, pData );
			i++;
			iter++;
		}
		return;
	}


	//===============================================================================//
    //
    //
    //===============================================================================//
	u_int CsGameObject::JoinSphere( RegionId_t rid )
	{
		TS_Lock lock( &m_lSphere );
		std::map< RegionId_t, u_int >::iterator iter=m_Sphere.find(rid);
		if( iter!=m_Sphere.end() )
		{
			return iter->second++;
		}
		else
		{
			m_Sphere.insert( std::map< RegionId_t, u_int >::value_type(rid, 1) );
		}
		return 1;
	}


	//===============================================================================//
    //
    //
    //===============================================================================//
	u_int CsGameObject::ExitSphere( RegionId_t rid )
	{
		TS_Lock lock( &m_lSphere );
		std::map< RegionId_t, u_int >::iterator iter=m_Sphere.find(rid);
		if( iter!=m_Sphere.end() )
		{
			iter->second--;
			if( iter->second==0 )
			{
				m_Sphere.erase(iter);
				return 0;
			}
			else
			{
				return iter->second;
			}
		}
		return 0;
	}


	//===============================================================================//
    //
    //
    //===============================================================================//
	bool CsGameObject::IsRegionExist( RegionId_t rid )
	{
		TS_Lock lock( &m_lSphere );
		std::map< RegionId_t, u_int >::iterator iter=m_Sphere.find(rid);
		if( iter!=m_Sphere.end() )
			return true;
		return false;
	}
	
//	//===============================================================================//
//    //
//    //
//    //===============================================================================//
//	bool CsGameObject::EnterEntity( entityid_t eid, void* prop, size_t len )
//	{
//		TS_Lock lock( &m_JoinLock );
//
//		std::map< RegionId_t, u_int > iter = m_Sphere.find( rid );
//		if( iter!= m_Sphere.end() )
//		{
//			// 
//			return false;
//		}
//		//m_JoinEntity.insert( std::set<connectionid_t>::value_type( eid) );
//		return true;
//	}
//	//===============================================================================//
//    //
//    //
//    //===============================================================================//
//	bool CsGameObject::DepartEntity( entityid_t eid )
//	{
//		TS_Lock lock( &m_JoinLock );
//
//		std::map< RegionId_t, u_int > iter = m_Sphere.find( rid );
//		if( iter!= m_Sphere.end() )
//		{
//			return false;
//		}
//
//		m_JoinEntity.erase( eid );
//		return true;
//	}




//    //===============================================================================//
//    //
//    //
//    //===============================================================================//
//	bool CsGameObject::FindEntity( connectionid_t eid )
//	{
//		TS_Lock lock( &m_JoinLock );
//		std::set<connectionid_t>::iterator iter = m_JoinEntity.find( eid );
//		if( iter!= m_JoinEntity.end() )
//			return true;
//		return false;
//	}
    //===============================================================================//
    //
    //
    //===============================================================================//
	int CsGameObject::GetUsed( void )
	{
		TS_Lock lock( &m_lUsed );
		return m_Usedcount;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	int CsGameObject::AddUsed( int i )
	{
		TS_Lock lock( &m_lUsed );
		m_Usedcount+=i;
		return m_Usedcount;
	}


	//===============================================================================//
	//
	//
	//===============================================================================//
	void CsGameObject::WaitForSingleUsed( int i )
	{
		while(1)
		{
			sleep(100);
			TS_Lock lock( &m_lUsed );
			if( m_Usedcount<=0 )
			{
				m_Usedcount=i;
				return;
			}
			
		}
	}

//	//===============================================================================//
//    //
//    //
//    //===============================================================================//
//	bool CsGameObject::JoinEntity( entityid_t eid )
//	{
//		TS_Lock lock( &m_JoinLock );
//		std::set<entityid_t>::iterator iter = m_JoinEntity.find( eid );
//		if( iter!= m_JoinEntity.end() )
//		{
//			return false;
//		}
//		m_JoinEntity.insert( std::set<connectionid_t>::value_type( eid) );
//		return true;
//	}
//
//    //===============================================================================//
//    //
//    //
//    //===============================================================================//
//    bool CsGameObject::ExitEntity( entityid_t eid )
//	{
//		TS_Lock lock( &m_JoinLock );
//		std::set<entityid_t>::iterator iter = m_JoinEntity.find( eid );
//		if( iter== m_JoinEntity.end() )
//		{
//			return false;
//		}
//		m_JoinEntity.erase( eid );
//		return true;
//	}

};
