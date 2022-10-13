#ifndef CSTCOMMANDMESSAGE_H
#define CSTCOMMANDMESSAGE_H

#include "IOSocket.h"
//#include"ddebug.h"
#include "pktdebug.h"

//======================================
//	CommandHeader Information
//======================================
#define MAX_MESSAGE_SIZE	1024
enum CsMessageType  { CsCUSERDEFINE,
                      CsCSYSDEFINE = MAX_MESSAGE_SIZE+1 };




//======================================
//	Message strcut
//======================================
#pragma pack (push,1)

struct CsPacketHeader
{
	int    Sender0;
	int    Sender1;
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


//#define CsCommadnMessageLen( h )    sizeof(CsPacketHeader)+sizeof(CsCommandHeader)+( (h).Header.CommandLen )
#define CsCommandMessageLen( h )   sizeof(CsCommandHeader)+( (h).Header.CommandLen )
#pragma pack (pop)


struct CsCallsocket
{
    int             e;
    IO::TCPSocket* fd;
};
    //===============================================================================//
    //
    //
    //===============================================================================//
	inline int CsSendRequest( IO::TCPSocket* s, int eid, unsigned short cid, char* bp, size_t len )
	{
		/*
		CsCommandHeader hcommand;
		hcommand.CommandId  = cid;
		hcommand.CommandLen = len;

		CsPacketHeader  packet;
		packet.Command.CommandId  = cid;
		packet.CommandLen.CommandId  = len;
		*/

		CsHeaderMessage header;
		header.HPacket.Sender0 = 0;
		header.HPacket.Sender1 = eid;
		header.HCommand.CommandId  = cid;
		header.HCommand.CommandLen  = len;

		Databuf msgbuf[2];
		msgbuf[0].buf = (char*)&header;
		msgbuf[0].len = sizeof(CsHeaderMessage);
		msgbuf[1].buf = (char*)bp;
		msgbuf[1].len = len;

		dbg_print_packet(cid,bp,len,dbg_get_sock_name(s),true);	// just for debugging. define _PKTDEBUG to enable this.
		return s->Send( msgbuf, 2 );
	}


    //===============================================================================//
    //
    //
    //===============================================================================//
	inline int CsRecvRequest( IO::TCPSocket* s, int& eid, unsigned short& cid, char* bp, unsigned int& len )
	{
		/*
		CsCommandHeader hcommand;
		s->Recv( (char*)&hcommand, sizeof(CsCommandHeader) );
		cid = hcommand.CommandId;
		len = hcommand.CommandLen;
		*/
		CsHeaderMessage header;
		s->Recv( (char*)&header, sizeof(CsHeaderMessage) );
		eid = header.HPacket.Sender1;
		cid = header.HCommand.CommandId;
		len = header.HCommand.CommandLen;
		return s->Recv( bp, len );
	}	

#endif// CSTCOMMANDMESSAGE_H
