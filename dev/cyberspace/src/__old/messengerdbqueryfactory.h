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
namespace MessengerDBQueryFactory
{

	inline Util::String SetNickname( userid_t uid, const char* nickname )
	{
		Util::String out;
		out.format( "UPDATE participator SET Nickname = '%s' WHERE UserId = '%d'", nickname, uid );
		return out;
	}


	inline Util::String GetNickname( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT Nickname FROM participator WHERE UserId = '%d'", uid );
		return out;
	}


	inline Util::String SetState( userid_t uid, short state )
	{
		Util::String out;
		out.format( "UPDATE participator SET State = '%d' WHERE UserId = '%d'", state, uid );
		return out;
	}


	inline Util::String GetState( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT State FROM participator WHERE UserId = '%d'", uid );
		return out;
	}

	inline Util::String SetFriendlist( int eid, const std::vector<userid_t>* friendlist )
	{
		char query[2048];

	    UINT buff[200];
		memset( buff, 0, 800 );
		UINT fbuff[_MAX_FRIENDLIST];
		memset( fbuff, 0, _MAX_FRIENDLIST*sizeof(UINT) );
		UINT ss=MIN( _MAX_FRIENDLIST, friendlist->size() ) ;

		UINT c=0;
		for( UINT i=0; i<ss; i++ )
		{
			if( (*friendlist)[i]!=0 )
			{
			    fbuff[c]=(*friendlist)[i];
				c++;
			}
		}

	    size_t buflen = MakeResToString( (char*)buff, 800, (char*)fbuff,  _MAX_FRIENDLIST*sizeof(UINT) );
	    sprintf( query, "UPDATE participator SET Friends = '" );
	    char* ptr=query;
	    ptr+=strlen( query );
	    memcpy( ptr, buff, buflen );
	    ptr+=buflen;
	    sprintf( ptr, "' WHERE UserId = '%d'", eid );

		return query;
	}

	inline Util::String GetFriendlist( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT Friends FROM participator WHERE UserId = '%d'", uid );
		return out;
	}
	
	
	inline Util::String SetPicture( int eid, void* pic, size_t len )
	{
		char buf[512];

		sprintf( buf, "UPDATE participator SET Picture = '");
	    size_t index = strlen(buf);

	    size_t buff_len = index + (len * 2) + strlen("' WHERE UserId = '%d'") + 512;
		std::vector< char > query;
		query.resize( buff_len );
		char* outbuf = &(query[0]);
		memcpy(outbuf, buf, index * sizeof(char));
		//index += mysql_escape_string( outbuf + index, (const char*)pic, len );

		index += MakeResToString( outbuf + index, buff_len - index, (const char*)pic, len );
		sprintf(outbuf + index, "' WHERE UserId = '%d'", eid );
		return outbuf;
	}

	inline Util::String GetPicture( userid_t uid )
	{
		Util::String out;
		out.format( "SELECT Picture FROM participator WHERE UserId = '%d'", uid );
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
		out.format( "DELETE FROM conference WHERE ConferenceId = '%d'", cid );
		return out;
	}
};

#endif