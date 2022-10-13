#ifndef __VORBISDRIVER__
#define __VORBISDRIVER__

#include <windows.h>
#include <stdio.h>
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>
#include "Vox.h"


#pragma    comment(lib, "libogg_static.lib")
#pragma    comment(lib, "libvorbis_static.lib")
#pragma    comment(lib, "libvorbisfile_static.lib")


// OggVorbis ???
// ?????
#pragma comment(lib, "winmm.lib")

enum THREADSTATE{
    THREAD_RUN,
    THREAD_PAUSE,
    THREAD_STOP
};

#define MAXBUFFER    4
#define BUFFERSIZE    (1024 * 64)

typedef unsigned int (__stdcall *THREADFUNC)(void* pParam);

class VorbisDriver : public Vox
{
public:
    // 0.50
    __stdcall VorbisDriver();
    __stdcall ~VorbisDriver();
    bool __stdcall        Load(char* pFileName);
    bool __stdcall        Play();
    bool __stdcall        Pause();
    bool __stdcall        Release();
    bool __stdcall        SetLoop(int Count=_VoxPlayForever);
    bool __stdcall        CheckDevice();
    bool __stdcall        SetVolume(float Volume);
    float __stdcall       GetVolume();
    bool __stdcall        Fade(float StartVolume, float EndVolume, int FadeTime);

    bool __stdcall        Delete();

    // 0.60
    float __stdcall       GetTotalTime();
    float __stdcall       GetCurrentTime();
    bool __stdcall        Seek(float Time);
    VOXSTATE __stdcall    GetStatus();
    VOXCOMMENT* __stdcall GetComment();


    // 0.61
    char* __stdcall VorbisDriver::ParseComment(char* pFieldName, CHARACTERCODE CharacterCode = CODE_SJIS);

private:
    int __stdcall Decode();
    static void __stdcall CALLBACK WaveProc(HWAVEOUT hWaveOut, UINT msg,
            DWORD instance, DWORD param1, DWORD param2);
    static unsigned int __stdcall ThreadProc(void* pThreadParam);

private:
    FILE *fp;
    char fname[_MAX_FNAME];
    char ConvBuffer[4096]; /* take 4k out of the data segment, not the stack */

    OggVorbis_File vf;
    vorbis_info *pInfo;
    VOXCOMMENT Comment;
    CHAR* pSJISBuffer; // ParseComment

    HWAVEOUT hWaveOut;
    WAVEHDR    WaveHdr[MAXBUFFER];

    HANDLE hSemaphore;
    HANDLE hThread;
    THREADSTATE ThreadState;

    bool  IsDeviceExist;
    bool  IsVorbisDriverInitialized;
    int   DecodePage;
    int   LoopCounter;
    float Volume;
    float SeekOffset;

    // 
    bool IsFade;
    float StartVolume, EndVolume;
    DWORD FadeStartTime;
    int FadeTime;
};

#endif // __VORBISDRIVER__
