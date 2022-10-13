//
//  會呼叫到的資料庫存取函式
//  若要改資料庫 使用這個
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
#ifndef MESSAGERDBFUNCTION_H
#define MESSAGERDBFUNCTION_H


#include <assert.h>
#include <string>
#include "CsDef.h"
#include "CsErr.h"
#include <XMySqlDB.h>
#include "MessagerDBQueryFactory.h"
#include <T_Lock.h>


namespace Picno
{

    #define ACCSTATE_OFFLINE     0
	#define ACCSTATE_ONLINE      1
    #define ACCSTATE_FREEZE      2
		
	namespace Messager
	{
		#define GET_DB_RESULT( r, col, row )    ( ((XMySqlDB*)mydb)->GetQueryResult( r, col, row ) )
		T_Lock g_Querylock;

		inline void SetFriendlist( XDataBase* mydb, userid_t uid, std::vector<userid_t> friendlist )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::SetFriendlist( uid, &(friendlist[0]) ) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return;
			}
			((XMySqlDB*)mydb)->FreeResult( result );
			return;
		}

		inline void GetFriendlist( XDataBase* mydb, userid_t uid, std::vector<userid_t>& friendlist )
		{
			TS_Lock lock( &g_Querylock );
			friendlist.empty();
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::GetFriendlist( uid ) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return;
			}

			DBResBuf pBuffer=((XMySqlDB*)mydb)->GetRawField( result, 0, 0 );
			((XMySqlDB*)mydb)->FreeResult( result );
			int* buf=(int*)(&(pBuffer[0]));
			for( UINT i=0; i<_MAX_FRIENDLIST; i++ )
			{
				if( buf[i]!=CsINVALID_ID )
					friendlist.push_back( buf[i] );
			}
			return;
		}

		inline void SetNickname( XDataBase* mydb, userid_t uid, const char* nickname )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::SetNickname( uid, nickname ) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return;
			}
			((XMySqlDB*)mydb)->FreeResult( result );
			return;
		}

		inline std::string GetNickname( XDataBase* mydb, userid_t uid )
		{
			TS_Lock lock( &g_Querylock );
			std::string nickname;
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::GetNickname( uid ) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return nickname;
			}
			nickname = GET_DB_RESULT( result, 0, 0);
			
			((XMySqlDB*)mydb)->FreeResult( result );
			return nickname;
		}
		
		inline void SetPicture( XDataBase* mydb, userid_t uid, void* pic, size_t len )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::SetPicture( uid, pic, len ) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return;
			}
			((XMySqlDB*)mydb)->FreeResult( result );
			return;
		}

		inline DBResBuf GetPicture( XDataBase* mydb, userid_t uid )
		{
			TS_Lock lock( &g_Querylock );

			DBResBuf pBuffer;
			pBuffer.clear();
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::GetPicture(uid) );
			if(!result || ((XMySqlDB*)mydb)->GetXDBResultSize( result )<=0)
			{
				return pBuffer;
			}

			// TODO: Pin
			// 這裡要回來改
			pBuffer = ((XMySqlDB*)mydb)->GetRawField( result, 0, 0 );
			((XMySqlDB*)mydb)->FreeResult( result );
			return pBuffer;
		}

		inline void SetState( XDataBase* mydb, userid_t uid, short state )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::SetState( uid, state ) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return;
			}
			((XMySqlDB*)mydb)->FreeResult( result );
			return;
		}

		inline short GetState( XDataBase* mydb, userid_t uid )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::GetState( uid ) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return 0;
			}
			short state=atoi( GET_DB_RESULT( result, 0, 0).c_str() );
			((XMySqlDB*)mydb)->FreeResult( result );
			return state;
		}



		inline ConferenceId_t CreateConference( XDataBase* mydb, userid_t uid )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::CreateConference(uid) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return 0;
			}
			((XMySqlDB*)mydb)->FreeResult( result );

			((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::GetConferenceId(uid) );
			if(!result)
			{
				return 0;
			}

			ConferenceId_t cid = atoi( GET_DB_RESULT( result, 0, 0).c_str() );
			((XMySqlDB*)mydb)->FreeResult( result );
			return cid;
		}

		inline bool RemoveConference( XDataBase* mydb, ConferenceId_t cid )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessagerDBQueryFactory::RemoveConference(cid) );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return false;
			}
			((XMySqlDB*)mydb)->FreeResult( result );
			return true;
		}

	};
};

#endif
