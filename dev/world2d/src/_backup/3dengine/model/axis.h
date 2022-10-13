//Axis
//
//    3D Martix
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
#ifndef AXIS_H
#define AXIS_H

//#include <Kernel3D.h>
#include <vector>
#include "VfwGraphic.h"
//#include "..//File//PFile.h"

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
class CPAxis
{
public:

    int                     ID;
	
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    PA::Matrixf*              m_pMatrix;
    CPAxis*                 parent;
    std::vector<CPAxis*>    child;
    operator  PA::Matrixf*()  { return m_Matrix; }

#endif
#ifdef _RWKERNEL_

	RwFrame*                m_pMatrix;
    
#endif

    enum _AXIS { AXIS_X, AXIS_Y, AXIS_Z };

    CPAxis();
    ~CPAxis();

    void*       Create( void );

    Pbool       empty( void );

	PA::Matrixf*  GetWorldMatrix( void );
    PA::Matrixf*  GetMatrix( void );


	void        Erect( void );
    void        Identity( void );

    void        Translate( const LG::Vertex3f* pos );
    void        Rotate( const LG::Vector3f* axis, float angle );
    void        Scale( const LG::Vertex3f* scale ) {  };

    void        ReTranslate( const LG::Vertex3f* pos );
    void        ReRotate( const LG::Vector3f* axis, float angle );
    void        ReScale( const LG::Vertex3f* scale ) {  };

    void        PreTranslate( const LG::Vertex3f* pos );
    void        PreRotate( const LG::Vector3f* axis, float angle );
    void        PreScale( const LG::Vertex3f* scale ) {  };

    //void        Merge( _AXIS axis, VfVector3f vec );

    LG::Vertex3f* At( void );
    LG::Vertex3f* Up( void );
    LG::Vertex3f* Right( void );
    LG::Vertex3f* Position( void );


    void        Load( Pstr fpath );
    void        Save( Pstr fpath );

    //LG::Vertex3f     LAt            ( void );
    //LG::Vertex3f     LUp            ( void );
    //LG::Vertex3f     LRight         ( void );
    //LG::Vertex3f     LPosition      ( void );
};




#endif
