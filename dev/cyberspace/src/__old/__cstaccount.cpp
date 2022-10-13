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
#include "CstAccount.h"
//#include "CstEntity.h"
#include "CsEntityManage.h"

#include <assert.h>

namespace Cyberspace
{
	using namespace GSO;

    //===============================================================================//
    //
    //
    //===============================================================================//
	CsAccount::CsAccount( ConnectionId_t id )
	{ 
		m_AId     = id;
		m_Username="";
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	CsAccount::~CsAccount()
	{
		Clear();
		//delete accname;
		/*
        std::map< EntityId_t, CsEntity* >::iterator iter = m_Entities.begin();
	    while( iter != m_Entities.end() )
		{
		    SAFE_DELETE( iter->second );
			iter++;
		}
		*/
		//m_Username[0]='\0';	// clear, there may be some bug
		//SAFE_DELETE_ARRAY( m_Username );
		
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	ConnectionId_t CsAccount::GetId( void )
	{
		return m_AId;
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	const char*  CsAccount::GetUsername( void )
	{
		return m_Username.c_str();
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	void CsAccount::Clear( void )
	{
		TS_Lock lock( &m_Lock );
		m_AId=CsINVALID_ID;      //每次分配的
		m_Privilege=0;   // 權力代號
		m_Entities.clear();
		m_JoinEntity.clear();
		m_Username="";
		return;
	}
	
	

    //===============================================================================//
    //建立一個新的ENTITY
    //
    //===============================================================================//	
    
	GSO::GENTITY CsAccount::CreateEntity( EntityId_t eid, const char* name, void* propbuf, size_t proplen )
	{
		TS_Lock lock( &m_Lock );
		std::map< EntityId_t, GENTITY >::iterator iter = m_Entities.find( eid );
		if( iter!= m_Entities.end() )
		{
			Beep( 1000, 1000 );
			//assert( 0 && "Entity already in this account!!" );
			return 0;
		}
		
		if( g_EntityManage.GetCsEntityByEId(eid)!=NULL )
		{
			//assert( 0 && "Entity already exist!!" );
			Beep( 1000, 1000 );
			return 0;
		}
		
		GSO::GENTITY e = g_EntityManage.CreateEntity( eid, name, propbuf, proplen );
		CsEntity* entity = g_EntityManage.GetCsEntityByDesp( e );
		entity->m_AId=m_AId;
		entity->m_Ownfd=m_Ownfd;
		
		m_Entities.insert( std::map< EntityId_t, GENTITY >::value_type( entity->GetId(), e ) );
		return e;

	}
		
										 
										 
    //===============================================================================//
   	//刪除一個ENTITY
    //
    //===============================================================================//
	void CsAccount::DeleteEntity( GSO::GENTITY& e )
	{
		TS_Lock lock( &m_Lock );
		assert( e!=0 && "entity dispitcher can't be 0 " );

		//RemoveGEntity(e);
		CsEntity* entity = g_EntityManage.GetCsEntityByDesp( e );
		if( entity == NULL )
		{
			assert( e!=0 && "entity dispitcher can't be find " );
			return;
		}
		
		EntityId_t eid = entity->GetId();
		
		m_Entities.erase( eid );
		g_EntityManage.DeleteEntity( e );

	}




    //===============================================================================//
    //
    //
    //===============================================================================//
	GSO::GENTITY CsAccount::GetEntity( EntityId_t eid )
	{
		TS_Lock lock( &m_Lock );
		std::map< EntityId_t, GSO::GENTITY >::iterator iter = m_Entities.find( eid );
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
	void CsAccount::SetPrivilege( int pri )
	{
		m_Privilege = pri;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	int CsAccount::GetPrivilege( void )
	{
		return m_Privilege;
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	bool CsAccount::JoinEntity( EntityId_t eid )
	{
		TS_Lock lock( &m_Lock );
		std::set<EntityId_t>::iterator iter = m_JoinEntity.find( eid );
		if( iter!= m_JoinEntity.end() )
		{
			return false;
		}
		m_JoinEntity.insert( std::set<EntityId_t>::value_type( eid) );
		return true;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	bool CsAccount::FindEntity( EntityId_t eid )
	{
		//TS_Lock lock( &m_Lock );
		std::set<EntityId_t>::iterator iter = m_JoinEntity.find( eid );
		if( iter!= m_JoinEntity.end() )
			return true;
		return false;
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
    void CsAccount::ExitEntity( EntityId_t eid )
	{
		TS_Lock lock( &m_Lock );
		m_JoinEntity.erase( eid );
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void CsAccount::ForAllEntity( GSO::EntityCallBack udels, void* pData )
	{
		TS_Lock lock( &m_Lock );

		UINT i = 0;
		std::map<EntityId_t, GENTITY>::iterator iter = m_Entities.begin();
		while (iter != m_Entities.end())
		{
			udels( i, (void*)iter->second, pData );
			i++;
			iter++;
		}
		return;
	}

}

