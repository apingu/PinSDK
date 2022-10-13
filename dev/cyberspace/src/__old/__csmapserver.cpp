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
//
//
//
#include "CsMapServer.h"
#include "CsConnect.h"
#include "CstAccount.h"
namespace Cyberspace
{
	#define GET_ENTITY(e)                   ( (CsEntity*)(GetCsEntity(e)) )
	using namespace GSO;
	namespace GSO
	{
	    BOOL DeleteAllEntity( unsigned int order, void* pEntity, void* pData )
		{
		    CsEntity* e = (CsEntity*)pEntity;
		    SAFE_DELETE( e );
			return true;
		}
	};

	//===============================================================
	//
	//
	//===============================================================
	// 簡單的東西幹麻稿的這麼複雜??
	CsEntity* GetEntity( GENTITY e )
	{
		EntityRoom* er = EntityRoom::GetRoom( e );
		if( er != NULL )
		    return (CsEntity*)EntityRoom::GetRoom( e )->GetEntity( e );
		return NULL;
	}
	//===============================================================
	//
	//
	//===============================================================
	CsRegion::CsRegion( RegionId_t id )
	{
		m_Id = id;
	}

	//===============================================================
	//
	//
	//===============================================================
	CsRegion::~CsRegion()
	{

	}
	//===============================================================
	// 對所有 member 進行某動作...
	//
	//===============================================================
	void CsRegion::ForAllEntities(EntityCallBack callback,void *UData,void *pExcept)
	{
	UtAutoCS __cs(&m_CS);
		int i;
		std::list<CsEntity *>::iterator ile;
		for(i=0,ile=m_Entities.begin();ile!=m_Entities.end();ile++,i++){
			if(pExcept!=*ile){
				(*callback)(i,*ile,UData);
			}
		}
	}
	//===============================================================
	//
	//
	//===============================================================
	bool CsRegion::InsertEntity(CsEntity *cse)
	{
		EntityList::iterator ile;
		if(EntityGroup::InsertEntity(cse)){
			ile=m_Entities.end();
			cse->m_Iter=--ile;
			return true;
		} else
			return false;
	}
	//===============================================================
	//
	//
	//===============================================================
	bool CsRegion::RemoveEntity(CsEntity *cse)
	{
	UtAutoCS __cs(&m_CS);
		if(cse->m_Iter!=NULL){
			m_Entities.erase(cse->m_Iter);
			cse->m_Iter=NULL;
			return true;
		} else
			return false;
	}

	void CsRegion::Clear()					// 清除所有的項目. 暫時禁止使用
	{
		assert(0 && "CsRegion::Clear() is called!!");
	}

	int SortRegion( const void* e0, const void* e1 )
	{
		return (*(int*)(e0)) - (*(int*)(e1));
	}
	//===============================================================
	// 增添所有 entity list
	// 除了石化的傢伙以外
	//===============================================================
	int	CsRegion::AddAllSockets(vector<IO::TCPSocket*>& socklist,bool npcsvr)
	{
		IO::TCPSocket *sk;
		CsAccount *csa;
		std::list<CsEntity*>::iterator ile;
		CsEntity *cse;
		int i,j;
		for(i=0,ile=m_Entities.begin();ile!=m_Entities.end();++ile){
			cse=*ile;
			sk=cse->GetSocket();
			csa=cse->GetAccount();
			// If it was logged out, nothing should be send
			if(!csa)
				continue;
			if(!npcsvr && csa->IsNPC())		// filter out NPC server
				continue;
			if(cse->IsStoned())	// stoned one won't know I'm changed
				continue;

			if(csa->GetNumEntities()>1){	// more than one entity, may have same
				for(j=0;j<socklist.size();j++){
					if(socklist[j]==sk)
						break;
				}
				if(j>=socklist.size()){
					socklist.push_back(sk);	++i;
				}
			} else {
				socklist.push_back(sk);	++i;
			}
		}
		return i;
	}
	//===============================================================
	// MapServer
	//
	//===============================================================
	MapServer::MapServer()
	{

	}


	//===============================================================
	//
	//
	//===============================================================
	MapServer::~MapServer()
	{
		//ForAllEntity( GSO::DeleteAllEntity, NULL );
        std::map< RegionId_t, CsRegion* >::iterator iter = m_Regions.begin();
        while( iter != m_Regions.end() )
		{
			SAFE_DELETE( iter->second );
		}
	}

	//===============================================================
	//
	//
	//===============================================================
	CsRegion* MapServer::GetRegion( RegionId_t rid )
	{
		// 新版...
		std::map< RegionId_t, CsRegion* >::iterator iter = m_Regions.find( rid );
		if( iter != m_Regions.end() )
			return iter->second;

		CsRegion* region= new CsRegion(rid);
        OnGetNeighbors( region->GetId(), (region->m_Neighbors) );
		if( !region->m_Neighbors.empty() )
	        qsort( &(region->m_Neighbors)[0], region->m_Neighbors.size(), sizeof(int), SortRegion );
	    m_Regions.insert( std::map< RegionId_t, CsRegion* >::value_type( rid, region ) );
	    return region;
	}

	//===============================================================
	// 對某一區所有 entity 執行 cbf
	//===============================================================
	void MapServer::ForAllEntities(int rid,EntityCallBack cbf,void *data,void *excp,int level)
	{
		CsRegion *reg=GetRegion(rid);
		if(reg){
			reg->ForAllEntities(cbf,data,excp);
			if(level>0){
				for(int i=0;i<reg->m_Neighbors.size();i++){
					ForAllEntities(reg->m_Neighbors[i],cbf,data,excp,level-1);
				}
			}
		}
	}
	//===============================================================
	// 對全世界所有 entity 執行 func(直到 return false)
	//===============================================================
	void MapServer::ForAllEntities(EntityCallBack func,void *data)
	{
		MAP_ENTP mp;
		CsEntity *cse;
		for(mp=m_EntityMap.begin();mp!=m_EntityMap.end();++mp){
			cse=mp->second;
			// tell whether it is used?
			if(cse->IsInUse()){
				if(!(*func)(mp->first,cse,data))
					break;
			}
		}
	}
	//===============================================================
	// 和 GENTITY 相關的函式
	//===============================================================
	RegionId_t MapServer::GetEntityRegionId( GENTITY e )
	{
		CsRegion* r = GetCsEntity(e)->m_Region;
		if( r!=NULL )
			return r->GetId();
		return -1;
	}
	EntityId_t MapServer::GetEntityId( GENTITY e )
	{
		return GET_ENTITY(e)->GetId();
	}
	void* MapServer::GetEntityPropbuf( GENTITY e )
	{
		return GET_ENTITY(e)->GetPropbuf();
	}
	size_t MapServer::GetEntityProplen( GENTITY e )
	{
		return GET_ENTITY(e)->GetProplen();
	}
	IO::TCPSocket* MapServer::GetEntitySocket( GENTITY e )
	{
		return ((CsCallsocket *)e)->fd;	// 配合 20040720改
	}
	CsEntity* MapServer::GetCsEntity( GENTITY e )
	{
		return (CsEntity*)(((EntityRoom *)this)->GetEntity(e));
	}

	//===============================================================
	// 同時造一個 GENTITY 以及 CsEntity 並且放入本類別管理
	//===============================================================
	GENTITY MapServer::CreateEntity(EntityId_t eid, void* propbuf, int proplen, IO::TCPSocket* s)
	{
		CsEntity* entity = NewCsEntity(eid,propbuf,proplen,s);
		if(entity){
			GENTITY e = InsertEntity( entity );
			entity->SetDispatcher( e );
			entity->SetRegion(NULL);
			m_EntityMap[eid]=entity;
			return e;
		} else
			return (GENTITY)0;
	}
	void MapServer::RemoveGEntity(GENTITY e)
	{
		CsEntity *cse;
		cse=(CsEntity *)RemoveEntity(e);
		if(cse){
			DeleteCsEntity(cse);
		}
	}
	// ----------------------------------------------------------------------------------
	CsEntity* MapServer::GetEntity( int eid, MAP_ENTP *ime )
	{
		MAP_ENTP imce;
		imce=m_EntityMap.find(eid);
		if(imce!=m_EntityMap.end()){
			if(ime)	*ime=imce;
			return (CsEntity *)(imce->second);
		} else {
			if(ime)	*ime=NULL;
			return NULL;
		}
	}
	GENTITY MapServer::GetGEntity( int eid )
	{
		CsEntity *cse;
		cse=GetEntity(eid,NULL);
		if(cse)
			return cse->m_Dispatcher;
		else
			return CsID_NULL;
	}

	// 計算跨 region 會影響的 region 有哪些
	// region: region 號碼
	// num: -1=傳送離去訊息,1=傳送進入訊息,0=傳送移動訊息
	int	MapServer::BuildRegionBCList(int srcrgn,int dstrgn,int flgs,RGN_LIST *output)	// 計算跳區影響的 region
	{
		// 這 code 真蠢...
		int i,n;
		bool ovr=false;
		CsRegion *csr;
		map<int,int> imap;
		map<int,int>::iterator imi;
		
		if(flgs & 1){	// 有來源
			csr=GetRegion(srcrgn);
			n=0;
			if(csr){
				imap[srcrgn]=-1;
				for(i=0;i<csr->m_Neighbors.size();i++)
					imap[csr->m_Neighbors[i]]=-1;
			}
		}
		if(flgs & 2){	// 有目標
			csr=GetRegion(dstrgn);
			if(csr){
				imap[dstrgn]=imap[dstrgn]+1;
				for(i=0;i<csr->m_Neighbors.size();i++)
					imap[csr->m_Neighbors[i]]=imap[csr->m_Neighbors[i]]+1;
			}
		}
		for(n=0,imi=imap.begin();imi!=imap.end();imi++,n++){
			output[n].region=imi->first;
			output[n].num=imi->second;
		}
		return n;
	}

	// 製造一個 CsEntity 衍生的類別給真 Server 用
	CsEntity *MapServer::NewCsEntity(EntityId_t eid,void *prop,int plen,IO::TCPSocket *socket)
	{
		CsEntity* entity = new CsEntity(eid);
		entity->m_Ownfd = socket;
		entity->SetPropbuf( prop, plen );
		entity->SetRegion(NULL);
		return entity;
	}
	// 移除一個 CsEntity 衍生的類別.
	void MapServer::DeleteCsEntity(CsEntity *cse)
	{
		MAP_ENTP ime;
		CsRegion *csr;
		CsEntity *cse2;
		if(cse){
			csr=cse->GetRegion();	// 想不到還會出現
			if(csr)
				csr->RemoveEntity(cse);
			cse2=GetEntity(cse->GetId(),&ime);
			if(cse2==cse){
				m_EntityMap.erase(ime);
			}
			delete cse;
		}
	}
	// For EntityGroup class ============================================================
	//===============================================================
	//
	//
	//===============================================================
	bool EntityGroup::InsertEntity(CsEntity *cse)
	{
	UtAutoCS __cs(&m_CS);
		EntityList::iterator ile;
		if(m_Entities.size()>=m_Max)
			return false;

		for(ile=m_Entities.begin();ile!=m_Entities.end();ile++)
			if(*ile==cse)	return true;	// 已經加入過了

		m_Entities.push_back(cse);
		return true;
	}
	//===============================================================
	//
	//
	//===============================================================
	bool EntityGroup::RemoveEntity(CsEntity *cse)
	{
	UtAutoCS __cs(&m_CS);
		EntityList::iterator ile;
		for(ile=m_Entities.begin();ile!=m_Entities.end();ile++){
			if(cse==*ile){
				m_Entities.erase(ile);
				return true;
			}
		}
		return false;
	}
	//===============================================================
	// 找出本區內的所有 Socket(不重複), 增添到給定 vector 內. 當然也不會和本來有的重複
	//
	//===============================================================
	int	EntityGroup::AddAllSockets(vector<IO::TCPSocket*>& socklist,bool npcsvr)
	{
		IO::TCPSocket *sk;
		CsAccount *csa;
		std::list<CsEntity*>::iterator ile;
		CsEntity *cse;
		int i,j;
		for(i=0,ile=m_Entities.begin();ile!=m_Entities.end();++ile){
			cse=*ile;
			sk=cse->GetSocket();
			csa=cse->GetAccount();
			// If it was logged out, nothing should be send
			if(!csa)
				continue;
			if(!npcsvr && csa->IsNPC())		// filter out NPC server
				continue;

			if(csa->GetNumEntities()>1){	// more than one entity, may have same
				for(j=0;j<socklist.size();j++){
					if(socklist[j]==sk)
						break;
				}
				if(j>=socklist.size()){
					socklist.push_back(sk);	++i;
				}
			} else {
				socklist.push_back(sk);	++i;
			}
		}
		return i;
	}
	//===============================================================
	// 增添所有 entity list
	//
	//===============================================================
	int	EntityGroup::AddAllEntities(std::list<CsEntity*>& elist)
	{
		std::list<CsEntity*>::iterator ile;
		int i;
		for(i=0,ile=m_Entities.begin();ile!=m_Entities.end();++ile,++i)
			elist.push_back(*ile);
		return i;
	}
	//===============================================================
	// 
	//
	//===============================================================
	EntityGroup::EntityGroup()
	:m_Id(0),m_Max(0x7fffffff)
	{}
	//===============================================================
	// 清除所有元件
	// 本類別不會利用外部資料
	//===============================================================
	void EntityGroup::Clear()	// 清除所有的項目.
	{
		m_Entities.clear();
	}


};//Cyberspace
