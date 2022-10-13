#pragma warning(disable:4786)
#include <stdio.h>	// for NULL
#include <assert.h>

//#include "Debug.h"
#include "CsDef.h"
#include "IOSocket.h"
#include "Cyberspace_server.h"
#include "CsServiceBase.h"

static char default_ver[]="No version infomation\n";

#define VARTOSTR(fmt)	\
	va_list va;			\
	va_start(va,fmt);	\
	vsprintf(fmtbuf,fmt,va);	\
	va_end(va)

namespace Cyberspace
{
	GameServer* ServiceBase::s_GameServer = 0;

	ServiceBase::ServiceBase()
	: version_info(default_ver)
	{
//		m_dbg = &ConsoleOutput::Instance();
	}

	bool ServiceBase::OnInit()
	{
		service_id = -1;
		return true;
	}

	void ServiceBase::SetLogMessage(const char *format,...)
	{
		char fmtbuf[512];		// For thread safe, ...
//		if(!m_dbg || !m_dbg_en)
//			return;
		VARTOSTR(format);
		//m_dbg->Write(fmtbuf);
		// TODO: Sean
		// 先醬搞
		printf(fmtbuf);
	}
/*
	void ServiceBase::SetLogObject(DebugOutput *obj)
	{
		m_dbg = obj;
	}
*/
	void ServiceBase::EnableLog(bool b)
	{
		m_dbg_en = b;
	}



	
/*
	// for ServiceMgr
	ServiceMgr::ServiceMgr()
	{
		memset(services,0,sizeof(ServiceBase *)*MAX_SERVICES_PER_SERVER);
	}
	// 提供服務
	int ServiceMgr::AddService(ServiceBase *b)	// 不管 init 或是訊息處理的初始化, 只管掛上去
	{
		for (int i=0;i<MAX_SERVICES_PER_SERVER;++i)
		{
			if(!services[i])
			{
				services[i]=b;
				services[i]->mgr=this;
				registerService(services[i],true);
				return i;
			}
		}
		return -1;
	}
*/
/*
	void ServiceMgr::RemoveService(ServiceBase *b)	// 只管解除掛載
	{
		for(int i=0;i<MAX_SERVICES_PER_SERVER;++i){
			if(services[i]==b){
				if(b)	b->Release();
				services[i]=NULL;
				registerService(services[i],false);
				break;
			}
		}
	}
*/
/*
	ServiceBase *ServiceMgr::FindServiceByID(int id)	// 搜尋服務
	{
		for(int i=0;i<MAX_SERVICES_PER_SERVER;++i){
			if(services[i] && services[i]->service_id==id){
				return services[i];
			}
		}
		return NULL;
	}
*/

/*
#define FOR_ALL_ENTITY(x)	for((x)=0;(x)<MAX_SERVICES_PER_SERVER;(x)++){	\
			if(services[x]!=NULL){
#define END_ALL_ENTITY()	}}

	void ServiceMgr::RunServices()	// 執行所有的 Service::Run
	{
		int i;
		STIME t;
		ServiceBase *sb;

		ServiceBase::run_time=GetBootTime();
		FOR_ALL_ENTITY(i)
			sb=services[i];
			t=ServiceBase::run_time-sb->last_run;
			if(sb->isrun && t>sb->interval){
				sb->Run();
				sb->last_run=ServiceBase::run_time;
			}
		END_ALL_ENTITY()
	}
*/
/*
	void ServiceMgr::OnJoin(int eid,int etype,char *edat,int dlen)	// 角色進入(通知所有模組)
	{
		int i;
		FOR_ALL_ENTITY(i)
			services[i]->OnJoin(eid,etype,edat,dlen);
		END_ALL_ENTITY()
	}
	void ServiceMgr::OnExit(int eid)	// 角色離開(通知所有模組)
	{
		int i;
		for(i=MAX_SERVICES_PER_SERVER-1;i>=0;i--){
			if(services[i]!=NULL)
				services[i]->OnExit(eid);
		}
	}
*/
/*
	void ServiceMgr::Shutdown()			// 將所有 service 卸載, 停用
	{
		for(int i=MAX_SERVICES_PER_SERVER-1;i>=0;i--){
			if(services[i]!=NULL){
				services[i]->Release();
				registerService(services[i],false);
			}
		}
	}
*/
/*
	ServiceMgr::~ServiceMgr()
	{
		Shutdown();
	}
*/
/*
	void ServiceMgr::registerService(ServiceBase *sb,bool reg)
	{
		_MSGDSPC_TBL *mdtbl;
		int i;
		mdtbl=sb->dispatcher_table;
		if(!mdtbl)	return;
		for(i=0;mdtbl[i].channel!=0;++i){
			if(reg)
				m_MsgDisp->RegisterChannel(mdtbl[i].channel,mdtbl[i].function);
			else
				m_MsgDisp->RegisterChannel(mdtbl[i].channel,NULL);
		}
	}
*/
	/// 新造資料時(所有資料均會被刪掉, 所以不可以直接連結)
/*
	void ServiceMgr::OnCreateEntity(int eid,int aid,const char *name,char *edat,int dlen)
	{
		int i;
		FOR_ALL_ENTITY(i)
			services[i]->OnCreateEntity(eid,aid,name,edat,dlen);
		END_ALL_ENTITY()
	}
*/
	/// 刪除資料時
/*
	void ServiceMgr::OnDeleteEntity(int eid)
	{
		int i;
		FOR_ALL_ENTITY(i)
			services[i]->OnDeleteEntity(eid);
		END_ALL_ENTITY()
	}
*/
	// 改變區域時
/*
	void ServiceMgr::OnZoneChanged(int eid,const EntityPosSpec_t *pos)
	{
		int i;
		FOR_ALL_ENTITY(i)
			services[i]->OnZoneChanged(eid,pos);
		END_ALL_ENTITY()
	}
*/
	// 儲存目前狀態
/*
	void ServiceMgr::Sync()
	{
		int i;
		FOR_ALL_ENTITY(i)
			services[i]->OnSync();
		END_ALL_ENTITY()
	}
*/
/*
	bool ServiceMgr::DLLInit(const HKOSVF_DLL *def,Cyberspace::MessageDispatcher* md)
	{
		HINSTANCE hinst;
		Cyberspace::MessageChannelFunction mcf;
		int i;

		hinst=LoadLibrary(def->libname);
		if(hinst!=NULL){
			for(i=0;def->funcs[i].channel!=0 && def->funcs[i].func_name!=NULL;++i){
				mcf=(Cyberspace::MessageChannelFunction)GetProcAddress(hinst,def->funcs[i].func_name);
				if(!mcf){	// roll back!!
					DLLFree(def,md);
					return false;
				} else {
					md->RegisterChannel(def->funcs[i].channel,mcf);
				}
			}
		}
		return true;
	}
	void ServiceMgr::DLLFree(const HKOSVF_DLL *def,Cyberspace::MessageDispatcher* md)
	{
		for(int i=0;def->funcs[i].channel!=0;++i)
			md->RegisterChannel(def->funcs[i].channel,NULL);
	}

*/
/*	void ServiceMgr::RegisterAllServices(bool reg)
	{
		int i,j;
		_MSGDSPC_TBL *mdtbl;
		for(i=0;i<MAX_SERVICES_PER_SERVER;i++){
			if(services[i]!=NULL)
				RegisterService(services[i],reg);
		}
	} */
	// 目前
/*	IO::TCPSocket *GetEntitySocket(long pUserData)	///< 給各個 service message handler 用
	{
		assert(false);
		return (IO::TCPSocket *)pUserData;
	}
*/	// 除錯用

};


