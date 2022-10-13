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

#include "TeraZonaEntityMgr.h"



////////////////////////////////////////////////////////////////////////////////
//

//=============================================================================
// name :
// desc :
//=============================================================================
CEntity::CEntity()
{
	m_MsgVec.depth( 10 );
	m_DelayBufSize = 0;
}

//=============================================================================
// name :
// desc :
//=============================================================================
void CEntity::Destroy( void )
{
	for( int i=0; i <m_MsgVec.size(); i++ )
		delete m_MsgVec[i];

	m_MsgVec.clear();

	//delete [] props;
	//size = 0;
}

//=============================================================================
// name :
// desc :
//=============================================================================
CPMessage* CEntity::Getmessage( void )
{
	int s = m_MsgVec.size();
	if( s <= 0 )
		return 0;

	int msgOrder = (s-1)-m_DelayBufSize;
	if( msgOrder < 0 )
		return NULL;
	//CPMessage* msg = m_MsgVec[s-m_DelayBufSize-1];
	//CPMessage* msg = m_MsgVec.pop();
	//m_MsgVec.erase( m_MsgVec.begin() );
	return m_MsgVec[msgOrder];
}

//=============================================================================
// name :
// desc :
//=============================================================================
void CEntity::Putmessage( CPMessage* msg )
{
	CPMessage* recv = m_MsgVec.push( msg );
	SAFE_DELETE( recv );
}


//=============================================================================
// name :
// desc :
//=============================================================================
Puint CEntity::MessageSize( void )
{
    return m_MsgVec.size();
}











////////////////////////////////////////////////////////////////////////////////
//

//=============================================================================
// name :
// desc :
//=============================================================================
CEntityMgr::CEntityMgr()
{




}


//=============================================================================
// name :
// desc :
//=============================================================================
CEntityMgr::~CEntityMgr()
{
	for( int i =0; i < m_EntityVec.size(); i++ )
	{
		m_EntityVec[i]->Destroy();
		delete m_EntityVec[i];
	}
	m_EntityVec.clear();
}


//=============================================================================
// name :
// desc :
//=============================================================================
CEntity* CEntityMgr::operator[]( int id ) 
{
	return m_EntityVec[id];
}


//=============================================================================
// name :
// desc :
//=============================================================================
int CEntityMgr::EntitySize( void )
{
	return m_EntityVec.size();
}


//=============================================================================
// name :
// desc :
//=============================================================================
void CEntityMgr::AddEntity( int eID, Pbyte* props, int size )
{
	CEntity* entity = new CEntity;
	entity->eID = eID;
	entity->DisplaySize = size;
	entity->DisPlayprop = props;
	//entity->props = new Pbyte[size];
	//memcpy( entity->props, props, size );
	//m_EntityVec.push_back( entity );

}

//=============================================================================
// name :
// desc :
//=============================================================================
void CEntityMgr::DelEntity( int eID )
{
	for( int i=0; i < m_EntityVec.size(); i++ )
	{
		if( m_EntityVec[i]->eID == eID )
		{
			m_EntityVec[i]->Destroy();
			delete m_EntityVec[i];
			m_EntityVec.erase( m_EntityVec.begin()+i );
			return;
		}
	}
	return;
}

//=============================================================================
// name :
// desc :
//=============================================================================
CEntity* CEntityMgr::GetEntity( int eID )
{
	for( int i=0; i < m_EntityVec.size(); i++ )
	{
		if( m_EntityVec[i]->eID == eID )
		{
			return m_EntityVec[i];
		}
	}
	return 0;
}


/*
//=============================================================================
// name :
// desc :
//=============================================================================
int CEntityMgr::PutMsg( int eID, Pbyte* msg, int size )
{
	for( int i=0; i < m_EntityVec.size(); i++ )
	{
		if( m_EntityVec[i].id == eID )
		{
			m_EntityVec[i].PushMessage( msg, size );
			return 1;
		}
	}
	return 0;
}


//=============================================================================
// name :
// desc :
//=============================================================================
Puint CEntityMgr::GetMsg( int eID, Pbyte** msg )
{
	for( int i=0; i < m_EntityVec.size(); i++ )
	{
	    if( m_EntityVec[i].id == eID )
		{
			return m_EntityVec[i].PopMessage( msg );
		}
	}
	return 0;
}
*/

//=============================================================================
// name :
// desc :
//=============================================================================
Pbyte* CEntityMgr::GetMsgById( int eID )
{
	return NULL;
}

