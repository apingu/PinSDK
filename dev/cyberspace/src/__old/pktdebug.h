#ifndef _PKTDEBUG_H_
#define _PKTDEBUG_H_

// for debugging...
// if _PKTDEBUG not defined, it will do nothing.
// Since actual data format is depending on implementation, it should be written
// in other place than this engine.

#ifdef _PKTDEBUG
extern void dbg_print_packet(unsigned short channel,const char *data,int len, \
							 const char *sockname,bool to);
extern const char *dbg_get_sock_name(void *socket_ptr);	// Get Account of socket
extern bool dbgpkt_resolve;	// control variable
#else
#define dbg_print_packet	//
#define dbg_get_sock_name	//
#endif

#endif	// _PKTDEBUG_H_



