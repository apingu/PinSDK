//VfwKernel.h
//
//  vfw3d kernel define  for renderware
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
//                                               Copyright    (C)    2002    Pin
//
#ifndef KERNEL_H
#define KERNEL_H

#define _RWKERNEL_

#ifndef _RENDERWARE_
#define _RENDERWARE_
#endif

#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rprandom.h>
#include <rpmatfx.h>
#include <rphanim.h>
#include <rpskin.h>
#include <rtcharse.h>
#include <rt2d.h>
#include <rtimport.h>
#include <rtpick.h>


//#include "..//..//Base//pdef.h"
//#include "..//..//Base//tImage.h"
//typedef RpClump  rwModel;

#pragma comment ( lib, "rwcore.lib" )     /* link with RW lib */ 
#pragma comment ( lib, "rpworld.lib" )    /* link with RW lib */ 
#pragma comment ( lib, "rtAnim.lib" )     /* link with RW lib */ 
#pragma comment ( lib, "rphanim.lib" )    /* link with RW lib */ 
#pragma comment ( lib, "rpskin.lib" )     /* link with RW lib */ 
#pragma comment ( lib, "rt2d.lib" )       /* link with RW lib */ 
//#pragma comment ( lib, "rtslerp.lib" )  /* link with RW lib */ 
#pragma comment ( lib, "rtquat.lib" )     /* link with RW lib */ 
//#pragma comment ( lib, "rtimport.lib" ) /* link with RW lib */ 
#pragma comment ( lib, "rtcharse.lib" )   /* link with RW lib */ 
#pragma comment ( lib, "rpmatfx.lib" )    /* link with RW lib */ 
#pragma comment ( lib, "rpcollis.lib" )   /* link with RW lib */ 
#pragma comment ( lib, "rprandom.lib" )   /* link with RW lib */ 
#pragma comment ( lib, "rplogo.lib" )     /* link with RW lib */ 
#pragma comment ( lib, "rtintsec.lib" )   /* link with RW lib */ 
#pragma comment ( lib, "rtpick.lib" )     /* link with RW lib */ 



#include <rtpng.h>
#include <rtbmp.h>
#include <rttiff.h>
#pragma comment ( lib, "rttiff.lib" )   /* link with RW lib */ 
#pragma comment ( lib, "rtpng.lib" )    /* link with RW lib */ 
#pragma comment ( lib, "rtbmp.lib" )    /* link with RW lib */ 


typedef RwImage*( *ImageCallBackRead )( const RwChar* imageName );



#endif
