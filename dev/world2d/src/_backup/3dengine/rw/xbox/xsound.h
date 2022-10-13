/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: sound.h
 * Original Author: Mark Harmon
 ********************************************************************/
#ifndef SOUND_H
#define SOUND_H

#include <rwcore.h>

#if (!defined(_XBOX))
#include <windows.h>
#include <mmsystem.h>
#endif

#if defined(_XBOX)
#include <xtl.h>
#endif

#include <dsound.h>

enum GameSound
{
  SND_ENGINE_START, 
  SND_ENGINE1, 
  SND_DRIVE_DIRT, 
  SND_CARSKID1,
  SND_COLLIDE2,
  SND_GUNEMPTY,
  SND_LMACHINEGUN, 
  SND_GUNHIT,
  NUM_SOUNDS,
};

#define SND_VOLUME_MIN DSBVOLUME_MIN
#define SND_VOLUME_MAX DSBVOLUME_MAX
#define SND_VOLUME_RANGE abs(DSBVOLUME_MAX - DSBVOLUME_MIN)

#define SND_FREQUENCY_ORIG DSBFREQUENCY_ORIGINAL
#define SND_FREQUENCY_MIN DSBFREQUENCY_MIN 
#define SND_FREQUENCY_MAX DSBFREQUENCY_MAX 

/**
 * Initializes the sound engine. This sound library uses directx.
 * @param hwnd the handle to the main window
 *****************************************************************************/
BOOL psInitSoundEngine();

/**
 * Terminates the sound engine.
 *****************************************************************************/
void psTermSoundEngine();

/**
 * Plays one of the game sounds
 * @param soundNo one of the enumerated sounds
 * @param isLooping specifies whether or not to play the sound in a loop. Call
 *        the function StopGameSound to stop the sound from playing.
 *****************************************************************************/
void psPlayGameSoundDefault(UINT16 soundNo, BOOL isLooping);

/**
 * Plays one of the game sounds if it is not already playing, otherwise
 * sound is allowed to continue playing.
 * @param soundNo one of the enumerated sounds
 * @param isLooping specifies whether or not to play the sound in a loop. Call
 *        the function StopGameSound to stop the sound from playing.
 *****************************************************************************/
void psPlayGameSoundOnceDefault(UINT16 soundNo, BOOL isLooping);

/**
 * Stops one of the game sounds
 * @param soundNo one of the enumerated sounds loaded into the sound engine
 *****************************************************************************/
void psStopGameSound(UINT16 soundNo);

/**
 * Sets the frequency (pitch) of the specified sound. If the sound is already
 * playing then the change happens without stopping the sound.
 * @param soundNo one of the enumerated sounds loaded into the sound engine
 * @param frequency the desired frequency of the sound. The value must be in 
                    the range DSBFREQUENCY_MIN to DSBFREQUENCY_MAX. 
 *****************************************************************************/
void psSetSoundFrequency(UINT16 soundNo, UINT32 frequency);

/**
 * Stops one of the game sounds
 * @param soundNo one of the enumerated sounds loaded into the sound engine
 * @param volume a value representing the volume. For windows this value 
 *               ranges from DSBVOLUME_MIN to DSBVOLUME_MAX.
 *****************************************************************************/
void psSetSoundVolume(UINT16 soundNo, INT32 volume);

void psStopAllSounds();
#endif