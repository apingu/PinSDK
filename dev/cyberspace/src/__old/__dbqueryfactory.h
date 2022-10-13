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
#ifndef DBQUERYFACTORY_H
#define DBQUERYFACTORY_H

#include <tString.h>
#include <time.h>
#include <fString.h>
#include <vector>
#include <Debug.h>
#include <basestdlib.h>

#include "CsGameDef.h"//暫時定義，整合完拿掉


//extern int db_encode_data(const tchar_t *data,int dlen,tchar_t *encdata);
//extern int db_decode_data(const tchar_t *encdata,tchar_t *decbuff);

//--------------------------------------------------------
// Database Command Define
//--------------------------------------------------------
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"select MemberId from member where Username = '%s' and Password = '%s'",usr,pwd
//#define TRY_CONNECTION	10
namespace DBQueryFactory
{

	inline Util::String_t CreateUniqueId( u_int group )
	{
		Util::String_t out;
		out.format( "INSERT INTO uniqueid( gid ) VALUES('%d')", group );
		return out;
	}

	inline Util::String_t DeleteUniqueId( uniqueid_t uid )
	{
		Util::String_t out;
		out.format( "DELETE FROM uniqueid WHERE uid = '%lld'", uid );
		return out;
	}


	inline Util::String_t GetLastInsertId( void )
	{
		Util::String_t out;
		out.format( "SELECT LAST_INSERT_ID()" );
		return out;
	}

	inline Util::String_t GetIdGroup( uniqueid_t uid )
	{
		Util::String_t out;
		out.format( "SELECT gid FROM uniqueid WHERE uid = '%lld'", uid );
		return out;
	}

	inline Util::String_t LockTable( const tchar_t* tbl_name, int rw=1 )
	{
		Util::String_t out;
		if(rw==1)
		    out.format( "LOCK TABLES %s WRITE", tbl_name );
		else
			out.format( "LOCK TABLES %s READ", tbl_name );
		return out;
	}


	inline Util::String_t UnLockTable( void )
	{
		Util::String_t out;
		out.format( "UNLOCK TABLES" );
		return out;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	inline Util::String_t SetCharSet( const tchar_t* charset )
	{
		Util::String_t out;
		out.format( "SET NAMES '%s'", charset );
		return out;
	}




	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// history table
	
	//
	// Set the new password by member id
	//
	inline Util::String_t SetMemberPassword( connectionid_t aid, const tchar_t* password )
	{
		Util::String_t out;
		out.format( "UPDATE member SET Password = '%s' WHERE MemberId = '%lld'", password, aid );
		return out;
	}


	//
	// check the username and password
	//
	inline Util::String_t CheckLogin( const tchar_t* username, const tchar_t* password )
	{
		Util::String_t out;
		out.format( "SELECT MemberId FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		//tchar_t out[512];
		//_csprintf( out, "SELECT MemberId, Actived FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}

	inline Util::String_t GetMemberPower( connectionid_t aid )
	{
		Util::String_t out;
		out.format( "SELECT Power FROM member WHERE MemberId = '%lld'", aid );
		//tchar_t out[512];
		//_csprintf( out, "SELECT MemberId, Actived FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}

	inline Util::String_t GetMemberId( const tchar_t* name )
	{
		Util::String_t out;
		out.format( "SELECT MemberId FROM member WHERE Username = '%s'", name );
		//tchar_t out[512];
		//_csprintf( out, "SELECT MemberId, Actived FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}

	inline Util::String_t GetMemberName( connectionid_t aid )
	{
		Util::String_t out;
		out.format( "SELECT Username FROM member WHERE MemberId = '%lld'", aid );
		//tchar_t out[512];
		//_csprintf( out, "SELECT MemberId, Actived FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}
	
	inline Util::String_t SetMemberState( connectionid_t aid, int state )
	{
		Util::String_t out;
		out.format( "UPDATE member SET State = '%d' WHERE MemberId = '%lld'", state, aid );
		//tchar_t out[512];
		//_csprintf( out, "UPDATE member SET Actived='%d', LastLogin=NOW(), WHERE MemberId='%lld'", active, aid );
		//_csprintf( out, "UPDATE member SET Actived='%d' WHERE MemberId='%lld'", active, aid );
		return out;
	}

	inline Util::String_t GetMemberState( connectionid_t aid )
	{
		Util::String_t out;
		out.format( "SELECT State FROM member WHERE MemberId = '%lld'", aid );
		return out;
	}

	inline Util::String_t GetLoginState( connectionid_t aid )
	{
		Util::String_t out;
		out.format( "SELECT State, LastLogoutSec FROM member WHERE MemberId = '%lld'", aid );
		return out;
	}


	//inline Util::String_t SetMemberLogin( connectionid_t aid, const tchar_t* mac, ipa_t ip, int state )
	inline Util::String_t SetMemberLogin( connectionid_t aid, macn_t mac, int state )
	{
		Util::String_t out;
		out.format( "UPDATE member SET Macaddress = '%lld', State = '%d' WHERE MemberId = '%lld'", mac, state, aid );
		//out.format( "UPDATE member SET LastLogin=NOW() WHERE MemberId='%d'", aid );
		//tchar_t out[512];
		//_csprintf( out, "UPDATE member SET Actived='%d', LastLogin=NOW(), WHERE MemberId='%d'", active, aid );
		//_csprintf( out, "UPDATE member SET Actived='%d' WHERE MemberId='%d'", active, aid );
		return out;
	}


	inline Util::String_t SetMemberLogout( connectionid_t aid, int state, longlong currtime )
	{
		Util::String_t out;
		out.format( "UPDATE member SET State = '%d', LastLogoutSec = '%lld' WHERE MemberId = '%lld'", state, currtime, aid );
		//tchar_t out[512];
		//_csprintf( out, "UPDATE member SET Actived='%d', LastLogin=NOW(), WHERE MemberId='%d'", active, aid );
		//_csprintf( out, "UPDATE member SET Actived='%d' WHERE MemberId='%d'", active, aid );
		return out;
	}

	inline Util::String_t SetMemberLogout( connectionid_t aid, int state )
	{
		Util::String_t out;
		out.format( "UPDATE member SET State = '%d' WHERE MemberId = '%lld'", state, aid );
		//tchar_t out[512];
		//_csprintf( out, "UPDATE member SET Actived='%d', LastLogin=NOW(), WHERE MemberId='%d'", active, aid );
		//_csprintf( out, "UPDATE member SET Actived='%d' WHERE MemberId='%d'", active, aid );
		return out;
	}


	inline Util::String_t GetMemberPoint( connectionid_t aid )
	{
		Util::String_t out;
		out.format( "SELECT Point FROM member WHERE MemberId = '%lld'", aid );
		return out;
	}

	inline Util::String_t SetMemberPoint( connectionid_t aid, longlong point )
	{
		Util::String_t out;
		out.format( "UPDATE member SET Point = '%lld' WHERE MemberId = '%lld'", point, aid );
		return out;
	}

	inline Util::String_t AddMemberPoint( connectionid_t aid, longlong point )
	{
		Util::String_t out;
		out.format( "UPDATE member SET Point = Point + '%lld' WHERE MemberId = '%lld'", point, aid );
		return out;
	}


	inline Util::String_t CheckIdentity( const char* username )
	{
		Util::String_t out;
		out.format( "SELECT State, Macaddress, Type FROM member WHERE Username = '%s'", username );
		//tchar_t out[512];
		//_csprintf( out, "SELECT MemberId, Actived FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}


	inline Util::String_t CheckIdentity( connectionid_t aid )
	{
		Util::String_t out;
		out.format( "SELECT State, Macaddress, Type FROM member WHERE MemberId = '%lld'", aid );
		//tchar_t out[512];
		//_csprintf( out, "SELECT MemberId, Actived FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}

	
	inline Util::String_t GetIdentity( connectionid_t aid )
	{
		Util::String_t out;
		out.format( "SELECT Username, Power FROM member WHERE MemberId = '%lld'", aid );
		//tchar_t out[512];
		//_csprintf( out, "SELECT MemberId, Actived FROM member WHERE Username = '%s' AND Password = '%s'", username, password );
		return out;
	}



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// history table
	inline Util::String_t GetHistory( uniqueid_t hid )
	{
		Util::String_t out;
		out.format( "SELECT Duration, Frequency FROM history WHERE HistoryId = '%lld'", hid );
		return out;
	}

	inline Util::String_t AddHistory( connectionid_t aid, ipa_t ip, macn_t macn )
	{
		Util::String_t out;
		out.format( "INSERT INTO history( HistoryId, MemberId, FromIp, Mac, LastLogin, Duration, Frequency ) VALUES( '0', '%lld', '%s', '%lld', NOW(), '0', '0')", aid, ip, macn );
		return out;
	
	}

	inline Util::String_t SetLoginHistory( uniqueid_t hid, connectionid_t aid, ipa_t ip, macn_t macn, unsigned int frequency )
	{
		Util::String_t out;

		//localtime(&timep);
		
// 		struct tm *p = gmtime(&timep);
// 		tchar_t buff[50];
// 		sprintf( buff, "%d-%d-%d %d:%d:%d", p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec );
// 		out.format( "UPDATE history SET LastLogin = '%s', FromIp = '%s', Frequency = '%d' WHERE MemberId = '%lld'", buff, ip, frequency, aid );


		//out.format( "UPDATE history SET LastLogin = '%s', FromIp = '%s', Frequency = '%d' WHERE MemberId = '%lld'", asctime(localtime(&timep)), ip, frequency, aid );
		out.format( "UPDATE history SET LastLogin = NOW(), MemberId = '%lld', FromIp = '%s', Mac = '%lld', Frequency = '%d' WHERE HistoryId = '%lld'", aid, ip, macn, frequency, hid );
		return out;
	}

	
	inline Util::String_t SetLogoutHistory( uniqueid_t hid, unsigned int duration )
	{
		Util::String_t out;

		//localtime(&timep);

// 		struct tm *p = gmtime(&timep);
// 		tchar_t buff[50];
// 		sprintf( buff, "%d-%d-%d %d:%d:%d", p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec );
// 		out.format( "UPDATE history SET LastLogout ='%s', Duration = '%d' WHERE MemberId = '%lld'", buff, duration, aid );

		//out.format( "UPDATE history SET LastLogout = '%s', Duration = '%d' WHERE MemberId = '%lld'", asctime(localtime(&timep)), duration, aid );
		out.format( "UPDATE history SET LastLogout = NOW(), Duration = '%d' WHERE HistoryId = '%lld'", duration, hid );
		return out;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// entity

	// 只有基本部分
/*	inline Util::String_t InsertEntity( unsigned int Memberid, const tchar_t* name)
	{
		tchar_t commbuf[512];
		_csprintf(commbuf, "INSERT INTO player(MemberId, Name) VALUES( '%d', '%s')", Memberid, name  );
	
		return commbuf;
	}
*/	// 完整部分
	inline Util::String_t InsertEntity( connectionid_t Memberid, const tchar_t* name, const tchar_t *prop, int len)
	{
		if( len <= 0 )
			return NULL;
		if( len >= 10000 )
			return NULL;
		tchar_t buf[512];
		//const tchar_t *psource = (const tchar_t *)prop;

		_csprintf( buf, "INSERT INTO player( MemberId, Name, Property ) VALUES( '%lld', '%s', '", Memberid, name );
	    long index = strlen(buf);

        size_t buff_len = index + (len*2) + strlen("')") + 256;
		std::vector< tchar_t > query;
		query.resize( buff_len );

		tchar_t* outbuf=NULL;
		if( buff_len>0 )
		{
			outbuf=&(query[0]);
			memcpy(outbuf, buf, index*sizeof(tchar_t));
			index += _MakeResToString( outbuf+index, buff_len, prop, len );

			_csprintf( &(outbuf[index]), "')" );
		}
		//Util::String_t s;
		//s=outbuf;
		//memset( outbuf, 0, buff_len );
		//delete [] outbuf;
		//return s;
		return outbuf;
	}

	inline Util::String_t RemoveEntity( entityid_t eid )
	{
		tchar_t buf[512];
		_csprintf( buf,"DELETE FROM player WHERE PlayerId = '%lld'", eid );
		return buf;
	}

	inline Util::String_t RemoveEntity( const tchar_t* name )
	{
		tchar_t buf[512];
		_csprintf( buf,"DELETE FROM player WHERE Name = '%s'", name );
		return buf;
	}

	inline Util::String_t MarkDeleteEntity( const tchar_t* name, long time )
	{
		tchar_t buf[512];
		_csprintf( buf,"UPDATE player SET DeleteTime = '%d' WHERE Name = '%s'", time, name );
		return buf;
	}
//#include <mysql.h>


	inline Util::String_t UpdateEntity( entityid_t eid, const tchar_t *prop, int len )
	{
		if( len <= 0 )
			return NULL;
		if( len >= 10000 )
			return NULL;

		tchar_t buf[51200];

//		_csprintf( buf, "UPDATE player SET Property = '");
//		//DebugMessage( "Update 1", "Database:UpdateEntity");
//	    size_t index = strlen(buf);
//		//DebugMessage( "Update 2", "Database:UpdateEntity");
//
//	    size_t buff_len = index + (len*2) + strlen("' WHERE PlayerId = '%d'") + 256;
//		//DebugMessage( "Update 3", "Database:UpdateEntity");
//		//std::vector< tchar_t > query;
//		//query.resize( buff_len );
//		//DebugMessage( "Update 4", "Database:UpdateEntity");
//		tchar_t* outbuf = new tchar_t[buff_len];
//		memcpy(outbuf, buf, index * sizeof(tchar_t));
//		//DebugMessage( "Update 5", "Database:UpdateEntity");
//
//	    index += MakeResToString( (outbuf + index), (buff_len - index), prop, len );
//		//DebugMessage( "Update 6", "Database:UpdateEntity");
//		_csprintf(outbuf+index, "' WHERE PlayerId = '%d'", eid);
//		//DebugMessage( "Update 7", "Database:UpdateEntity");
//
//		std::vector< tchar_t > query;
//		query.resize( buff_len );
//		strcpy( (&(query[0])), outbuf );
//		SAFE_DELETE( outbuf );
//		DebugMessage( "Update 8", "Database:UpdateEntity");
//		return (&(query[0]));

		//_csprintf( buf, "UPDATE player SET Property = '%s' WHERE PlayerId = '%lld'", prop, eid);
		//return buf;


		_csprintf( buf, "UPDATE player SET Property = '");
	    size_t index = strlen(buf);

	    size_t buff_len = index + (len*2) + strlen("' WHERE PlayerId = '%lld'") + 256;
		std::vector< tchar_t > query;
		query.resize( buff_len );
		tchar_t* outbuf = NULL;
		if( buff_len>0 )
		{
			outbuf=&(query[0]);
		    memcpy(outbuf, buf, index * sizeof(tchar_t));
	        index += _MakeResToString( outbuf + index, buff_len - index, prop, len );
		    _csprintf(outbuf+index, "' WHERE PlayerId = '%lld'", eid);
		}
		return outbuf;

	}

	inline Util::String_t GetEntityRecord( const tchar_t* name )
	{
		tchar_t out[512];
		_csprintf( out, "SELECT MemberId, PlayerId, Property, DeleteTime FROM player WHERE Name = '%s'", name );
		return out;
	}


	inline Util::String_t GetEntityRecord( entityid_t eid )
	{
		Util::String_t out;
		out.format( "SELECT MemberId, Name, Property, DeleteTime FROM player WHERE PlayerId = '%lld'",eid );
		//_csprintf(out,"SELECT MemberId, Name, Property FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}


	inline Util::String_t GetEntityProperity( entityid_t eid )
	{
		Util::String_t out;
		out.format( "SELECT Property FROM player WHERE PlayerId = %lld", eid );
		//_csprintf(out,"SELECT MemberId, Name, Property FROM player WHERE PlayerId = '%d'",eid);
		return out;
	}


	inline Util::String_t GetEntityId( const tchar_t* name )
	{
		Util::String_t out;
		out.format( "SELECT PlayerId FROM player WHERE Name = '%s'", name );
		return out;
	}	


	inline Util::String_t SetEntityState( entityid_t eid, int state )
	{
		Util::String_t out;
		out.format( "UPDATE player SET State = '%d' WHERE PlayerId = '%lld'", state, eid );
		//tchar_t out[512];
		//_csprintf( out, "UPDATE member SET Actived='%d', LastLogin=NOW(), WHERE MemberId='%d'", active, aid );
		//_csprintf( out, "UPDATE member SET Actived='%d' WHERE MemberId='%d'", active, aid );
		return out;
	}

	inline Util::String_t GetEntityState( entityid_t eid )
	{
		Util::String_t out;
		out.format( "SELECT State FROM player WHERE PlayerId = '%lld'", eid );
		return out;
	}
	
	inline Util::String_t SetEntityState( const tchar_t* name, int state )
	{
		Util::String_t out;
		out.format( "UPDATE player SET State = '%d' WHERE Name = '%s'", state, name );
		//tchar_t out[512];
		//_csprintf( out, "UPDATE member SET Actived='%d', LastLogin=NOW(), WHERE MemberId='%d'", active, aid );
		//_csprintf( out, "UPDATE member SET Actived='%d' WHERE MemberId='%d'", active, aid );
		return out;
	}

	inline Util::String_t GetEntityState( const tchar_t* name )
	{
		Util::String_t out;
		out.format( "SELECT State FROM player WHERE Name = '%s'", name );
		return out;
	}

	inline tchar_t *QueryEntitiesOfMember( tchar_t *out, connectionid_t aid )
	{
		_csprintf(out, "SELECT PlayerId FROM player WHERE MemberId = '%lld'", aid);
		return out;
	}

};

#define DB_COMMAND_CHECK_LOGIN(u,p)		"SELECT MemberId FROM member WHERE Username = '"u"' AND Password = '"p"'"
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"select MemberId from member where Username = '"usr"' and Password = '"pwd"'"


#endif //DBQueryFactory
