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
#define _X11

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <unistd.h>

#include <Graphic.h>

	typedef Display                   Pdisplay;
	typedef Window                    Pwnd       
    typedef GC                        Pmgc;
    typedef Pdword                    Pwproperty;
    typedef XEvent                    Pevent;

#endif


#endif
