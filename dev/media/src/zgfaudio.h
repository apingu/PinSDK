/*
 ZGF Audio module
 (c) 2002 Zona Inc. All rights reserved.
 Support available from http://www.zona.net and mailto:support@zona.net
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

#ifndef _ZGFAUDIO_H
#define _ZGFAUDIO_H

// Description of a sound file
typedef struct {
	char*        filename;			// waveform filename (exclude file extension)
	float        volume;			// default volume
	float        pitch;				// default pitch
	unsigned int flags;				// see below
	unsigned int frequency;			// do not set, set internally
	unsigned int data;				// do not set, set internally
} sound;

// Flags for sound playback
#define ZGFAUDIO_FLAG_LOOPING (0x01)	// Set to loop sound (on PS2 waveform must be a looping waveform)
#define ZGFAUDIO_FLAG_SINGLE  (0x02)	// Set to signal that there can only be a single instance of this sound playing
#define ZGFAUDIO_FLAG_DOPPLER (0x04)	// Set to apply doppler shift if sound is moved

// Audio functions
void ZgfAudioInit             (int window, int device);
void ZgfAudioUpdate           (float elapsedtime);
void ZgfAudioTerm             (void);
void ZgfAudioSetDirectory     (const char* directory);
void ZgfAudioLoadSounds       (sound* soundlist, int sizeofsoundlist);
void ZgfAudioUnloadSounds     (sound* soundlist, int sizeofsoundlist);
void ZgfAudioSetListenerRange (float range);
void ZgfAudioSetMasterVolume  (float volume);
int  ZgfAudioPlaySound        (sound* sounddata, float x, float y, float z);
void ZgfAudioStopSound        (int channel);
void ZgfAudioSetPosition      (int channel, float x, float y, float z);
void ZgfAudioSetVolume        (int channel, float volume);
void ZgfAudioSetPitch	      (int channel, float pitch);
void ZgfAudioSetPan           (int channel, float pan);
void ZgfAudioStopAll          (void);
void ZgfAudioFade             (float fadetime);

#endif
