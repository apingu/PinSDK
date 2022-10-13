/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: sound.cpp
 * Original Author: Mark Harmon
 ********************************************************************/
#include "sound.h"

#include "CSound.h"
#include "../skeleton.h"
#include "win.h"

/*****************************************************************************/
char strSoundFiles[NUM_SOUNDS][24] = 
{
  "start1.wav",     /* SND_ENGINE_START */
  "engn_dod.wav",   /* SND_ENGINE1 */
  "driv_drt.wav",   /* SND_DRIVE_DIRT */
  "carSkid1.wav",   /* SND_CARSKID1 */
  "collide2.wav",   /* SND_COLLIDE2 */
  "gunEmpty.wav",   /* SND_GUNEMPTY */
  "MGun00.wav",     /* SND_LMACHINEGUN */
  "gunhit.wav",     /* SND_GUNHIT */
};

static CSound* Sound = NULL;
static CSoundBuffer** Sounds = NULL;

//#define NO_SOUND

/*****************************************************************************/
BOOL psInitSoundEngine()
{
  HWND hwnd = NULL; //PSGLOBAL(window);
  char path[MAX_PATH];

//  if(hwnd == NULL) return FALSE;

#ifdef NO_SOUND
  return TRUE;
#endif

  // cleanup first, in case calling again after first time
  psTermSoundEngine();

  Sound = new CSound();
  Sound->Create(hwnd);

  Sounds = new CSoundBuffer*[NUM_SOUNDS];

  for(int i = 0; i < NUM_SOUNDS; i++)
  {
    Sounds[i] = new CSoundBuffer();
    sprintf(path, "./resources/sound/%s", strSoundFiles[i]);
    Sounds[i]->LoadFromPakWav(Sound, path, 1);
    if(Sounds[i]->isLOADED == FALSE)
    {
      RwChar msg[128];
      sprintf(msg,"Error opening sound file %s.", path);
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
void psPlayGameSoundOnceDefault(UINT16 soundNo, BOOL isLooping)
{
#ifndef NO_SOUND
  if(Sounds[soundNo]->isPlaying()) return;
  Sounds[soundNo]->PlayDefault((isLooping?DSBPLAY_LOOPING:0));
#endif
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
