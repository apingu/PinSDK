/*********************************************************************
 * Copyright (c) 2001, Mark Harmon All Rights Reserved
 *
 * Filename: CSound.h
 * Original Author: Mark Harmon
 ********************************************************************/
#ifndef CSOUND_H
#define CSOUND_H

#include <dsound.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////
// CDXSound Class
//////////////////////////////////////////////////////////////////////////////////
class CSound
{
public:
	CSound(void);
	~CSound(void);

	BOOL Create(HWND hWnd);
	BOOL GetCaps(void);
	LPDIRECTSOUND GetDirectSound(void) {return m_lpDS;};

private:
	LPDIRECTSOUND m_lpDS;
	DSCAPS m_DSCaps;
};

//////////////////////////////////////////////////////////////////////////////////
// CSoundBuffer Class
//////////////////////////////////////////////////////////////////////////////////
class CSoundBuffer
{
public:
	BYTE isLOADED;
	BYTE pad;
	
	struct RiffHeader
	{
		char	RIFF[4];		// "RIFF"
		DWORD	dwSize;		// remaining length after this
	};
	struct ChunkHeader
	{
		char	id[4];	// "fmt "
		DWORD	len;		// remaining length of chunk after header
	};

	struct Waveheader
	{
		RiffHeader rh;
		char	WAVE[4];		// "WAVE"
		ChunkHeader ch;

		WORD	wFormatTag;			// format category
		WORD  wChannels;			// Number of Channels
		DWORD	dwSamplesPerSec;	// Sample Rate
		DWORD	dwAvgBytesPerSec;	// for buffer estimation
		WORD	wBlockAlign;		// data block size

		WORD	wBitsPerSample;	// bit rate of sample. 8 or 16
		BYTE	data[4];          // "DATA"
		DWORD	dwDataSize;			// length of sample data
	};

	CSoundBuffer(void);
	~CSoundBuffer(void);

	BOOL CreateSoundBuffer(DWORD dwBufSize, DWORD dwFreq,DWORD dwSampleBits,DWORD dwBlkAlign, BOOL bStereo);
	void FreeBuffers(void);
	LPDIRECTSOUNDBUFFER GetFreeBuffer(void);

  BOOL LoadFromPakWav(CSound* pDS, char *fname, int numBuf);
  BOOL LoadFromPakHandle(void* file);

	BOOL LoadFromWav(CSound* pDS, char* fname, HANDLE hFile = NULL, int Num = 1);
	BOOL LoadFromHandle(HANDLE hFile);
	BOOL ReadDataFromHandle(HANDLE hFile, DWORD dwSize);

	BOOL PlayDefault(DWORD dwFlags);
	BOOL Play(int Pan, int volume, int freq, DWORD dwFlags);
	BOOL PlaySame(DWORD dwFlags);
	BOOL PlayFromView(WORD centerX, WORD centerY, WORD soundX, WORD soundY);
	BOOL Stop(void);
	
	void SetVolume(LONG Volume);
	void SetFrequency(DWORD frequency);
	BOOL isPlaying(void);

private:
	BYTE numBuffers;
	BYTE m_Current;
	CSound* m_pDS;
	LPDIRECTSOUNDBUFFER *Buffers;
};

#endif