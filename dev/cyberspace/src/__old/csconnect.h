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
//        Update:
//                              Pin      2004/12/26
//                                                  for package struct
//
//
//
#ifndef CSCONNECT_H
#define CSCONNECT_H

#include "IOSocket.h"
#include "T_Lock.h"
#include <time.h>
#include <assert.h>
//#include"ddebug.h"
//#include "pktdebug.h"

//======================================
//	CommandHeader Information
//======================================
#define MAX_MESSAGE_SIZE	0XFFFF-1
enum    CsMessageType       { CsCUSERDEFINE,
                              CsCSYSDEFINE = 0XFFFF };
                      //CsCSYSDEFINE = MAX_MESSAGE_SIZE+1 };
//======================================
//	Message strcut
//======================================
#pragma pack (push, 1)
struct CsPacketHeader
{
	int    IdentNum;
	int    TimeStamp;
	int    Sender0;
	int    Sender1;
	int    Reserve0;
	int    Reserve1;
};




struct CsCommandHeader
{
	unsigned short	CommandId;
	unsigned int	CommandLen;
};


struct CsHeaderMessage
{
	CsPacketHeader  HPacket;
	CsCommandHeader HCommand;
};

struct CsCommandMessage
{
	CsCommandHeader Header;
	char			Buff[MAX_MESSAGE_SIZE];
};


typedef struct 
{
	short len;
	char  string[1];
} NETSTR;

//#define CsCommadnMessageLen( h )    sizeof(CsPacketHeader)+sizeof(CsCommandHeader)+( (h).Header.CommandLen )
#pragma pack (pop)

//typedef int Stamp_t;
struct CsCallsocket
{
	int             e;
	IO::TCPSocket* fd;
};


// Sean:
// Wood，這個是pin註解掉的，他現在把封包改成上面的樣子，所以，從現在開始
// 2004/12/18 1410，server和client要重新編譯來匹配，不然會當掉
/* 
//======================================
//	Message strcut
//======================================
#pragma pack (push, 1)

struct CsPacketHeader
{
	int    Sender0;
	int    TimeStamp;
	int    Sender1;
	int    Reserve0;
};

struct CsCommandHeader
{
	unsigned short	CommandId;
	unsigned int	CommandLen;
};


struct CsHeaderMessage
{
	CsPacketHeader  HPacket;
	CsCommandHeader HCommand;
};


struct CsCommandMessage
{
	CsCommandHeader Header;
	char			Buff[MAX_MESSAGE_SIZE];
};

typedef struct 
{
	short len;
	char  string[1];
} NETSTR;
#define NETSTR_SIZE( s )    s->len+sizeof(short)



//#define CsCommadnMessageLen( h )    sizeof(CsPacketHeader)+sizeof(CsCommandHeader)+( (h).Header.CommandLen )
#define CsCommandMessageLen( h )   sizeof(CsCommandHeader)+( (h).Header.CommandLen )
#pragma pack (pop)



struct CsCallsocket
{
    int             e;
    IO::TCPSocket* fd;
};
*/

///////////////////////////////////////////////////////////////////////////////////////
// command IO

namespace CsCore
{
	extern int g_Identification_Number;

    //===============================================================================//
    //
    //
    //===============================================================================//
	inline int CsSendPacket( IO::TCPSocket* s, int eid, unsigned short cid, char* bp, size_t len )
	{
		char addrbuf[128];
		CsHeaderMessage header;
		//header.HPacket.IdentNum    = g_Identification_Number;
		header.HPacket.TimeStamp   = time( (time_t*)NULL );
		header.HPacket.Sender0     = get_ipn( s->GetLocalAddress( addrbuf, 128 ) );
		header.HPacket.Sender1     = eid;
		header.HCommand.CommandId  = cid;
		header.HCommand.CommandLen = len;

		Databuf msgbuf[2];
		msgbuf[0].buf = (char*)&header;
		msgbuf[0].len = sizeof(CsHeaderMessage);
		msgbuf[1].buf = (char*)bp;
		msgbuf[1].len = len;
		//sleep(1);
		int ret = s->Send( msgbuf, 2 );
		/*
		while( s->Send( msgbuf, 2 ) != 0 )
		{
			if( s->m_sockfd == 0 )
				break;
		//if( ret != 0 )
		//{
			s->PrintErrorCode( s->GetErrorNo() );
			//perror( "send message error! ");
		//}
		}
		*/
		//s->m_Lock.UnLock();
		//sleep( MSG_INTERVAL );
		return ret;
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	inline int CsRecvPacket( IO::TCPSocket* s, int& eid, unsigned short& cid, char* bp, unsigned int& len )
	{
		int ret,rlen;
		char *wptr;
		CsHeaderMessage *header;

		// 我們在此使用 s->user_var, 所以再上層不能使用.
		// 使用方法: [1]-[21]給 CsHeaderMessage 使用, [0]為state(sizeof(read))

	//	printf("Already has %d bytes,",s->user_var[0]);
		if(s->user_var[0]<sizeof(CsHeaderMessage)){
			rlen = sizeof(CsHeaderMessage);
			wptr = s->user_var+1;
			ret = s->Recv( wptr, rlen);
			// 沒接收完不做事才對
			if(ret >= rlen)
			{
				// 進行封包有效性檢測
				header = (CsHeaderMessage *)&s->user_var[1];
				if(header->HCommand.CommandLen > 2048)
					s->user_var[0] = 0;
				else
					s->user_var[0] = (char)ret;
	//			if(s->user_var[0]>0)	printf("Header(%d) received!",ret);
	//			else					printf("Header invalid!");
			}
		}
		if(s->user_var[0]>=sizeof(CsHeaderMessage))
		{
			header = (CsHeaderMessage *)&s->user_var[1];
			rlen = len = header->HCommand.CommandLen;
			wptr = bp;
			ret = s->Recv( wptr, rlen);
			if(ret>=rlen){	// 輸入有效! 全部讀入完成!!
				s->user_var[0]=0;
				eid = header->HPacket.Sender1;
				cid = header->HCommand.CommandId;
	//			printf("Data received(c=0x%x)!\n",cid);
				return ret;
			}
		}
	//	printf("\n");
		return 0;

		/*
		CsHeaderMessage header;
		if( s->Recv( (char*)&header, sizeof(CsHeaderMessage) ) <= 0 )
			return 0;
		//確保資料的正確性
		//if( header.HPacket.IdentNum != g_Identification_Number )
		//	return;
		eid = header.HPacket.Sender1;
		cid = header.HCommand.CommandId;
		if( ( len = header.HCommand.CommandLen ) != 0 )
		{
			s->Recv( bp, len );

			//	assert( ret != len );
		}
	
		return len;
		*/
	}


};


///////////////////////////////////////////////////////////////////////////////////////
// command factory
// %d, %l %h, %s, %v, %c

#define NETSTR_SIZE( s )    s->len+sizeof(short)
#define CsCommandMessageLen( h )   sizeof(CsCommandHeader)+( (h).Header.CommandLen )

namespace CsFactory
{
	void MakeCommand( CsCommandMessage* out, int CommandId, const char* fmt, ... );
	void SplitCommand( CsCommandMessage* out, int CommandId, const char* fmt, ... );
};


#endif// CSTCOMMANDMESSAGE_H
