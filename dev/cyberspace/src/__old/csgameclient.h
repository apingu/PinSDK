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
#ifndef CSGAMECLIENT_H
#define CSGAMECLIENT_H

#include <vector>
//#include <EntityRoom.h>
#include "CsDef.h"
#include "Cyberspace_client.h"
#include <T_Lock.h>


#define MAX_PING_COUNT	5
#define PING_TIMEOUT	2000

namespace Cyberspace
{

	class GameClient : public Client// public GSO::EntityRoom
	{
	public:
		GameClient();
		~GameClient();

		bool       Connect( char* host, unsigned short port );
		// step 1: login and wait for OnLogin get TRUE...
		bool       Login( char* username, char* password );
		/// 這個和 Login() 功能一樣, 但是不會等傳回, 請用 OnLogin() 攔截.
 		void       DoLogin(const char *username,const char *passwd);
		// step 2: get all entity
		bool       GetAllEntity( void );
		// step 3: you can create entity,delete one,or select one.
		//			once selected, and OnGetSelectEntityId() called, game started.
		void       CreateNewEntity( const char* name, void* prop, size_t len );
/*
#ifdef _NEW_VERSION_0005
		//傳上去要砍的角色名
		void       DeleteEntity( const char* name);
		//通知SERVER 要選擇一個角色使用
		void       SelectEntity( const char* name );
#else
*/
		void       DeleteEntity(EntityId_t eid);
		void       SelectEntity(EntityId_t eid);
		//(目前當作同 EntityJoin, 不過是用登入帳號和名稱)
		void       SelectEntity( const char* name );
		// step 4: 場景之進出
		void       EntityJoin( const char* name );	// 加入遊戲(NPC server用)
		void       EntityJoin( EntityId_t eid );	// 加入遊戲
		void	   EntityExit( EntityId_t eid );	// 要離開遊戲了(從 server 離開)
		void       EntityChangeRegion(EntityId_t eid,RegionId_t rid);	// 換區
//#endif
		// step 5: logout.
		bool       Logout( void );

		///////////////////////////////////////////////////////////////////////
		//Callback function
		/// 當 login 成功時會呼叫(在接收封包thread)(after step 1)
		/// @param isOK: 是否成功登入?
		virtual void OnLogin(AccountId_t aid){};
/*
#ifdef _NEW_VERSION_0005
		/// 當詢問所有 entity 獲得回答時(每隻一次,最後一隻以後還會傳name=NULL表示沒了)
		/// 或是要求造新的 entity 的回答(name=NULL 表示失敗,只會有一次)
		/// 請小心順序, 以上兩個運算不要混合了. (after step 1 or 3)
		//virtual void OnGetRecordEntity( const char* name, void* prop, size_t len ){};
		virtual void OnGetRecordEntity( const char* name, void* prop, size_t len ){};
		/// 選擇一個人的回答 -> 收到後可以開始遊戲 (after step 3)
		//將選擇的人的 eid 傳下來
		virtual void OnGetSelectEntityId( EntityId_t eid ){};
		/// Server 主動通知角色移動
		/// 另外使用 EnterEntity 指令時, 如果傳回 entityId<0 則表示錯誤
		virtual void OnEntityJoinGame( int entityId, void* prop, size_t len ){};
		virtual void OnEntityExitGame( int entityId ){};
#else
*/
		//取得可選的人的姓名會比較好
		virtual void OnGetRecordEntity( EntityId_t eid, void* prop, size_t len ){};
		/// 選擇一個人的回答 -> 收到後可以開始遊戲 (after step 3)
		virtual void OnGetSelectEntityId(EntityId_t eid,RegionId_t rid){};
		/// Server 主動通知角色移動 Entity(eid) 進入 Region(rid)
		virtual void OnEnterRegion( int eid, int rid ){	};	// 場景要切到哪個 region?
		/// 我或他人"加入世界", 但是 client 這邊是加入可見範圍
		virtual void OnEntityJoinGame( int eid, void* prop, size_t len )=0;
		/// 我或他人"離開世界", 但是 client 這邊是離開可見範圍
		virtual void OnEntityExitGame( int entityId )=0;

//#endif

		/// 刪除一個人的回答(name=NULL表示刪除失敗) (after step 3)
		virtual void OnEntityDeleted(int retcode){};
		/// 這個不知道要不要(logout 成功後)(after step 5)
		//virtual void OnLogout(){};

		void StartPing();
		unsigned int GetPingTime();
		void DoPing();	// 這個是為了能正確處理 ping time-out 狀況要呼叫的.目前無處可掛
		inline void		SetMyEID(int eid){	m_EID=eid;	}
		inline int SendMessage(int eid,ChannelId_t cid, char* bp, unsigned int len)
		{	return Client::SendMessage(eid,cid,bp,len);	}
		inline int SendMessage(ChannelId_t cid, char* bp, unsigned int len)
		{	return Client::SendMessage(m_EID,cid,bp,len);	}
	protected:

		char         m_Host[255];
		u_short      m_Port;
		char         m_Connected;
		bool		 m_recv;	// some message got
		int			 m_EID;		// 用這個來填入 EID 欄位...

		virtual void OnRecvRequest( const char* command );
		//void         OnRecvRequest( const char* bp, size_t len );

		// ping count buffer
		unsigned int *_pingbuf;	// ping buffer, 進行中才有.
		int _premain;
		bool ping_lock;
		unsigned int ping_avg;	// 測量的結果(平均值)(不記入 time-out)
		unsigned int ping_max;	// 最大值(可能表示有斷線)
		void doPing(int getseq);	// ping 數值測試用處理
		void stopPing();

	private:
		T_Lock m_CSLock;
	};

};//Cyberspace

#endif//CSSERVERMAP_H
