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
//                                               Copyright    (C)    1999    Pin
//
#include "libp.h"


#ifdef _WIN32

//#include <winsock.h>

//=============================================================================
//  Name : inet_aton
//  Desc :
//=============================================================================
int gettimeofday( struct timeval* tvp, struct timezone* tzp )
{
	struct _timeb tb;

	_ftime( &tb );
	if( tvp )
	{
		tvp->tv_sec = tb.time;
		tvp->tv_usec = tb.millitm * 1000;
	}
	if( tvp )
	{
		tzp->tz_minutestwest = tb.timezone;
		tzp->tz_dsttime = tb.dstflag;
	}
	return 0;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
/*
char* strerror( int err )
{
	if( err >= 0 && err < sys_nerr )
		return sys_errlist[ err ];
	else if( err >= MINBSDSOCKERR && err < MAXBSDSOCKERR )
		return bsdsocketerrs[ err - MINBSDSOCKERR ];
	else if( err == WSASYSNOTREADY )
		return "Network subsystem is unusable;
	else if( err == WSAVERNOTSUPPORTED )
	    return "This version of Winsock not supported";
	else if( err == WSANOTINITALISED )
		return "Winsock not initialized";
	else
		return "Unknow error";
}
*/
#endif //win32
//#else //linux

#ifdef _UNIX
//=============================================================================
//  Name :
//  Desc :
//=============================================================================
sighndlr_t* signal( int sig, sighndlr_t* hndlr )
{
	struct sigaction act;
	struct sigaction xact;
	act.sa_handler = hndlr;
	act.sa_flags = 0;
	sigemptyset( &act.sa_mask );
	if( sigaction( sig, &act, &xact ) < 0 )
		return SIG_ERR;
	return xact.sa_handler;
}

//=============================================================================
//  Name : init() 
//  Desc :
//=============================================================================
void init( char** argv )
{
#ifdef _WIN32

    WSADATA wsadata;

    ( program_name = strrchr( argv[0], '\\' ) ) ?
        program_name++ : ( program_name = argv[0] );
    WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
#endif
}

	
#endif

