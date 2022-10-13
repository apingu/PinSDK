/*********************************************************************
 * Copyright (c) 2001, Mark Harmon All Rights Reserved
 *
 * Filename: CSoundBuffer.cpp
 * Original Author: Mark Harmon
 *
 * Special version for renderware and pak file support.
 ********************************************************************/

#ifdef _XBOX
	#include <xtl.h>
	#define DSBPAN_LEFT                 -10000
	#define DSBPAN_CENTER               0
	#define DSBPAN_RIGHT                10000
#endif
#if !defined(_XBOX) && defined(WIN32)
#include <windows.h>
#include <mmsystem.h>
#endif

#include <rwcore.h>
#include <ZGlobal.h>

#include "../skeleton.h"
#include "CSound.h"

#ifndef RELEASE
#define RELEASE(x) if(x!=NULL) { x->Release(); x = NULL; }
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
CSoundBuffer::CSoundBuffer()
{
	isLOADED = 0;
	m_pDS = NULL;
	Buffers = NULL;
	m_Current = 0;
	numBuffers = 0;
}

//////////////////////////////////////////////////////////////////////////////////

CSoundBuffer::~CSoundBuffer()
{
	FreeBuffers();
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void CSoundBuffer::FreeBuffers()
{
	for(int i = 0; i < numBuffers; i++){
		RELEASE(Buffers[i]);
	}
	if(Buffers) delete [] Buffers;

	m_pDS = NULL;
	isLOADED = 0;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

BOOL CSoundBuffer::CreateSoundBuffer(DWORD dwBufSize,DWORD dwFreq,DWORD dwSampleBits,
												 DWORD dwBlkAlign,BOOL bStereo)
{
	WAVEFORMATEX pcmwf;
	DSBUFFERDESC dsbdesc;

	// Set up wave format structure.
	memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
	pcmwf.wFormatTag		= WAVE_FORMAT_PCM;      
	pcmwf.nChannels      = bStereo ? 2 : 1;
	pcmwf.nSamplesPerSec = dwFreq;
	pcmwf.nBlockAlign    = (WORD)dwBlkAlign;
	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
	pcmwf.wBitsPerSample     = (WORD)dwSampleBits;

	// Set up DSBUFFERDESC structure.
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize		= sizeof(DSBUFFERDESC);
	dsbdesc.dwBufferBytes   = dwBufSize; 
	dsbdesc.lpwfxFormat     = (LPWAVEFORMATEX)&pcmwf;

#ifdef _XBOX
	dsbdesc.dwFlags   =	0;
#endif
#if !defined(_XBOX) && defined(WIN32)
	dsbdesc.dwFlags   =	DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME
								| DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2;
#endif

	LPDIRECTSOUND pds = m_pDS->GetDirectSound();
  HRESULT hr = pds->CreateSoundBuffer(&dsbdesc, &Buffers[m_Current], NULL);
#ifndef _XBOX
	if(hr != DS_OK)
  {
    switch(hr)
    {
    case DSERR_ALLOCATED:
      MessageBox(NULL, _TEXT("DSERR_ALLOCATED"), _TEXT("DSOUND ERR"), MB_OK);
      break;
/*    case DSERR_BUFFERTOOSMALL:
      MessageBox(NULL, _TEXT("DSERR_BUFFERTOOSMALL"),
        _TEXT("DSOUND ERR"),MB_OK);
      break; */
    case DSERR_BADFORMAT:
      MessageBox(NULL, _TEXT("DSERR_BADFORMAT"),
        _TEXT("DSOUND ERR"), MB_OK);
      break;
/*    case DSERR_DS8_REQUIRED:
      MessageBox(NULL, _TEXT("DSERR_DS8_REQUIRED"),
        _TEXT("DSOUND ERR"), MB_OK);
      break; */
    case DSERR_INVALIDPARAM:
      MessageBox(NULL, _TEXT("DSERR_INVALIDPARAM"),
        _TEXT("DSOUND ERR"), MB_OK);
      break;
    case DSERR_UNSUPPORTED:
      MessageBox(NULL, _TEXT("DSERR_UNSUPPORTED"),
        _TEXT("DSOUND ERR"), MB_OK);
      break;
    case DSERR_OUTOFMEMORY:
      MessageBox(NULL, _TEXT("DSERR_OUTOFMEMORY"),
        _TEXT("DSOUND ERR"), MB_OK);
      break;
    }
    return FALSE;
  }
#endif

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
BOOL CSoundBuffer::isPlaying(void)
{
	DWORD Status;
	HRESULT rval;
	BYTE	i;

	if(Buffers == NULL || m_pDS == NULL) return FALSE;

	for(i=0; i < numBuffers; i++){
		rval = Buffers[i]->GetStatus(&Status);
		if(rval < 0) Status = 0;
		if((Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING) return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////

LPDIRECTSOUNDBUFFER CSoundBuffer::GetFreeBuffer(void)
{
	DWORD Status;
	WORD bufNo;
	HRESULT rval;

	if(Buffers == NULL) return NULL;

	for(bufNo = 0; bufNo < numBuffers; bufNo++){
		
		rval = Buffers[bufNo]->GetStatus(&Status);
		if(rval < 0) Status = 0;
		if((Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING) continue;
		
		break;
	}

	if(bufNo == numBuffers){
		bufNo = 0;
		Buffers[bufNo]->Stop();
		Buffers[bufNo]->SetCurrentPosition(0);
	}

	return Buffers[bufNo];
}

//////////////////////////////////////////////////////////////////////////////////
BOOL CSoundBuffer::LoadFromWav(CSound* pDS, TCHAR *fname, HANDLE hFile, int Num)
{
	DWORD dwFilePos;
	HRESULT hr = NULL;
	int i;

	isLOADED = 0;
	m_Current=0;

	if(fname == NULL && hFile == NULL) return FALSE;

	// check to see if there is a valid direct sound object.
	if(pDS->GetDirectSound() == NULL){
		m_pDS = NULL;
		return NULL;
	}else m_pDS = pDS;

	numBuffers = Num;


  if(fname)
  {
		hFile = CreateFile(fname, GENERIC_READ,FILE_SHARE_READ,
					NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL);
		if(hFile==INVALID_HANDLE_VALUE)
    {
			TCHAR szMsg[256];
			_stprintf(szMsg, _T("CreateFile failed in CSoundBuffer::LoadFromWav. %s not loaded.\n"), fname);
			OutputDebugString(szMsg);
			return FALSE;
		}
	}

	Buffers = new LPDIRECTSOUNDBUFFER[Num];
	if(Buffers == NULL) goto FAILURE;

	dwFilePos = SetFilePointer(hFile,0,0,FILE_CURRENT);

	if(!LoadFromHandle(hFile)) goto FAILURE;

	for(i = 1; i < numBuffers; i++)	{
		LPDIRECTSOUND pds = m_pDS->GetDirectSound();
#ifndef _XBOX
		hr = pds->DuplicateSoundBuffer(Buffers[0], &Buffers[i]);
		if FAILED(hr){
			SetFilePointer(hFile,dwFilePos,0,FILE_BEGIN);
			m_Current=i;
			if(!LoadFromHandle(hFile)) goto FAILURE;
		}
#endif
	}

	if(fname) CloseHandle(hFile);
	return TRUE;
FAILURE:
	if(fname && hFile) CloseHandle(hFile);
	FreeBuffers();
	return FALSE;
}


/*****************************************************************************/
// big giant ugly function!
BOOL CSoundBuffer::LoadFromPakWav(CSound* pDS, TCHAR *fname, int numBuf)
{
  int i=0;
  RwFileFunctions *fileFunctions;
  RwChar *path = (RwChar *)NULL;
  void *file = NULL;

  fileFunctions = RwOsGetFileInterface();
  if(!fileFunctions ) return FALSE;

	// check to see if there is a valid direct sound object.
	if(pDS->GetDirectSound() == NULL)
  {
		m_pDS = NULL;
		return NULL;
	}
  else m_pDS = pDS;

  path = RsPathnameCreate(fname);
	isLOADED = 0;
	m_Current=0;
	numBuffers = numBuf;

  // get the file size, why no file size functions in renderware???
  file = fileFunctions->rwfopen(path, _T("rb"));
  if(!file)
  {
		TCHAR szMsg[256];
		_stprintf(szMsg, _T("CSoundBuffer::LoadFromPakWav. %s not loaded.\n"), fname);
		OutputDebugString(szMsg);
    RsPathnameDestroy(path);
    return FALSE;
  }

  RsPathnameDestroy(path);

	Buffers = new LPDIRECTSOUNDBUFFER[numBuf];
	if(Buffers == NULL) goto FAILURE;

  if(!LoadFromPakHandle(file)) goto FAILURE;


	for(i = 1; i < numBuffers; i++)	
  {
		LPDIRECTSOUND pds = m_pDS->GetDirectSound();
#ifndef _XBOX
		if(FAILED(pds->DuplicateSoundBuffer(Buffers[0], &Buffers[i])))
    {
			m_Current=i;
			if(!LoadFromPakHandle(file)) goto FAILURE;
		}
#endif 
	}
 
  fileFunctions->rwfclose(file);
	return TRUE;

FAILURE:
	if(file) fileFunctions->rwfclose(file);
	FreeBuffers();
	return FALSE;
}

/*****************************************************************************/
BOOL CSoundBuffer::LoadFromPakHandle(void* file)
{
	DWORD dwSize;
	BOOL bStereo;
	Waveheader Wavhdr;
  RwFileFunctions *fileFunctions;

  fileFunctions = RwOsGetFileInterface();
  if(!fileFunctions ) return FALSE;

  // load wav header
  fileFunctions->rwfseek(file, 0, SEEK_SET);
  fileFunctions->rwfread(&Wavhdr, sizeof(Wavhdr), 1, file);

	// Figure out the size of the data region
	dwSize = Wavhdr.dwDataSize;
	// Is this a stereo or mono file?
	bStereo = Wavhdr.wChannels > 1 ? TRUE : FALSE;

	// Create the sound buffer for the wave file
	if(!CreateSoundBuffer(dwSize, Wavhdr.dwSamplesPerSec,
		Wavhdr.wBitsPerSample, Wavhdr.wBlockAlign, bStereo))
		return FALSE;

  {
   	LPVOID pData1;
	  DWORD  dwData1Size;
	  LPVOID pData2;
	  DWORD  dwData2Size;

  	if(DS_OK != Buffers[m_Current]->Lock(0, dwSize, &pData1, &dwData1Size, 
                                       &pData2, &dwData2Size, 
                                       DSBLOCK_FROMWRITECURSOR))
    return FALSE;

	  // Read in first chunk of data
	  if(dwData1Size > 0) {
      fileFunctions->rwfread(pData1, dwData1Size, 1, file);
	  }
	  // Read in second chunk if necessary
	  if(dwData2Size > 0) {
      fileFunctions->rwfread(pData2, dwData2Size, 1, file);
	  }

	  // Unlock data in buffer
	  if(DS_OK != Buffers[m_Current]->Unlock(pData1, dwData1Size, pData2, 
                                           dwData2Size))
      return FALSE;
  }

	isLOADED = 1;
	m_Current=0;

  return TRUE;
}

/*****************************************************************************/
BOOL CSoundBuffer::LoadFromHandle(HANDLE hFile)
{
	DWORD dwReadSize;
	DWORD fPtr;
	Waveheader Wavhdr;
	DWORD dwSize;
	BOOL bStereo;

	fPtr = SetFilePointer(hFile,0,0,FILE_CURRENT) + sizeof(Wavhdr.rh);

	ReadFile(hFile,&Wavhdr,sizeof(Wavhdr),&dwReadSize,NULL);
	if(dwReadSize!=sizeof(Wavhdr)) return NULL;

	// Figure out the size of the data region
	dwSize = Wavhdr.dwDataSize;
	// Is this a stereo or mono file?
	bStereo = Wavhdr.wChannels > 1 ? TRUE : FALSE;

	// Create the sound buffer for the wave file
	if(!CreateSoundBuffer(dwSize, Wavhdr.dwSamplesPerSec,
		Wavhdr.wBitsPerSample, Wavhdr.wBlockAlign, bStereo))
		return FALSE;

	// Read the data for the wave file into the sound buffer
	if(!ReadDataFromHandle(hFile, dwSize))	return FALSE;

	SetFilePointer(hFile,(fPtr + Wavhdr.rh.dwSize),0,FILE_BEGIN);

	isLOADED = 1;
	m_Current=0;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
BOOL CSoundBuffer::ReadDataFromHandle(HANDLE hFile, DWORD dwSize)
{
	DWORD dwReadSize;
	LPVOID pData1;
	DWORD  dwData1Size;
	LPVOID pData2;
	DWORD  dwData2Size;
	HRESULT rval;

	rval = Buffers[m_Current]->Lock(0, dwSize, &pData1, &dwData1Size, &pData2, &dwData2Size, DSBLOCK_FROMWRITECURSOR);
	if(rval != DS_OK) return FALSE;

	// Read in first chunk of data
	if(dwData1Size > 0) {
		ReadFile(hFile,pData1,dwData1Size,&dwReadSize,NULL);
		if(dwReadSize!=dwData1Size){
			TCHAR holder[256];
			_stprintf(holder, _T("Data1 : %d, dwdata: %d, hFile: %d"), pData1, dwData1Size, hFile);
			OutputDebugString(holder);
			return FALSE;
		}
	}

	// Read in second chunk if necessary
	if(dwData2Size > 0) {
		ReadFile(hFile,pData2,dwData2Size,&dwReadSize,NULL);
		if(dwReadSize!=dwData2Size){
			return FALSE;
		}
	}

	// Unlock data in buffer
	rval = Buffers[m_Current]->Unlock(pData1, dwData1Size, pData2, dwData2Size);
	if(rval != DS_OK) return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

BOOL CSoundBuffer::PlayFromView(WORD centerX, WORD centerY,
										  WORD soundX, WORD soundY)
{
	if(m_pDS == NULL) return FALSE;
	int Pan; 
	LONG Vol;
	HRESULT rval;
	LPDIRECTSOUNDBUFFER buf = NULL;
	short	distX, distY;
	WORD	absDistX,absDistY;

	distX = soundX - (centerX>>1);
	distY = soundY - (centerY>>1);
	absDistX = abs(distX);
	absDistY = abs(distY);
	Pan = ((distX-distY)>>1)*100;

	// pan right
	if(distX > distY){ 
		if(Pan > DSBPAN_RIGHT) Pan = DSBPAN_RIGHT;
		Vol = -(absDistX + absDistY) * 30;
		if(Vol <= DSBVOLUME_MIN) return TRUE;
	}
	// pan left
	else{ 
		if(Pan < DSBPAN_LEFT) Pan = DSBPAN_LEFT;
		Vol = -(absDistX + absDistY) * 30;
		if(Vol <= DSBVOLUME_MIN) return TRUE;
	}

	buf = GetFreeBuffer();
	if(buf == NULL) return FALSE;

#ifndef _XBOX
	buf->SetPan(Pan);
#endif
	buf->SetVolume(Vol);
	rval = buf->Play(0, 0, 0);
	if(rval != DS_OK) return FALSE;

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////
// CSoundBuffer Play
//////////////////////////////////////////////////////////////////////////////////
BOOL CSoundBuffer::Play(int Pan, int volume, int freq, DWORD dwFlags)
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER buf = NULL;

	if(m_pDS == NULL) return FALSE;

	buf = GetFreeBuffer();
	if(buf == NULL) return FALSE;

	buf->SetFrequency(freq);
  buf->SetVolume(volume);
#ifndef _XBOX
	buf->SetPan(Pan);
#endif
	rval = buf->Play(0, 0, dwFlags);
	if(rval != DS_OK) return FALSE;

	return TRUE;
}

BOOL CSoundBuffer::PlayDefault(DWORD dwFlags)
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER buf = NULL;

	if(m_pDS == NULL) return FALSE;

	buf = GetFreeBuffer();
	if(buf == NULL) return FALSE;

	buf->SetFrequency(DSBFREQUENCY_ORIGINAL);
	buf->SetVolume(DSBVOLUME_MAX);
#ifndef _XBOX
	buf->SetPan(DSBPAN_CENTER);
#endif


	rval = buf->Play(0, 0, dwFlags);
	if(rval != DS_OK) return FALSE;

	return TRUE;
}

BOOL CSoundBuffer::PlaySame(DWORD dwFlags)
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER buf = NULL;

	if(m_pDS == NULL) return FALSE;

	buf = GetFreeBuffer();
	if(buf == NULL) return FALSE;

	rval = buf->Play(0, 0, dwFlags);
	if(rval != DS_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// CSoundBuffer Stop
//////////////////////////////////////////////////////////////////////////////////
BOOL CSoundBuffer::Stop()
{
	HRESULT rval;

	if(m_pDS == NULL) return FALSE;

	for(int i = 0; i < numBuffers; i++){
		rval = Buffers[i]->Stop();
		if(rval != DS_OK) return FALSE;

		rval = Buffers[i]->SetCurrentPosition(0);
		if(rval != DS_OK) return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// CSoundBuffer SetVolume
//////////////////////////////////////////////////////////////////////////////////
void CSoundBuffer::SetVolume(LONG Volume)
{
	if (Volume > DSBVOLUME_MAX) Volume = DSBVOLUME_MAX;
	if (Volume < DSBVOLUME_MIN) Volume = DSBVOLUME_MIN;
	for(int i = 0; i < numBuffers; i++)	Buffers[i]->SetVolume(Volume);
}


void CSoundBuffer::SetFrequency(DWORD frequency)
{
	for(int i = 0; i < numBuffers; i++)	Buffers[i]->SetFrequency(frequency);
}
