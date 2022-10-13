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
#include "MessengerDBQueryFactory.h"
#include <T_Lock.h>


namespace Picno
{

    #define ACCSTATE_OFFLINE     0
	#define ACCSTATE_ONLINE      1
    #define ACCSTATE_FREEZE      2
		
	namespace Messenger
	{
		#define GET_DB_RESULT( r, col, row )    ( ((XMySqlDB*)mydb)->GetQueryResult( r, col, row ) )
		T_Lock g_Querylock;

		inline void SetFriendlist( XDataBase* mydb, userid_t uid, const std::vector<userid_t>* friendlist )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::SetFriendlist( uid, friendlist ).c_str() );
			((XMySqlDB*)mydb)->FreeResult( result );
			return;
		}

		inline void GetFriendlist( XDataBase* mydb, userid_t uid, std::vector<userid_t>& friendlist )
		{
			TS_Lock lock( &g_Querylock );
			friendlist.clear();
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::GetFriendlist( uid ).c_str() );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return;
			}

			DBResBuf pBuffer=((XMySqlDB*)mydb)->GetQueryResource( result, 0, 0 );
			((XMySqlDB*)mydb)->FreeResult( result );

			int* buf=NULL;
			if( !pBuffer.empty() )
			{
			    buf=(int*)(&(pBuffer[0]));
				for( UINT i=0; i<_MAX_FRIENDLIST; i++ )
				{
					if( buf[i]!=0 )
						friendlist.push_back( buf[i] );
				}
			}
			return;
		}

		inline void SetNickname( XDataBase* mydb, userid_t uid, const char* nickname )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::SetNickname( uid, nickname ).c_str() );
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
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::GetNickname( uid ).c_str() );
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
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::SetPicture( uid, pic, len ).c_str() );
			((XMySqlDB*)mydb)->FreeResult( result );
			return;
		}

		inline DBResBuf GetPicture( XDataBase* mydb, userid_t uid )
		{
			TS_Lock lock( &g_Querylock );

			DBResBuf pBuffer;
			pBuffer.clear();
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::GetPicture(uid).c_str() );
			if(!result || ((XMySqlDB*)mydb)->GetXDBResultSize( result )<=0)
			{
				return pBuffer;
			}

			// TODO: Pin
			// 這裡要回來改
			pBuffer = ((XMySqlDB*)mydb)->GetQueryResource( result, 0, 0 );
			((XMySqlDB*)mydb)->FreeResult( result );
			return pBuffer;
		}

		inline void SetState( XDataBase* mydb, userid_t uid, short state )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result =	((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::SetState( uid, state ).c_str() );
//			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
//			{
//				return;
//			}
			((XMySqlDB*)mydb)->FreeResult( result );
			return;
		}

		inline short GetState( XDataBase* mydb, userid_t uid )
		{
			TS_Lock lock( &g_Querylock );
			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::GetState( uid ).c_str() );
			if( ((XMySqlDB*)mydb)->GetXDBResultEmpty(result) )
			{
				return 0;
			}
			short state=atoi( GET_DB_RESULT( result, 0, 0).c_str() );
			((XMySqlDB*)mydb)->FreeResult( result );
			return state;
		}



//		inline ConferenceId_t CreateConference( XDataBase* mydb, userid_t uid )
//		{
//			TS_Lock lock( &g_Querylock );
//			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::CreateConference(uid).c_str() );
//			((XMySqlDB*)mydb)->FreeResult( result );
//
//			((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::GetConferenceId(uid).c_str() );
//			if(!result)
//			{
//				return 0;
//			}
//
//			ConferenceId_t cid = atoi( GET_DB_RESULT( result, 0, 0).c_str() );
//			((XMySqlDB*)mydb)->FreeResult( result );
//			return cid;
//		}
//
//		inline bool RemoveConference( XDataBase* mydb, ConferenceId_t cid )
//		{
//			TS_Lock lock( &g_Querylock );
//			XDBResult result = ((XMySqlDB*)mydb)->ExecQuery( MessengerDBQueryFactory::RemoveConference(cid).c_str() );
//			((XMySqlDB*)mydb)->FreeResult( result );
//			return true;
//		}

	};
};

#endif
