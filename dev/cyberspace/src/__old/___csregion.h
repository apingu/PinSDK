#ifndef CSREGION_H
#define CSREGION_H

#pragma warning (disable : 4786)

#include "CsGameDef.h"
#include <map>
#include <safevalue.inl>

namespace Cyberspace
{

	///////////////////////////////////////////////////////////////////////////////////
	// CsRegion
	class CsRegion
	{
		friend class GameServer;
		friend class MapService;

		safevalue<RegionId_t>  Id;

		//RegionId_t Id;

	public:

		std::vector<RegionId_t>     m_Neighbors;
		//std::set<RegionId_t>      m_Neighbors;


		CsRegion( RegionId_t id );
		~CsRegion();

		RegionId_t   GetId( void );

		void         InsertNeighbor( RegionId_t rid );
		bool         IsNeighborExist( RegionId_t rid );

		void         InsertEntity( CsEntityDescriptor edp );
		void         RemoveEntity( CsEntityDescriptor edp );
		size_t       GetEntityCount( void );

		void         ForAllEntityBuffer( CsEntityBufferCallBack cbf, void *data );
		void         ForAllEntityDescriptor( CsEntityDescriptorCallBack cbf, void *data );

	public:
		// TODO: Pin
		// 這樣百很危險想個辦法拿掉
		T_Lock     m_Lock;
		std::map<entityid_t, CsEntityDescriptor> m_Entities;

	};


};
#endif
