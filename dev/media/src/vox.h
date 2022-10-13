#ifndef __VOX__
#define __VOX__

enum VOXSTATE{
    STATE_PAUSE = 0,
    STATE_PLAYING,
    STATE_STOP,
};

enum CHARACTERCODE{
    CODE_SJIS = 0,
    CODE_UNICODE,
    CODE_UTF8
};

#define _VoxPlayForever    0XFFFFFFFF

#pragma pack(push, 1)
struct VOXCOMMENT {
    char* *user_comments;
    int   *comment_lengths;
    int    comments;
    char  *vendor;
};
#pragma pack(pop)

class Vox
{
public:
    // 0.50
    virtual bool  __stdcall Load(char* pFileName) = 0;
    virtual bool  __stdcall Play() = 0;
    virtual bool  __stdcall Pause() = 0;
    virtual bool  __stdcall Release() = 0;
    virtual bool  __stdcall SetLoop(int Count=_VoxPlayForever) = 0;
    virtual bool  __stdcall CheckDevice() = 0;
    virtual bool  __stdcall SetVolume(float Volume) = 0;
    virtual float __stdcall GetVolume() = 0;
    virtual bool  __stdcall Fade(float StartVolume, float EndVolume, int FadeTime) = 0;

    virtual bool  __stdcall Delete() = 0;

    // 0.60
    virtual float  __stdcall GetTotalTime() = 0;
    virtual float  __stdcall GetCurrentTime() = 0;
    virtual bool   __stdcall Seek(float Time) = 0;
    virtual VOXSTATE    __stdcall GetStatus() = 0;
    virtual VOXCOMMENT* __stdcall GetComment() = 0;

    //virtual void __stdcall        Update()=0;

    // 0.61
    virtual char* __stdcall ParseComment(char* pFieldName, CHARACTERCODE CharacterCode = CODE_SJIS) = 0;
};

/*
// DLL?????
#ifndef VOXVC_EXPORTS

typedef int  __stdcall VoxLoad(char* pFileName);
typedef bool __stdcall VoxPlay(int ID);
typedef bool __stdcall VoxPause(int ID);
typedef bool __stdcall VoxRelease(int ID);
typedef bool __stdcall VoxSetLoop(int ID, int Count);
typedef bool __stdcall VoxCheckDevice(int ID);
typedef bool __stdcall VoxSetVolume(int ID, float Volume);
typedef int  __stdcall VoxGetVolume(int ID);
typedef bool __stdcall VoxFade(int ID, float StartVolume, float EndVolume, int FadeTime);
typedef bool __stdcall VoxDelete(int ID);
typedef int  __stdcall VoxGetTotalTime(int ID);
typedef int  __stdcall VoxGetCurrentTime(int ID);
typedef bool __stdcall VoxSeek(int ID, int Time);
typedef int  __stdcall VoxGetStatus(int ID);
typedef VOXCOMMENT* __stdcall VoxGetComment(int ID);
typedef char* __stdcall VoxParseComment(int ID, char* pFieldName, CHARACTERCODE CharacterCode = CODE_SJIS);

#endif
*/

#endif // __VOX__
