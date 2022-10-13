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
//                                               Copyright    (C)    2000    Pin
//
#ifndef G3DSECTOR_H
#define G3DSECTOR_H

#include <VfwGraphic.h> //3DEngine
#include <tString.h>
#include <vector>
#include <tShape.h>
#include <ObjectBlock.h>

//#include "G3DWorld.h"


class VfSector
{
	UINT ID;
	MEM::String    m_fname;
	MEM::String    m_texfname;
	VfBBox        m_BBox;
	CPObjectBlock m_Bounding;

public:

#ifdef _RWKERNEL_
	RpWorld*     m_pWorld;
#else
	UINT         m_collisionsize;
	Vf3DGeometry m_geometry;
#endif

	//GSECTOR      m_Descriptor;


	VfSector();
	~VfSector();

	VfError Create(VfBBox bBox);
	VfError Create(Pcstr fname);
	void    Release(void);

	VfError Save(Pcstr fname);

	Pcstr   filename(void);
	Pcstr   texfname(void){ return m_texfname; }

	VfBBox  GetBBox(void);
	UINT    GetPolygonsCount(void);
	UINT    GetVerticesCount(void);

	VfVertex3f Origin(void);


};


#endif

