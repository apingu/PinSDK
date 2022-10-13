//VfwKernel.h
//
//  vfw3d kernel define for OpenGL
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
//                                               Copyright    (C)    2001    Pin
//
#ifndef KERNEL3D_H
#define KERNEL3D_H

//#include "Frame.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
//#include <gl/glut.h>
#include <math.h>
#include <vector>
#include <tImage.h>     // Pin/Base/
#include <Graphic3D.h>  // Pin/Base/

#define _VFWKERNEL_

#ifdef _UNIX //linux
#define _XGLKERNEL_
#ifndef _XGL_
#define _XGL_
#endif
#include <GL/glx.h>
#include <string.h>
#include <X11/keysym.h>
#endif

#ifdef _WIN32
#define _WGLKERNEL_
#ifndef _WGL_
#define _WGL_
#pragma comment ( lib, "opengl32.lib" ) /* link with OpenGL lib */ 
#pragma comment ( lib, "glu32.lib" )    /* link with OpenGL lib */ 
#pragma comment ( lib, "glaux.lib" )    /* link with OpenGL lib */ 
#endif





#endif






