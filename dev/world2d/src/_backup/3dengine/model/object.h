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
//                                               Copyright    (C)    2001    Pin
//
#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

//Base engine define
//#include <Kernel3D.h>
//#include <VfwGraphic.h>  //3DEngine
#include <pdef.h>        //Base
#include "Axis.h" 
#include "Texture.h"
#include "ObjectBlock.h"


//#include<rwcore.h>
//#include<rpworld.h>


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
class CPObject : public CPAxis
{

    CPObjectBlock               m_Bounding;

public:
    //CPAxis*    m_pFrame;

#if defined (_XGLKERNEL_) || defined (_WGLKERNEL_)

    std::vector<VfVertex3f*>    m_vertex;//polygon

#endif

#ifdef _RWKERNEL_
public:
    RpAtomic*   m_pAtomic;
	operator    RpAtomic*() { return m_pAtomic; }
    RpAtomic&   operator=( RpAtomic* object );

	CPObject( RpAtomic* object );
#endif

public:

    CPTexture*  texture;

    CPObject();
    ~CPObject();

	void           Create( void );
	
    Pint           Load( Pcstr pathname );
    LG::BBoxf*     LocalBoundBox( LG::BBoxf* bbox );
    LG::Spheref*   LocalBoundSphere( LG::Spheref* bsphere );

	void        SetBBox( VfBBox* bbox );
	VfBBox*     GetBBox( void );
	void        SetBphere( VfBSphere* bsphere );
	VfBSphere*  GetBSphere( void );

};

#endif


