/*
 *  File:       CsServiceBase.h
 *	Version:	1.0
 *  Desc:       Service的基礎類別，所有的service請繼承此類別
 *  Creater:    Wood
 *  Create:     Mar 26, 2004
 *  Updater:    Sean
 *  Update:     05/02/16
 */

#ifndef _SERVICEBASE_H_
#define _SERVICEBASE_H_

#pragma warning(disable:4786)

#include "CsGameServer.h"

namespace Cyberspace {

//! Game Service 以功能群組為單位的 Service
class ServiceBase
{
	friend GameServer;
protected:
	//! 服務種類號碼(另行定義)
	int		service_id;

	const char*	version_info;	///< 版本資訊字串(請在 ctor 或 Init() 設定它)

	bool		m_dbg_en;		///< 除錯資訊
public:
	ServiceBase();
	virtual ~ServiceBase(){};
	// 取得本服務版本資訊
	inline const char *GetInfo(){	return version_info;	}

	// 以下至少三個需要改寫以符合各模組需求
	/// GameServer 要求初始化此模組, 此函式必須設定以上三個參數
	virtual bool OnInit();		// 可以在這裡 RegisterChannel...
	/// 結束此模組時呼叫
	virtual void OnRelease()					{}
	/// 這個主要是給 game module 使用, 用來處理各種事項...
	virtual void OnUpdate(GameFrame_t frames)	{}		// 定期執行的方法
	/// 有角色進入系統通知...(接收 server 的事件)
	virtual void OnJoin(CharacterId_t entity_id, char *edat, int dlen){}
	/// 有角色離開系統時通知...(接收 server 的事件)
	virtual void OnExit(CharacterId_t entity_id)			{}
	// 新加! 有角色新造以及刪除時(為了模組專屬資料庫處理專用)
	/// 新造資料時呼叫(這些資料變成可以更改,當作初始值)
	virtual void OnCreateEntity(CharacterId_t entity_id, const char *name, char *edat, int dlen){}
	/// 刪除資料時
	virtual void OnDeleteEntity(CharacterId_t entity_id, const char *name, void *prop, int plen) {};
	/// 儲存狀態資料時(要求將記憶體資料儲存起來)(系統維護時使用)(基本上存完後仍然繼續運作)
	virtual void OnSync(){}

public:
	/// 宿主
	static void SetHost(GameServer* game_server)	{	s_GameServer = game_server;		}

public:
	// TODO: Pin
	// sean: 這裡我把部分函式先mark，但是我不知道底層可以用什麼來輸出錯誤訊息
	// 所以可能要請老大幫個忙。
	// 另外，service輸出log有一個需求: 每一個service可以輸出自己的log，以便除錯。
	// 除錯用. 可以替換不同的除錯輸出裝置(See debug.h)
	void		SetLogMessage(const char *format,...);
//	void		SetLogObject(DebugOutput *obj);
	void		EnableLog(bool b);


protected:
	//1 --註冊回呼函式----------------------------------------------------------
	inline bool			RegisterChannel( channelid_t id, MessageChannelFunction func )							{		return s_GameServer->RegisterChannel(id, func);	}
	inline bool			UnRegisterChannel( channelid_t id )														{		return s_GameServer->UnRegisterChannel(id);		}

	//---Send message function-------------------------------------------------
	/// 會轉換人的位置，並將訊息送到一個區域以及鄰近區域
	inline void         PublishMessageOnSphere	( GSO::GENTITY who, channelid_t cid, void *data, size_t len )	{		s_GameServer->PublishMessageOnSphere(who, cid, data, len);	}
	/// 會轉換人的位置，將訊息送到一個區域
	inline void         PublishMessageOnRegion	( GSO::GENTITY who, channelid_t cid, void *data, size_t len )	{		s_GameServer->PublishMessageOnRegion(who, cid, data, len);	}

	//! 不會轉換人的位置，並將訊息送到一個區域以及鄰近區域( 有發送者 )
	inline void			PublishMessageToSphere  ( GSO::GENTITY who, RegionId_t rid, channelid_t cid, void *data, size_t len )	{	s_GameServer->PublishMessageToSphere(who, rid, cid, data, len);	}
	//! 不會轉換人的位置，並將訊息送到一個區域以及鄰近區域( 沒有發送者 )
	inline void			PublishMessageToSphere  ( RegionId_t rid, channelid_t cid, void *data, size_t len )						{	s_GameServer->PublishMessageToSphere(rid, cid, data, len);	}
	//! 不會轉換人的位置，將訊息送到一個區域( 有發送者 )
	inline void			PublishMessageToRegion  ( GSO::GENTITY who, RegionId_t rid, channelid_t cid, void *data, size_t len )	{	s_GameServer->PublishMessageToRegion(who, rid, cid, data, len);	}
	//! 不會轉換人的位置，將訊息送到一個區域( 沒有發送者 )
	inline void			PublishMessageToRegion  ( RegionId_t rid, channelid_t cid, void *data, size_t len )						{	s_GameServer->PublishMessageToRegion(rid, cid, data, len);	}

	//! 傳送訊息給 Entity(有DSM)
	inline int          SendMessageToEntity		( CharacterId_t eid, channelid_t cid, void *data, size_t len )					{	return s_GameServer->SendMessageToEntity(eid, cid, data, len);	}
	//! 傳送訊息給 Entity(有DSM)
	inline int			SendMessageToEntity		( CharacterId_t eid, GSO::GENTITY from, channelid_t cid, void *data, size_t len ){	return s_GameServer->SendMessageToEntity(eid, from, cid, data, len);	}
	
	//! 傳送訊息給 帳號(有DSM)
	inline int			SendMessageToAccount   ( ConnectionId_t aid, channelid_t cid, void *data, size_t len )					{	return s_GameServer->SendMessageToAccount(aid, cid, data, len);		}
	//! 傳送訊息給 帳號(有DSM)
	inline int			SendMessageToAccount   ( ConnectionId_t aid, GSO::GENTITY from, channelid_t cid, void *data, size_t len ){	return s_GameServer->SendMessageToAccount(aid, from, cid, data, len);	}
	//! 傳送訊息給 帳號(有DSM)
	inline int			SendMessageToAccount   ( const char* ane, channelid_t cid, void *data, size_t len )						{	return s_GameServer->SendMessageToAccount(ane, cid, data, len);	}
	//! 傳送訊息給 帳號(有DSM)
	inline int			SendMessageToAccount   ( const char* ane, GSO::GENTITY from, channelid_t cid, void *data, size_t len )	{	return s_GameServer->SendMessageToAccount(ane, from, cid, data, len);	}

	//! 廣播訊息 系統訊息傳給Server中每一個client
	inline void			BroadcastMessage       ( channelid_t cid, void *data, size_t len)										{	s_GameServer->BroadcastMessage(cid, data, len);	}
	//! 廣播訊息 系統訊息傳給Server中每一個client
	inline void			BroadcastMessage       ( channelid_t cid, GSO::GENTITY from, void *data, size_t len)					{	s_GameServer->BroadcastMessage(cid, from, data, len);	}

public:
	//---抓ENTITY 的資料 GetEntityXXXXX----------------------------------------
	inline RegionId_t       GetEntityRegionId( GSO::GENTITY e )							{	return s_GameServer->GetEntityRegionId(e);	}
	inline void*            GetEntityPropbuf( GSO::GENTITY e )							{	return s_GameServer->GetEntityPropbuf(e);	}
	inline size_t           GetEntityProplen( GSO::GENTITY e )							{	return s_GameServer->GetEntityProplen(e);	}
	inline IO::TCPSocket*   GetEntitySocket( GSO::GENTITY e )							{	return s_GameServer->GetEntitySocket(e);	}
	inline const char*      GetEntityName( GSO::GENTITY e )								{	return s_GameServer->GetEntityName(e);	}
	inline CharacterId_t    GetEntityId( GSO::GENTITY e )				    			{	return s_GameServer->GetEntityId(e);	}

	// 會很慢請儘量少用
	GSO::GENTITY			GetGEntity( CharacterId_t eid )								{	return s_GameServer->GetGEntity(eid);	}

private:
	static GameServer* s_GameServer;
	
};	// ServiceBase


};	// namespace

#endif	// _SERVICEBASE_H_
