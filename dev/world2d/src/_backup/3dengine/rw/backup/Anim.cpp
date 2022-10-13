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

#include "Anim.h"

/////RsEventStatus AppEventHandler(RsEvent event, void *param){ return rsEVENTNOTPROCESSED; }

//using namespace RW;

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
PCAnim::PCAnim()
{
    m_pClump = NULL;

    //HAnim
    memset( m_pHAnimSeqs, 0, sizeof( m_pHAnimSeqs ) );

    m_nHAnimNumSeqs = 0;
    m_nHAnimCurSeqs = -1;
    m_fHAnimPerSec = 1.0f;

    m_bHAnimOn = FALSE;
    m_bHAnimOn = FALSE;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
PCAnim::~PCAnim()
{
    Destroy();
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
bool PCAnim::Create( Pcstr szFilename, Pcstr szImagePath )
{
    if( m_pClump != NULL )
        return false;

    /////RwChar    *path   = NULL;
    RwStream* stream    = NULL;

    if( szImagePath )
    {
        //Set image path
        //path = psPathnameCreate( szImagePath );
        RwImageSetPath( szFilename );
        //psPathnameDestroy( path );
    }

    //Read in BSP stream if we can
    //path = psPathnameCreate( szFilename );
    stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, szFilename );

    //psPathnameDestroy( path );

    if( stream )
    {
        if( RwStreamFindChunk( stream, rwID_CLUMP, NULL, NULL ) )
        {
            m_pClump = RpClumpStreamRead( stream );
        }
        RwStreamClose( stream, NULL );
    }

    m_pFrame = RpClumpGetFrame( m_pClump );
    return ( m_pClump != NULL );
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void PCAnim::Destroy( void )
{
    if( m_pClump == NULL )
        return;

    RpClumpDestroy( m_pClump );
    m_pClump = NULL;
    m_pFrame = NULL;

    HAnimDestroy();
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
static RwFrame* GetChileFrameHierarchyCB( RwFrame* frame, void* data )
{
    RpHAnimHierarchy** pHierarchy   = ( RpHAnimHierarchy** ) data;
    *pHierarchy = RpHAnimFrameGetHierarchy( frame );
    if( *pHierarchy == NULL )
    {
        RwFrameForAllChildren( frame, GetChileFrameHierarchyCB, data );
        return frame;
    }
    return NULL;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
static RpHAnimHierarchy* GetHierarchy( RwFrame* frame )
{
    RpHAnimHierarchy* hierarchy = NULL;
    RwFrameForAllChildren( frame, GetChileFrameHierarchyCB, &hierarchy );
    return hierarchy;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
static RpAtomic* SetupHAnimCB( RpAtomic* atomic, void* data )
{
    RpSkinAtomicSetHAnimHierarchy( atomic, ( RpHAnimHierarchy * ) data );
    return atomic;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
RwBool PCAnim::HAnimLoadANM( Pcstr szFilename )
{
    RpHAnimHierarchy* hierarchy = ( RpHAnimHierarchy* ) NULL;
    //RwChar *path = szFilename;

    if( m_nHAnimNumSeqs < MAX_HANIM_SEQS )
    {
        //path = psPathnameCreate( szFilename );
        m_pHAnimSeqs[ m_nHAnimNumSeqs ] = RpHAnimAnimationRead( szFilename );

        //psPathnameDestroy( path );
        m_bHAnimOn = FALSE;
        m_fHAnimPerSec = 1.0f;

        if( m_pHAnimSeqs[ m_nHAnimNumSeqs ] != NULL )
        {
            hierarchy = GetHierarchy( m_pFrame );
            if( !hierarchy )
            {
                return false;
            }

            //make any skinned object point at the hierarchy
            RpHAnimHierarchySetFlags( hierarchy,
                                      ( RpHAnimHierarchyFlag )
                                      ( RpHAnimHierarchyGetFlags( hierarchy ) | rpHANIMHIERARCHYUPDATELTMS | rpHANIMHIERARCHYUPDATEMODELLINGMATRICES ) );

            //setup the currect animation
            RpHAnimHierarchySetCurrentAnim( hierarchy,
                                            m_pHAnimSeqs[ m_nHAnimNumSeqs ] );

            //attach the hierarchy to the RwFrame hierarchy
            RpHAnimHierarchyAttach( hierarchy );

            RpHAnimUpdateHierarchyMatrices( hierarchy );

            m_bHAnimOn = TRUE;
            m_nHAnimCurSeqs = m_nHAnimNumSeqs;
            m_bHasHAnim = TRUE;

            m_nHAnimNumSeqs++;
        }
    }
    return m_bHAnimOn;
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void PCAnim::HAnimUpdate( Pfloat delta )
{
    if( m_bHAnimOn )
    {
        RpHAnimHierarchy* hierarchy = ( RpHAnimHierarchy* ) NULL;
        RwReal inc;

        inc = delta * m_fHAnimPerSec;
        hierarchy = GetHierarchy( m_pFrame );

        if( !hierarchy )
            return;
        RpHAnimHierarchyAddAnimTime( hierarchy, inc );
        RpHAnimUpdateHierarchyMatrices( hierarchy );
    }
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void PCAnim::HAnimDestroy( void )
{
    for( int i = 0;i < MAX_HANIM_SEQS;i++ )
    {
        if( m_pHAnimSeqs[ i ] )
        {
            RpHAnimAnimationDestroy( m_pHAnimSeqs[ i ] );
            m_pHAnimSeqs[ i ] = ( RpHAnimAnimation * ) NULL;
        }
    }
}
