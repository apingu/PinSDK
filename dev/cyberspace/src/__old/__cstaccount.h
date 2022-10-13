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
#ifndef CSTACCOUNT_H
#define CSTACCOUNT_H

#pragma warning (disable : 4786)
#include <map>
#include <set>

#include <IOSocket.h>
#include "CsDef.h"
//#include "CstEntity.h"
#include "EntityRoom.h"


namespace Cyberspace
{
	class CsAccount// : public CsConnectedObject
	{
		friend class GameServer;
		
	    //AccountId_t     m_AId;      //固定的
		ConnectionId_t  m_AId;      //每次分配的
		std::string     m_Username;	// Account name
		int             m_Privilege;   // 權力代號

		std::map< EntityId_t, GSO::GENTITY > m_Entities;
		std::set<EntityId_t>		         m_JoinEntity;//已送加入訊息的 entity 第二個參數不給用

		/// 萬一出什麼問題請改陣列...
		

	public:

		CsAccount( ConnectionId_t id );
		~CsAccount();

		//Get account id
		ConnectionId_t  GetId( void );
		const char*     GetUsername( void );
		
		void            Clear( void );
		
		
		//建立一個新的ENTITY
		GSO::GENTITY       CreateEntity( EntityId_t eid,
			                             const char* name,
			                             void* propbuf,
										 size_t proplen );

		//刪除一個ENTITY
		void               DeleteEntity( GSO::GENTITY& e );
		
		

		//int          InsertEntity( GSO::GENTITY e );
		//int          RemoveEntity( GSO::GENTITY e );

		GSO::GENTITY GetEntity( EntityId_t eid );

		void         SetPrivilege( int pri );
		int          GetPrivilege( void );

		bool         JoinEntity( EntityId_t eid );
		bool         FindEntity( EntityId_t eid );
		void         ExitEntity( EntityId_t eid );

		void         ForAllEntity( GSO::EntityCallBack udels, void* pData );


		//bool         TestEntityOnRegion( RegionId_t rid );

		/*
		CsEntity*   CreateEntity( EntityId_t eid,
			                      void* property, u_int proplen );
		void        DeleteEntity( EntityId_t eid );
		*/
		T_Lock          m_Lock;
		char work[4];	// 這個供一些 function 用來做工作區, 記得不要依賴它(例如網路傳送時的標籤)
		IO::TCPSocket*  m_Ownfd;

		//get entity point by entityid
		//CsEntity*   GetEntity( EntityId_t eid );
	};
}

#endif//CSACCOUNT_H
