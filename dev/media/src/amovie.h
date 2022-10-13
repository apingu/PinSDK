/*==========================================================================================
 *
 *
 *
 *
 *                                                Use attention +amstrmid.lib
 *
 *
 *========================================================================================*/

#ifndef _Amovie_H_
#define _Amovie_H_

#include <ddraw.h>
#include <mmstream.h>
#include <amstream.h>
#include <ddstream.h>

#pragma comment (lib, "amstrmid.lib")     /* link with RW lib */ 

class CActiveMovie
{

private:

    IDirectDraw*             m_pDD;
    IDirectDrawSurface*      m_pDDPrimarySurface;

    IMultiMediaStream*       m_pMMStream;

    IMediaStream*            m_pPrimaryVidStream;
    IDirectDrawMediaStream*  m_pDDStream;
    IDirectDrawStreamSample* m_pSample;
    IDirectDrawSurface*      m_pSurface;

    RECT                     m_srcRect;
    RECT                     m_destRect;

    CActiveMovie*            m_pNext;
    CActiveMovie*            m_pAttach;


public:
    CActiveMovie();
    ~CActiveMovie();

    void Release( void );

    void SetDirectDraw( IDirectDraw* pDD, IDirectDrawSurface* pPrimarySurface );
    void SetDestRect( long l, long t, long r, long b );

    void SetNextMovie( CActiveMovie* movie );
    void SetAttachMovie( CActiveMovie* movie );

    CActiveMovie* GetNextMovie( void ) const { return m_pNext; }

    BOOL LoadFile( const char* filename );
    BOOL Play( void );
    BOOL Stop( void );

    BOOL RenderFrameToPrimary( void );
    BOOL RenderFrameToSurface( IDirectDrawSurface* pSurface );
    BOOL RenderFrameToBuff( BYTE* Buf, int tell, int length, POINT point );
    BOOL ScaleRenderFrameToBuff( BYTE* Buf, int tell, int length, POINT point );
};

#endif
