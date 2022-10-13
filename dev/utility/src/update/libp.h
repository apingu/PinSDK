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
#ifndef LIBP_H
#define LIBP_H

#include <pdef.h>

#ifdef _WIN32
#include <sys/timeb.h>
    //char* strerror    ( int err );
    void  init        ( char** argv );
    int   gettimeofday( struct timeval* tvp, struct timezone* tzp );
	
#else //linux

    sighndlr_t* signal( int sig, sighndlr_t* hndlr );
	
#endif



#endif
