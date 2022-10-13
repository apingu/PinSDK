#ifndef _DBINTERFCE_H_
#define _DBINTERFCE_H_

// 這部分的實作只牽涉如何取得介面指定的資料, 不必分 SQL 指令之類的.
// 請延伸此類別來用
// initialization 部分請實際程式自行處理
// By Wood Woo, Sep 30, 2004
// GameServer 改成祇對此層存取而已
#include "CstAccount.h"

// 以後角色一律定為 Entity
class DBInterface
{
	// 可變部分
public:
	virtual bool Reconnect()=0;	// 不論第一次或是以後重連都用這個. 假定可以重連接
	// 對 Game server 服務部分
	virtual bool GetNPCList(const char *tblname,vector<Cyberspace::NPC_Account_List_t> *output)=0;
	virtual bool GetGMList(const char *tblname,vector<Cyberspace::GM_Account_List_t> *output)=0;
	virtual int  CheckLogin(const char *account,const char *password,BOOL *loggedin,BOOL *banned)=0;
	virtual bool CheckSilent(const char *account)=0;
	virtual void WriteLastLogin(int aid)=0;
	virtual void WriteLastLogout(int aid)=0;
	virtual void WriteLoginIP(int aid,const char *ipstr)=0;
	// 其實也是臨時性的, 將來可能需要再細分
	virtual bool IsEntityNameExist(const char *name,AccountId_t *aid,EntityId_t *eid)=0;
	virtual bool DeletePlayerData(EntityId_t eid)=0;
	virtual int  CreatePlayer(int aid,int eid,const char *name,const char *data,int dlen)=0;
	virtual int  UpdateEntityData(EntityId_t eid,const char *data,int dlen)=0;
	virtual bool SetAccountLoginFlag(AccountId_t acc,bool in)=0;
	virtual int  ReadEntityData(EntityId_t eid,AccountId_t *aid,char *name,char *outbuf,int bufmax)=0;
	virtual int	 GetEntitiesOfAccount(AccountId_t aid,vector<EntityId_t> &eids)=0;
	virtual int  GetEntityIdByName(const char *name,int account)=0;
//	virtual int  CheckEntityExist(EntityId_t eid,char *outbuf,int bufmax)=0;
//	virtual bool GetPlayerName(EntityId_t eid,char *buf);
//	virtual bool IsEntityOfAccount(AccountId_t aid,EntityId_t eid)=0;
	// 繼續往下延伸...
};

#include "XMySqlDB.h"
// default one
class DefaultDBInterface : public DBInterface
{
protected:
	XMySqlDB *dbptr;
public:
	void SetDB(XMySqlDB *db){	dbptr=db;	}
	bool Reconnect();
	// 實作部分
	bool GetNPCList(const char *tblname,vector<Cyberspace::NPC_Account_List_t> *output);
	bool GetGMList(const char *tblname,vector<Cyberspace::GM_Account_List_t> *output);
	int  CheckLogin(const char *account,const char *password,BOOL *loggedin,BOOL *banned);
	bool CheckSilent(const char *account);
	void WriteLastLogin(int aid);
	void WriteLastLogout(int aid);
	void WriteLoginIP(int aid,const char *ipstr);

	bool IsEntityNameExist(const char *name,AccountId_t *aid,EntityId_t *eid);
	bool DeletePlayerData(EntityId_t eid);
	int  CreatePlayer(int aid,int eid,const char *name,const char *data,int dlen);
	bool SetAccountLoginFlag(AccountId_t acc,bool in);
	int  UpdateEntityData(EntityId_t eid,const char *data,int dlen);
	int	 GetEntitiesOfAccount(AccountId_t aid,vector<EntityId_t> &eids);
	int  GetEntityIdByName(const char *name,int account);
	int  ReadEntityData(EntityId_t eid,AccountId_t *aid,char *name,char *outbuf,int bufmax);
};

#endif	// _DBINTERFCE_H_
