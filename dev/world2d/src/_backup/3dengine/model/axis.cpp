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
#include "Axis.h"
#include <PFile.h>


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
CPAxis::CPAxis()
{
    ID = -1;
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    parent = NULL;
    child.clear();
#endif

#ifdef _RWKERNEL_
    m_pMatrix = NULL;
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
CPAxis::~CPAxis()
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    SAFE_DELETE( m_pMatrix );
#endif
#ifdef _RWKERNEL_
	if( m_pMatrix != NULL )
    {
        //RwFrameDestroy( m_pMatrix );
        m_pMatrix = NULL;
    }
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void* CPAxis::Create( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return m_pMatrix = new PA::Matrixf;
#endif
#ifdef _RWKERNEL_
    return m_pMatrix = RwFrameCreate();
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
Pbool CPAxis::empty( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    if( m_pMatrix == NULL )
        return TRUE;
#endif
#ifdef _RWKERNEL_
    if( m_pMatrix == NULL )
        return TRUE;
#endif
    return FALSE;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
PA::Matrixf* CPAxis::GetWorldMatrix( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)

#endif
#ifdef _RWKERNEL_
    return ( PA::Matrixf * ) RwFrameGetMatrix( m_pMatrix );
#endif
    return NULL;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
PA::Matrixf* CPAxis::GetMatrix( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return m_pMatrix;
#endif
#ifdef _RWKERNEL_
    return ( PA::Matrixf * ) RwFrameGetLTM( m_pMatrix );
#endif
    return NULL;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPAxis::Identity( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)

#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation

    RwMatrix* pMatrix   = RwFrameGetMatrix( m_pMatrix );

    //Get the camera position and orientation
    RwV3d* vPos         = RwMatrixGetPos( pMatrix );
    RwV3d* vAt          = RwMatrixGetAt( pMatrix );
    RwV3d* vUp          = RwMatrixGetUp( pMatrix );
    RwV3d* vRight       = RwMatrixGetRight( pMatrix );

    //Set the default orientation of the camera
    vAt->x = 0;  vAt->y = 0;  vAt->z = 1;
    vUp->x = 0;  vUp->y = 1;  vUp->z = 0;
    vRight->x = 1; vRight->y = 0; vRight->z = 0;
    vPos->x = 0; vPos->y = 0; vPos->z = 0;

    //Get the camera position and orientation
    RwMatrix* pMatrixL  = RwFrameGetLTM( m_pMatrix );

    //Get the camera position and orientation
    RwV3d* vPosL        = RwMatrixGetPos( pMatrixL );
    RwV3d* vAtL         = RwMatrixGetAt( pMatrixL );
    RwV3d* vUpL         = RwMatrixGetUp( pMatrixL );
    RwV3d* vRightL      = RwMatrixGetRight( pMatrixL );

    //Set the default orientation of the camera
    vAtL->x = 0;  vAtL->y = 0;  vAtL->z = 1;
    vUpL->x = 0;  vUpL->y = 1;  vUpL->z = 0;
    vRightL->x = 1; vRightL->y = 0; vRightL->z = 0;
    vPosL->x = 0; vPosL->y = 0; vPosL->z = 0;
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f* CPAxis::At( void )
{
    if( m_pMatrix == NULL )
        return NULL;
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( LG::Vertex3f * ) ( m_pMatrix->a + 8 );
#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation
    return ( LG::Vertex3f * ) RwMatrixGetAt( RwFrameGetLTM( m_pMatrix ) );
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f* CPAxis::Up( void )
{
    if( m_pMatrix == NULL )
        return NULL;
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( LG::Vertex3f * ) ( m_pMatrix->a );
#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation
    return ( LG::Vertex3f * ) RwMatrixGetUp( RwFrameGetLTM( m_pMatrix ) );
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f* CPAxis::Right( void )
{
    if( m_pMatrix == NULL )
        return NULL;
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( *( LG::Vertex3f * ) ( &m_pMatrix->a + 4 ) );
#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation
    return ( LG::Vertex3f * ) RwMatrixGetRight( RwFrameGetLTM( m_pMatrix ) );
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f* CPAxis::Position( void )
{
    if( m_pMatrix == NULL )
        return NULL;
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( LG::Vertex3f * ) ( m_pMatrix->pos + 12 );
#endif
#ifdef _RWKERNEL_
    //RwMatrix* pCameraMatrix = RwFrameGetMatrix( m_pMatrix );
    //Get the camera position and orientation
    return (LG::Vertex3f*)RwMatrixGetPos( RwFrameGetLTM( m_pMatrix ) );;
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPAxis::Translate( const LG::Vertex3f* pos )
{
    if( m_pMatrix == NULL )
        return;
#ifdef _RWKERNEL_
    RwFrameTranslate( m_pMatrix, ( RwV3d * ) pos, rwCOMBINEPOSTCONCAT );
#endif
    return;
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPAxis::Rotate( const LG::Vector3f* axis, float angle )
{
    if( m_pMatrix == NULL )
        return;
#ifdef _RWKERNEL_
    RwFrameRotate( m_pMatrix, ( RwV3d * ) axis, angle, rwCOMBINEPOSTCONCAT );
#endif
    return;
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPAxis::ReTranslate( const LG::Vertex3f* pos )
{
    if( m_pMatrix == NULL )
        return;
#ifdef _RWKERNEL_
    RwFrameTranslate( m_pMatrix, ( RwV3d * ) pos, rwCOMBINEREPLACE );
#endif
    return;
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPAxis::ReRotate( const LG::Vector3f* axis, float angle )
{
    if( m_pMatrix == NULL )
        return;
#ifdef _RWKERNEL_
    RwFrameRotate( m_pMatrix, ( RwV3d * ) axis, angle, rwCOMBINEREPLACE );
#endif
    return;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPAxis::PreTranslate( const LG::Vertex3f* pos )
{
    if( m_pMatrix == NULL )
        return;
#ifdef _RWKERNEL_
    RwFrameTranslate( m_pMatrix, ( RwV3d * ) pos, rwCOMBINEPRECONCAT );
#endif
    return;
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPAxis::PreRotate( const LG::Vector3f* axis, float angle )
{
    if( m_pMatrix == NULL )
        return;
#ifdef _RWKERNEL_
    RwFrameRotate( m_pMatrix, ( RwV3d * ) axis, angle, rwCOMBINEPRECONCAT );
#endif
    return;
}

/*
//=============================================================================
//  Name : 
//  Desc :
//          at = new at,  at cross up = right,  roght cross at = up
//=============================================================================
void CPAxis::Merge( _AXIS axis, VfVector3f vec )
{
#ifdef _RWKERNEL_

#endif
}
*/


//=============================================================================
//  Name : 
//  Desc :
//          at = new at,  at cross up = right,  roght cross at = up
//=============================================================================
void CPAxis::Load( Pstr fpath )
{
    CPFile pf;
    pf.open( fpath );
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    pf.read( m_pMatrix, sizeof( PA::Matrixf ), 1 );
#endif
#ifdef _RWKERNEL_
    pf.read( RwFrameGetLTM( m_pMatrix ), sizeof( RwMatrix ), 1 );
#endif
    pf.close();
}


//=============================================================================
//  Name : 
//  Desc :
//          at = new at,  at cross up = right,  roght cross at = up
//=============================================================================
void CPAxis::Save( Pstr fpath )
{
    CPFile pf;
    pf.open( fpath );
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    pf.write( m_pMatrix, sizeof( PA::Matrixf ), 1 );
#endif
#ifdef _RWKERNEL_
    pf.write( RwFrameGetLTM( m_pMatrix ), sizeof( RwMatrix ), 1 );
#endif
    pf.close();
}


/*
//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f CPAxis::LAt( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( (LG::Vertex3f*)(&m_pMatrix.at) );
#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation
    return *(LG::Vertex3f*)RwMatrixGetAt( RwFrameGetLTM( m_pMatrix ) );
#endif

}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f CPAxis::LUp( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( (LG::Vertex3f*)(&m_pMatrix.up) );
#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation
    return *(LG::Vertex3f*)RwMatrixGetUp( RwFrameGetLTM( m_pMatrix ) );
#endif
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f CPAxis::LRight( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( (LG::Vertex3f*)(&m_pMatrix.right) );
#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation
    return *(LG::Vertex3f*)RwMatrixGetRight( RwFrameGetLTM( m_pMatrix ) );
#endif

}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Vertex3f CPAxis::LPosition( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return ( (LG::Vertex3f*)(&m_pMatrix.pos) );
#endif
#ifdef _RWKERNEL_
    //Get the camera position and orientation
    return *(LG::Vertex3f*)RwMatrixGetPos( RwFrameGetLTM( m_pMatrix ) );
#endif
}
*/
