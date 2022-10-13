//
//
//    3D Role animation 
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
#include "Anim.h"
#include "debug.h"


#ifdef _RWKERNEL_
#include <RwFile.h>
#endif

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
CPAnim::CPAnim()
{
    //m_pClump = NULL;

    //HAnim
    //memset( m_pHAnimSeqs, 0, sizeof( m_pHAnimSeqs ) );
    //m_pHAnimSeqs.clear();

    //m_nHAnimNumSeqs = 0;
    //m_nHAnimCurSeqs = -1;
    m_persec = 1.0f;
    m_duration = 1.0f;
    m_currenttime = 0.0f;
    m_pHAnim = NULL;


    //m_bHAnimOn = FALSE;
    //m_bHAnimOn = FALSE;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
CPAnim::~CPAnim()
{
    Destroy();
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void CPAnim::Destroy( void )
{
#ifdef _RWKERNEL
    /*
    for( int i = 0; i < m_pHAnimSeqs.size(); i++ )
    {
        if( m_pHAnimSeqs[i] )
        {
            RpHAnimAnimationDestroy( m_pHAnimSeqs[i] );
            //m_pHAnimSeqs[i] = ( RpHAnimAnimation *)NULL;
        }
    }
    m_pHAnimSeqs.clear();
    */
    RpHAnimAnimationDestroy( m_pHAnim );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//  RW function
#ifdef _RWKERNEL_
//=============================================================================
//  Name :
//  Desc :
//=============================================================================
static RwFrame* GetChildFrameHierarchy( RwFrame* frame, void* data )
{
    RpHAnimHierarchy** pHierarchy   = ( RpHAnimHierarchy** ) data;
    *pHierarchy = RpHAnimFrameGetHierarchy( frame );
    if( *pHierarchy == NULL )
    {
        RwFrameForAllChildren( frame, GetChildFrameHierarchy, data );
        return frame;
    }
    return NULL;
}

static RpHAnimHierarchy* GetHierarchy( RwFrame* frame )
{
    RpHAnimHierarchy* hierarchy = NULL;

    //Return the hierarchy for this model...
    RwFrameForAllChildren( frame, GetChildFrameHierarchy, &hierarchy );
    return hierarchy;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
static RpAtomic* SetHierarchyForSkinAtomic( RpAtomic* atomic, void* data )
{
#if (defined(RPSKIN310_H))
    RpSkinSetHAnimHierarchy( RpSkinAtomicGetSkin( atomic ),
                             ( RpHAnimHierarchy * ) data );
#else /* (defined(RPSKIN310_H)) */
    RpSkinAtomicSetHAnimHierarchy( atomic, ( RpHAnimHierarchy * ) data );
#endif /* (defined(RPSKIN310_H)) */
    return atomic;
}
#endif


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
VfError CPAnim::Load( Pcstr szFilename )
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

    if( ( m_pHAnim = RtAnimAnimationRead( szFilename ) ) == NULL )
        return VFE_CREATEANIMFAIL;

    *fileFunctions = RWFileInterface;
    m_duration = m_pHAnim->duration; // the sum of animation
#endif

    return VFE_SUCCESS;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pbool CPAnim::Transform( CPModel* model )
{
    //if( curanim >= m_pHAnimSeqs.size() )
    //  return FALSE;

    //m_nHAnimCurSeqs = curanim;
    if( model == NULL )
        return FALSE;

#ifdef _RWKERNEL_
    if( ( model->m_pMatrix != NULL ) && ( model->m_pClump != NULL ) )
    {
        RpHAnimHierarchy* hierarchy = GetHierarchy( model->m_pMatrix );
        //**necess
        RpClumpForAllAtomics( model->m_pClump,
                              SetHierarchyForSkinAtomic,
                              ( void * ) hierarchy );

        //RpHAnimHierarchySetCurrentAnim( hierarchy, m_pHAnimSeqs[m_nHAnimCurSeqs] );
        RpHAnimHierarchySetCurrentAnim( hierarchy, m_pHAnim );
        RpHAnimHierarchySetCurrentAnimTime( hierarchy, 0.0f );
        m_currenttime = 0.0f;

        return TRUE;
    }
#endif
    return FALSE;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pvoid CPAnim::BeginBlend( CPModel* model )
{
#ifdef _RWKERNEL_
    /*
     * For the OutHierarchy the app will use the standard key frame callbacks...
     */
    RpHAnimHierarchy* hierarchy = NULL;
    hierarchy = GetHierarchy( model->m_pMatrix );

    m_blend[ 0 ] = RpHAnimHierarchyCreateFromHierarchy( hierarchy,
                                                        ( RpHAnimHierarchyFlag )
                                                        hierarchy->flags,
                                                        hierarchy->currentAnim->maxInterpKeyFrameSize );

    m_blend[ 1 ] = RpHAnimHierarchyCreateFromHierarchy( hierarchy,
                                                        ( RpHAnimHierarchyFlag )
                                                        hierarchy->flags,
                                                        hierarchy->currentAnim->maxInterpKeyFrameSize );


    //For the OutHierarchy the app will use the standard key frame callbacks...
    RpHAnimHierarchySetKeyFrameCallBacks( m_blend[ 0 ],
                                          rpHANIMSTDKEYFRAMETYPEID );

    // ...also give this hierarchy a parent frame. This requires the app to
    // set the parentFrame to NULL before a call to RpHAnimHierarchyDestroy...
    m_blend[ 0 ]->parentFrame = hierarchy->parentFrame;

#endif
}



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pfloat CPAnim::Blend( CPModel* model,
                      Pfloat delta,
                      CPModel* pre,
                      CPModel* next )
{
#ifdef _RWKERNEL_
    RpHAnimHierarchy* OutHierarchy  = GetHierarchy( model->m_pMatrix );


    //RpHAnimHierarchySetCurrentAnim( InHierarchy1, oldanim->GetAnim() );
    //RpHAnimHierarchySetCurrentAnim( InHierarchy2, oldanim->GetAnim() );

    //RpClumpForAllAtomics( model->m_pClump, SetHierarchyForSkinAtomic, (void *)OutHierarchy );

    RpClumpForAllAtomics( model->m_pClump,
                          SetHierarchyForSkinAtomic,
                          ( void * ) OutHierarchy );

    //m_currenttime += ( delta* m_persec );
    //Pfloat alpha = 1.0f - m_duration;
    RpHAnimHierarchyBlend( OutHierarchy, m_blend[ 0 ], m_blend[ 1 ], delta );


    m_duration -= delta;
    m_currenttime += delta;

    //RpHAnimHierarchyAddAnimTime( hierarchy, ( delta* m_persec ) );
    RpHAnimUpdateHierarchyMatrices( OutHierarchy );
    //m_currenttime = OutHierarchy->currentAnim->currentTime;
    if( m_duration <= 0.0f )
        m_currenttime = 0;
#endif

    return m_currenttime;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pfloat CPAnim::Update( CPModel* model, Pfloat delta )
{
    if( model == NULL )
        return 0.0f;

#ifdef _RWKERNEL_

    RpHAnimHierarchy* hierarchy = GetHierarchy( model->m_pMatrix );

    //static RpHAnimHierarchy *inHierarchy1 = NULL;
    //RwReal           inc;

    //inc = delta* m_fHAnimPerSec;
    //hierarchy = GetHierarchy( model->m_pMatrix );

    if( !hierarchy )
        return 0;

    //m_fHAnimSumSec += ( delta* m_fHAnimPerSec );
    RpHAnimHierarchyAddAnimTime( hierarchy, ( delta * m_persec ) );
    RpHAnimUpdateHierarchyMatrices( hierarchy );

    //get now animation current time
    m_currenttime = hierarchy->currentAnim->currentTime;
#endif
    return m_currenttime;
}
