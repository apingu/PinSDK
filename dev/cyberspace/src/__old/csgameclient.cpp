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

#pragma warning (disable : 4786)

#include "CsGameClient.h"
#include "CsGameCommand.h"
#include "Debug.h"

// user defined functions: 臨時性(Todo)
extern int usrMakeEnumPacket(int type,char *output);

namespace Cyberspace
{
	int LoginFunction( void *pData, unsigned int length, void *socket );

    //===============================================================================//
    //
    //
    //===============================================================================//
	GameClient::GameClient()
		:m_EID(0)
	{
		memset( m_Host, '0', sizeof( m_Host ) );
		m_Port = 0;
		m_Connected = 0;
		_pingbuf=NULL;
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	GameClient::~GameClient()
	{
		
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	bool GameClient::Connect( char* host, unsigned short port )
	{
		strcpy( m_Host, host );
		m_Port = port;
		return true;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::OnRecvRequest( const char* command )
	{
TS_Lock cs(&m_CSLock);
		CsCommandMessage* hSysMsg = (CsCommandMessage*)command;
		unsigned int *seqp,l = 0;
		int ia[4];
		char *cp;
		switch( hSysMsg->Header.CommandId )
		{
			case CsS_C_LOGIN_FEEDBACK:
			{
//UtAutoCS cs(&m_CSLock);
				AccountId_t aid=*(AccountId_t *)hSysMsg->Buff;
				if(aid > 0)
				{
					m_Connected = 1;
					m_Id = aid;
					OnLogin(aid);
				}
				else
				{
					m_Connected = -1;
					//ShutDown();
					OnLogin(aid);
				}
			}
			break;
			case CsS_C_GETENTITYRECORD_FEEDBACK:
			{
//UtAutoCS cs(&m_CSLock);
/*
#ifdef _NEW_VERSION_0005
    		    char* name=(char*)CsGetRecordEntityMessage::GetEntityName(hSysMsg);
				void* property = CsGetRecordEntityMessage::GetProperty( hSysMsg, l );
				//if(*name=='\0')	name=NULL;
				OnGetRecordEntity( name, property, l );
#else
				*/
    			EntityId_t eid=CsGetRecordEntityMessage::GetEntityID(hSysMsg);
				void* property = CsGetRecordEntityMessage::GetProperty( hSysMsg, l);
				//if(*name=='\0')	name=NULL;
				OnGetRecordEntity( eid, property, l);
//#endif

			}
			break;
			case CsC_FREEBACK_SELECTENTITY:
			{
//UtAutoCS cs(&m_CSLock);
				/*
#ifdef _NEW_VERSION_0005
				OnGetSelectEntityId( CsSelectEntityIdMessage::GetEntityId( hSysMsg ) );

#else
*/
				m_EID=CsSelectEntityIdMessage::GetEntityId(hSysMsg);
				OnGetSelectEntityId(m_EID,
				                   	CsSelectEntityIdMessage::GetRegionID( hSysMsg ));
//#endif
			}
			break;
			case CsC_FEEDBACK_DELETEENTITY:
				{
//UtAutoCS cs(&m_CSLock);
				OnEntityDeleted(CsDeleteEntityMessage::GetFeedBackRet(hSysMsg));
				}
				break;
			//case CsC_FEEDBACK_ENTERENTITY:	// 進入某區
			case CsC_FEEDBACK_ENTITYJOIN:
				{
//UtAutoCS cs(&m_CSLock);
				
					
					ia[0]=CsEnterEntityMessage::GetEntityID(hSysMsg);
					cp=(char *)CsEnterEntityMessage::GetProperty(hSysMsg,ia[3]);
					OnEntityJoinGame(ia[0],cp,ia[3]);
				}
				break;
			//case CsC_FEEDBACK_DEPARTENTITY:	// 離開遊戲
			case CsC_FEEDBACK_ENTITYEXIT:
				{
//UtAutoCS cs(&m_CSLock);
					/*
#ifdef _NEW_VERSION_0005
					cp=&hSysMsg->Buff[sizeof(EntityId_t)];
					len=hSysMsg->Header.CommandLen-sizeof(EntityId_t);
					OnDepartSphere( CsDepartEntityMessage::GetEntityID(hSysMsg) );
#else
					*/
					//cp=(char *)CsDepartEntityMessage::GetProperty(hSysMsg,ia[3]);
					OnEntityExitGame(CsEntityExitMessage::GetEntityID(hSysMsg));
//#endif
				}
				break;
			case CsC_FEEDBACK_PING:
				{
//UtAutoCS cs(&m_CSLock);
					seqp=(unsigned int *)hSysMsg->Buff;
					doPing(*seqp);
				}
				break;
			case CsC_REQUEST_PING:	// 目前沒用(給Server測量用)
				{
//UtAutoCS cs(&m_CSLock);
				}
				break;
			case CsC_ENTERREGION:
				OnEnterRegion(CsChangeRegionRequest::GetEntityID(hSysMsg),
					CsChangeRegionRequest::GetRegionID(hSysMsg));
				break;
		}
		
		Client::OnRecvRequest( command );
		return;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	bool GameClient::Login( char *username, char *password )
	{
		ChannelId_t     id=0;
		unsigned int len=0;
		if( ( m_Host==0 ) && ( m_Port == 0 ) )
		{
			return false;
		}
		Client::Connect( m_Host, m_Port );
		CsCommandMessage hChMsg;// = (CsCommandMessage2 *)&m_send_cmd;
		CsLoginMessage::SetRequestMessage(&hChMsg,username,password);
		Client::SendMessage( CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg) );

		for( UINT i = 0; i<20; ++i )
		{
			if( m_Connected==1 )
				return true;
			else
			{
				Sleep( 1000 );
				continue;
			}
		}

		Client::ShutDown();
		return false;
	}

	// 非同步型 login ------------------------------------------------------------
	void GameClient::DoLogin(const char *username,const char *passwd)
	{
		ChannelId_t id=0;
		unsigned int len=0;
		if((m_Host!=0)||(m_Port!=0)){
			Client::Connect( m_Host, m_Port );
			CsCommandMessage hChMsg;
			CsLoginMessage::SetRequestMessage(&hChMsg,username,passwd);
			Client::SendMessage(CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg));
		}
	}
/*
#ifdef _NEW_VERSION_0005
    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::CreateEntity( const char* name, void* prop, size_t len )
	{
		CsCommandMessage hChMsg;
		CsCreateEntityMessage::SetRequestMessage( &hChMsg, name, prop, len );
		SendMessage( CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg) );
		return;
	}

    //===============================================================================//
    //
    // 告訴SERVER 你要選擇哪一個人
    //===============================================================================//
	void GameClient::SelectEntity( const char* name )
	{
		CsCommandMessage hChMsg;
		CsSelectEntityMessage::SetRequestMessage(&hChMsg,name);
		SendMessage( CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg) );
		return;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::DeleteEntity( const char* name )
	{
		CsCommandMessage hChMsg;
		CsDeleteEntityMessage::SetRequestMessage( &hChMsg, name );
		SendMessage(CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg));
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::EntityJoin( EntityId_t eid )
	{
		CsCommandMessage hChMsg;
		CsEntityJoinMessage::SetRequestMessage( &hChMsg, eid );
		SendMessage(CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg));
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::EntityExit( EntityId_t eid )
	{
		CsCommandMessage hChMsg;
		CsEntityExitMessage::SetRequestMessage( &hChMsg, eid );
		SendMessage(CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg));
	}


#else	
  */
    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::CreateNewEntity( const char* name, void* prop, size_t len )
	{
		CsCommandMessage hChMsg;
		CsCreateEntityMessage::SetRequestMessage( &hChMsg, name, prop, len );
		Client::SendMessage( CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg) );
		return;
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::SelectEntity(EntityId_t eid)
	{
		CsCommandMessage hChMsg;
		CsSelectEntityMessage::SetRequestMessage(&hChMsg,eid);
		Client::SendMessage( CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg) );
		return;
	}
    //===============================================================================//
    // 用名稱選擇... 目前等於加入世界了
    //===============================================================================//
	void GameClient::SelectEntity( const char* name )
	{
		EntityJoin(name);
	}

    //===============================================================================//
    //
    //
    //===============================================================================//
	void GameClient::DeleteEntity(EntityId_t eid)
	{
		CsCommandMessage hChMsg;
		CsDeleteEntityMessage::SetRequestMessage( &hChMsg, eid);
		Client::SendMessage(CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg));
	}

	//===============================================================================//
    /// EntityJoin: 加入世界
    /// @param eid: entity ID
    //===============================================================================//	
	void GameClient::EntityJoin( EntityId_t eid )
	{
		CsCommandMessage hChMsg;

		CsEnterEntityRequest::SetRequestMessage(&hChMsg,eid);
		Client::SendMessage(CsID_NULL,CsCSYSDEFINE,(char*)&hChMsg,CsCommandMessageLen(hChMsg));
		return;
	}
	//===============================================================================//
    /// EntityJoin: 加入遊戲世界
    /// @param eid: name
    //===============================================================================//	
	void GameClient::EntityJoin( const char* name )
	{
		CsCommandMessage hChMsg;
		CsEnterEntityRequest2::SetRequestMessage(&hChMsg,name);
		Client::SendMessage( CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg) );
		return;
	}
    //===============================================================================//
	/// EntityExit: 離開遊戲
    /// @param eid: entity ID
    //===============================================================================//	
	void GameClient::EntityExit( EntityId_t eid )
	{
		CsCommandMessage hChMsg;
		//CsDepartEntityRequest::SetRequestMessage(&hChMsg,eid);
		CsEntityExitMessage::SetRequestMessage(&hChMsg,eid);
		Client::SendMessage(CsID_NULL,CsCSYSDEFINE,(char*)&hChMsg,CsCommandMessageLen(hChMsg));
	}
    //===============================================================================//
	/// EntityChangeRegion: 主動要求移出世界
    /// @param eid: entity ID
	/// @param rid: 新的 region
    //===============================================================================//	
	void GameClient::EntityChangeRegion(EntityId_t eid,RegionId_t rid)	// 換區
	{
		CsCommandMessage hChMsg;
		CsChangeRegionRequest::SetRequestMessage(&hChMsg,eid,rid);
		Client::SendMessage(CsID_NULL,CsCSYSDEFINE,(char*)&hChMsg,CsCommandMessageLen(hChMsg));
	}

//#endif




	bool GameClient::Logout( void )
	{
		m_Connected=0;
		CsCommandMessage hChMsg;
		hChMsg.Header.CommandId=CsC_REQUEST_LOGOUT;
		hChMsg.Header.CommandLen=0;
		Client::SendMessage(CsID_NULL, CsCSYSDEFINE, (char*)&hChMsg, CsCommandMessageLen(hChMsg));
		// 等著被 Server 斷線就可以了....

		//sleep( 100000 );
		
		//while(getReceiverId()!=NULL)
		//{
			
		//}
	
		Client::ShutDown();
		return true;
	}

	bool GameClient::GetAllEntity()
	{
		CsCommandMessage hChMsg;
		hChMsg.Header.CommandId = CsC_REQUEST_GETALLENTITYRECORD;
		hChMsg.Header.CommandLen = 0;
		return 0<=Client::SendMessage(CsID_NULL,CsCSYSDEFINE,(char*)&hChMsg,CsCommandMessageLen(hChMsg));
	}

	void GameClient::DoPing()	// 這個是為了能正確處理 ping time-out 狀況要呼叫的.目前無處可掛
	{
		unsigned t,t0;
		if(_pingbuf){
			t=GetTickCount();
			t0=_pingbuf[_premain+1+MAX_PING_COUNT];
			if(t-t0>PING_TIMEOUT){
				_pingbuf[_premain+1]=0xffffffff;
				DBGPrint("ping %d time-out\n",_premain+1);
				doPing(-1);
				if(_premain<0){	// all over
					stopPing();
				}
			}
		}
	}
	void GameClient::stopPing()
	{
		unsigned t,t2;
		if(!_pingbuf)	return;
		if(ping_lock)	return;	// MUTEX
		ping_lock=true;
		for(ping_avg=t2=t=0;t<MAX_PING_COUNT;++t){
			DBGPrint("result: %d %d\n",_pingbuf[t],_pingbuf[t+MAX_PING_COUNT]);
			if(ping_max<_pingbuf[t])
				ping_max=_pingbuf[t];
			if(_pingbuf[t]!=0xffffffff){
				ping_avg+=_pingbuf[t];
				++t2;
			}
		}
		ping_avg/=(t2*2);
		delete _pingbuf;
		_pingbuf=NULL;
		ping_lock=false;
	}
	// 目前沒有考慮到如果斷線的狀況, 這點要注意...
	void GameClient::doPing(int getseq)
	{
		char buf[64];
		unsigned t;
		//t=GSO::gResClock->GetTickTime();
		t=GetTickCount();
		if(getseq>=0){	// 剛收到的
			if(getseq==_premain+1){	// 必須依照順序
				// round-trip-time x 2
				_pingbuf[getseq]=t-_pingbuf[getseq+MAX_PING_COUNT];
				DBGPrint("get ping %d time=%d\n",getseq,_pingbuf[getseq]);
				if(getseq==0){	// 全收到了?
					stopPing();
				}
			} else {	// 這個已經 time-out 掉了, 但不必送下一個
				return;
			}
		}
		// 發下一個
		if(_premain>=0){
			_pingbuf[_premain+MAX_PING_COUNT]=t;
			DBGPrint("send %d on %d\n",_premain,t);
			CsPingMessage::SetRequestMessage((CsCommandMessage *)buf,_premain);
			CsCommandMessage *bp=(CsCommandMessage *)buf;
			Client::SendMessage(CsID_NULL,CsCSYSDEFINE,buf,CsCommandMessageLen(*bp));
			_premain--;
		}
	}
	void GameClient::StartPing()
	{
		if(_premain>0)	return;	// 進行中, 不能重複
		_pingbuf=new unsigned int[MAX_PING_COUNT*2];
		ping_avg=0xffffffff;	// 改為無效數值
		ping_max=0;
		_premain=4;
		ping_lock=false;
		doPing(-1);
	}
	unsigned int GameClient::GetPingTime()
	{
		return ping_avg;
	}

/*	void GameClient::GetAllEntity(int type)	// 取得除了一般角色外的物件
	{
		CsCommandMessage ccm;
		int sz=usrMakeEnumPacket(type,ccm.Buff);
		ccm.Header.CommandLen=sz;
		ccm.Header.CommandId=CsC_REQUEST_GETALLENTITYREC2;
		SendMessage(CsID_NULL,CsCSYSDEFINE,(char *)&ccm,CsCommandMessageLen(ccm));
	} */
};//Cyberspace

