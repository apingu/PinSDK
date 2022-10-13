// the information of server machine
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
#ifndef CSTARBITNODE_H
#define CSTARBITNODE_H

#include "IOSocket.h"

#pragma pack (push,1)

//start server node
typedef struct _cstHOSTNODE
{
	char           name[32];
    DWORD          port;
	time_t         connecttime;
	unsigned short maxprocess;
	unsigned short maxconnect;
	unsigned short usedconnect;
	unsigned int   maxmemorysize;
	unsigned int   usedmemorysize;
}CstHostNode;

//server connect node
typedef struct _cstCONNECTNODE
{
	int  id;
	MACAddress_t macaddr;
	bool translate;
	IO::TCPSocket* socket;
}CstConnectNode;

#pragma pack (pop)


#endif