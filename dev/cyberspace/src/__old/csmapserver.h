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
#ifndef CSMAPSERVER_H
#define CSMAPSERVER_H

#pragma warning(disable:4786)


#include "CsDef.h"

#include <map>
#include <list>
#include <EntityRoom.h>

#include <vector>
#include "CstEntity.h"
#include "UtCriticalSection.h"

namespace Cyberspace
{
	typedef std::list<CsEntity *>	EntityList;
	///////////////////////////////////////////////////////////////////////////////////
	// EntityGroup: 一個收集 CsEntity 的群組. 可以是區域, 可以是隊伍, 可以是...
	// 20040811 Wood: 隊伍, 區域等等人物集合體的 super class
	// 另外這個類別不會偷看 CsEntity 的內容, 也不會運用裡面的欄位. 但是分支要不要用
	// 就是他的問題了...
	class EntityGroup
	{
	protected:
		int						m_Id;			// ID. 但是不同用途的可以重複
		int						m_Max;			// 能添加的最大上限
		UtCriticalSection		m_CS;			// Enum/Add/Del must have Critical section
		EntityList				m_Entities;		// 這個只是指標, 不保存
	public:

		EntityGroup();
		inline int	GetId(void){	return m_Id;	}
		inline void SetId(int id){	m_Id=id;	}
		inline void SetMaxSize(int sz){	m_Max=sz;	}
		// 增刪, 但不 delete CsEntity 物件
		virtual bool InsertEntity(CsEntity *);
		virtual bool RemoveEntity(CsEntity *);
		virtual void Clear();	// 清除所有的項目.
		inline int	EntityCount(){	return m_Entities.size();	}
		// 這兩個是給 join/move 時候廣播訊息用
		// 找出本區內的所有 Socket(不重複), 增添到給定 vector 內. 當然也不會和本來有的重複)
		// 20040923: 增加是否包含 NPC server 的功能
		int			AddAllSockets(vector<IO::TCPSocket*>& socklist,bool npcsvr);
		// 增添所有 entity list
		int			AddAllEntities(std::list<CsEntity*>& elist);
	};
	///////////////////////////////////////////////////////////////////////////////////
	// CsRegion
	// 20040811 Wood: 由於有隊伍和這個操作近似, 所以變更為有一個 super class
	// 20040708 Wood: 增加一個類別 Zone, 和 Client 的地圖 1:1 對應. 並包含很多 Region.
	//				但是只是讓 Region 可以查所屬而已
	// 20040623 Wood: 本類別記得一個區域所有的 CsEntity, 並進行傳輸訊息的服務
	//				和 GSO::GENTITY 脫勾化(該物請利用 CsMapServer)
    class CsRegion : public EntityGroup
	{
		friend class MapServer;		// 我的管理器
	public:
		int			m_Zone;			// 所屬 Zone 代號
		std::vector<RegionId_t>     m_Neighbors;

		CsRegion( RegionId_t id );
		~CsRegion();

		bool	InsertEntity(CsEntity *);	// 會對 CsEntity 內容操作, 故要改!!
		bool	RemoveEntity(CsEntity *);	// 會對 CsEntity 內容操作, 故要改!!
		void	Clear();					// 清除所有的項目. 暫時禁止使用
		void	ForAllEntities(GSO::EntityCallBack callback,void *UData,void *pExcept=NULL);
		// 增添所有 socket list, 除了石化的傢伙以外
		int		AddAllSockets(vector<IO::TCPSocket*>& socklist,bool npcsvr);
	};

	typedef std::map< EntityId_t, CsEntity *> MAP_ENT;
	typedef MAP_ENT::iterator				  MAP_ENTP;
	typedef struct {	int region,num;	}	RGN_LIST;	// 廣播用 region list
	///////////////////////////////////////////////////////////////////////////////////
	// MapServer
    class MapServer	: protected GSO::EntityRoom
	{
	protected:
		//紀錄地圖中,每一個 ragion 的資料
		std::map< RegionId_t, CsRegion *> m_Regions;
		MAP_ENT m_EntityMap;	// 所有 CsEntity 指標查詢器, 但不負責管理

	public:
		MapServer();
		~MapServer();
		// 需要改造的 ------------------------------------------------------------------
		virtual u_int      OnGetMapCx( void ){return 0;};
		virtual u_int      OnGetMapCy( void ){return 0;};
		virtual u_int      OnGetMapCz( void ){return 0;};
		// 改成某個固定結構, 所以不需傳 size...
		virtual RegionId_t OnGetRegionID(void *prop){	return 0;	};
		//當有一個新物品要加入地圖中,傳回該物品存在的 ragion 索引值
		virtual RegionId_t OnEnterEntity( u_int eid, void *EntityProp, unsigned int PropLen ){return 0;}
		virtual RegionId_t OnDepartEntity( u_int eid, void *EntityProp, unsigned int PropLen ){return 0;}
		//取出 ragion 列表中,其中一個 ragion 的鄰居列表
		virtual void       OnGetNeighbors( int rid, std::vector<RegionId_t> &neighbors ){ neighbors.push_back(rid); }
		// 製造以及消除 CsEntity 或是衍生的類別. 凡是使用衍生類別的要改
		virtual CsEntity  *NewCsEntity(EntityId_t eid,void *prop,int plen,IO::TCPSocket *);
		virtual void       DeleteCsEntity(CsEntity *);
		// 結束需要改造的 --------------------------------------------------------------
		// 和 GSO::GENTITY 有關的
		RegionId_t         GetEntityRegionId( GSO::GENTITY e );
		void*              GetEntityPropbuf( GSO::GENTITY e );
		size_t             GetEntityProplen( GSO::GENTITY e );
		IO::TCPSocket*     GetEntitySocket( GSO::GENTITY e );
		EntityId_t         GetEntityId( GSO::GENTITY e );
		CsEntity*          GetCsEntity( GSO::GENTITY e );
		CsEntity*          GetEntity( int eid, MAP_ENTP *mapep = NULL );
		GSO::GENTITY       GetGEntity( int eid );
		// 造 CsEntity 以及 GSO::GENTITY 兩個
		GSO::GENTITY       CreateEntity(EntityId_t eid,void *prop,int plen,IO::TCPSocket *);
		// 從整個系統移除物件
		void			   RemoveGEntity(GSO::GENTITY e);
		void               GetCsRegion( GSO::GENTITY e );

		/// 對某一區所有 entity 執行 cbf() (用 data 傳參數, 請自行確認type)
		/// 遞迴式的給每個區域的物體(level=0只有本區,level=1本區+鄰近, level=2...)
		void	           ForAllEntities(int rid,GSO::EntityCallBack cbf,	\
			void *data,void *excp=NULL,int level=0);
		/// 不分區的 for all (某些特殊功能需要, 例如送所有的給 NPC server)
		void               ForAllEntities(GSO::EntityCallBack func,void *data);

		// flgs: 兩個 bit, 10=沒有來源 region, 01=沒有目標 region, 11=兩方都有, 00=兩方都沒有??!
		int		BuildRegionBCList(int srcrgn,int dstrgn,int flgs,RGN_LIST *output);	// 計算跳區影響的 region
	protected:
		CsRegion*          GetRegion( RegionId_t rid );
	};

};//Cyberspace

#endif//CSSERVERMAP_H