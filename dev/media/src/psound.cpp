#include "PSound.h"
#include "SoundCodec.h"
#include <string.h>


 
namespace MediaDevice
{

	namespace PSoundDeviceGlobal
	{

		ISoundSource*      g_BGMSSource;		
		ISoundSource*      g_CurSSource;		
		//static SoundManager_Param sndparm;
		//System::PThread*          g_SndThread=NULL;

		//WavPlayerLoaded*          snds[16];
		//WavPlayerStream*          music;
		//int                       sndvol,musvol;
		//bool                      to_close;
		//bool                      no_func;
		//T_Lock                    g_locker;
	}


	PSound::PSound()
	{
		m_BGMVolume = 1.0f;
		m_SoundVolume = 1.0f;
		m_bInit=PSL_Init();
	}

	PSound::~PSound()
	{
		PSL_Free();
		//delete PSoundDeviceGlobal::locker;
	}


	void PSound::LoadSound(tchar_t* file)
	{
		strcpy(m_SndFilename, file);
	}

	void PSound::LoadBGM(tchar_t* file)
	{
		strcpy(m_BGMFilename, file);
		PSoundDeviceGlobal::g_BGMSSource = PSL_GetAvailableSource();
		PSoundDeviceGlobal::g_BGMSSource->SetUsable(false);
	}


// 	void PSound::SetMute(bool is_mute)
// 	{
// 		m_IsMute = is_mute;
// 		if (m_IsMute)
// 			Stop();
// 		else
// 			Play();
// 	}


	void PSound::PlayBGM(bool loop)
	{
		if(!m_bInit)	return;

		PSoundDeviceGlobal::g_BGMSSource->Stop();
		PSoundDeviceGlobal::g_BGMSSource->AttachBuffer( PSL_GetBufferWithName(m_BGMFilename) );
		PSoundDeviceGlobal::g_BGMSSource->Play(loop);
		m_bBGMPlaying = true;
		
	}


	void PSound::PlaySound(bool loop)
	{
		
		if(!m_bInit)	return;

		PSoundDeviceGlobal::g_CurSSource = PSL_GetAvailableSource();
		PSoundDeviceGlobal::g_CurSSource->AttachBuffer(PSL_GetBufferWithName(m_SndFilename));
		PSoundDeviceGlobal::g_CurSSource->Play(loop);
		m_bSoundPlaying = true;
	}



	void PSound::StopBGM()
	{
//		m_bFunc=false;
		m_bBGMPlaying = false;
		PSL_StopSource( PSoundDeviceGlobal::g_BGMSSource );
	}

	void PSound::StopSound()
	{
		m_bSoundPlaying = false;
		PSL_StopAllSource();
	}

	void PSound::Update()
	{
		PSL_ClearBuffer();
		return;
	}

	void PSound::SetVolumeBGM(int percent)	// 設定 BGM, 應該立刻有效
	{
		m_BGMVolume = (float)(percent/100.0);
		if( PSoundDeviceGlobal::g_BGMSSource !=NULL )
		    PSoundDeviceGlobal::g_BGMSSource->SetVolume(m_BGMVolume);
	}
	void PSound::SetVolumeSound(int percent)	// 設定 SE, 以後的聲音有效(不追溯寄往)
	{
		m_SoundVolume = (float)(percent/100.0);
		if( PSoundDeviceGlobal::g_CurSSource !=NULL )
		    PSoundDeviceGlobal::g_CurSSource->SetVolume(m_BGMVolume);
	}

	bool PSound::GetBGMPlaying( void )
	{
		return m_bBGMPlaying;

	}

	bool PSound::GetSoundPlaying( void )
	{
		return m_bSoundPlaying;

	}

};

