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

#include <stdio.h>
#include <string.h>
#include <pdef.h>
#include <Graphic2D.h>

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
        Puint  uMsg;
        Puint  key;
        Pint   x;
        Pint   y;
    } Pevent;


#endif