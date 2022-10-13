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
#ifndef CSTREGION_H
#define CSTREGION_H

#include "CsDef.h"
#include <map>
#include <EntityRoom.h>


namespace Cyberspace
{
	///////////////////////////////////////////////////////////////////////////////////
	// CsRegion
	class CsRegion
	{
		RegionId_t Id;

	public:
		
		std::vector<RegionId_t>      m_Neighbors;
	

		CsRegion( RegionId_t id );
		~CsRegion();

		RegionId_t   GetId( void );

		void         InsertEntity( GSO::GENTITY edp );
		void         RemoveEntity( GSO::GENTITY edp );
		size_t       GetEntityCount( void );
		void         ForAllEntity( GSO::EntityCallBack udels, void* pData );

		
		
	private:

		T_Lock m_UtLock;
		std::map<EntityId_t, GSO::GENTITY> m_Entities;
	};
};

