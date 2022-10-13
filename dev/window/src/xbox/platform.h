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
#ifndef PLATFORM_H
#define PLATFORM_H

#ifndef _XBOX
    #define _XBOX
#endif

//#include <stdio.h>
//#include <stdlib.h>
//#include <Xbox.h>
#include <Graphic2D.h>

	typedef void*                     Pdisplay;
    typedef void*                     Pwnd;
    typedef void*                     Pmgc;
    typedef void*                     Pwfont;
    typedef void*                     Pwbrush;
    typedef void*                     Pwproperty ;
	typedef void*                     Pwinstance;
    //typedef MSG                       Pevent;
    typedef struct w32Event
    {
        Puint  uMsg;
        Puint  key;
        Pint   x;
        Pint   y;
    } Pevent;


#ifdef _DEBUG
	#pragma comment ( lib, "xapilibd.lib" )   /* link with Xbox lib */ 
	#pragma comment ( lib, "d3d8d.lib" )      /* link with Xbox lib */ 
	#pragma comment ( lib, "d3dx8d.lib" )     /* link with Xbox lib */ 
	#pragma comment ( lib, "xgraphicsd.lib" ) /* link with Xbox lib */ 
	#pragma comment ( lib, "dsoundd.lib" )    /* link with Xbox lib */ 
	#pragma comment ( lib, "dmusicd.lib" )    /* link with Xbox lib */ 
	#pragma comment ( lib, "xnetd.lib" )      /* link with Xbox lib */ 
	#pragma comment ( lib, "xboxkrnl.lib" )   /* link with Xbox lib */ 
#else
	#pragma comment ( lib, "xapilib.lib" )    /* link with Xbox lib */ 
	#pragma comment ( lib, "d3d8.lib" )       /* link with Xbox lib */ 
	#pragma comment ( lib, "d3dx8.lib" )      /* link with Xbox lib */ 
	#pragma comment ( lib, "xgraphics.lib" )  /* link with Xbox lib */ 
	#pragma comment ( lib, "dsound.lib" )     /* link with Xbox lib */ 
	#pragma comment ( lib, "dmusic.lib" )     /* link with Xbox lib */ 
	#pragma comment ( lib, "xnet.lib" )       /* link with Xbox lib */ 
	#pragma comment ( lib, "xboxkrnl.lib" )   /* link with Xbox lib */ 
#endif



#endif
	