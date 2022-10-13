//#include <windows.h>
#include "AMovie.h"
#include <pf-stdlib.h>

//#include "..\\Base\\pf-debug.h"  

#define RELEASE(x) if (x) { (x)->Release(); (x) = NULL; }


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
static HRESULT OpenMMStream( const char* pszFileName, IDirectDraw* pDD, IMultiMediaStream** ppMMStream )
{
    HRESULT  hr;

    IAMMultiMediaStream* pAMStream;

    *ppMMStream = NULL;

    //logf("OpenMMStream.CoCreateInstance\n");

    hr = CoCreateInstance( CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
                           IID_IAMMultiMediaStream, ( void** )&pAMStream );
    if( FAILED( hr ) ) goto Exit;

    //logf("OpenMMStream.Initialize\n");

    hr = pAMStream->Initialize( STREAMTYPE_READ, 0, NULL );
    if( FAILED( hr ) ) goto Exit;

    //logf("OpenMMStream.AddMediaStream (Video)\n");

    hr = pAMStream->AddMediaStream( pDD, &MSPID_PrimaryVideo, 0, NULL );
    if( FAILED( hr ) ) goto Exit;

    //logf("OpenMMStream.AddMediaStream (Audio)\n");

    hr = pAMStream->AddMediaStream( NULL, &MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, NULL );
    if( FAILED( hr ) ) goto Exit;


    WCHAR    wPath[ _MAX_PATH ];

    MultiByteToWideChar( CP_ACP, 0, pszFileName, -1, wPath, sizeof( wPath ) / sizeof( wPath[ 0 ] ) );

    //logf("OpenMMStream.OpenFile\n", wPath);

    hr = pAMStream->OpenFile( wPath, 0 );
    if( FAILED( hr ) ) goto Exit;

    *ppMMStream = pAMStream;

    pAMStream->AddRef();

Exit:
    if( FAILED( hr ) )
        //logf("failed with HRESULT(0x%8.8X)\n", hr); 

        if( pAMStream == NULL )
        {
            //logf("Could not create a CLSID_MultiMediaStream object\n"
            //   "Check you have run regsvr32 amstream.dll or\n"
            //   "call CoInitialize(NULL) before\n");
        }

    RELEASE( pAMStream );

    return hr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
CActiveMovie::CActiveMovie()
{
    m_pDD = NULL;
    m_pDDPrimarySurface = NULL;

    m_pMMStream = NULL;

    m_pPrimaryVidStream = NULL;
    m_pDDStream = NULL;
    m_pSample = NULL;
    m_pSurface = NULL;

    m_pNext = NULL;
    m_pAttach = NULL;

    m_destRect.left = 0;
    m_destRect.top = 0;
    m_destRect.right = 800;
    m_destRect.bottom = 600;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
CActiveMovie::~CActiveMovie()
{
    Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CActiveMovie::Release( void )
{
    RELEASE( m_pMMStream );

    RELEASE( m_pPrimaryVidStream );
    RELEASE( m_pDDStream );
    RELEASE( m_pSample );
    RELEASE( m_pSurface );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CActiveMovie::SetDirectDraw( IDirectDraw* pDD, IDirectDrawSurface* pPrimarySurface )
{
    m_pDD = pDD;
    m_pDDPrimarySurface = pPrimarySurface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CActiveMovie::SetDestRect( long l, long t, long r, long b )
{
    m_destRect.left = l;
    m_destRect.top = t;
    m_destRect.right = r;
    m_destRect.bottom = b;

    //  RECT destRect = { 0,   0, 640, 480 };    // 4:3
    //  RECT destrect = { 0, 105, 640, 270 };    // 16:9
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CActiveMovie::SetAttachMovie( CActiveMovie* movie )
{
    m_pAttach = movie;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CActiveMovie::SetNextMovie( CActiveMovie* movie )
{
    m_pNext = movie;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL CActiveMovie::LoadFile( const char* filename )
{
    HRESULT  hr;

    m_pNext = NULL;
    m_pAttach = NULL;

    if( m_pDD == NULL )
        return FALSE;

    //logf("LoadFile.OpenMMStream %s\n", filename);

    hr = OpenMMStream( filename, m_pDD, &m_pMMStream );
    if( FAILED( hr ) ) goto Exit;

    //logf("LoadFile.GetMediaStream\n");

    hr = m_pMMStream->GetMediaStream( MSPID_PrimaryVideo, &m_pPrimaryVidStream );
    if( FAILED( hr ) ) goto Exit;

    //logf("LoadFile.QueryInterface\n");

    hr = m_pPrimaryVidStream->QueryInterface( IID_IDirectDrawMediaStream, ( void** )&m_pDDStream );
    if( FAILED( hr ) ) goto Exit;

    //logf("LoadFile.CreateSample\n");

    hr = m_pDDStream->CreateSample( NULL, NULL, 0, &m_pSample );
    if( FAILED( hr ) ) goto Exit;

    //logf("LoadFile.GetSurface\n");

    hr = m_pSample->GetSurface( &m_pSurface, &m_srcRect );
    if( FAILED( hr ) ) goto Exit;

    //logf("LoadFile.SetState\n");

    hr = m_pMMStream->SetState( STREAMSTATE_STOP ); // STREAMSTATE_RUN);
    if( FAILED( hr ) ) goto Exit;

Exit:
    if( FAILED( hr ) )
    {
        //logf("failed with HRESULT(0x%8.8X)\n", hr); 
        return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL CActiveMovie::Play( void )
{
    if( m_pMMStream == NULL )
        return FALSE;

    HRESULT  hr = m_pMMStream->SetState( STREAMSTATE_RUN );

    if( m_pAttach )
        m_pAttach->Play();

    if( FAILED( hr ) )
    {
        //logf("failed with HRESULT(0x%8.8X)\n", hr); 
        return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL CActiveMovie::Stop( void )
{
    if( m_pMMStream == NULL )
        return TRUE;

    HRESULT  hr = m_pMMStream->SetState( STREAMSTATE_STOP );

    if( m_pAttach )
        m_pAttach->Stop();

    if( FAILED( hr ) )
    {
        //logf("failed with HRESULT(0x%8.8X)\n", hr); 
        return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL CActiveMovie::RenderFrameToPrimary( void )
{
    return RenderFrameToSurface( m_pDDPrimarySurface );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL CActiveMovie::RenderFrameToSurface( IDirectDrawSurface* pSurface )
{
    if( m_pSample == NULL )
        return FALSE;

    HRESULT  hr;

    hr = m_pSample->Update( SSUPDATE_CONTINUOUS, NULL, NULL, 0 );

    if( hr == MS_S_ENDOFSTREAM )
    {
        if( m_pAttach )
        {
            m_pAttach->Stop();
            m_pAttach->Release();
            m_pAttach = NULL;
        }
        Release();

        return FALSE;
    }

    if( m_pAttach )
    {
        if( m_pAttach->RenderFrameToSurface( m_pSurface ) == FALSE )
        {
            m_pAttach = m_pAttach->GetNextMovie();

            if( m_pAttach )
                m_pAttach->Play();
        }
    }

    // blit surface to surface
    pSurface->Blt( &m_destRect, m_pSurface, &m_srcRect, DDBLT_WAIT, NULL );

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
DDSURFACEDESC pDesc;
BOOL CActiveMovie::RenderFrameToBuff( BYTE* Buf, int tell, int length, POINT point )
{
    if( m_pSample == NULL )
        return FALSE;

    /*
    if (m_pSample->Update(0, NULL, NULL, 0) != S_OK)
    {
        //Release();
        //return FALSE;
    }
    */

    HRESULT hr = m_pSample->Update( SSUPDATE_CONTINUOUS, NULL, NULL, 0 );
    if( hr == MS_S_ENDOFSTREAM )
    {
        if( m_pAttach )
        {
            m_pAttach->Stop();
            m_pAttach->Release();
            m_pAttach = NULL;
        }
        Release();

        return FALSE;
    }
    pDesc.dwSize = sizeof( pDesc );



    hr = m_pSample->GetSurface( &m_pSurface, &m_srcRect );
    if( FAILED( hr ) ) return FALSE;//goto Exit;

    // blit m_pSurface to memory
    if( FAILED( m_pSurface->Lock( 0, &pDesc, DDLOCK_WAIT, NULL ) ) )
    {
        MessageBoxA( NULL, "Surface Lock failure", "Lock Failed", MB_OK );
        exit( false );
    }

    //BYTE *pDest = (BYTE *)pDesc.lpSurface + (point.y * pDesc.lPitch + (point.x << 1));
    //BYTE* pSrc = Buf;

    BYTE* pDest = Buf;
    BYTE* pSrc = ( BYTE* )pDesc.lpSurface + ( point.y * pDesc.lPitch + ( point.x << 1 ) );
    //int line = length;
    int line = ( m_srcRect.right ) * 2;

    //for(register int  row = 0; row < tell ; row++)
    for( register int row = 0; row < m_srcRect.bottom; row++ )
    {
        memcpy( pDest, pSrc, line );
        //__asm
        //{
        //    MOV esi, pSrc
        //    MOV edi, pDest
        //    MOV ecx, line
        //    MOV al , cl
        //    SHR ecx, 2
        //    REP MOVSD
        //    MOV cl , al
        //    AND cl , 3
        //    REP MOVSB
        //}
        //pDest += pDesc.lPitch; // 指向下一列 pixel 起始位址
        //pSrc = pSrc + length;
        pSrc += pDesc.lPitch; // 指向下一列 pixel 起始位址
        pDest = pDest + length;
    }
    m_pSurface->Unlock( 0 ); // 處理完 surface 之後 unlock

    return true;
}



BOOL CActiveMovie::ScaleRenderFrameToBuff( BYTE* Buf, int tell, int length, POINT point )
{
    if( m_pSample == NULL )
        return FALSE;

    /*
    if (m_pSample->Update(0, NULL, NULL, 0) != S_OK)
    {
        //Release();
        //return FALSE;
    }
    */

    HRESULT hr = m_pSample->Update( SSUPDATE_CONTINUOUS, NULL, NULL, 0 );
    if( hr == MS_S_ENDOFSTREAM )
    {
        if( m_pAttach )
        {
            m_pAttach->Stop();
            m_pAttach->Release();
            m_pAttach = NULL;
        }
        Release();

        return FALSE;
    }
    pDesc.dwSize = sizeof( pDesc );
    pDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    pDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    pDesc.dwWidth = length;
    pDesc.dwHeight = tell;
    IDirectDrawSurface* pScaleSurf;
    RECT ScaleRect = { 0, 0, length / 2, tell };
    m_pDD->CreateSurface( &pDesc, &pScaleSurf, NULL );
    pScaleSurf->Blt( &ScaleRect, m_pSurface, &m_srcRect, DDBLT_WAIT, NULL );


    hr = m_pSample->GetSurface( &m_pSurface, &m_srcRect );
    if( FAILED( hr ) ) return FALSE;//goto Exit;

    // blit m_pSurface to memory
    if( FAILED( pScaleSurf->Lock( 0, &pDesc, DDLOCK_WAIT, NULL ) ) )
    {
        MessageBoxA( NULL, "Surface Lock failure", "Lock Failed", MB_OK );
        exit( false );
    }

    BYTE* pDest = Buf;
    BYTE* pSrc = ( BYTE* )pDesc.lpSurface + ( point.y * pDesc.lPitch + ( point.x << 1 ) );
    int line = length;

    //for(register int  row = 0; row < tell ; row++)
    for( register int row = 0; row < tell; row++ )
    {
        //memcpy(pDest, pSrc, line);
        pfstd::memcpy( pDest, pSrc, line );

        //__asm
        //{
        //    MOV esi, pSrc
        //    MOV edi, pDest
        //    MOV ecx, line
        //    MOV al , cl
        //    SHR ecx, 2
        //    REP MOVSD
        //    MOV cl , al
        //    AND cl , 3
        //    REP MOVSB
        //}
        pSrc += pDesc.lPitch; // 指向下一列 pixel 起始位址
        pDest = pDest + length;
    }
    pScaleSurf->Unlock( 0 ); // 處理完 surface 之後 unlock

    return true;
}





/* exam

    CoInitialize(NULL);

    // Set active movie
    AMovie.SetDirectDraw((IDirectDraw *) lpDD, VScreen.GetFrontDDS());


    // Load movie
    makefilename(str, "Movies\\Open%02d.mv", episode);

    if (AMovie.LoadFile(str) == FALSE)
        return FALSE;

    AMovie.SetDestRect(0, 0, 640, 480);




    AMovie.Play();




    if (LastWndMsg == WM_LBUTTONDOWN || LastWndMsg == WM_KEYDOWN)
    {
        // Allow user to terminate episode #0
        AMovie.Stop();
        AMovie.Release();
    }
    else
    {
        // Render movie frame to primary surface
        if (AMovie.RenderFrameToPrimary() == FALSE)
        {
            // This movie has been closed (stopped and released)!
        }

        return;
    }

*/


/*

//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1997 - 1998  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

#include <windows.h>
#include <stdio.h>
#include "ddraw.h"
#include "mmstream.h"
#include "amstream.h"
#include "ddstream.h"


#define RELEASE(x) if (x) (x)->Release();
#define CHECK_ERROR(x)     \
   if (FAILED(hr = (x))) { \
       printf(#x "  failed with HRESULT(0x%8.8X)\n", hr); \
       goto Exit;          \
   }


HRESULT OpenMMStream(const char*  pszFileName, IDirectDraw *pDD, IMultiMediaStream **ppMMStream)
{
    *ppMMStream = NULL;
    IAMMultiMediaStream *pAMStream;
    HRESULT hr;

    CHECK_ERROR(CoCreateInstance(CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
                 IID_IAMMultiMediaStream, (void **)&pAMStream));
    CHECK_ERROR(pAMStream->Initialize(STREAMTYPE_READ, 0, NULL));
    CHECK_ERROR(pAMStream->AddMediaStream(pDD, &MSPID_PrimaryVideo, 0, NULL));
    CHECK_ERROR(pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, NULL));

    WCHAR       wPath[_MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, wPath, sizeof(wPath)/sizeof(wPath[0]));

    CHECK_ERROR(pAMStream->OpenFile(wPath, 0));

    *ppMMStream = pAMStream;
    pAMStream->AddRef();

Exit:
    if (pAMStream == NULL) {
    printf("Could not create a CLSID_MultiMediaStream object\n"
           "Check you have run regsvr32 amstream.dll\n");
    }
    RELEASE(pAMStream);
    return hr;
}




HRESULT RenderStreamToSurface(IDirectDraw *pDD, IDirectDrawSurface *pPrimary,
                  IMultiMediaStream *pMMStream)
{

    HRESULT hr;
    IMediaStream *pPrimaryVidStream = NULL;
    IDirectDrawMediaStream *pDDStream = NULL;
    IDirectDrawSurface *pSurface = NULL;
    IDirectDrawStreamSample *pSample = NULL;

    RECT rect;

    CHECK_ERROR(pMMStream->GetMediaStream(MSPID_PrimaryVideo, &pPrimaryVidStream));
    CHECK_ERROR(pPrimaryVidStream->QueryInterface(IID_IDirectDrawMediaStream, (void **)&pDDStream));

    CHECK_ERROR(pDDStream->CreateSample(NULL, NULL, 0, &pSample));
    CHECK_ERROR(pSample->GetSurface(&pSurface, &rect));

    CHECK_ERROR(pMMStream->SetState(STREAMSTATE_RUN));

    while (true) {
    if (pSample->Update(0, NULL, NULL, 0) != S_OK) {
        break;
    }
    pPrimary->Blt(&rect, pSurface, &rect, DDBLT_WAIT, NULL);
    }

Exit:
    RELEASE(pPrimaryVidStream);
    RELEASE(pDDStream);
    RELEASE(pSample);
    RELEASE(pSurface);

    return hr;
}


int _CRTAPI1
main(
    int argc,
    char* argv[]
   )
{
    if (argc < 2) {
    printf("Usage : showstrm movie.ext\n");
    exit(0);
    }
    CoInitialize(NULL);
    IDirectDraw *pDD;

    HRESULT hr = DirectDrawCreate(NULL, &pDD, NULL);
    if (SUCCEEDED(hr)) {
    DDSURFACEDESC ddsd;
    IDirectDrawSurface *pPrimarySurface;

    pDD->SetCooperativeLevel(GetDesktopWindow(), DDSCL_NORMAL);

    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    hr = pDD->CreateSurface(&ddsd, &pPrimarySurface, NULL);
    if (SUCCEEDED(hr)) {
        IMultiMediaStream *pMMStream;
        hr = OpenMMStream(argv[1], pDD, &pMMStream);
        if (SUCCEEDED(hr)) {
        RenderStreamToSurface(pDD, pPrimarySurface, pMMStream);
        pMMStream->Release();
        }
        pPrimarySurface->Release();
    }
    pDD->Release();
    } else {
    printf("Could not open DirectDraw - check it is installed\n");
    }
    CoUninitialize();
    return 0;
}
*/