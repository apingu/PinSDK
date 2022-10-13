/*
 Audio module
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

#ifndef _AUDIO_H
#define _AUDIO_H

// Sound description
typedef struct {
	char*        filename;			// waveform filename (exclude file extension)
	float        volume;			// default volume
	float        pitch;				// default pitch
	unsigned int flags;				// see below
	unsigned int frequency;			// do not set, set internally
	unsigned int data;				// do not set, set internally
} sound;

// Sound flags
#define AUDIO_FLAG_LOOPING (0x01)	// Set to loop sound (on PS2 waveform must be a looping waveform)
#define AUDIO_FLAG_SINGLE  (0x02)	// Set to signal that there can only be a single instance of this sound playing
#define AUDIO_FLAG_DOPPLER (0x04)	// Set to apply doppler shift if sound is moved

// Audio functions
void audio_init             (int window, int device);
void audio_update           (float elapsedtime);
void audio_deinit           (void);
void audio_setdirectory     (const char* directory);
void audio_loadsounds       (sound* soundlist, int sizeofsoundlist);
void audio_unloadsounds     (void);
void audio_setlistenerrange (float range);
void audio_setmastervolume  (float volume);
int  audio_playsound        (int sound, float x, float y, float z);
void audio_stopsound        (int channel);
void audio_setposition      (int channel, float x, float y, float z);
void audio_setvolume        (int channel, float volume);
void audio_setpitch	        (int channel, float pitch);
void audio_setpan           (int channel, float pan);
void audio_stopall          (void);
void audio_fadeout          (float fadetime);

#endif
