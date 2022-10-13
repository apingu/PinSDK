#include "CsConnect.h"

int csSendPacket( IO::TCPSocket* s, int eid, unsigned short cid, char* bp, size_t len )
{
	char addrbuf[128];
	CsHeaderMessage header;
	header.HPacket.Sender0 = get_ipn( s->GetLocalAddress( addrbuf, 128 ) );
	header.HPacket.TimeStamp = time( (time_t*)NULL );
	header.HPacket.Sender1 = eid;
	header.HCommand.CommandId  = cid;
	header.HCommand.CommandLen  = len;

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
	// Sean
	sleep(0);
	return ret;
}


int csReadPacket( IO::TCPSocket* s, int& eid, unsigned short& cid, char* bp, unsigned int& len )
{
	int ret,l,rlen;
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
		if(ret >= rlen){
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
	if(s->user_var[0]>=sizeof(CsHeaderMessage)){
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
}
