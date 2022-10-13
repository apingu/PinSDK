
#pragma warning( disable:4786 )
#pragma warning( disable:4355 )

#include "CsDef.h"
#include "Debug.h"
#include <malloc.h>
#include "DBInterface.h"
#include "CstAccount.h"
#include "CsGameServer.h"
#include "CsCommand.h"


// 本檔案專門放 CsGameServer 中資料庫存取部分, 不進行其他服務的實作
// By Wood, 20040518
// 大幅變更成為一個介面, 20040930

// Database access services =========================================================
// for debugging
void dump_sql_result(DBResult *rst,XMySqlDB *db)
{
	int i,j;
	Util::String sval;
	if(rst){
		for(j=0;j<rst->rows();++j){
			for(i=0;i<rst->columns();++i){
				sval=db->GetQueryResult(rst,i,j);
				DBGPrint("data(%d,%d)=%s\n",j,i,sval);
			}
		}
	}
}

// Useful ==========================================================================
// 這個針對一般字串, 將 data 去除保留字後放入 outbuf
char *makeResString(char *outbuf,const char *data)
{
	int i,index=0;
	for(i=0;data[i]!='\0';++i)
	{ 
		switch(data[i])
		{
			// those must have prefixed '\'
		case '\'':
		case '\\':
		case '\"':
			outbuf[index++] = '\\';
		default:
			outbuf[index++] = data[i];
		}
	}
	outbuf[index]='\0';
	return outbuf+index;
}

// 這個針對buffer內容(無尾端0), 將 data 去除保留字後放入 outbuf
int makeResString(char *outbuf,const char *data,int dlen)
{
	int i,index=0;
	for(i=0;i<dlen;++i)
	{ 
		switch(data[i])
		{
		case 0:
			outbuf[index++] = '\\';
			outbuf[index++] = '0';
			break;
			// those must have prefixed '\'
		case '\'':
		case '\\':
		case '\"':
			outbuf[index++] = '\\';
		default:
			outbuf[index++] = data[i];
		}
	}
	return index;
}

char *makeUpdateCharDataStr(char *outbuf,int eid,const char *data,int dlen)
{
	const TCHAR *psource = (const TCHAR *)data;
	int idx,index;

	strcpy(outbuf, "UPDATE player SET Property='");
	index = strlen(outbuf);
	idx=makeResString(outbuf+index,data,dlen);
	index+=idx;
	sprintf(outbuf+index,"' WHERE PlayerId='%d'",eid);
	return outbuf;
}

void makeInsertEntityCmd(unsigned int accountid,int myeid, const char* name,const char *prop, int len,
						 char *output)
{
	char* outbuf=NULL,*wp;
	char buf[512];
	const TCHAR *psource; /* = (const TCHAR *)prop; */
	short *sa;
	int rlen,index;

	rlen=2*(len+4)+256;	// 足夠放置整個字串的空間?
//	wp = new TCHAR [rlen];
	wp=output;

	memcpy(buf+4,prop,len);
	sa=(short *)buf;
	sa[0]=1,sa[1]=len;	// 3tj

	if(myeid<0){
		sprintf( wp, "INSERT INTO player( AccountId, Name, Property ) VALUES( '%d', '%s', '", accountid,name);
	} else {
		sprintf( wp, "INSERT INTO player( PlayerId, AccountId, Name, Property ) VALUES( '%d', '%d', '%s', '", myeid, accountid,name);
	}
	outbuf=wp+strlen(wp);
	psource=buf;
	index=0;
	rlen=len+4;
	for( unsigned int i=0 ; i<rlen ; i++)
	{ 
		switch(psource[i])
		{
		case 0:
			outbuf[index++] = '\\';
			outbuf[index++] = '0';
			break;
		case '\'':
		case '\\':
		case '\"':
			outbuf[index++] = '\\';
			outbuf[index++] = psource[i];
			break;
		default:
			outbuf[index++] = psource[i];
			break;
		}
	}
	
	outbuf[index++] = '\'';
	outbuf[index++] = ')';
	outbuf[index++] = 0;
	return;
}


static const char *tm_format="%04d-%02d-%02d %02d:%02d:%02d";
// 將 time_t 轉成 MySQL DATETIME 的字串
const char *ToDBTime(time_t *tt,char *buf)
{
	struct tm *lt;
	lt=localtime(tt);
	sprintf(buf,tm_format,
		lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	return buf;
}

// 將目前時間換算成 MySQL DATETIME 格式字串
const char *StrCurrentTime(char *buf)
{
	time_t t;
	time(&t);
	return ToDBTime(&t,buf);
}

// 將 MySQL DATETIME 轉換成 tm
void GetDBTime(const char *dbtime,struct tm *lt)
{
	int t6[6];
	sscanf( dbtime,tm_format,
		    &t6[0],&t6[1],&t6[2],&t6[3],&t6[4],&t6[5]);
	lt->tm_year	=t6[0];
	lt->tm_mon	=t6[1];
	lt->tm_mday	=t6[2];
	lt->tm_hour	=t6[3];
	lt->tm_min	=t6[4];
	lt->tm_sec	=t6[5];
}

// 實作部分 ======================================================================
int DefaultDBInterface::UpdateEntityData(EntityId_t eid,const char *data,int dlen)
{
	char buf[1024];
	char *db_ptr;
	short *s;
	DBResult *result;
	Util::String ms;

	db_ptr=(char *)alloca(dlen+4);
	if(db_ptr){
		s=(short *)db_ptr;
		memcpy(db_ptr+4,data,dlen);
		s[0]=1;	// 三太子檔頭!!
		s[1]=dlen;
		makeUpdateCharDataStr(buf,eid,db_ptr,dlen+4);
		result = (DBResult*)dbptr->ExecQuery(buf);
	}
	return 1;
}

int DefaultDBInterface::GetEntityIdByName(const char *name,int account)
{
	char cmd[128];
	int eid,i,aid;

	sprintf( cmd, "SELECT AccountId, PlayerId, FROM player WHERE Name = '%s'", name );
	DBResult *result = (DBResult*)dbptr->ExecQuery(cmd);

	eid=CSERR_CHAR_NOT_EXIST;
	if(result){
		for(i=0;i<result->size();i++){
			aid=atoi(dbptr->GetQueryResult(result,0,i));
			if(aid==account){
				eid=atoi(dbptr->GetQueryResult(result,1,i));
				break;
			}
		}
	}
	return eid;
}

int	DefaultDBInterface::GetEntitiesOfAccount(AccountId_t aid,vector<EntityId_t> &eids)
{
	Util::String sval;
	char cmd[128];
	int i;
	sprintf(cmd, "SELECT PlayerId FROM player WHERE AccountId = '%d'", aid);
	DBResult *result = (DBResult*)dbptr->ExecQuery(cmd);
	if(!result)
		return 0;
	for(i=0;i<result->size();++i){
		sval=dbptr->GetQueryResult(result,0,i);
		eids.push_back(sval.toint());
	}
	return i;
}

bool DefaultDBInterface::IsEntityNameExist(const char *name,AccountId_t *aid,EntityId_t *eid)
{
	char cmd[512];
	DBResult *result;

	sprintf( cmd, "SELECT AccountId, PlayerId FROM player WHERE Name = '%s'",name);
	result = (DBResult*)dbptr->ExecQuery(cmd);
	if(result!=NULL){
		// 判斷部分
		if(dbptr->GetQueryResult(result,0,0).empty()){
			return false;
		} else {
			if(aid)
				*aid=dbptr->GetQueryResult(result,0,0).toint();
			if(eid)
				*eid=dbptr->GetQueryResult(result,1,0).toint();
			return true;
		}
	}
	return false;
}

bool DefaultDBInterface::DeletePlayerData(EntityId_t eid)
{
	char cmd[80];
	DBResult *result;
	sprintf(cmd,"DELETE FROM player WHERE PlayerId = '%d'",eid);
	result=(DBResult*)dbptr->ExecQuery(cmd);	// 真的送出
	return result!=NULL;
}

bool DefaultDBInterface::SetAccountLoginFlag(AccountId_t aid,bool active)
{
	char cmd[128];
	DBResult *result;
	sprintf( cmd, "UPDATE account SET Actived='%d' WHERE AccountId='%d'", active, aid);
	result = (DBResult*)dbptr->ExecQuery(cmd);
	return result!=NULL;
}

bool DefaultDBInterface::GetNPCList(const char *tblname,vector<Cyberspace::NPC_Account_List_t> *output)
{
	DBResult *result;
	char cmd[512];
	int i;
	Cyberspace::NPC_Account_List_t data;

	sprintf( cmd, "SELECT name FROM %s",tblname);
	result = (DBResult*)dbptr->ExecQuery(cmd);
	if(result){
		for(i=0;i<result->size();i++){
			data.name	=dbptr->GetQueryResult(result,0,i);
			output->push_back(data);
		}
	}
	// 不管有或沒有都 OK
	return true;
}

bool DefaultDBInterface::GetGMList(const char *tblname,vector<Cyberspace::GM_Account_List_t> *output)
{
	DBResult *result;
	char cmd[512];
	int i;
	Cyberspace::GM_Account_List_t data;

	sprintf( cmd, "SELECT name, level FROM %s",tblname);
	result = (DBResult*)dbptr->ExecQuery(cmd);
	if(result){
		for(i=0;i<result->size();i++){
			data.name	=dbptr->GetQueryResult(result,0,i);
			data.level	=atoi(dbptr->GetQueryResult(result,1,i));
			output->push_back(data);
		}
	}
	// 不管有或沒有都 OK
	return true;
}

int DefaultDBInterface::CheckLogin(const char *account,const char *password,BOOL *loggedin,BOOL *banned)
{
	DBResult *result;
	char cmd[512],*cp;
	int i;
	Cyberspace::GM_Account_List_t data;

	// 字串看來也要去除保留字啊...
	strcpy(cmd,"SELECT AccountId, Actived, Banned FROM account WHERE Username = \'");
	cp=cmd+strlen(cmd);
	cp=makeResString(cp,account);
	strcat(cp,"\' AND Password = \'");	cp+=strlen(cp);
	cp=makeResString(cp,password);
	strcat(cp,"\'");

//	sprintf( cmd, "SELECT AccountId, Actived, Banned FROM account WHERE Username = '%s' AND Password = '%s'",
//		account, password);
	result = (DBResult*)dbptr->ExecQuery(cmd);
	if(result && !result->empty()){
		i			=atoi(dbptr->GetQueryResult(result,0,0));
		*loggedin	=(0!=atoi(dbptr->GetQueryResult(result,1,0)));
		*banned		=(0!=atoi(dbptr->GetQueryResult(result,2,0)));
	} else {
		i=-1,*loggedin=*banned=TRUE;
	}
	return i;
}

bool DefaultDBInterface::CheckSilent(const char *account)
{
	DBResult *result;
	char cmd[512];
	int i;
	Cyberspace::GM_Account_List_t data;

	sprintf( cmd, "SELECT Silent FROM account WHERE Username = '%s'", account);
	result = (DBResult*)dbptr->ExecQuery(cmd);
	if(result && !result->empty()){
		i=atoi(dbptr->GetQueryResult(result,0,0));
	} else {
		i=0;	// default: not silent. But should not happen!!
	}
	return i!=0;
}

void DefaultDBInterface::WriteLastLogin(int aid)
{
	DBResult *result;
	char cmd[512],tbuf[32];
	sprintf(cmd,"UPDATE account SET LastLogin='%s' WHERE AccountId='%d'",
		StrCurrentTime(tbuf),aid);
	result = (DBResult*)dbptr->ExecQuery(cmd);
}

void DefaultDBInterface::WriteLastLogout(int aid)
{
	DBResult *result;
	char cmd[512],tbuf[32];
	sprintf(cmd,"UPDATE account SET LastLogout='%s' WHERE AccountId='%d'",
		StrCurrentTime(tbuf),aid);
	result = (DBResult*)dbptr->ExecQuery(cmd);
}

void DefaultDBInterface::WriteLoginIP(int aid,const char *ipstr)
{
	DBResult *result;
	char cmd[512];
	sprintf(cmd,"UPDATE account SET FromIP='%s' WHERE AccountId='%d'",
		ipstr,aid);
	result = (DBResult*)dbptr->ExecQuery(cmd);
}

bool DefaultDBInterface::Reconnect()
{
	//dbptr->reconnect();
	return true;	// 將來要配合改, 如果使用這部分的話!
}

int DefaultDBInterface::ReadEntityData(EntityId_t eid,AccountId_t *aid,char *name,char *outbuf,int bufmax)
{
	char cmd[80];
	int len,i,j,l;
	Util::String ms;

	DBResult *result;
	sprintf( cmd, "SELECT AccountId, Name, Property FROM player WHERE PlayerId = '%d'",eid );
	result = (DBResult*)dbptr->ExecQuery(cmd);
	if(!result || result->size()<=0){
		return -1;
	} else {
		if(aid)
			*aid = atoi(dbptr->GetQueryResult( result, 0, 0 ));
		if(name)
			strcpy(name,dbptr->GetQueryResult(result,1,0));
		if(outbuf)
		{
			DBResBuf rbuf=dbptr->GetResource("player","Property","PlayerId",eid, len );
			l=rbuf.size();
			if(l>0){
				len=l-4;
				assert(len<=bufmax);
				for(j=0,i=4;j<len;++i,++j)
					outbuf[j]=rbuf[i];
				//memcpy( outbuf, &rbuf[4], len );
			}
		} else
			len=0;

		return len;
	}
}

int DefaultDBInterface::CreatePlayer(int aid,int eid,const char *name,const char *data,int dlen)
{
		DBResult *result;
		char cmd[1024];
		int oeid;

		// 造紀錄
		makeInsertEntityCmd(aid,eid,name,data,dlen,cmd);
		result=(DBResult*)dbptr->ExecQuery(cmd);
		if(!result)
		{
			return CSERR_DATA_CREATE_FAIL;
		}
		// 查驗是否 OK
//		sprintf( cmd, "SELECT AccountId, PlayerId, Property FROM player WHERE Name = '%s'", name );
		sprintf( cmd, "SELECT PlayerId FROM player WHERE Name = '%s'", name );
		result=(DBResult*)dbptr->ExecQuery(cmd);	// 真的送出
		if(!result)
		{
			return CSERR_DATA_CREATE_FAIL;
		}
		oeid=atoi(dbptr->GetQueryResult(result,0,0));

		return oeid;
}


