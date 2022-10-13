/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: sound.cpp
 * Original Author: Mark Harmon
 ********************************************************************/
#include "sound.h"

#include "audio.h"
#include "../skeleton.h"

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


//#define NO_SOUND

static int soundtable[NUM_SOUNDS];

/*****************************************************************************/
BOOL psInitSoundEngine()
{
	memset (soundtable, -1, sizeof(soundtable));
  return TRUE;
}


/*****************************************************************************/
BOOL psPlayGameSoundDefault(UINT16 soundNo, BOOL isLooping)
{
	audio_playsound (soundNo, 1.0f, 0.0f, 1.0f);
	return TRUE;
}

/*****************************************************************************/
BOOL psPlayGameSoundOnceDefault(UINT16 soundNo, BOOL isLooping)
{
	if (soundtable[soundNo] != -1)
	{
		// If sound is still playing then exit
		if (audio_isplaying (soundtable[soundNo])) return TRUE;

		// Otherwise play it again (except it it's sound #3)
		if (soundNo == 3) return TRUE;
	}

	float volume = 1.0f;
	if (soundNo == 3) volume = 0.6f;

	soundtable[soundNo] = audio_playsound (soundNo, volume, 0.0f, 1.0f);
	return TRUE;
}

/*****************************************************************************/
void psSetSoundFrequency(UINT16 soundNo, UINT32 frequency)
{
	return;
}

/*****************************************************************************/
void psSetSoundVolume(UINT16 soundNo, INT32 volume)
{
	return;
}

/*****************************************************************************/
void psStopGameSound(UINT16 soundNo)
{
	if (soundtable[soundNo] == -1) return;

	audio_stopsound (soundtable[soundNo]);
	soundtable[soundNo] = -1;
	return;
}

/*****************************************************************************/
void psTermSoundEngine()
{
	return;
}


/*****************************************************************************/
void psStopAllSounds()
{
	audio_stopall(0);
	memset (soundtable, -1, sizeof(soundtable));
	return;
}
