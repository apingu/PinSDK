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

#include <Graphic2D.h>


#ifdef WIN32

	typedef void*                     Pdisplay;
    typedef HWND                      Pwnd;
    typedef HDC                       Pmgc;
    typedef HFONT                     Pwfont;
    typedef HBRUSH                    Pwbrush;
    typedef WNDCLASS                  Pwproperty;
	typedef HINSTANCE                 Pwinstance;
    //typedef MSG                       Pevent;


	typedef struct w32Style
	{
		DWORD dwStyle;
		DWORD dwExStyle;
	}PWstyle;

	#pragma comment ( lib, "kernel32.lib" )   /* link with Win32 lib */ 
	#pragma comment ( lib, "user32.lib" )     /* link with Win32 lib */ 
	#pragma comment ( lib, "gdi32.lib" )      /* link with Win32 lib */ 
	#pragma comment ( lib, "shell32.lib" )    /* link with Win32 lib */ 
	#pragma comment ( lib, "ole32.lib" )      /* link with Win32 lib */ 

	//#pragma comment ( lib, "winspool.lib" )   /* link with Win32 lib */ 
	//#pragma comment ( lib, "comdlg32.lib" )   /* link with Win32 lib */ 
	//#pragma comment ( lib, "advapi32.lib" )   /* link with Win32 lib */ 
	//#pragma comment ( lib, "oleaut32.lib" )   /* link with Win32 lib */ 
	//#pragma comment ( lib, "uuid.lib" )   /* link with Win32 lib */ 
	//#pragma comment ( lib, "odbc32.lib" )   /* link with Win32 lib */ 
	//#pragma comment ( lib, "odbccp32.lib" )   /* link with Win32 lib */ 


#endif

#ifdef LINUX

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <unistd.h>
//#include <Graphic.h>

	typedef Display                   Pdisplay;
	typedef Window                    Pwnd       
	typedef GC                        Pmgc;
	typedef Pdword                    Pwproperty;
	typedef XEvent                    Pevent;

#endif



#ifdef _PS2

#include <stdio.h>
#include <string.h>
//#include <pdef.h>

	typedef void*                     Pdisplay;
	typedef void*                     Pwnd;
	typedef void*                     Pmgc;
	typedef void*                     Pwfont;
	typedef void*                     Pwbrush;
	typedef void*                     Pwproperty ;
	typedef void*                     Pwinstance;
	//typedef MSG                       Pevent;
	typedef struct ps2Event
	{
		uint_t  uMsg;
		uint_t  key;
		int_t   x;
		int_t   y;
	} Pevent;

#endif


#ifdef _XBOX

	//#include <stdio.h>
	//#include <stdlib.h>
	//#include <Xbox.h>
    //#include <Graphic2D.h>

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
		uint_t  uMsg;
		uint_t  key;
		int_t   x;
		int_t   y;
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


#endif // PLATFORM_H