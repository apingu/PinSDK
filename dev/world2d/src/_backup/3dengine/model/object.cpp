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
#include "Object.h"
#include <Debug.h>

#ifdef _RWKERNEL_
#include <RwFile.h>
#endif

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
CPObject::CPObject()
{
    texture = NULL;
#ifdef _RWKERNEL_
    m_pAtomic = 0;
#endif

#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    m_vertex.clear();
#endif
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
CPObject::~CPObject()
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    m_vertex.clear();
#endif

#ifdef _RWKERNEL_
    if( m_pAtomic != 0 )
    {
        //RpAtomicDestroy( m_pAtomic );
        m_pAtomic = NULL;
		m_pMatrix = NULL;
    }
#endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CPObject::Create( void )
{



}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
Pint CPObject::Load( Pcstr pathname )
{
#ifdef _RWKERNEL_
    RwFileFunctions* fileFunctions;
    RwFileFunctions RWFileInterface;

    fileFunctions = RwOsGetFileInterface();
    RWFileInterface = *fileFunctions;

    fileFunctions->rwfopen = RWFile::_PakFOpen;
    fileFunctions->rwfclose = RWFile::_PakFClose;
    fileFunctions->rwfread = RWFile::_PakFRead;
    fileFunctions->rwfgets = RWFile::_PakFGets;
    fileFunctions->rwfeof = RWFile::_PakFEof;
    fileFunctions->rwfseek = RWFile::_PakFSeek;

    RwStream* stream;

    stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, pathname );
    if( stream )
    {
        if( RwStreamFindChunk( stream, rwID_ATOMIC, NULL, NULL ) )
            m_pAtomic = RpAtomicStreamRead( stream );

        RwStreamClose( stream, NULL );
    }

    RwError err;
    RwErrorGet( &err );

    if( err.errorCode == 22 )
    {
        DebugMessage( "failed to load model textures", "VFRW" );
        return VFE_TEXTURELOADFAIL;
    }

    *fileFunctions = RWFileInterface;
    return 1;
#endif
    return 0;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::BBoxf* CPObject::LocalBoundBox( LG::BBoxf* bbox )
{
#ifdef _RWKERNEL_

    //RwBBox rwBbox;

    RpGeometry* geometry= RpAtomicGetGeometry( m_pAtomic );  // Get the geometry we need

    if( geometry )
    {
        RpMorphTarget* pMorphTarget = RpGeometryGetMorphTarget( geometry, 0 ); 

        if( pMorphTarget )
        {
            //Calculate the bounding box of the current atomic's geometry
            RwBBoxCalculate( ( RwBBox * ) ( bbox ),
                             RpMorphTargetGetVertices( pMorphTarget ),
                             RpGeometryGetNumVertices( geometry ) );
        }
    }

    //(*bbox) = rwbbox;
#endif

    return bbox;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
LG::Spheref* CPObject::LocalBoundSphere( LG::Spheref* bsphere )
{
#ifdef _RWKERNEL_

    RpGeometry* geometry= RpAtomicGetGeometry( m_pAtomic );  // Get the geometry we need

    if( geometry )
    {
        RpMorphTarget* pMorphTarget = RpGeometryGetMorphTarget( geometry, 0 );

        if( pMorphTarget )
        {
            //Calculate the bounding box of the current atomic's geometry
            //RwBBoxCalculate(bbox, RpMorphTargetGetVertices(pMorphTarget),
            RpGeometryGetNumVertices( geometry );
            RpMorphTargetCalcBoundingSphere( pMorphTarget,
                                             ( RwSphere * ) bsphere );
        }
    }

    //RwMatrix* transform = RwFrameGetLTM(RpAtomicGetFrame( m_pAtomic ) );
    //RwV3dTransformPoints(&bsphere->center, &bsphere->center, 1, transform);

#endif
    return bsphere;
}









/////////////////////////////////////////////////////////////////
//  RenderWare function
//
//
//
//
//
//
//
//
//
//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
#ifdef _RWKERNEL_
RpAtomic& CPObject::operator=( RpAtomic* object )
{
    m_pAtomic = object;
    m_pMatrix = RpAtomicGetFrame( m_pAtomic );
    return *m_pAtomic;
}

CPObject::CPObject( RpAtomic* object )
{
	operator=( object );
}

#endif


