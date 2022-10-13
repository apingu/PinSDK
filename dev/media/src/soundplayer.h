/********************************************************************
created:    2005/07/20
created:    20:7:2005   19:12
filename:     \HELLOKITTYONLINE\HKO\UTIL\SoundPlayer.h
file path:    \HELLOKITTYONLINE\HKO\UTIL
file base:    SoundPlayer
file ext:    h
author:        Sean

purpose:    Play sound
*********************************************************************/

#ifndef PFSOUND_PLAYER_H
#define PFSOUND_PLAYER_H

#include <vector>

class Vox;
class T_Lock;

class SoundPlayer
{
public:
    SoundPlayer();
    ~SoundPlayer();

    //void Disable();    // 整個關掉, 測試時期專用(但是其他功能都假裝有作)(一旦呼叫了是無法回復的)

    void                    Create();
    void                    Destroy();

    bool                    PlayWaveSound(char* file, int loop = 1);

    // for wave BGM
    bool                    LoadWaveBGM(char* file);
    bool                    PlayWaveBGM(int loop = -1);
    bool                    PauseWaveBGM();
    bool                    CloseWaveBGM();
    const char*             GetBGMFilename();

    //void Clear();
    void                    ProcessSounds();    // Reload sound data when necessary. Must be called
                
    int                     GetVolumeBGM();
    void                    SetVolumeBGM(int percent);        // 設定 BGM, 應該立刻有效
    int                     GetVolumeSound();
    void                    SetVolumeSound(int percent);    // 設定 SE, 以後的聲音有效(不追溯寄往)

private:

    bool                    m_bCreate;
    int                     m_threadid;
    int                     sndvol;
    int                     musvol;
    Vox*                    pBGMDriver;
    std::vector<Vox*>       pSndDriverList;
    char                    m_BGMFilename[_MAX_LPATH];

    static void soundplayer_thread_func(void *idat);
    static void soundplayer_thread_loadbgm_func(void *idat);
    static void soundplayer_thread_loadsnd_func(void *idat);

};

#endif //PICNO_SOUND_PLAYER_H