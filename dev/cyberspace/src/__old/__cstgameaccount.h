#ifndef CSGAMEACCOUNT_H
#define CSGAMEACCOUNT_H


#pragma warning (disable : 4786)

#include "CsGameDef.h"
#include <map>
#include <set>
#include <T_Lock.h>


namespace Cyberspace
{
	class CsGameObject// : public CsIdentity
	{
		friend class GameServer;

		std::map<entityid_t, _cstLPENTITYDESCRIPTOR>   m_Entities;
		T_Lock                                         m_lLive;
		
		T_Lock                                         m_lSphere;
		T_Lock                                         m_lUsed;
		int                                            m_Usedcount;

		CsConnectionDescriptor                         m_Ownfd;
		connectionid_t                                 m_CId;

		
		std::map< RegionId_t, u_int >                  m_Sphere;


		//T_Lock                         m_JoinLock;   //除錯用
		//std::set< entityid_t >	     m_JoinEntity; //已送加入訊息的 entity 第二個參數不給用


	public:

		CsGameObject( connectionid_t id, CsConnectionDescriptor s );
		//CsGameAccount();
		~CsGameObject();

		connectionid_t GetCId( void ){ return m_CId; }
		//time_t         GetCreateTime( void ){ return m_CreateTime; }

		//void           Release( void );
		

		
		//建立一個新的ENTITY
		_cstLPENTITYDESCRIPTOR     CreateEntity( entityid_t    eid,
			                                 const char*   name,
			                                 const byte_t* propbuf,
								             size_t        proplen );

		//刪除一個ENTITY
		void                   DeleteEntity( _cstLPENTITYDESCRIPTOR& e );
				
		_cstLPENTITYDESCRIPTOR GetEntity( entityid_t eid );

		void                   ForAllEntity( CsEntityDescriptorCallBack udels, void* pData );

		virtual void           Clear( void );

		u_int                  JoinSphere( RegionId_t rid );
		u_int                  ExitSphere( RegionId_t rid );
		bool                   IsRegionExist( RegionId_t rid );

		void                   PublishEnterEntity( void );
		void                   PublishDepartEntity( void );


		CsConnectionDescriptor GetSocket( void ) { return m_Ownfd; }
		
		int                    GetUsed( void );
		int                    AddUsed( int i );
		void                   WaitForSingleUsed( int i=0 );


		bool                   JoinEntity( entityid_t eid );
		bool                   ExitEntity( entityid_t eid );


	};

};



#endif  //CSGAMEACCOUNT_H