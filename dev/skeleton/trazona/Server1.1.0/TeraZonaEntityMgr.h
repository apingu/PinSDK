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
#ifndef TERAZONAENTITYMGR_H
#define TERAZONAENTITYMGR_H

//#include <queue>
#include <vector>
#include <PMessage.h>
#include <DepthQueue.h>

#include <ZonaGSPublish.h>


///////////////////////////////////////////////////////////////////////////////
// entity class

class CEntity
{
	//std::queue< CPMessage* > m_MsgVec;
	int m_DelayBufSize;
	DepthQueue< CPMessage* > m_MsgVec;

public:

	//ZonaServerEntity* entity;

	int    eID;
	int    rID;
	int    DisplaySize;
	Pbyte* DisPlayprop;

	CEntity();
	~CEntity(){};

	void       Destroy     ( void );
	void       Putmessage  ( CPMessage* msg );
	CPMessage* Getmessage  ( void );
	Puint      MessageSize ( void );

};


///////////////////////////////////////////////////////////////////////////////
// entity management
//template<class _Type>
class CEntityMgr
{

	std::vector< CEntity* > m_EntityVec;

public:

	CEntityMgr();
	~CEntityMgr();

	CEntity*  operator[]( int id );
	int       EntitySize( void );

	void      AddEntity( int eid, Pbyte* props, int size );
	void      DelEntity( int eid );
	CEntity*  GetEntity( int eid );

	//int     PutMsg( int eid, Pbyte* msg, int size );
	//Puint    GetMsg( int eid, Pbyte** msg );

	
	Pbyte*  GetMsgById( int eid );

};




#endif