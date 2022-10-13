//
//
//
//
//
//
//
//
//
//   以多個ENTITYMGR來達成 取出資料或是刪除資料時 只有一部分的LOCK會被鎖住 以加快資料存取的速度
//
//
//
//
//
#ifndef CSENTITYPOOL_H
#define CSENTITYPOOL_H

#pragma warning(disable:4786)

#include "CsSCSDef.h"
#include "CsGameDef.h"
#include <EntityPool.h>
//#include <safevalue.inl>
#include <vector>
#include <map>
#include <list>
#include <vecmap.inl>

namespace Net
{
	class TCPSocket;
}

struct  _cstENTITYDESCRIPTOR;
typedef _cstENTITYDESCRIPTOR*     _cstLPENTITYDESCRIPTOR;

namespace Cyberspace
{
	class CsRegion;
	
    class CsEntityManage// : protected GSO::EntityPool
    {
		
		pt::vecmap<_cstLPENTITYDESCRIPTOR>            m_EntityCatchBuff;

		static T_Lock                                 s_lRecycleBin;
		static std::list<_cstLPENTITYDESCRIPTOR>      s_RecycleBin;

		//safevalue<bool>                               m_bLive;

				
    public:

        CsEntityManage();
        ~CsEntityManage();

		void                              Initialize(int c);
		void                              Finialize(void);
        
   		//建立一個新的ENTITY
		static CsEntityDescriptor         CreateEntity(entityid_t     eid,
			                                            const char*    name,
			                                            const byte_t*  propbuf,
										                size32_t       proplen,
										                connectionid_t cid,            // 加快速度
										                CsConnectionDescriptor owner,
										                CseENTITY_FORM type);

		//刪除一個ENTITY
		static void                       DeleteEntity(CsEntityDescriptor& e);

		// 會很慢請儘量少用
		static CsEntityDescriptor         GetEntityDescriptor(entityid_t eid);
		static size32_t                   GetAllEntityDescriptor(std::vector<CsEntityDescriptor>& entitylist);
		static unsigned int               GetEntityCount(void);


		// descriptor的取用
		// 用這種方式來達成 分散管理器讓取用時LOCK同時被LOCK的機會降低以提高速度
		static  entityid_t                GetEntityId(CsEntityDescriptor e);
		//
		static  connectionid_t            GetCsEntityConnectionId(CsEntityDescriptor e);
		static  std::string               GetCsEntityName(CsEntityDescriptor e);


        // 這是為了資料安全性，在做區域交換的時候 從外面鎖起來
		static  CsRegion*                 GetCsEntityRegion(CsEntityDescriptor e);
		static  void                      SetCsEntityRegion(CsEntityDescriptor e, CsRegion* region);

		static  long                      GetCsEntityLong(CsEntityDescriptor e);
		static  void                      SetCsEntityLong(CsEntityDescriptor e, long st);


		static  CseENTITY_FORM            GetEntityForm   (CsEntityDescriptor e);
		static  void                      SetEntityForm   (CsEntityDescriptor e, CseENTITY_FORM fm);

		
		static  CsConnectionDescriptor    GetCsEntityConnectionDescriptor  (CsEntityDescriptor e);

		// mode == true  開啟這個Entity的TOCK 其他人無法存取直到Update被執行
		// mode == flase 傳出一塊被複製的區域不保證該資料不會同時被其他人存取
		static  CsResBuf                  GetCsEntityPropbuf     (CsEntityDescriptor e, CsePROPBUF_MODE mode=CsPM_READ, int_t st=0);
		static  int                       UpdateCsEntityPropbuf  (CsEntityDescriptor e, const byte_t* prop, size32_t len);


		// 用eid來取得他擺放的MGR
		static CsEntityManage&            GetInstance(entityid_t eid);

        static T_Lock*                    GetEntityLock(CsEntityDescriptor e);

		static void                       Update();

	private:

		static void                       AddEntityRefereceCount(entityid_t eid, void** e, int cnt);

	
    };
    
	//extern std::vector<CsEntityManage>   g_EntityManage;
	extern CsEntityManage    g_EntityManage;
	//extern CsEntityManage* g_EntityManage;
	//extern int             g_EntityManageLen;
//#define _GETENTITYCONTAINERINDEX(eid)    (eid%g_EntityManage.size())
};

#endif