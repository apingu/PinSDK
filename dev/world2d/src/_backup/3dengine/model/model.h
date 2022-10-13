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
//     Pin 2002/11/3
//                                               Copyright    (C)    2001    Pin
//
#ifndef MODEL_H
#define MODEL_H

//#include <Axis.h>
#include "Object.h"


class CPModel : public CPAxis
{
    MEM::String m_fname;
	MEM::String m_texfname;
	LG::BBoxf   m_BBox;
	LG::Spheref m_BSphere;

protected:


#if defined (_WGLKERNEL) || defined (_XGLKERNEL_)

#endif

#ifdef _RWKERNEL_
public:
    RpClump*   m_pClump;
    operator   RpClump*() { return m_pClump; }
    RpClump&   operator=( RpClump* model );

    CPModel( RpClump* Camera );
#endif //_RWKERNEL_

    std::vector<CPObject*>  m_object;

protected:

    void        UpdateBoundBox( void );
    void        UpdateBoundSphere( void );

public:

    CPModel();
    ~CPModel();

    Pstr        filename( void );
	Pstr        texfname( void );

    int         empty( void );
    void        Destroy( void );

    VfError     Load( Pcstr fname );

    CPObject*   Object( Puint no ) { return ( no < m_object.size() ) ? m_object[ no ] : 0 ; }


    int         GetObjectCount( void );

	LG::BBoxf      GetBBox( void );
	LG::Spheref    GetBSphere( void );
	LG::BBoxf      GetLocalBox( void );
	LG::Spheref    GetLocalSphere( void );


    //int         AddObject ( RpAtomic* obj )
    //RpAtomic*   LoadObject( const char *path, RwTexDictionary *texDict );
};


#endif

