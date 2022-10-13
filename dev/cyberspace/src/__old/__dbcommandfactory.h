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
#ifndef DBCOMMANDFACTORY_H
#define DBCOMMANDFACTORY_H

#include <tString.h>
#include <time.h>
#include <fString.h>
#include <vector>

#include "CsDef.h"//暫時定義，整合完拿掉


//extern int db_encode_data(const char *data,int dlen,char *encdata);
//extern int db_decode_data(const char *encdata,char *decbuff);

//--------------------------------------------------------
// Database Command Define
//--------------------------------------------------------
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"select AccountId from account where Username = '%s' and Password = '%s'",usr,pwd
//#define TRY_CONNECTION	10
namespace DBCommandFactory
{
/*
#ifdef _NEW_VERSION_0005
	
	inline mem::String CheckLogin( const char* username, const char* password )
	{
		mem::String out;
		out.format( "SELECT AccountId, Actived FROM account WHERE Username = '%s' AND Password = '%s'", username, password );
		//char out[512];
		//sprintf( out, "SELECT AccountId, Actived FROM account WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}
	
	inline mem::String SetAccountActive( AccountId_t aid, bool active )
	{
		mem::String out;
		out.format( "UPDATE account SET Actived='%d' WHERE AccountId='%d'", active, aid );
		//char out[512];
		//sprintf( out, "UPDATE account SET Actived='%d', LastLogin=NOW(), WHERE AccountId='%d'", active, aid );
		//sprintf( out, "UPDATE account SET Actived='%d' WHERE AccountId='%d'", active, aid );
		return out;
	}
	// 只有基本部分
	// 完整部分
	inline mem::String InsertEntity( unsigned int accountid, const char* name, const char *prop, int len)
	{
		char buf[512];
		const TCHAR *psource = (const TCHAR *)prop;

		sprintf( buf, "INSERT INTO player( AccountId, Name, Property ) VALUES( '%d', '%s', '", accountid,name);
	    long index = strlen(buf);
	    size_t buff_len = index + len*2 + 2;
		std::vector< char > query;
		query.resize( buff_len );
	    //outbuf = new TCHAR [buff_len];
		char* outbuf = &(query[0]);
		memcpy(outbuf, buf, index*sizeof(TCHAR));

		index += MakeResToString( outbuf+index, buff_len, prop, len );

	    outbuf[index++] = '\'';
	    outbuf[index++] = ')';
	    outbuf[index++] = 0;
		return outbuf;
	}

	inline mem::String RemoveEntity( const char* name )
	{
		char buf[512];
		sprintf( buf,"DELETE FROM player WHERE Name = '%s'", name );
		return buf;
	}


	inline mem::String UpdateEntity( int eid, const char *prop, int len )
	{
		char buf[512];
		const TCHAR *psource = (const TCHAR *)prop;

		sprintf( buf, "UPDATE player SET Property='");
	    long index = strlen(buf);
	    size_t buff_len = index + len*2 + 2;
		std::vector< char > query;
		query.resize( buff_len );
	    //outbuf = new TCHAR [buff_len];
		char* outbuf = &(query[0]);
		memcpy(outbuf, buf, index*sizeof(TCHAR));

		index += MakeResToString( outbuf+index, buff_len, prop, len );
		sprintf(outbuf+index,"' WHERE PlayerId='%d'",eid);
		return outbuf;

	}

	inline mem::String GetEntityRecord( const char* name )
	{
		char out[512];
		sprintf( out, "SELECT AccountId, PlayerId, Property FROM player WHERE Name = '%s'", name );
		return out;
	}


	inline mem::String GetEntityRecord( int eid )
	{
		mem::String out;
		out.format( "SELECT AccountId, Name, Property FROM player WHERE PlayerId = '%d'",eid );
		//sprintf(out,"SELECT AccountId, Name, Property FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}

	inline char *QueryEntitiesOfAccount(char *out,int aid)
	{
		sprintf(out, "SELECT PlayerId FROM player WHERE AccountId = '%d'", aid);
		return out;
	}

#else
  */
	inline Util::String CheckLogin( const char* username, const char* password )
	{
		char out[512];
		sprintf( out, "SELECT AccountId, Actived FROM account WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}
	
	inline Util::String SetAccountActive( AccountId_t aid, bool active )
	{
		char out[512];
		//sprintf( out, "UPDATE account SET Actived='%d', LastLogin=NOW(), WHERE AccountId='%d'", active, aid );
		sprintf( out, "UPDATE account SET Actived='%d' WHERE AccountId='%d'", active, aid );
		return out;
	}
	// 完整部分
	inline Util::String InsertEntity( unsigned int accountid,int myeid, const char* name,const char *prop, int len)
	{
		char* outbuf=NULL,*wp;
		char buf[512];
		const TCHAR *psource; /* = (const TCHAR *)prop; */
		short *sa;
		int rlen,index;

		rlen=2*(len+4)+256;	// 足夠放置整個字串的空間?
		wp = new TCHAR [rlen];
		assert(wp!=NULL);
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
//	    long index = strlen(buf);
//	    size_t buff_len = index + len*2 + 2;
//	    outbuf = new TCHAR [buff_len];
//		memcpy(outbuf, buf, index*sizeof(TCHAR));

		//MakeResToString( prop, len, outbuf, buff_len );
				
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
		return wp;
	}


	inline Util::String GetEntityRecord( const char* name )
	{
		char out[512];
		sprintf( out, "SELECT AccountId, PlayerId, Property FROM player WHERE Name = '%s'", name );
		return out;
	}

	inline Util::String GetEntityRecordsByAccoundID( int aid )
	{
		char out[512];
		sprintf( out, "SELECT Name, PlayerId, Property FROM player WHERE AccountId = '%d'", aid );
		return out;
	}

	inline char* GetEntityRecord(char *out,const char* name)
	{
		sprintf( out, "SELECT AccountId, PlayerId, Property FROM player WHERE Name = '%s'", name );
		return out;
	}

/*	inline char *GetEntityRecord(char *out,int eid)
	{
		sprintf(out,"SELECT AccountId, Name, Property FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}
*/	inline char *CheckEntityExist(char *out,int eid)	// only check for existance? and load prop...
	{
		sprintf(out,"SELECT Property FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}

	inline char *QueryEntitiesOfAccount(char *out,int aid)
	{
		sprintf(out, "SELECT PlayerId FROM player WHERE AccountId = '%d'", aid);
		return out;
	}
	inline char *QueryDataOfEntity(char *out,int eid)
	{
		sprintf(out,"SELECT Name, Pos, Color FROM player WHERE PlayerId = '%d'", eid);
		return out;
	}
	inline char *DeleteEntity(char *out,int eid)
	{
		sprintf(out,"DELETE FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}
	inline char *GetAccountOfEntity(char *out,int eid)
	{
		// very strange. Now change it...
		sprintf( out, "SELECT AccountId FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}
	inline char *GetEntityName(char *out,int eid)
	{
		sprintf( out, "SELECT Name FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// old version
	/*

	inline mem::String GetEntityRecordsByAccoundID( int aid )
	{
		char out[512];
		sprintf( out, "SELECT Name, PlayerId, Property FROM player WHERE AccountId = '%d'", aid );
		return out;
	}




	/*
	inline char* GetEntityRecord(char *out,const char* name)
	{
		sprintf( out, "SELECT AccountId, PlayerId, Property FROM player WHERE Name = '%s'", name );
		return out;
	}

	*/


	/*
	inline mem::String GetEntityOwner( const char* name )
	{
		char out[512];
		sprintf( out, "SELECT AccountId FROM player WHERE Name = '%s'", name );
		return out;
	}


	inline mem::String GetEntityId( const char* name )
	{
		char out[512];
		sprintf( out, "SELECT PlayerId FROM player WHERE Name = '%s'", name );
		return out;
	}

	inline mem::String GetEntityProp( const char* name )
	{
		char out[512];
		sprintf( out, "SELECT Properity FROM player WHERE Name = '%s'", name );
		return out;
	}
	*/
	/*
	inline char *QueryDataOfEntity(char *out,int eid)
	{
		sprintf(out,"SELECT Name, Pos, Color FROM player WHERE PlayerId = '%d'", eid);
		return out;
	}
/*	inline char *UpdatePlayerData(char *out,int eid,const char *prop,size_t len)
	{
		sprintf(out,"UPDATE player SET Pos = 5 WHERE PlayerId = '%d'", eid);
		return out;
	}
	// 刪除角色資料
	inline char *DeleteEntity(char *out,const char *name)
	{
		sprintf(out,"DELETE FROM player WHERE Name = '%s'",name);
		return out;
	} */
	/*
	inline char *DeleteEntity(char *out,int eid)
	{
		sprintf(out,"DELETE FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}
//	inline char *GetEntityOfAccount(char *out,int aid,int eid)
	inline char *GetAccountOfEntity(char *out,int eid)
	{
		// very strange. Now change it...
//		sprintf( out, "SELECT Name FROM player WHERE AccountId = '%d' AND PlayerId = '%d' ",aid,eid);
		sprintf( out, "SELECT AccountId FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}
	inline char *GetEntityName(char *out,int eid)
	{
		sprintf( out, "SELECT Name FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}
/*	int GetEntityData(int eid,char *buf)
	{
		MYSQLDB->GetResource
	} */
//#endif
};

#define DB_COMMAND_CHECK_LOGIN(u,p)		"SELECT AccountId FROM account WHERE Username = '"u"' AND Password = '"p"'"
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"select AccountId from account where Username = '"usr"' and Password = '"pwd"'"


#endif //DBCOMMANDFACTORY_H