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

//01. == C 系統文件/C++ 系統文件
#include <assert.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
#include <pf-thread.h>
#include <T_Lock.h>
#include <pf-debug.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "VorbisDriver.h"
#include "SoundPlayer.h"

//#pragma comment (lib, "Vox.lib")


//Vox*              pBGMDriver=NULL;
//std::vector<Vox*> pSndDriverList;

//pfkrl::Thread* g_SndThread=NULL;


//WavPlayerLoaded *snds[16];
//WavPlayerStream *music=NULL;

//bool no_func=true;

//static SoundManager_Param sndparm;
/*
extern "C" void log_write(int level, const char* message)
{
    OutputDebugStringA(message);
}
extern "C" void log_write_code(int level, unsigned code, const char* message)
{
    OutputDebugStringA(message);
}

static int stdlv;

extern "C" void log_printf(int level, const char* fmt,...)
{
    va_list mrkr;
    char common_msg[256];

    if (level>stdlv)
        return;
    va_start(mrkr, fmt);
    vsprintf(common_msg, fmt, mrkr);
    va_end(mrkr);
    log_write(level, common_msg);
}
extern "C" void dbgprintf(const char* fmt,...)
{
}
extern "C" void log_setlevel(int lv)
{
    stdlv = lv;
}


static void report_sound_error(const char* desc, const char* filename)
{
    //char buf[256];
    //sprintf(buf,"Error: %s %s",desc,filename);
    //g_MainWnd.MsgBox(buf,"Sound error");
}
*/

/*
// 直接讀好像會慢 所以 擺在thread裡面讀
struct PFSoundLoadBlock
{
    char filename[256];
    int  sndvol;
    int  loop;
    Vox* pDriver;
};

T_Lock                         PFSoundLoadPosLock;
int                            PFSoundLoadPos=0;
T_Lock                         PFSoundLoadBlockListLock[5];
std::vector<PFSoundLoadBlock*> PFSoundLoadBlockList[5];
*/

T_Lock  g_PFSoundLocker;

static int findFmt( const char* filename )
{
    char* cp = strchr( ( char* )filename, '.' );

    if( cp != NULL )
    {
        if( !stricmp( cp, ".wav" ) )
        {
            return 1;
        }
        else if( !stricmp( cp, ".ogg" ) )
        {
            return 2;
        }
    }

    return -1;    // unknown!!
}

// In another thread
void SoundPlayer::soundplayer_thread_func( void* idat )
{
    //        STIME t;
    //        char buf[64];
    SoundPlayer* inst = ( SoundPlayer* )idat;

    while( inst->m_bCreate )
    {
        inst->ProcessSounds();
        //Sleep(2000);    // Is this necessary? Maybe not.
        //            t=GetBootTime();
        //            sprintf(buf,"t=%6d:",t);    OutputDebugString(buf);
    }
}

//void soundload_thread_func(void *idat);


SoundPlayer::SoundPlayer()
{
    m_bCreate = false;
    m_threadid = 0;
    strcpy( m_BGMFilename, "" );
}

SoundPlayer::~SoundPlayer()
{}

void SoundPlayer::Create()
{
    if( !m_bCreate )
    {
        sndvol = 0;
        musvol = 0;
        pBGMDriver = NULL;
        m_bCreate = true;
        m_threadid = pfkrl::Thread::BeginProcess( soundplayer_thread_func, this, _T( "SoundPlayer::Create" ) );
    }
}


void SoundPlayer::Destroy()
{
    if( m_bCreate )
    {
        // 先關閉
        CloseWaveBGM();
        {
            TS_Lock lock( &g_PFSoundLocker );

            for( u_int i = 0; i < pSndDriverList.size(); i++ )
            {
                if( pSndDriverList[ i ] )
                {
                    pSndDriverList[ i ]->Delete();
                    //SAFE_DELETE(pSndDriverList[i]);
                }
            }

            pSndDriverList.clear();
        }
        // 讓執行緒跳出
        m_bCreate = false;

        for( int i = 0; i < 10; i++ )
        {
            if( !pfkrl::Thread::TestProcessRun( m_threadid ) )
            {
                break;
            }

            Sleep( 500 );
        }
    }
}

struct soundplayer_loaddata
{
    SoundPlayer* pSoundPlayer;
    Vox* pSndDriver;
    int          loop;
    char         filename[ 512 ];
};

/*
void SoundPlayer::soundplayer_thread_loadbgm_func(void *idat)
{
    soundplayer_loaddata* loaddata=(soundplayer_loaddata*)idat;

    bool ret=false;
    if(!(ret=loaddata->pSoundPlayer->pBGMDriver->Load(loaddata->filename)))
    {
        char buff[256];
        sprintf(buff, "play sound file %s failure\n", loaddata->filename);
        OutputDebugString(buff);
        loaddata->pSoundPlayer->pBGMDriver->Delete();
        loaddata->pSoundPlayer->pBGMDriver=NULL;
    }
    else
    {
        loaddata->pSoundPlayer->pBGMDriver->Play();
    }
    SAFE_DELETE(loaddata);
}
*/

bool SoundPlayer::LoadWaveBGM( char* file )
{
    bool ret = false;

    if( !m_bCreate )
    {
        assert( NULL && "Create Sound Player Object first" );
        return ret;
    }

    CloseWaveBGM();
    int ft = findFmt( file );

    switch( ft )
    {
        case 1:    // WAV
            //music = new WavPlayerStream;
            break;

        case 2:    // OGG
            pBGMDriver = new VorbisDriver;
            break;

        default:
            assert( 0 || _T( "music create failure" ) );
            pBGMDriver = NULL;
    }

    if( pBGMDriver )
    {
        /*
        soundplayer_loaddata* loaddata= new soundplayer_loaddata;
        loaddata->pSoundPlayer=this;
        loaddata->pSndDriver = pBGMDriver;
        loaddata->loop=0;
        strcpy(loaddata->filename, file);

        pfkrl::Thread::BeginProcess(soundplayer_thread_loadbgm_func, loaddata);
        */

        /*
        PFSoundLoadBlock* block=new PFSoundLoadBlock;
        strcpy(block->filename, file);
        block->sndvol = pBGMDriver->GetVolume()*100;
        block->loop = m_BGMLoop;
        block->pDriver = pBGMDriver;

        pfkrl::Thread::BeginProcess(soundload_thread_func,   block);
        (/

        /*
        TS_Lock lock0(&PFSoundLoadPosLock);

        TS_Lock lock(&PFSoundLoadBlockListLock[PFSoundLoadPos]);
        PFSoundLoadBlockList[PFSoundLoadPos].push_back(block);

        if(PFSoundLoadPos<(5-1))
            PFSoundLoadPos++;
        else
            PFSoundLoadPos=0;
            */
        if( !( ret = pBGMDriver->Load( file ) ) )
        {
            Pf_ERROR( "SoundPlayer::LoadWaveBGM: play sound file %s failure\n", file );
            pBGMDriver->Delete();
            pBGMDriver = NULL;
            strcpy( m_BGMFilename, "" );
        }
        else
        {
            strcpy( m_BGMFilename, file );
        }

        /*
        else
        {
            pBGMDriver->Play();
        }
        */
    }

    return ret;
    /*
    if (no_func)
    return;
    locker->Lock();
    if (music)
    {
    if (!(music->GetStatus() == SNDTS_NOTLOAD))
    music->Stop();
    Sleep(0);
    SAFE_DELETE(music);
    }

    if (music)
    {
    if (!music->LoadFile(file, 0))
    {
    report_sound_error("Cannot load music file:", file);
    SAFE_DELETE(music);
    }
    else
    {
    music->SetVolume(musvol);
    DS_AddPlayer(music, false);
    }
    }
    locker->UnLock();
    */
}

void SoundPlayer::soundplayer_thread_loadsnd_func( void* idat )
{
    soundplayer_loaddata* loaddata = ( soundplayer_loaddata* )idat;
    bool ret = false;
    // 因為這邊 撥久了會再VorbisDriver::Load waveOutOpen那邊 卡注 所以先使用多緒的方式

    if( ( ret = loaddata->pSndDriver->Load( loaddata->filename ) ) == true )
    {
        loaddata->pSndDriver->SetVolume( ( ( float )loaddata->pSoundPlayer->sndvol / ( float )100 ) );
        loaddata->pSndDriver->SetLoop( loaddata->loop );
        loaddata->pSndDriver->Play();
        TS_Lock lock( &g_PFSoundLocker );
        loaddata->pSoundPlayer->pSndDriverList.push_back( loaddata->pSndDriver );
        Pf_INFO( "SoundPlayer::soundplayer_thread_loadsnd_func: sound size %d\n", loaddata->pSoundPlayer->pSndDriverList.size() );
    }
    else
    {
        Pf_ERROR( "SoundPlayer::soundplayer_thread_loadsnd_func: play sound file %s failure\n", loaddata->filename );
        loaddata->pSndDriver->Delete();
    }

    SAFE_DELETE( loaddata );
}

bool SoundPlayer::PlayWaveSound( char* file, int loop )
{
    //DWORD loadtime = ::GetTickCount();
    bool ret = false;

    if( !m_bCreate )
    {
        assert( NULL && "Create Sound Player Object first" );
        return ret;
    }

    {
        // 連續播放會當掉 原因不明所以先用限制次數的方法
        TS_Lock lock( &g_PFSoundLocker );

        if( pSndDriverList.size() >= 32 )
        {
            return ret;
        }
    }

    //DWORD Time01 = ::GetTickCount();
    Vox* pSndDriver = NULL;
    int ft = findFmt( file );

    switch( ft )
    {
        case 1:    // WAV
            //music = new WavPlayerStream;
            break;

        case 2:    // OGG
            pSndDriver = new VorbisDriver;
            break;

        default:
            assert( 0 || _T( "sound create failure" ) );
            pSndDriver = NULL;
    }

    //DWORD Time02 = ::GetTickCount();

    if( pSndDriver )
    {
        //DWORD Time03 = ::GetTickCount();
        ret = true;
        soundplayer_loaddata* loaddata = new soundplayer_loaddata;
        loaddata->pSoundPlayer = this;
        loaddata->pSndDriver = pSndDriver;
        loaddata->loop = loop;
        strcpy( loaddata->filename, file );
        //DWORD Time04 = ::GetTickCount();
        pfkrl::Thread::BeginProcess( soundplayer_thread_loadsnd_func, loaddata, _T( "SoundPlayer::LoadData" ) );
        //DWORD Time05 = ::GetTickCount();
        /*
        if((Time05-loadtime)>5)
        {
            char buff[256];
            sprintf(buff, "Load snd time 01 %d\n", Time01-loadtime);
            OutputDebugString(buff);
            sprintf(buff, "Load snd time 02 %d\n", Time02-Time01);
            OutputDebugString(buff);
            sprintf(buff, "Load snd time 03 %d\n", Time03-Time02);
            OutputDebugString(buff);
            sprintf(buff, "Load snd time 04 %d\n", Time04-Time03);
            OutputDebugString(buff);
            sprintf(buff, "Load snd time 05 %d\n", Time05-Time04);
            OutputDebugString(buff);
        }
        */
        /*
        PFSoundLoadBlock* block=new PFSoundLoadBlock;
        strcpy(block->filename, file);
        block->sndvol = sndvol;
        block->loop = loop;
        block->pDriver = pSndDriver;

        pfkrl::Thread::BeginProcess(soundload_thread_func,   block);
        */
        /*
        TS_Lock lock0(&PFSoundLoadPosLock);

        TS_Lock lock(&PFSoundLoadBlockListLock[PFSoundLoadPos]);
        PFSoundLoadBlockList[PFSoundLoadPos].push_back(block);

        if(PFSoundLoadPos<(5-1))
            PFSoundLoadPos++;
        else
            PFSoundLoadPos=0;
            */
    }

    return ret;
    /*
    if (no_func)
        return;

    bool bplay=false;
    locker->Lock();
    _REPLAYSOUND:
    for (int i = 0; i < sndparm.max_loadsnd; i++)
    {
        if (snds[i]->GetStatus() == SNDTS_STOPPED ||
            snds[i]->GetStatus() == SNDTS_NOTLOAD)
        {
            assert(file != NULL);
            if (!snds[i]->LoadFile(file, 0))
            {
                // 檔名錯誤我也沒輒
                // 所以也當成正確撥了
                report_sound_error("Error loading sound file:", file);
            }
            else
            {
                snds[i]->Play(1);
                snds[i]->SetVolume(sndvol);
            }
            bplay=true;
            break;
        }
    }

    // 重新分配
    if(!bplay)
    {
        SAFE_DELETE(snds[0]);
        for(int i = 0; i<sndparm.max_loadsnd-1; i++)
        {
            snds[i]=snds[i+1];
        }
        snds[sndparm.max_loadsnd-1] = new WavPlayerLoaded;
        DS_AddPlayer(snds[sndparm.max_loadsnd-1], false);

        // 如果這樣都撥不出來 總不能一直卡?
        bplay=true;
        goto _REPLAYSOUND;
    }
    locker->UnLock();
    */
}

bool SoundPlayer::PlayWaveBGM( int loop )
{
    bool ret = false;

    if( !m_bCreate )
    {
        assert( NULL && "Create Sound Player Object first" );
        return ret;
    }

    if( pBGMDriver )
    {
        if( loop = -1 )
        {
            loop = _VoxPlayForever;
        }

        pBGMDriver->SetLoop( loop );

        if( ret = pBGMDriver->Play() )
        {
            SetVolumeBGM( musvol );
        }
    }

    return ret;
    //     if (no_func)
    //         return;
    //     if (music != NULL)
    //     {
    //         music->Play(loop);
    //         music->SetVolume(musvol);
    //     }
}
bool SoundPlayer::PauseWaveBGM()
{
    bool ret = false;

    if( !m_bCreate )
    {
        assert( NULL && "Create Sound Player Object first" );
        return ret;
    }

    if( pBGMDriver )
    {
        ret = pBGMDriver->Pause();
    }

    //if (music != NULL)
    //    music->Stop();
    return ret;
}
bool SoundPlayer::CloseWaveBGM()
{
    bool ret = false;

    if( !m_bCreate )
    {
        assert( NULL && "Create Sound Player Object first" );
        return ret;
    }

    if( pBGMDriver )
    {
        ret = pBGMDriver->Delete();
        pBGMDriver = NULL;
        //SAFE_DELETE(pBGMDriver);
    }

    strcpy( m_BGMFilename, "" );
    //SAFE_DELETE(music);
    return ret;
}

const char* SoundPlayer::GetBGMFilename()
{
    return m_BGMFilename;
}


/*
void soundload_thread_func(void *idat)
{
    PFSoundLoadBlock *inst = (PFSoundLoadBlock*)idat;

    inst->pDriver->Load(inst->filename);
    inst->pDriver->SetVolume(((float)inst->sndvol/(float)100));
    inst->pDriver->SetLoop(inst->loop);
    inst->pDriver->Play();
    if(pBGMDriver!=inst->pDriver)
    {
        TS_Lock lock(locker);
        pSndDriverList.push_back(inst->pDriver);
    }
    inst->pDriver=NULL;
    SAFE_DELETE(inst);

    //        STIME t;
    //        char buf[64];
    /*
    SoundPlayer *inst = (SoundPlayer*)idat;
    while (!m_bCreate)
    {
        std::vector<PFSoundLoadBlock*> tmplist;

        {
            TS_Lock lock1(&PFSoundLoadBlockListLock);
            for(int i = 0; i<PFSoundLoadBlockList.size(); i++)
            {
                if(PFSoundLoadBlockList[i])
                {
                    tmplist.push_back(PFSoundLoadBlockList[i]);
                }
            }
            PFSoundLoadBlockList.clear();
        }

        DWORD t1 = GetTickCount();

        for(int i = 0; i<tmplist.size(); i++)
        {
            if(tmplist[i])
            {
                tmplist[i]->pDriver->Load(tmplist[i]->filename);
                tmplist[i]->pDriver->SetVolume(((float)tmplist[i]->sndvol/(float)100));
                tmplist[i]->pDriver->SetLoop(tmplist[i]->loop);
                tmplist[i]->pDriver->Play();
                if(pBGMDriver!=tmplist[i]->pDriver)
                {
                    TS_Lock lock(locker);
                    pSndDriverList.push_back(tmplist[i]->pDriver);
                }
                tmplist[i]->pDriver=NULL;
                SAFE_DELETE(tmplist[i]);
            }
        }

        DWORD t2 = GetTickCount();

        //if((t2-t1)<500)
        Sleep(5);
        //else
            //Sleep(2);
        //PFSoundLoadBlockList.clear();

    }
    m_bCreate = m_bCreate;
    */
    //}

void SoundPlayer::ProcessSounds()    // Reload sound data when necessary. Must be called
{
    {
        TS_Lock lock( &g_PFSoundLocker );
        bool isclear = false;

        for( u_int i = 0; i < pSndDriverList.size(); i++ )
        {
            if( pSndDriverList[ i ] )
            {
                //pSndDriverList[i]->Update();
                if( pSndDriverList[ i ]->GetStatus() == STATE_STOP )
                {
                    //VorbisDriver* pSndDriver = (VorbisDriver*)pSndDriverList[i];
                    pSndDriverList[ i ]->Delete();
                    //delete pSndDriver;
                    pSndDriverList[ i ] = NULL;
                    //SAFE_DELETE(pSndDriverList[i]);
                    isclear = true;
                }
            }
        }

        if( isclear )
        {
            for( u_int i = 0; i < pSndDriverList.size(); i++ )
            {
                if( pSndDriverList[ i ] == NULL )
                {
                    pSndDriverList.erase( pSndDriverList.begin() + i );
                    i--;
                }
            }
        }

        //TS_Lock lock(locker);
        //if (no_func)    return;
        //DS_Process();
    }
    Sleep( 1000 );
}

int SoundPlayer::GetVolumeBGM()
{
    return musvol;
}

void SoundPlayer::SetVolumeBGM( int percent )    // 設定 BGM, 應該立刻有效
{
    musvol = percent;

    if( pBGMDriver )
    {
        pBGMDriver->SetVolume( ( ( float )percent / ( float )100 ) );
    }

    //if (music != NULL)
    //    music->SetVolume(percent);
}

int SoundPlayer::GetVolumeSound()    // 設定 SE, 以後的聲音有效(不追溯寄往)
{
    return sndvol;
}

void SoundPlayer::SetVolumeSound( int percent )    // 設定 SE, 以後的聲音有效(不追溯寄往)
{
    sndvol = percent;
}

