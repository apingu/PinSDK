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
#ifndef MESSAGERDBQUERYFACTORY_H
#define MESSAGERDBQUERYFACTORY_H

#include <tString.h>
#include <time.h>
#include <fString.h>
#include <vector>

#include "CsDef.h"
#include "PMSGDef.h"


//extern int db_encode_data(const char *data,int dlen,char *encdata);
//extern int db_decode_data(const char *encdata,char *decbuff);

//--------------------------------------------------------
// Database Command Define
//--------------------------------------------------------
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"select AccountId from account where Username = '%s' and Password = '%s'",usr,pwd
//#define TRY_CONNECTION	10
namespace MessagerDBQueryFactory
{

	inline Util::String SetNickname( userid_t uid, const char* nickname )
	{
		Util::String out;
		out.format( "UPDATE user SET Nickname = '%s' WHERE ID = '%d'", nickname, uid );
		return out;
	}


	inline Util::String GetNickname( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT Nickname FROM user WHERE ID = '%d'", uid );
		return out;
	}


	inline Util::String SetState( userid_t uid, short state )
	{
		Util::String out;
		out.format( "UPDATE user SET State = '%d' WHERE ID = '%d'", state, uid );
		return out;
	}


	inline Util::String GetState( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT State FROM user WHERE ID = '%d'", uid );
		return out;
	}

	inline Util::String SetFriendlist( int eid, void* friendlist )
	{
		char buf[512];

		sprintf( buf, "UPDATE user SET Friends='");
	    size_t index = strlen(buf);

	    size_t buff_len = index + _MAX_FRIENDLIST * 2 + 2 + strlen("' WHERE ID='%d'") + 20;
		std::vector< char > query;
		query.resize( _MAX_FRIENDLIST );
		char* outbuf = &(query[0]);
		memcpy(outbuf, buf, index * sizeof(char));

		index += MakeResToString( outbuf + index, buff_len - index, (const char*)friendlist, _MAX_FRIENDLIST );
		sprintf(outbuf + index, "' WHERE ID='%d'", eid );
		return outbuf;
	}

	inline Util::String GetFriendlist( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT Friends FROM user WHERE ID = '%d'", uid );
		return out;
	}
	
	
	inline Util::String SetPicture( int eid, void* pic, size_t len )
	{
		char buf[512];

		sprintf( buf, "UPDATE user SET Picture='");
	    size_t index = strlen(buf);

	    size_t buff_len = index + len * 2 + 2 + strlen("' WHERE ID='%d'") + 20;
		std::vector< char > query;
		query.resize( len );
		char* outbuf = &(query[0]);
		memcpy(outbuf, buf, index * sizeof(char));

		index += MakeResToString( outbuf + index, buff_len - index, (const char*)pic, len );
		sprintf(outbuf + index, "' WHERE ID='%d'", eid );
		return outbuf;
	}

	inline Util::String GetPicture( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT Picture FROM user WHERE ID = '%d'", uid );
		return out;
	}

	inline Util::String CreateConference( userid_t uid )
	{
		Util::String out;
		out.format( "INSERT INTO conference( UserId ) VALUES( '%d' )", uid );
		return out;
	}


	inline Util::String GetConferenceId( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT ConferenceId FROM conference WHERE UserId = '%d'", uid );
		return out;
	}


	inline Util::String RemoveConference( ConferenceId_t cid )
	{
		Util::String out;
		out.format( "DELETE FROM player conference ConferenceId = '%d'", cid );
		return out;
	}
};

#endif