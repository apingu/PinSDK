/*********************************************************************
 * Copyright (c) 2001, Mark Harmon All Rights Reserved
 *
 * Filename: CSound.cpp
 * Original Author: Mark Harmon
 ********************************************************************/

#ifdef _XBOX
	#include <xtl.h>
#endif
#if !defined(_XBOX) && defined(WIN32)
#include <windows.h>
#include <mmsystem.h>
#endif

#include <DSound.h>
#include "CSound.h"

//////////////////////////////////////////////////////////////////////////////////
// CSound Constructor
//////////////////////////////////////////////////////////////////////////////////
CSound::CSound()
{
	m_lpDS = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// CSound Destructor
//////////////////////////////////////////////////////////////////////////////////
CSound::~CSound()
{
	if(m_lpDS){
		m_lpDS->Release();
		m_lpDS=NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// CSound Create
//////////////////////////////////////////////////////////////////////////////////
BOOL CSound::Create(HWND hWnd)
{
	HRESULT hr;

	hr = DirectSoundCreate(NULL, &m_lpDS, NULL);
	if FAILED(hr){
		m_lpDS = NULL; //stupid safety precaution
		return FALSE;
	}

	LPDIRECTSOUNDBUFFER pDSBPrimary;

	hr = m_lpDS->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);
	if FAILED(hr) return FALSE;

	// Set primary buffer format to 22kHz and 16-bit output.
	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
	wfx.wFormatTag      = WAVE_FORMAT_PCM; 
	wfx.nChannels       = 2; 
	wfx.nSamplesPerSec  = 22050; 
	wfx.wBitsPerSample  = 16; 
	wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	//if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )	return FALSE;

	// Get the primary buffer 
	DSBUFFERDESC        dsbdesc;
	ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
	dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
	dsbdesc.dwBufferBytes = 0;
//	dsbdesc.lpwfxFormat   = NULL;
	dsbdesc.lpwfxFormat = &wfx;
#ifdef _XBOX
	dsbdesc.dwFlags       = 0;
#endif
#if !defined(_XBOX) && defined(WIN32)
	dsbdesc.dwFlags       = DSBCAPS_PRIMARYBUFFER;
#endif

	if( FAILED( hr = m_lpDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
	{
		int err = GetLastError();
		return FALSE;
	}

	if(pDSBPrimary) pDSBPrimary->Release();
	pDSBPrimary = NULL;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// CSound GetCaps
//////////////////////////////////////////////////////////////////////////////////
BOOL CSound::GetCaps(void)
{
	HRESULT hr;

	hr = m_lpDS->GetCaps(&m_DSCaps);
	if(hr != DS_OK) return FALSE;

	return TRUE;
}

