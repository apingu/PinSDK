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
#include "CsEntityManage.h"
#include <assert.h>
#include "CsGameEntity.h"
#include <PFThread.h>
#include <Debug.h>
#include <PFTimer.h>

struct _cstENTITYDESCRIPTOR
{
    friend Cyberspace::CsEntityManage;
private:
    CsePROPBUF_MODE              mode;        //0 沒有取出BUFF的狀態 1取出為讀取作用  2取出為寫入作用
	int_t                        mode_stage;  //debug 用 用一個編號 記錄 現在是幾號 給他 鎖住
	Cyberspace::CsEntity         entity;
	DWORD                        bldtime;     // 建立的時間
	DWORD                        deltime;     // 刪除的時間

	int_t                        cref;
	T_Lock                       userlock;    // 給使用者自行使用的
    //Cyberspace::CsEntityManage*  container;
};




namespace Cyberspace
{

	CsEntityManage  g_EntityManage;
	DWORD           g_tmEntityMgrUpdate=0;
	PFTimer         g_EntityMgrTimer;

	T_Lock                              CsEntityManage::s_lRecycleBin;
	std::list< _cstLPENTITYDESCRIPTOR>  CsEntityManage::s_RecycleBin;




    //System::PFThread            g_DelPolling;
	System::hThread             g_DelPolling=0;


//#define _GetCsEntityByDesp(e)             (CsEntity*)e->container->EntityPool::GetEntityBuffer(e->edesp)
#define _ENTITYMGR_UPDATE_TICK               60000  // 1分鐘清一次
#define _ENTITYMGR_DELETE_TICK              180000  // 3分鐘清一次
    //inline  _cstENTITYDESCRIPTOR*                  _ENTITYDP2_cstENTITYDESCRIPTOR*(GSO::GENTITYDP e) { return e; }

#define GetEntityContainerLock(eid)        (&g_EntityManage.m_EntityCatchBuff.m_lock[g_EntityManage.m_EntityCatchBuff.getpos(eid)])
#define GetEntityContainer(eid)            (&g_EntityManage.m_EntityCatchBuff.m_list[g_EntityManage.m_EntityCatchBuff.getpos(eid)])
#define _GetcstLPENTITYDESCRIPTOR(ee)      ((_cstLPENTITYDESCRIPTOR)((void*)ee))


    //===============================================================
    //
    //
    //===============================================================			
    CsEntityManage::CsEntityManage()
    {
        Finialize();
    }


    //===============================================================
    //
    //
    //===============================================================   
    CsEntityManage::~CsEntityManage()
    {
        //m_bLive.SetValue(false);
        {
			for(int i=0; i<m_EntityCatchBuff.vecsize(); i++)
			{
				TS_Lock rlock(&m_EntityCatchBuff.m_lock[i]);
				_vecmap_iterator(_cstENTITYDESCRIPTOR*) iter=m_EntityCatchBuff.m_list[i].begin();
				while(iter!=m_EntityCatchBuff.m_list[i].end())
				{
					_cstENTITYDESCRIPTOR* e = iter->second;
					SAFE_DELETE(e);
					iter++;
				}
				m_EntityCatchBuff.m_list[i].clear();

			}

			TS_Lock rbinlock(&s_lRecycleBin);
			if(!s_RecycleBin.empty())
			{
				std::list<_cstLPENTITYDESCRIPTOR>::iterator riter=s_RecycleBin.begin();
				while(riter!=s_RecycleBin.end())
				{
					_cstENTITYDESCRIPTOR*  e=(_cstENTITYDESCRIPTOR*)(*riter);
					SAFE_DELETE(e);

					riter++;
				}
				s_RecycleBin.clear();
			}
        }

    }


    //===============================================================
    //
    //
    //===============================================================
    void CsEntityManage::Initialize(int c)
    {
		m_EntityCatchBuff.init(c);
        //m_bLive.SetValue(true);
    }



    //===============================================================
    //
    //
    //===============================================================      
    void CsEntityManage::Finialize(void)
    {
        //m_bLive.SetValue(false);
    }

	//===============================================================
	//
	//
	//===============================================================
	void CsEntityManage::Update()
	{
		//if(g_EntityManage)
		{
			DWORD currtime=PFTimer::GetTickMSTime();
			if((currtime-g_tmEntityMgrUpdate)>_ENTITYMGR_UPDATE_TICK)
			{
				TS_Lock rbinlock(&s_lRecycleBin);

				if(!s_RecycleBin.empty())
				{
					std::vector<std::list<_cstLPENTITYDESCRIPTOR>::iterator> deliter;
					std::list<_cstLPENTITYDESCRIPTOR>::iterator iter=s_RecycleBin.begin();
					while(iter!=s_RecycleBin.end())
					{
						_cstENTITYDESCRIPTOR* e=(_cstENTITYDESCRIPTOR*)(*iter);
						TS_Lock lock0(GetEntityContainerLock(e->entity.GetId()));
						if((currtime-e->deltime)>_ENTITYMGR_DELETE_TICK)
						{
							e->mode       =CsPM_NONE;
							e->mode_stage =0;
							e->bldtime    =0;
							e->deltime    =0;
							SAFE_DELETE(e);
							deliter.push_back(iter);
						}
						iter++;
					}

					for(int j=0; j<deliter.size(); j++)
					{
						s_RecycleBin.erase(deliter[j]);
					}
					deliter.clear();

				}

				g_tmEntityMgrUpdate=currtime;
			}
	    }
	}

    //===============================================================
    //
    //建立一個新的ENTITY
    //===============================================================      
    CsEntityDescriptor CsEntityManage::CreateEntity(entityid_t eid, const char* name,
													 const byte_t* propbuf, size32_t proplen,
													 connectionid_t cid,
													 CsConnectionDescriptor owner,
													 CseENTITY_FORM type)
    {
        if((propbuf==NULL)||(proplen==0))
        {
            assert(0 && "CsEntityManage::CreateEntity: entity buffer is a 0 ");
            return CsEntityDescriptor(CsINVALID_ID, _CSOBJDESCRIPTOR_TYPE_NONE, NULL, NULL);
        }

		int mpos=g_EntityManage.m_EntityCatchBuff.getpos(eid);
		TS_Lock lock(&g_EntityManage.m_EntityCatchBuff.m_lock[mpos]);

		// 先找 catch buffer 裡面
        _vecmap_iterator(_cstENTITYDESCRIPTOR*) iter = g_EntityManage.m_EntityCatchBuff.m_list[mpos].find(eid);
        if(iter != g_EntityManage.m_EntityCatchBuff.m_list[mpos].end())
        {
            _cstENTITYDESCRIPTOR* ept = iter->second;
			ept->entity.SetId(eid);
            ept->entity.SetType(type);
            ept->entity.SetName(name);
            ept->entity.SetPropbuf(propbuf, proplen);
			ept->entity.SetConnection(cid, owner);
			//ept->cref++;
			ept->mode=CsPM_NONE;
			ept->mode_stage=0;
			ept->bldtime=PFTimer::GetTickMSTime();
			ept->deltime=0;

            return CsEntityDescriptor(eid, _CSOBJDESCRIPTOR_TYPE_ENTITY, ept, AddEntityRefereceCount);
        }




		_cstENTITYDESCRIPTOR* ge=NULL;

		// 直接一個一個 找ID 這邊只是拿來除錯
		iter = g_EntityManage.m_EntityCatchBuff.m_list[mpos].begin();
		while(iter!=g_EntityManage.m_EntityCatchBuff.m_list[mpos].end())
		{
			if(iter->second->entity.GetId()==eid)
			{
				ge=iter->second;
				break;
			}
			iter++;
		}

        if(ge!= NULL)
        {
            assert(NULL&&"Entity already exist!!");
            return CsEntityDescriptor(CsINVALID_ID, _CSOBJDESCRIPTOR_TYPE_NONE, NULL, NULL);
        }

		
        
		//放到快取buff裏 回收 減少NEW的次數
        _cstENTITYDESCRIPTOR* ept=NULL;// =  new _cstENTITYDESCRIPTOR;

		{
			TS_Lock rbinlock(&s_lRecycleBin);
			if(s_RecycleBin.empty())
			{
				ept =  new _cstENTITYDESCRIPTOR;
			}
			else
			{

				ept=(_cstENTITYDESCRIPTOR*)s_RecycleBin.back();
				s_RecycleBin.pop_back();
				char buff[256];
				sprintf(buff, "Get Entity buff %lld from RecycleBin\n", eid);
				OutputDebugString(buff);
			}
		}

		ept->entity.SetId(eid);
		ept->entity.SetType(type);
		ept->entity.SetName(name);
		ept->entity.SetPropbuf(propbuf, proplen);
		ept->entity.SetConnection(cid, owner);
        ept->cref   = 0;
		ept->mode   = CsPM_NONE;
		ept->mode_stage = 0;
		ept->bldtime=PFTimer::GetTickMSTime();
		ept->deltime=0;
        g_EntityManage.m_EntityCatchBuff.m_list[mpos].insert(std::map< entityid_t, _cstENTITYDESCRIPTOR* >::value_type(eid, ept));

        return CsEntityDescriptor(eid, _CSOBJDESCRIPTOR_TYPE_ENTITY, ept, AddEntityRefereceCount);
    }


    //===============================================================
    //
    //刪除一個ENTITY
    //===============================================================
    void CsEntityManage::DeleteEntity(CsEntityDescriptor& e)
    {
		assert(!_IS_NULLENTITYDESCRIPTOR(e) && "DeleteEntity entity dispitcher can't be 0 ");
        if(_IS_NULLENTITYDESCRIPTOR(e))
            return;

		if(_GetcstLPENTITYDESCRIPTOR(e)->deltime!=0)
			return;

		entityid_t eid=GetEntityId(e);
		TS_Lock lock(GetEntityContainerLock(eid));
		_GetcstLPENTITYDESCRIPTOR(e)->deltime=PFTimer::GetTickMSTime();
		_vecmap_map(_cstENTITYDESCRIPTOR*)* entitycontainer=GetEntityContainer(eid);
		entitycontainer->erase(_GetcstLPENTITYDESCRIPTOR(e)->entity.GetId());
		{
			TS_Lock rbinlock(&s_lRecycleBin);


			s_RecycleBin.push_back(_GetcstLPENTITYDESCRIPTOR(e));
        }
    }



    //===============================================================
    //
    //
    //===============================================================
    CsEntityDescriptor CsEntityManage::GetEntityDescriptor(entityid_t eid)
    {
		int mpos=g_EntityManage.m_EntityCatchBuff.getpos(eid);
		TS_Lock lock(&g_EntityManage.m_EntityCatchBuff.m_lock[mpos]);

        _vecmap_iterator(_cstENTITYDESCRIPTOR*) iter = g_EntityManage.m_EntityCatchBuff.m_list[mpos].find(eid);
        if(iter != g_EntityManage.m_EntityCatchBuff.m_list[mpos].end())
        {
			if(iter->second->entity.GetId()!=CsINVALID_ID)
                return CsEntityDescriptor(iter->second->entity.GetId(), _CSOBJDESCRIPTOR_TYPE_ENTITY, iter->second, AddEntityRefereceCount);
        }

		_cstENTITYDESCRIPTOR* ee=NULL;
		iter =g_EntityManage. m_EntityCatchBuff.m_list[mpos].begin();
		while(iter!=g_EntityManage.m_EntityCatchBuff.m_list[mpos].end())
		{
			if(iter->second->entity.GetId()==eid)
			{
				ee=iter->second;
				break;
			}
			iter++;
		}

        if(ee!=NULL)
		{
			if(ee->entity.GetId()!=CsINVALID_ID)
			{
				g_EntityManage.m_EntityCatchBuff.m_list[mpos].erase(iter);
                g_EntityManage.m_EntityCatchBuff.m_list[mpos].insert(std::map< entityid_t, _cstENTITYDESCRIPTOR* >::value_type(eid, ee));
			}
			else
			{
				return CsEntityDescriptor(CsINVALID_ID, _CSOBJDESCRIPTOR_TYPE_NONE, NULL, NULL);
			}
		}


        return CsEntityDescriptor(eid, _CSOBJDESCRIPTOR_TYPE_ENTITY, ee, AddEntityRefereceCount);
    }



    //===============================================================
    //
    //
    //===============================================================
    entityid_t CsEntityManage::GetEntityId(CsEntityDescriptor e)
    {
		if(_IS_NULLENTITYDESCRIPTOR(e))
            return CsINVALID_ID;

		return (entityid_t)e;

		//TS_Lock lock(GetEntityContainerLock(e->entity.GetId()));
        //return _GetcstLPENTITYDESCRIPTOR(e)->entity.GetId();
    }



    //===============================================================
    //
    //
    //===============================================================
    connectionid_t CsEntityManage::GetCsEntityConnectionId(CsEntityDescriptor e)
    {
		if(_IS_NULLENTITYDESCRIPTOR(e))
            return CsINVALID_ID;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
        return _GetcstLPENTITYDESCRIPTOR(e)->entity.GetConnectionId();
    }


    //===============================================================
    //
    //
    //===============================================================
    std::string CsEntityManage::GetCsEntityName(CsEntityDescriptor e)
    {
		if(_IS_NULLENTITYDESCRIPTOR(e))
            return "";

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
        return _GetcstLPENTITYDESCRIPTOR(e)->entity.GetName();
    }


    //===============================================================
    //
    //
    //===============================================================
    CsRegion* CsEntityManage::GetCsEntityRegion(CsEntityDescriptor e)
    {
		if(_IS_NULLENTITYDESCRIPTOR(e))
            return NULL;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
        return _GetcstLPENTITYDESCRIPTOR(e)->entity.GetRegion();
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsEntityManage::SetCsEntityRegion(CsEntityDescriptor e, CsRegion* region)
	{
		if(_IS_NULLENTITYDESCRIPTOR(e))
			return;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
    	_GetcstLPENTITYDESCRIPTOR(e)->entity.SetRegion(region->GetId(), region);
		return;
	}

	//===============================================================
	//
	//
	//===============================================================	
	long CsEntityManage::GetCsEntityLong(CsEntityDescriptor e)
	{
		if(_IS_NULLENTITYDESCRIPTOR(e))
			return 0;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
		return _GetcstLPENTITYDESCRIPTOR(e)->entity.GetLong();
	}

	//===============================================================
	//
	//
	//===============================================================
	void CsEntityManage::SetCsEntityLong(CsEntityDescriptor e, long st)
	{
		if(_IS_NULLENTITYDESCRIPTOR(e))
			return;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
		_GetcstLPENTITYDESCRIPTOR(e)->entity.SetLong(st);
		return;
	}


	//===============================================================
	//
	//
	//===============================================================
	CseENTITY_FORM CsEntityManage::GetEntityForm(CsEntityDescriptor e)
	{
		if(_IS_NULLENTITYDESCRIPTOR(e))
			return CsEF_NULL;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
		return (CseENTITY_FORM)_GetcstLPENTITYDESCRIPTOR(e)->entity.GetType();
	}


	//===============================================================
	//
	//
	//===============================================================
	 void CsEntityManage::SetEntityForm(CsEntityDescriptor e, CseENTITY_FORM fm)
	 {
		 if(_IS_NULLENTITYDESCRIPTOR(e))
		     return;

		 TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
	     _GetcstLPENTITYDESCRIPTOR(e)->entity.SetType(fm);
		 return;
	 }


	 namespace CsBaseServerGlobal
	 {
		 extern int UpdateStage;
	 };


	
	//===============================================================
	//
	//
	//===============================================================
	CsResBuf CsEntityManage::GetCsEntityPropbuf(CsEntityDescriptor e, CsePROPBUF_MODE mode, int_t st)
	{
		CsResBuf tempbuf;

		if(_IS_NULLENTITYDESCRIPTOR(e))
			return tempbuf;

		entityid_t eid=GetEntityId(e);

		_cstLPENTITYDESCRIPTOR ee=_GetcstLPENTITYDESCRIPTOR(e);

		bool  bfail=true;
		int_t bst  =0;
		for(int i=0; i<1000; i++)
		{
			{
				//TS_Lock ulock(&e->entity.m_lPropBuf);
				TS_Lock lock(GetEntityContainerLock(eid));
				bst=ee->mode_stage;
				if(ee->mode!=CsPM_WRITE)
				{
					// 成功取得
					if(mode==CsPM_WRITE)
					{
						ee->mode   =CsPM_WRITE;
						ee->mode_stage =st;
					}
					else
					{

					}

					// 暫時性
					if((ee->entity.GetProplen()>0)&&(ee->entity.GetProplen()<_1MB_)&&(((int)ee->entity.GetPropbuf())>100))
					{
						tempbuf.resize(ee->entity.GetProplen());
						IOMEMCPY(_GetResBufPtr(tempbuf), ee->entity.GetPropbuf(), _GetResBufLen(tempbuf));
					}
					else
					{
						char buff[256];
						sprintf(buff, "CsEntityManage::GetCsEntityPropbuf buffer is NULL");
						OutputDebugString(buff);
						DebugOutPutf(buff, "CsEntityManage::GetCsEntityPropbuf");
					}
					return tempbuf;
					//bfail     =false;
					//break;
				}
			}
			Sleep(10);
		}

		//  做滿 1000 * 10毫秒 都取不到
		if(bfail)
		{
			char buff[256];
			sprintf(buff, "CsEntityManage:: lock %lld %d failure on %d", eid, mode, bst);
			OutputDebugString(buff);
			DebugOutPutf(buff, "CsEntityManage::GetCsEntityPropbuf");
		}


/*
		if(mode==CsPM_WRITE)
		{
			// 之後不管是要讀/寫都不准再取用
			// 直到被解開後

			//CsBaseServerGlobal::UpdateStage=505213;
// #ifdef _DEBUG
// 			char buff[255];
// 			sprintf(buff, _T("Lock one entity write on %d\n"), e->eid);
// 			OutputDebugString(buff);
// #endif
			//entity->m_lPropBuf.Lock();
			// 等到沒有被LOCK的時候
			bool  bfail=true;
			int_t bst  =e->mode_stage;
			for(int i=0; i<1000; i++)
			{
				{
					TS_Lock ulock(&e->entity.m_lPropBuf);
					if(e->mode!=CsPM_WRITE)
					{
						e->mode   =CsPM_WRITE;
						e->mode_stage =st;
						bfail     =false;
						break;
					}
				}
				Sleep(10);
			}

			// 都鎖住10秒了 還是拿不到 只好還是給你
			if(bfail)
			{
				char buff[256];
				sprintf(buff, "CsEntityManage:: lock %lld write failure on %d", e->entity.GetId(), bst);
				OutputDebugString(buff);
				DebugOutPutf(buff, "CsEntityManage::GetCsEntityPropbuf");
			}
		}
		else
		{
			//CsBaseServerGlobal::UpdateStage=505214;
			// 如果有人鎖住唯寫
			// 直到被解開後 才能再進來
		    //TS_Lock ulock(&e->entity.m_lPropBuf);
			//e->mode=CsPM_READ;
			bool  bfail=true;
			for(int i=0; i<1000; i++)
			{
				{
					TS_Lock ulock(&e->entity.m_lPropBuf);
					if(e->mode!=CsPM_WRITE)
					{
						bool  bfail=false;
						break;
					}
				}
				Sleep(10);
			}

			// 都鎖住10秒了 還是拿不到 只好還是給你
			if(bfail)
			{
				char buff[256];
				sprintf(buff, "CsEntityManage:: lock %lld read failure on %d", e->entity.GetId(), bst);
				OutputDebugString(buff);
				DebugOutPutf(buff, "CsEntityManage::GetCsEntityPropbuf");
			}
		}
*/

		//CsBaseServerGlobal::UpdateStage=505215;

		//TS_Lock lock(&e->container->m_lEntity);
		//TS_Lock lock(GetEntityContainerLock(e));

		//CsBaseServerGlobal::UpdateStage=505217;

		return tempbuf;

	}


	//===============================================================
	//
	//
	//===============================================================
	int CsEntityManage::UpdateCsEntityPropbuf(CsEntityDescriptor e, const byte_t* prop, unsigned int len)
	{
		if(_IS_NULLENTITYDESCRIPTOR(e))
			return 0;

		int ret=0;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
		if(_GetcstLPENTITYDESCRIPTOR(e)->mode==CsPM_WRITE)
		{
			// 剛剛取出的時候被設定為唯寫
			// 寫入後解開他
			_GetcstLPENTITYDESCRIPTOR(e)->entity.SetPropbuf(prop, len);
			ret=1;
			//entity->m_lPropBuf.UnLock();
		}
		else
		{
			//設定讀取用的 不准寫入資料
		}

		_GetcstLPENTITYDESCRIPTOR(e)->mode=CsPM_NONE;
		return ret;
	}



	//===============================================================
	//
	//
	//===============================================================
	CsConnectionDescriptor CsEntityManage::GetCsEntityConnectionDescriptor(CsEntityDescriptor e)
	{
		if(_IS_NULLENTITYDESCRIPTOR(e))
			return NULL;

		TS_Lock lock(GetEntityContainerLock(GetEntityId(e)));
		return _GetcstLPENTITYDESCRIPTOR(e)->entity.GetConnectionDescriptor();
	}

	//===============================================================
	//
	//
	//===============================================================
	unsigned int CsEntityManage::GetEntityCount(void)
	{
		return g_EntityManage.m_EntityCatchBuff.size();
	}


	//===============================================================
	//
	// 用eid來取得他擺放的MGR
	//===============================================================
    CsEntityManage&  CsEntityManage::GetInstance(entityid_t eid)
	{
		return g_EntityManage;
		//return g_EntityManage[(eid%g_EntityManageLen)]; 
	}


	//===============================================================
	//
	//
	//===============================================================
	size32_t CsEntityManage::GetAllEntityDescriptor(std::vector<CsEntityDescriptor>& entitylist)
	{
		for(int i=0; i<g_EntityManage.m_EntityCatchBuff.vecsize(); i++)
		{
			TS_Lock lock(&g_EntityManage.m_EntityCatchBuff.m_lock[i]);
			_vecmap_iterator(_cstENTITYDESCRIPTOR*) iter = g_EntityManage.m_EntityCatchBuff.m_list[i].begin();
			while(iter!=g_EntityManage.m_EntityCatchBuff.m_list[i].end())
			{
				entitylist.push_back(CsEntityDescriptor(iter->first, _CSOBJDESCRIPTOR_TYPE_ENTITY, iter->second, AddEntityRefereceCount));
				iter++;
			}
		}
		return entitylist.size();
	}

    //===============================================================
    //
    //
    //===============================================================
    T_Lock* CsEntityManage::GetEntityLock(CsEntityDescriptor e)
    {
        return &_GetcstLPENTITYDESCRIPTOR(e)->userlock;
    }

	//===============================================================
	//
	//
	//===============================================================
	void CsEntityManage::AddEntityRefereceCount(entityid_t eid, void** e, int cnt)
	{
		if(_IS_NULLENTITYDESCRIPTOR((*e)))
			return;

		TS_Lock lock(GetEntityContainerLock(eid));
		_GetcstLPENTITYDESCRIPTOR((*e))->cref+=cnt;
		//return _GetcstLPENTITYDESCRIPTOR((*e))->cref;
	}


	/*
	//===============================================================
	//
	//
	//===============================================================
	bool CsEntityManage::UpdateEntity(_cstENTITYDESCRIPTOR* e,
	const char*   name,
	const byte_t* propbuf,
	size32_t        proplen)
	{
	TS_Lock lock(&m_lEntity);

	CsEntity *entity = _GetCsEntityByDesp(e);
	if(entity==NULL)
	return false;
	entity->SetName(name);
	entity->SetDispatcher(e);
	entity->SetPropbuf(propbuf, proplen);
	return true;

	}
	*/






	//	//===============================================================
	//	//
	//	//
	//	//===============================================================		
	//	CsEntity* CsEntityManage::GetCsEntityByEId(entityid_t eid)
	//	{
	//		TS_Lock lock(&m_lEntity);
	//		_vecmap_iterator(_cstENTITYDESCRIPTOR*) iter = m_EntityCatchBuff.find(eid);
	//		if(iter != m_EntityCatchBuff.end())
	//		{
	//			return (CsEntity*)EntityPool::GetEntity(iter->second);
	//		}
	//		
	//		CheckEntityResult* er = new CheckEntityResult;
	//		er->id = eid;
	//		er->result=NULL;
	//		ForAllEntity(CheckEidCB, er);
	//		_cstENTITYDESCRIPTOR* e = er->result;
	//		delete er;
	//		if(e==0)
	//			return NULL;
	//
	//		m_EntityCatchBuff.insert(std::map< entityid_t, _cstENTITYDESCRIPTOR* >::value_type(eid, e));
	//		return (CsEntity*)EntityPool::GetEntity(e);
	//		
	//	}
	//	
	//	
	//	//===============================================================
	//	//
	//	//
	//	//===============================================================
	//	CsEntity* CsEntityManage::GetCsEntityByDesp(_cstENTITYDESCRIPTOR* e)
	//	{
	//		TS_Lock lock(&m_lEntity);
	//		assert(e!=0 && "entity dispitcher can't be 0 ");
	//		return (CsEntity*)EntityPool::GetEntity(e);
	//		
	//	}


	//===============================================================
	//
	//
	//===============================================================
	//	size32_t CsEntityManage::GetCsEntityProplen(_cstENTITYDESCRIPTOR* e)
	//	{
	//		TS_Lock lock(&m_lEntity);
	//		CsEntity* entity = _GetCsEntityByDesp(e);
	//		if(entity==NULL)
	//		{
	//			assert(entity!=0 && "GetCsEntityProplen entity is a 0 ");
	//			return NULL;			
	//		}
	//		return entity->GetProplen();
	//	}

	//===============================================================
	//
	//
	//===============================================================
	//	uint_t CsEntityManage::GetEntityUsedCount(_cstENTITYDESCRIPTOR* e)
	//	{
	//		TS_Lock lock(&m_lEntity);
	//		CsEntity* entity = _GetCsEntityByDesp(e);
	//		if(entity==NULL)
	//		{
	//			assert(entity!=0 && "entity is a 0 ");
	//			return NULL;			
	//		}
	//		return entity->GetUsedPropbufCount();
	//	}

	//===============================================================
	//
	//
	//===============================================================
	// 	void CsEntityManage::FreeCsEntityPropbuf(_cstENTITYDESCRIPTOR* e)
	// 	{
	// 		TS_Lock lock(&m_lEntity);
	// 		CsEntity* entity = _GetCsEntityByDesp(__cstENTITYDESCRIPTOR*2ENTITYDP(e));
	// 		if(entity==NULL)
	// 		{
	// 			TS_Lock lock(&s_lRecycleBin);
	// 			std::list< void* >::iterator iter=s_RecycleBin.begin();
	// 			while(iter!=s_RecycleBin.end())
	// 			{
	// 				entity = (CsEntity*)(*iter);
	// 				if(entity->GetDispatcher()== e)
	// 				{
	// 					entity->FreePropbuf();
	// 					return;
	// 				}
	// 				iter++;
	// 			}
	// 			char buff[256];
	// 			sprintf(buff, "free no find %d\n", e);
	// 			OutPutMessage(buff, "CsEntityManage");
	// 			//assert(entity!=0 && "FreeCsEntityPropbuf entity is a 0 ");
	// 			return;			
	// 		}
	// 		entity->FreePropbuf();
	// 		return;
	// 	}


	

};

