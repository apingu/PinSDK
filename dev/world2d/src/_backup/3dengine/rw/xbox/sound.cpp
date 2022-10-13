/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: sound.cpp
 * Original Author: Mark Harmon
 ********************************************************************/
#include "ZGlobal.h"
#include "sound.h"
#include "CSound.h"
#include "../skeleton.h"

#ifdef _XBOX
	//#include "xbox.h"
#endif
#if !defined(_XBOX) && defined(WIN32)
	#include "win.h"
#endif

/*****************************************************************************/
char strSoundFiles[NUM_SOUNDS][32] = 
{
  "sound/start1.wav",     /* SND_ENGINE_START */
  "sound/engn_dod.wav",   /* SND_ENGINE1 */
  "sound/driv_drt.wav",   /* SND_DRIVE_DIRT */
  "sound/carSkid1.wav",   /* SND_CARSKID1 */
  "sound/collide2.wav",   /* SND_COLLIDE2 */
  "sound/gunEmpty.wav",   /* SND_GUNEMPTY */
  "sound/MGun00.wav",     /* SND_LMACHINEGUN */
  "sound/gunhit.wav",     /* SND_GUNHIT */
  "sound/explos.wav",     /* SND_GUNHIT */
  "sound/spawn.wav",      /* SND_SPAWN */
  "sound/getAmmo.wav",    /* SND_ACQUIRE_AMMO */
  "sound/getRscu.wav",    /* SND_ACQUIRE_RSCU */
};

static CSound* Sound = NULL;
static CSoundBuffer** Sounds = NULL;

//#define NO_SOUND

/*****************************************************************************/
BOOL psInitSoundEngine()
{

#ifdef NO_SOUND
  return TRUE;
#endif

#ifdef _XBOX
  HWND hwnd = NULL;
#else
  HWND hwnd = PSGLOBAL(window);
	if(hwnd == NULL) return FALSE;
#endif

  RwChar *path; 

  // cleanup first, in case calling again after first time
  psTermSoundEngine();

  Sound = new CSound();
  Sound->Create(hwnd);

  Sounds = new CSoundBuffer*[NUM_SOUNDS];

  for(int i = 0; i < NUM_SOUNDS; i++)
  {
    Sounds[i] = new CSoundBuffer();
    path = RsResourcePathCreate(strSoundFiles[i]);
    Sounds[i]->LoadFromPakWav(Sound, path, 1);
    RsPathnameDestroy(path);
    if(Sounds[i]->isLOADED == FALSE)
    {
      RwChar msg[128];
      _stprintf(msg,_T("Error opening sound file %s."), path);
      RsErrorMessage(msg);
      return FALSE;
    }
  }

  return TRUE;
}


/*****************************************************************************/
void psPlayGameSoundDefault(UINT16 soundNo, BOOL isLooping)
{
#ifndef NO_SOUND
  Sounds[soundNo]->PlayDefault((isLooping?DSBPLAY_LOOPING:0));
#endif
}

/*****************************************************************************/
BOOL psPlayGameSoundOnceDefault(UINT16 soundNo, BOOL isLooping)
{
#ifndef NO_SOUND
  if(Sounds[soundNo]->isPlaying()) return FALSE;
  Sounds[soundNo]->PlayDefault((isLooping?DSBPLAY_LOOPING:0));
#endif
  return TRUE;
}

/*****************************************************************************/
void psSetSoundFrequency(UINT16 soundNo, UINT32 frequency)
{
#ifndef NO_SOUND
  Sounds[soundNo]->SetFrequency(frequency);
#endif
}

/*****************************************************************************/
void psSetSoundVolume(UINT16 soundNo, INT32 volume)
{
#ifndef NO_SOUND
  Sounds[soundNo]->SetVolume(volume);
#endif
}

/*****************************************************************************/
void psStopGameSound(UINT16 soundNo)
{
#ifndef NO_SOUND
  if(Sounds) Sounds[soundNo]->Stop();
#endif
}

/*****************************************************************************/
void psStopAllSounds()
{
  if(Sounds)
  {
    for(int i = 0; i < NUM_SOUNDS; i++)
      Sounds[i]->Stop();
  }
}


/*****************************************************************************/
void psTermSoundEngine()
{
  if(Sounds)
  {
    for(int i = 0; i < NUM_SOUNDS; i++) delete Sounds[i];
    delete [] Sounds;
  }
  Sounds = NULL;

  if(Sound) delete Sound;
  Sound = NULL;
}


/*****************************************************************************/
