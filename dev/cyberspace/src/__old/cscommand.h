// Cyber space common define file
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
// 20040624: 把 NEW_VERSION 的東西好的地方融入系統內, 並統一化
// 20040519: 又搞出一堆沒法 compile 的東西, 好像沒define NEWVERSION_0005 就什麼也不是一樣
// 20040505: Wood 將 tcode 去除, 只留EID
// 20040427: Wood 將許多協定中的 entity name 改成 tcode+EID

#ifndef CSCOMMAND_H
#define CSCOMMAND_H

#pragma warning(disable:4786)

#include "csdef.h"
#include "CsConnect.h"
#include "CsErr.h"

#include <vector>

/*
typedef struct {
	short len;
	char string[1];
} NETSTR;
#define NETSTR_SIZE( s )    s->len+sizeof(short)
*/

enum CsSYSTEMCOMMAND {
					// login, logout
					   CsC_REQUEST_LOGIN,
                       CsC_FEEDBACK_LOGIN,
					   CsC_REQUEST_LOGOUT,
					   // create entity
					   CsC_REQUEST_CREATENEWENTITY,   //->CsC_FEEDBACK_GETENTITYRECORD
   					   CsC_REQUEST_GETALLENTITYRECORD,//->CsC_FEEDBACK_GETENTITYRECORD
					   CsC_FEEDBACK_GETENTITYRECORD,
					   // select entity(no use now)
					   CsC_REQUEST_SELECTENTITY,      //when select one entity by name it will feedback entity id
					   CsC_FREEBACK_SELECTENTITY,
					   // delete entity
					   CsC_REQUEST_DELETEENTITY,	  // 刪除
					   CsC_FEEDBACK_DELETEENTITY,
					   // join game/region
					   CsC_REQUEST_ENTITYJOIN,	// client -> server
					   CsC_FEEDBACK_ENTITYJOIN,	// server -> client
					   // exit game/region
					   CsC_REQUEST_ENTITYEXIT,	// client -> server					   
					   CsC_FEEDBACK_ENTITYEXIT,	// server -> client
					   // change region
					   CsC_REQUEST_CHANGE_REGION,	// C->S, request to change region
					   // join with name
					   CsC_REQUEST_ENTITYJOIN2,	// C->S string
					   CsC_ENTERREGION,			// S->C EID RID
					   //CsC_REQUEST_GETALLENTITYREC2,	// 依照種類查詢(臨時性,已經不需要) entity
					   // Ping用, client:Req, server:Feedback, client:measure time
					   CsC_REQUEST_PING,
					   CsC_FEEDBACK_PING,
					   
					   //Dispatch message
					   CsDC_REQUEST_REGISTER_SERVER,
					   
					   // 本 Cyberspace server 定義的就到此為止, 後面留下一點給分出的 Server 用.
					   CsC_USER,				// 上層定義的, 基本伺服器不定義
					   CsC_SYSTEM_MSG_END=128	// 最終標記
};
/// 格式(僅資料部分):
/// 隱含:
/// EID: entity ID(依照種別的物件號碼), +才是有效(4 bytes)
/// string(): 我的字串格式:(short)+data

/// CsC_REQUEST_LOGIN: username+'\0'+password+'\0'
/// CsC_FEEDBACK_LOGIN: 改為 (int)AccountID(MAX_UINT=fail)
/// CsC_REQUEST_CREATENEWENTITY: string(name)+一些 property(結構未定)
/// CsC_FEEDBACK_GETENTITYRECORD: EID+屬性資料(不定長度)
/// CsC_REQUEST_GETALLENTITYRECORD:	沒資料(因為這個不必資料)
/// CsC_REQUEST_SELECTENTITY: EID
/// CsC_FREEBACK_SELECTENTITY: EID+MAP_value

/// CsC_REQUEST_ENTITYJOIN: (int)EID
/// CsC_FEEDBACK_ENTITYJOIN: (int)EID+一些 property(結構未定)
/// CsC_REQUEST_ENTITYEXIT: (int)EID
/// CsC_FEEDBACK_ENTITYEXIT: (int)EID
/// CsC_REQUEST_LOGOUT: 沒資料
// 新的:
/// CsC_REQUEST_DELETEENTITY: EID(同selectentity)
/// CsC_FEEDBACK_DELETEENTITY: ???
///	CsC_REQUEST_PING: seq_number
/// CsC_FEEDBACK_PING: seq_number (直接傳回)
/// CsC_REQUEST_CLIENTSTART: entity ID to start
/// CsC_ENTERREGION: EID+RID(int)
/// CsC_REQUEST_CHANGE_REGION: EID(int),RID(int)
/// CsC_REQUEST_ENTITYJOIN2: string(name)



//============================================



//======================================
//	Login Command Message
//======================================
namespace CsLoginMessage
{
	inline void SetRequestMessage(CsCommandMessage *msg, const char *username, const char *password)
	{
		char *p;
		msg->Header.CommandId  = CsC_REQUEST_LOGIN;
		msg->Header.CommandLen = strlen(username) + 1 + strlen(password) + 1;
		
		p = msg->Buff;

		strcpy(p , username);
		p += strlen(username);
		*p++ = '\0';

		strcpy(p , password);
		p += strlen(password);
		*p++ = '\0';
	}

	inline void SetFeedbackMessage(CsCommandMessage *msg, AccountId_t aid)
	{
		msg->Header.CommandId = CsC_FEEDBACK_LOGIN;
		msg->Header.CommandLen = sizeof(aid);
		(*(AccountId_t *)msg->Buff) = aid;
	}

	inline char* GetUsername( CsCommandMessage *msg )
	{
		if( msg->Header.CommandId == CsC_REQUEST_LOGIN )
		    return msg->Buff;
		return NULL;
	}

	inline char* GetPassword( CsCommandMessage *msg )
	{
		if( msg->Header.CommandId == CsC_REQUEST_LOGIN )
		{
		    char *p = msg->Buff;
		    p += strlen(p);
		    return p+1;
		}
		return NULL;
	}
};

///////////////////////////////////////////////////////////////////////////////////////
// CLIENT 通知SERVER新增/刪除 角色
//======================================
//	Select Entity Command Message
//======================================
namespace CsCreateEntityMessage
{
	//use int client
	inline void SetRequestMessage( CsCommandMessage *msg, const char* name, void* properity, size_t len )
	{
		NETSTR *nstr;
		int l;
		msg->Header.CommandId  = CsC_REQUEST_CREATENEWENTITY;
		nstr=(NETSTR *)msg->Buff;
		nstr->len=strlen(name)+1;
		strcpy(nstr->string,name);
		l=NETSTR_SIZE(nstr);
		memcpy(msg->Buff+l,properity,len);
		l+=len;
		msg->Header.CommandLen=l;
	}

	inline const char* GetEntityName( CsCommandMessage *msg )
	{
		NETSTR *nstr;
		if( msg->Header.CommandId == CsC_REQUEST_CREATENEWENTITY )
		{
			nstr=(NETSTR *)msg->Buff;
			return nstr->string;
		}
		return NULL;
	}

	inline void* GetProperty( CsCommandMessage* msg, size_t& len )
	{
		NETSTR *nstr;
		int hl;
		if( msg->Header.CommandId == CsC_REQUEST_CREATENEWENTITY )
		{
			nstr=(NETSTR *)msg->Buff;
			hl=nstr->len+sizeof(short);
			len = msg->Header.CommandLen - hl;
			if( len==0 )
				return NULL;
			return msg->Buff+hl;
		}
		len=0;
		return NULL;
	}
};

// 刪除資料用
namespace CsDeleteEntityMessage
{
/*
#ifdef _NEW_VERSION_0005
	inline void SetRequestMessage( CsCommandMessage *msg, const char* name)
	{
		NETSTR *nstr;
		msg->Header.CommandId=CsC_REQUEST_DELETEENTITY;
		nstr=(NETSTR *)msg->Buff;
		nstr->len=strlen(name)+1;
		strcpy(nstr->string,name);
		msg->Header.CommandLen=NETSTR_SIZE(nstr);;
	}
	inline const char* GetEntityName(CsCommandMessage *msg)
	{
		NETSTR *nstr;
		if(msg->Header.CommandId==CsC_REQUEST_DELETEENTITY)
		{
			nstr=(NETSTR *)msg->Buff;
			return nstr->string;
		}
		return NULL;
	}
#else
*/
	inline void SetRequestMessage(CsCommandMessage *msg,EntityId_t eid)
	{
		msg->Header.CommandId=CsC_REQUEST_DELETEENTITY;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&eid,sizeof(EntityId_t));
	}
	inline EntityId_t GetEntityID(const CsCommandMessage *msg)
	{
		if(msg->Header.CommandId==CsC_REQUEST_DELETEENTITY)
			return *(EntityId_t *)(msg->Buff);
		else
			return CSERR_PACKET_MISMATCH;
	}
//#endif
	inline void SetFeedbackMessage(CsCommandMessage *msg, int retval)
	{
		msg->Header.CommandId=CsC_FEEDBACK_DELETEENTITY;
		msg->Header.CommandLen=sizeof(int);
		memcpy(msg->Buff,&retval,msg->Header.CommandLen);
	}
	inline int GetFeedBackRet(const CsCommandMessage *msg)
	{
		if(msg->Header.CommandId==CsC_FEEDBACK_DELETEENTITY)
			return *((int *)msg->Buff);
		else
			return CSERR_PACKET_MISMATCH;
	}
};




//======================================
//	Get record entity Command Message
//======================================
namespace CsGetRecordEntityMessage
{
	/*
#ifdef _NEW_VERSION_0005
	//傳給CLINET端已建立的角色資料ㄝ, 建立新角色成功後，與要求傳回以建立資料都會呼叫
	inline void SetFeedbackMessage( CsCommandMessage *msg, const char* name, const void* properity, size_t len )
	{
		NETSTR *nstr;
		int l;
		msg->Header.CommandId = CsC_FEEDBACK_GETENTITYRECORD;
		nstr=(NETSTR *)msg->Buff;
		nstr->len=strlen(name)+1;
		strcpy(nstr->string,name);
		l=NETSTR_SIZE(nstr);
		memcpy(msg->Buff+l,properity,len);
		l+=len;
		msg->Header.CommandLen=l;

	}
	inline const char* GetEntityName(const CsCommandMessage *msg)
	{
		NETSTR *nstr;
		if( msg->Header.CommandId == CsC_FEEDBACK_GETENTITYRECORD )
		{
			nstr=(NETSTR *)msg->Buff;
			return nstr->string;
		}
		return NULL;
	}
#else
	*/
	//傳給CLINET端已建立的角色資料ㄝ, 建立新角色成功後，與要求傳回以建立資料都會呼叫
	inline void SetFeedbackMessage( CsCommandMessage *msg, EntityId_t eid, const char* properity, size_t len )
	{
		msg->Header.CommandId = CsC_FEEDBACK_GETENTITYRECORD;
		msg->Header.CommandLen = sizeof(EntityId_t)+len;
		memcpy(msg->Buff,&eid,sizeof(EntityId_t));
		memcpy(msg->Buff+sizeof(EntityId_t),properity,len);
	}
	inline EntityId_t GetEntityID(const CsCommandMessage *msg)
	{
		if( msg->Header.CommandId == CsC_FEEDBACK_GETENTITYRECORD )
			return *(EntityId_t *)(msg->Buff);
		else
			return CSERR_PACKET_MISMATCH;
	}
//#endif

	inline void* GetProperty( CsCommandMessage* msg, size_t& len )
	{
		int hdr=sizeof(EntityId_t);
		if( msg->Header.CommandId == CsC_FEEDBACK_GETENTITYRECORD )
		{
			len = msg->Header.CommandLen - hdr;
			if( len>0 )
				return msg->Buff+hdr;
		}
		len=0;
		return NULL;
	}
}





//////////////////////////////////////////////////////////////////////////////
// Client 向SERVER要求使用一個角色
// 與 SERVER 向CLIENT 回報 使用角色的 ID
//======================================
//	Select Entity Command Message
//======================================
namespace CsSelectEntityMessage
{
	/*
#ifdef _NEW_VERSION_0005
	inline void SetRequestMessage( CsCommandMessage *msg,  const char* name )
	{
		NETSTR *nstr;
		msg->Header.CommandId  = CsC_REQUEST_SELECTENTITY;
		nstr=(NETSTR *)msg->Buff;
		nstr->len=strlen(name)+1;
		strcpy(nstr->string,name);
		msg->Header.CommandLen=NETSTR_SIZE(nstr);
	}

	inline const char* GetEntityName(const CsCommandMessage *msg)
	{
		NETSTR *nstr;
		if(msg->Header.CommandId == CsC_REQUEST_SELECTENTITY)
		{
			nstr=(NETSTR *)msg->Buff;
			return nstr->string;
		}
		return NULL;
	}
#else
	*/
	inline void SetRequestMessage(CsCommandMessage *msg,EntityId_t eid)
	{
		msg->Header.CommandId  = CsC_REQUEST_SELECTENTITY;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&eid,msg->Header.CommandLen);
	}
	inline EntityId_t GetEntityID(const CsCommandMessage *msg)
	{
		EntityId_t *eid;
		if(msg->Header.CommandId != CsC_REQUEST_SELECTENTITY)
			return CSERR_PACKET_MISMATCH;
		eid=(EntityId_t *)(msg->Buff);
		return *eid;
	}
//#endif
};


//======================================
//	when cleint choose one entity by name
//  server will feedback one entity id to
//  client
//  @return entity id
//======================================
namespace CsSelectEntityIdMessage
{
	/*
#ifdef _NEW_VERSION_0005
	inline void SetFeedbackMessage( CsCommandMessage *msg, int eid )
	{
		msg->Header.CommandId  = CsC_FREEBACK_SELECTENTITY;
		msg->Header.CommandLen=sizeof(int);
		memcpy(msg->Buff,&eid,sizeof(int));
	}
	inline int GetEntityId(const CsCommandMessage *msg )
	{
		if( msg->Header.CommandId == CsC_FREEBACK_SELECTENTITY )
		{
			return *(int*)(msg->Buff);
		}
		return CSERR_PACKET_MISMATCH;
	}
#else
	*/
	inline void SetFeedbackMessage(CsCommandMessage *msg,const char* name,int eid,int rid)
	{
		int idx;
		msg->Header.CommandId  = CsC_FREEBACK_SELECTENTITY;
		msg->Header.CommandLen=sizeof(int)*2;
		idx=0;
		memcpy(msg->Buff+idx,&eid,sizeof(int));
		idx+=sizeof(int);
		memcpy(msg->Buff+idx,&rid,sizeof(int));		
	}
	inline int GetEntityId(const CsCommandMessage *msg )
	{
		if( msg->Header.CommandId == CsC_FREEBACK_SELECTENTITY )
		{
			return *(int*)(msg->Buff);
		}
		return CSERR_PACKET_MISMATCH;
	}
	inline int GetRegionID(const CsCommandMessage *msg)
	{
		if( msg->Header.CommandId == CsC_FREEBACK_SELECTENTITY )
		{
			return *(int*)(msg->Buff+sizeof(int));
		} 
		return CSERR_PACKET_MISMATCH;
	}
//#endif
}





///////////////////////////////////////////////////////////////////////////////////////
//CLINET 告知 SERVER 正式加入/離開遊戲
//======================================
//	exit Entity Command Message
//======================================
namespace CsEntityJoinMessage
{
	// Join(eid)
	inline void SetRequestMessage( CsCommandMessage *msg, EntityId_t eid )
	{
		msg->Header.CommandId  = CsC_REQUEST_ENTITYJOIN;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&eid,msg->Header.CommandLen);
	}

	inline EntityId_t GetEntityID(const CsCommandMessage *msg)
	{
		EntityId_t *eid;
		if(msg->Header.CommandId != CsC_REQUEST_ENTITYJOIN)
			return CSERR_PACKET_MISMATCH;
		eid=(EntityId_t *)(msg->Buff);
		return *eid;
	}
	inline void SetFeedbackMessage(CsCommandMessage *msg,EntityId_t eid,char *prop,int plen)
	{
		msg->Header.CommandId=CsC_FEEDBACK_ENTITYJOIN;
		msg->Header.CommandLen=sizeof(EntityId_t)+plen;
		memcpy(msg->Buff,&eid,sizeof(EntityId_t));
		if(prop)
			memcpy(msg->Buff+sizeof(EntityId_t),prop,plen);
	}
};

//======================================
//	exit Entity Command Message
//======================================
namespace CsEntityExitMessage
{
	// ExitGame(eid)
	inline void SetRequestMessage( CsCommandMessage *msg,EntityId_t eid )
	{
		msg->Header.CommandId  = CsC_REQUEST_ENTITYEXIT;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&eid,msg->Header.CommandLen);
	}

	inline EntityId_t GetEntityID(const CsCommandMessage *msg)
	{
		EntityId_t *eid;
		if(msg->Header.CommandId != CsC_REQUEST_ENTITYEXIT &&
			msg->Header.CommandId != CsC_FEEDBACK_ENTITYEXIT )
			return CSERR_PACKET_MISMATCH;
		eid=(EntityId_t *)(msg->Buff);
		return *eid;
	}
	inline void SetFeedbackMessage(CsCommandMessage *msg,int code)
	{
		msg->Header.CommandId  = CsC_FEEDBACK_ENTITYEXIT;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&code,msg->Header.CommandLen);
	}
};
//#else

///////////////////////////////////////////////////////////////////////////////////////
//由SERVER主動通知 CLIENT 有角色加入/離開
//======================================
//	Enter Entity Command Message
//  由SERVER通知CLINET加入一個角色
//======================================
namespace CsEnterEntityMessage
{
	inline void SetFeedbackMessage( CsCommandMessage *msg, EntityId_t eid,RegionId_t rid,
		void* prop, size_t len )
	{
		msg->Header.CommandId  = CsC_FEEDBACK_ENTITYJOIN;
		msg->Header.CommandLen = len+sizeof(eid);
		memcpy( msg->Buff, &eid, sizeof(eid)  );
		memcpy( msg->Buff+sizeof(eid), prop, len );
	}

	inline EntityId_t GetEntityID(const CsCommandMessage *msg )
	{
		EntityId_t *eidp;
		if( msg->Header.CommandId == CsC_FEEDBACK_ENTITYJOIN )
		{
			eidp=(EntityId_t *)msg->Buff;
    	    return *eidp;
		}
		return CSERR_PACKET_MISMATCH;
	}
	inline const char *GetProperty(const CsCommandMessage *msg,int &len)
	{
		if( msg->Header.CommandId == CsC_FEEDBACK_ENTITYJOIN )
		{
			len=msg->Header.CommandLen-4;
    	    return msg->Buff+4;
		}
		return NULL;
	}
};

//======================================
//	Depart Entity Command Message
//  由SERVER通知CLINET移除一個角色
//======================================
/*
namespace CsDepartEntityMessage
{
	inline void SetFeedbackMessage( CsCommandMessage *msg, EntityId_t eid )
	{
		msg->Header.CommandId  = CsC_REQUEST_ENTITYEXIT;
		msg->Header.CommandLen = sizeof(eid);
		memcpy( msg->Buff, &eid, sizeof(eid)  );
	}

	inline EntityId_t GetEntityID(const CsCommandMessage *msg )
	{
		EntityId_t eid=CsID_NULL;
		if( msg->Header.CommandId == CsC_REQUEST_ENTITYEXIT)
		{
    	    memcpy( &eid, msg->Buff, sizeof(eid) );
		}
		return eid;
	}
	inline const char *GetProperty(const CsCommandMessage *msg,int &len)
	{
		if( msg->Header.CommandId == CsC_REQUEST_ENTITYEXIT)
		{
			len=msg->Header.CommandLen-sizeof(EntityId_t);
    	    return msg->Buff+sizeof(EntityId_t);
		}
		return NULL;
	}
};
*/
//#endif




//==============================================
// For ping message
//==============================================
namespace CsPingMessage
{
	inline void SetRequestMessage(CsCommandMessage *buf,int seq)
	{
		buf->Header.CommandId=CsC_REQUEST_PING;
		buf->Header.CommandLen=sizeof(int);
		memcpy(buf->Buff,(char *)&seq,sizeof(int));
	}
};



/////////////////////////////////////////////////////////////////////////////////////
//  Arbit server command
namespace CsDRegisterServerMessage
{
	inline void SetRequestMessage( CsCommandMessage *msg, const char* addr, unsigned int port )
	{
		msg->Header.CommandId=CsDC_REQUEST_REGISTER_SERVER;
		msg->Header.CommandLen= strlen(addr) + 1 + sizeof( unsigned int );
		memcpy( msg->Buff,addr, strlen(addr) );
		(*(unsigned int*)(msg->Buff+strlen(addr)+1))=port;
	}

	inline const char* GetIpAddress(const CsCommandMessage *msg )
	{
		if( msg->Header.CommandId!=CsDC_REQUEST_REGISTER_SERVER )
			return NULL;
		return msg->Buff;
	}

	inline unsigned int GetPort(const CsCommandMessage *msg )
	{
		if( msg->Header.CommandId!=CsDC_REQUEST_REGISTER_SERVER )
			return NULL;
		return (*(unsigned int*)(msg->Buff+strlen(msg->Buff)+1) );
	}
}

// client 要求離開遊戲
/*
namespace CsDepartEntityRequest
{
	inline void SetRequestMessage( CsCommandMessage *msg,EntityId_t eid )
	{
		msg->Header.CommandId = CsC_REQUEST_ENTITYEXIT;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&eid,sizeof(eid));
	}
	inline void SetFeedbackMessage( CsCommandMessage *msg,EntityId_t eid)
	{
		msg->Header.CommandId = CsC_FEEDBACK_ENTITYEXIT;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&eid,sizeof(eid));
	}
	inline int GetEntityId(const CsCommandMessage *msg)
	{
		int *ip;
		if(msg->Header.CommandId != CsC_REQUEST_ENTITYEXIT)
			return CSERR_PACKET_MISMATCH;
		ip=(int *)msg->Buff;
		return *ip;
	}
};
*/
// client 要求改變所在區
namespace CsChangeRegionRequest
{
	inline void SetRequestMessage( CsCommandMessage *msg,EntityId_t eid,RegionId_t rid )
	{
		msg->Header.CommandId = CsC_REQUEST_CHANGE_REGION;
		msg->Header.CommandLen=sizeof(EntityId_t)+sizeof(RegionId_t);
		memcpy(msg->Buff,&eid,sizeof(eid));
		memcpy(msg->Buff+4,&rid,sizeof(rid));
	}
	inline void SetFeedbackMessage( CsCommandMessage *msg,EntityId_t eid,RegionId_t rid)
	{
		msg->Header.CommandId = CsC_ENTERREGION;
		msg->Header.CommandLen=sizeof(EntityId_t)+sizeof(RegionId_t);
		memcpy(msg->Buff,&eid,sizeof(eid));
		memcpy(msg->Buff+4,&rid,sizeof(rid));
	}
	inline int GetEntityID(const CsCommandMessage *msg)
	{
		int *ip;
//		if(msg->Header.CommandId != CsC_REQUEST_CHANGE_REGION)
//			return CSERR_PACKET_MISMATCH;
		ip=(int *)msg->Buff;
		return *ip;
	}
	inline int GetRegionID(const CsCommandMessage *msg)
	{
		int *ip;
//		if(msg->Header.CommandId != CsC_REQUEST_CHANGE_REGION)
//			return CSERR_PACKET_MISMATCH;
		ip=(int *)(msg->Buff+4);
		return *ip;
	}
};	

// Client 要求 Server 將我加入適當的世界
namespace CsEnterEntityRequest
{
	inline void SetRequestMessage( CsCommandMessage *msg,EntityId_t eid)
	{
		msg->Header.CommandId = CsC_REQUEST_ENTITYJOIN;
		msg->Header.CommandLen=sizeof(EntityId_t);
		memcpy(msg->Buff,&eid,sizeof(eid));
	}
	inline void SetFeedbackMessage( CsCommandMessage *msg,EntityId_t eid,RegionId_t rid)
	{
		CsChangeRegionRequest::SetFeedbackMessage(msg,eid,rid);
	}
	inline int GetEntityID(const CsCommandMessage *msg)
	{
		int *ip;
//		if(msg->Header.CommandId != CsC_REQUEST_ENTITYJOIN)
//			return CSERR_PACKET_MISMATCH;
		ip=(int *)msg->Buff;
		return *ip;
	}
};	

namespace CsEnterEntityRequest2
{
	inline void SetRequestMessage( CsCommandMessage *msg,const char *name)
	{
		short s;
		msg->Header.CommandId = CsC_REQUEST_ENTITYJOIN2;
		s=(short)strlen(name);
		msg->Header.CommandLen=s+sizeof(short);
		memcpy(msg->Buff,&s,sizeof(s));
		memcpy(msg->Buff+sizeof(short),name,s);
	}
	inline void SetFeedbackMessage( CsCommandMessage *msg,EntityId_t eid,RegionId_t rid)
	{
		CsChangeRegionRequest::SetFeedbackMessage(msg,eid,rid);
	}
	inline int GetEntityName(const CsCommandMessage *msg,char *buf)
	{
//		if(msg->Header.CommandId != CsC_REQUEST_ENTITYJOIN2)
//			return CSERR_PACKET_MISMATCH;
		short *sp=(short *)msg->Buff;
		strncpy(buf,msg->Buff+sizeof(short),*sp);
		buf[*sp]='\0';
		return *sp;
	}
};

#endif //CSHMESSAGE_CPP
