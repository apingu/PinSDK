#ifndef PSOUND_H
#define PSOUND_H

#include <basedef.h>


namespace MediaDevice
{
	class PSound
	{
	public:
		PSound();
		~PSound();


		void LoadSound(tchar_t* file);
		void PlaySound(bool loop = true);

		void LoadBGM(tchar_t* file);
		void PlayBGM(bool loop = true);
		

		void StopSound();
		void StopBGM();

		void Close();
		// TODO: PauseWaveBGM()

		void Update();

		//void SetMute(bool is_mute = true);

		void SetVolumeBGM(int percent);	    // 設定 BGM, 應該立刻有效
		void SetVolumeSound(int percent);	// 設定 SE, 以後的聲音有效(不追溯寄往)

		bool GetBGMPlaying( void );
		bool GetSoundPlaying( void );


	private:

		float	                m_BGMVolume;
		float	                m_SoundVolume;
		bool					m_bBGMPlaying;
		bool					m_bSoundPlaying;
		bool                    m_bFunc;
		bool                    m_bInit;
		tchar_t                  m_BGMFilename[_MAX_PATH];
		tchar_t                  m_SndFilename[_MAX_PATH];
	};

};

#endif