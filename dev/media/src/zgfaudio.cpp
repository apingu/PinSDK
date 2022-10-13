/*
 ZGF Audio module
 (c) 2002 Zona Inc. All rights reserved.
 Support available from http://www.zona.net and mailto:support@zona.net
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

// Windows APIs
#if defined(_WINDOWS)
#include <dmusici.h>
#endif

// XBOX APIs
#if defined(_XBOX)
#include <dmusici.h>
#endif

// PS2 APIs
#if defined(_PS2)
#include <eekernel.h>
#include <sifdev.h>
#include <libsdr.h>
#endif

// Standard C APIs
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

// Module API
#include "..\include\zgfaudio.h"

/******************************************************************************
 Data and Declarations
******************************************************************************/

// Configuration
#define NUMCHANNELS 48						// Number of simultaneous channels supported

// Constants
#define CORESHIFT     (0)					// Bitwise shift for PS2 core specifier in libsd commands
#define VOICESHIFT    (1)					// Bitwise shift for PS2 voice specifier in libsd commands
#define COMMANDSHIFT  (8)					// Bitwise shift for PS2 command specifier in libsd commands

// Channel variables
typedef struct {
	sound*					  sounddata;	// Ptr to sound data for this instance
	float                     x;			// X position of sound
	float                     y;			// Y position of sound
	float                     z;			// Z position of sound
	float					  volume;		// Volume of sound
	float					  pan;			// Pan of sound
	float					  pitch;		// Pitch of sound
	#if defined(_WINDOWS) || defined(_XBOX)
	IDirectMusicSegmentState* state;		// Pointer to DX state object for segment
	IDirectSoundBuffer8*	  buffer8;		// Pointer to DX sound buffer that segment is playing in
	#endif
} _channel;


// Audio variables
static struct {
	#if defined(_WINDOWS) || defined(_XBOX)
	IDirectMusicLoader8*	  loader8;					// DX loader object
	IDirectMusicPerformance8* performance8;				// DX performance object
	IDirectMusicAudioPath*    path;						// DX path object
	#endif
	#if defined(_PS2)
	int                       device;					// Device to load files from
	int						  soundram;					// Next free memory in sound RAM
	#endif
	char					  directory[256];			// Current directory for sound files
	float                     attenuationrange;			// Distance at which sound attenuates to 0 db
	float					  fadetime;					// current fade time for fade out
	float					  fadelength;				// total length of fade out
	_channel				  channels[NUMCHANNELS];	// Data for each channel
} _audio;

// Forward references
#if defined(_PS2)
void _setvolumepan (int channel);
#endif

// PS2 VAG header format
#if defined(_PS2)
typedef struct
{
	unsigned int chunkid;			// "VAGp"
	unsigned int version;
	unsigned int reserved1;
	unsigned int datasize;
	unsigned int frequency;
	unsigned int reserved2;
	unsigned int reserved3;
	unsigned int reserved4;
	char         name[16];
} _vagheader;
#endif

/******************************************************************************
 Initialize the audio system
******************************************************************************/
void ZgfAudioInit (int window, int device)
{
	//---------------------------------------
	// Initialize Windows/XBOX audio
	//---------------------------------------

	#if defined(_WINDOWS)
	// Instance loader and performance objects
	HRESULT result = CoInitialize (NULL);
	assert ((result == S_OK) || (result == S_FALSE));
	result = CoCreateInstance (CLSID_DirectMusicLoader,      NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8,      (void**)&_audio.loader8);
	assert (result == S_OK);
	result = CoCreateInstance (CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&_audio.performance8);
	assert (result == S_OK);

	// Initialize performance object
	result = _audio.performance8->InitAudio (NULL, NULL, (HWND) window, DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, DMUS_AUDIOF_ALL, NULL);
	assert (result == S_OK);
	#endif

	#if defined(_XBOX)
	// Instance loader and performance objects
	HRESULT result = DirectMusicInitialize();
	assert (result == S_OK);
	result = DirectMusicCreateInstance (CLSID_DirectMusicLoader,      NULL, IID_IDirectMusicLoader8,      (void**)&_audio.loader8);
	assert (result == S_OK);
	result = DirectMusicCreateInstance (CLSID_DirectMusicPerformance, NULL, IID_IDirectMusicPerformance8, (void**)&_audio.performance8);
	assert (result == S_OK);

	// Initialize performance object
	result = _audio.performance8->InitAudioX (DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, 0, 0);
	assert (result == S_OK);
	#endif

	#if defined(_WINDOWS) || defined(_XBOX)
	// Tell performance object to generate segment notifications
	result = _audio.performance8->AddNotificationType(GUID_NOTIFICATION_SEGMENT);
	assert (result == S_OK);

	// Get the default audio path for the performance object
	result = _audio.performance8->GetDefaultAudioPath (&_audio.path);
	assert (result == S_OK);
	#endif

	//---------------------------------------
	// Initialize PS2 audio
	//---------------------------------------

	#if defined(_PS2)
	// Record default device
	_audio.device = device;

	// Initialize communication with sound module on IOP
	int result = sceSdRemoteInit();

	// Initialize sound module
	result = sceSdRemote (1, rSdInit, SD_INIT_COLD);

	// Set initial state of SPU cores
	for (int core = 0; core < 2; core++)
	{
		// Set initial core state
		sceSdRemote (1, rSdSetParam, SD_P_MMIX   + core, 0x0c00);	// Mix, Voice Dry Output -> Direct Output
		sceSdRemote (1, rSdSetParam, SD_P_MVOLL  + core, 0x3fff);	// Master volume, full
		sceSdRemote (1, rSdSetParam, SD_P_MVOLR  + core, 0x3fff);	// Master volume, full
		sceSdRemote (1, rSdSetParam, SD_P_EVOLL  + core, 0x0000);	// Effect volume, off
		sceSdRemote (1, rSdSetParam, SD_P_EVOLR  + core, 0x0000);	// Effect volume, off
		sceSdRemote (1, rSdSetParam, SD_P_AVOLL  + core, 0x0000);	// External volume, off
		sceSdRemote (1, rSdSetParam, SD_P_AVOLR  + core, 0x0000);	// External volume, off
		sceSdRemote (1, rSdSetParam, SD_P_BVOLL  + core, 0x0000);	// Sound data volume, off
		sceSdRemote (1, rSdSetParam, SD_P_BVOLR  + core, 0x0000);	// Sound data volume, off

		sceSdRemote (1, rSdSetSwitch, SD_S_PMON   + core, 0x00000000);	// No pitch modulation
		sceSdRemote (1, rSdSetSwitch, SD_S_NON    + core, 0x00000000);	// No noise
		sceSdRemote (1, rSdSetSwitch, SD_S_KON    + core, 0x00000000);	// Clear Key on flags
		sceSdRemote (1, rSdSetSwitch, SD_S_KOFF   + core, 0x00ffffff);	// Key off all voices
		sceSdRemote (1, rSdSetSwitch, SD_S_ENDX   + core, 0x00000000);	// Clear end point flags
		sceSdRemote (1, rSdSetSwitch, SD_S_VMIXL  + core, 0x00ffffff);	// Output to Dry left
		sceSdRemote (1, rSdSetSwitch, SD_S_VMIXR  + core, 0x00ffffff);	// Output to Dry right
		sceSdRemote (1, rSdSetSwitch, SD_S_VMIXEL + core, 0x00000000);	// No output to Wet left
		sceSdRemote (1, rSdSetSwitch, SD_S_VMIXER + core, 0x00000000);	// No output to Wet right

		// Set initial state for each voice
		for (int voice = 0; voice < 24; voice++)	// 24 voices per core
		{
			sceSdRemote (1, rSdSetParam, SD_VP_VOLL  + (voice << VOICESHIFT) + core, 0x0000);	// Volume, off
			sceSdRemote (1, rSdSetParam, SD_VP_VOLR  + (voice << VOICESHIFT) + core, 0x0000);	// Volume, off
			sceSdRemote (1, rSdSetParam, SD_VP_PITCH + (voice << VOICESHIFT) + core, 0x1000);	// No pitch modification
			sceSdRemote (1, rSdSetParam, SD_VP_ADSR1 + (voice << VOICESHIFT) + core, 0x00ff);	// Default envelope
			sceSdRemote (1, rSdSetParam, SD_VP_ADSR2 + (voice << VOICESHIFT) + core, 0x5fc0);	// Default envelope
		}
	}

	// Modify core 1 settings to enable external input from core 0
	sceSdRemote (1, rSdSetParam, SD_P_MMIX  + 1, 0x0c0c);
	sceSdRemote (1, rSdSetParam, SD_P_AVOLL + 1, 0x7fff);	// External volume, full
	sceSdRemote (1, rSdSetParam, SD_P_AVOLR + 1, 0x7fff);	// External volume, full

	// Set sound RAM pointer (byte address)
	_audio.soundram = 0x00005020;
	#endif


	//---------------------------------------
	// Common initialization
	//---------------------------------------

	// Clear the channels
	memset (_audio.channels, 0, sizeof(_audio.channels));

	// Clear the directory
	_audio.directory[0] = 0;

	// Clear the fade function
	_audio.fadetime = 0;

	// Set the default attenuation range
	_audio.attenuationrange = 1000.0f;
}


/******************************************************************************
 Shut down the audio system
******************************************************************************/
void ZgfAudioTerm (void)
{
	// Stop all sounds
	ZgfAudioStopAll();

	#if defined(_WINDOWS) || defined(_XBOX)
	// Shut down the performance object
	HRESULT result = _audio.performance8->CloseDown();
	assert (result == S_OK);

	// Release the loader and performance objects
	int refcount = _audio.loader8->Release(); 
	assert (refcount == 0);
	refcount = _audio.performance8->Release();
	assert (refcount == 0);
	#endif

	#if defined(_WINDOWS)
	// Release the COM library
	CoUninitialize();
	#endif
}


/******************************************************************************
 Update the audio system
******************************************************************************/
void ZgfAudioUpdate
(
	float elapsedtime			// Time in seconds since last update
)
{
	//---------------------------------------
	// Update Windows/XBOX audio
	//---------------------------------------

	#if defined(_WINDOWS) || defined(_XBOX)
	// Process DirectMusic messages
	while (1)
	{
		// Get the next message in the message queue
		DMUS_NOTIFICATION_PMSG* message;
		HRESULT result = _audio.performance8->GetNotificationPMsg (&message);
		assert (!FAILED(result));

		// Exit if there are no more messages, or if there has been a failure
		if ((result == S_FALSE) || (FAILED(result))) break;

		// We're only interested in segment end messages, so look for those
		if ((message->guidNotificationType == GUID_NOTIFICATION_SEGMENT) && (message->dwNotificationOption == DMUS_NOTIFICATION_SEGEND))
		{
			IDirectMusicSegmentState* state;

			// Get the segment state that ended
			state = (IDirectMusicSegmentState*) message->punkUser;

			// Find the channel that has a matching segment state
			for (int i = 0; i < NUMCHANNELS; i++)
			{
				if (_audio.channels[i].state == state)
				{
					// Dereference the state and buffer objects for this sound
					int refcount = _audio.channels[i].state->Release();
					    refcount = _audio.channels[i].buffer8->Release();

					// Indicate that channel is no longer in use and stop search
					_audio.channels[i].sounddata = 0;
					break;
				}
			}
		}
	}
	#endif


	//---------------------------------------
	// Update PS2 audio
	//---------------------------------------

	#if defined(_PS2)
	// Reclaim voices that have completed playback
	for (int i = 0; i < 2; i++)
	{
		// Read the end flags for this core
		int endpoints = sceSdRemote (1, rSdGetSwitch, SD_S_ENDX + i);

		// Check each end flag for playback completion
		for (int j = 0; j < 24; j++)
		{
			// If the end flag is set then clear the channel
			if (endpoints & (1 << j)) _audio.channels[(j<<1) + i].sounddata = 0;
		}
	}
	#endif


	//---------------------------------------
	// Common update
	//---------------------------------------

	// Process the fade out if active
	if (_audio.fadetime != 0)
	{
		// Tick off some time
		_audio.fadetime -= elapsedtime;

		if (_audio.fadetime > 0)
		{
			// We're not done yet so just reduce master volume
			ZgfAudioSetMasterVolume (_audio.fadetime / _audio.fadelength);
		}
		else
		{
			// We're done so stop all channels and reset master volume
			_audio.fadetime = 0;
			ZgfAudioStopAll();
			ZgfAudioSetMasterVolume (1.0f);
		}
	}
}


/******************************************************************************
 Set the directory to load sound files from
******************************************************************************/
void ZgfAudioSetDirectory
(
	const char* directory
)
{
	// If a null directory is specified then clear the directory and exit
	if ((!directory) || (*directory == 0)) {_audio.directory[0] = 0; return;}

	// Ensure directory name will not overflow our buffer
	assert (strlen(directory) < sizeof(_audio.directory)-1);

	// Make a copy of the directory
	strcpy (_audio.directory, directory);

	// Append a '\' if none is present
	char* end = &_audio.directory[strlen(_audio.directory)-1];
	if ((*end != '\\') && (*end != '/')) {*++end = '\\'; *++end = 0;}
}


/******************************************************************************
 Load a list of sound files
******************************************************************************/
void ZgfAudioLoadSounds (sound* soundlist, int sizeofsoundlist)
{
	// Calculate number of sounds in list
	int numsounds = sizeofsoundlist / sizeof(sound);

	//----------------------------
	// Load Windows/XBOX files
	//----------------------------

	#if defined(_WINDOWS) || defined(_XBOX)
	// Load all files in sound list
	for (int i = 0; i < numsounds; i++)
	{
		//-----------------------------------------------------
		// Build a device compliant path to the specified file
		//-----------------------------------------------------

		// Combine drive, directory, filename and extension into a single path
		char fullpath[256];
		fullpath[0] = 0;
		#if defined(_XBOX)
		strcpy (fullpath, "D:\\");
		#endif
		strcat (fullpath, _audio.directory);
		strcat (fullpath, soundlist[i].filename);
		strcat (fullpath, ".wav");
		assert (strlen(fullpath) < sizeof(fullpath));

		// Apply formatting rules to path
		char* p = fullpath;
		while (*p) {*p = (*p == '/') ? '\\': *p; p++;}

		//---------------------------------------------------------
		// Read waveform into a segment and initialize the segment
		//---------------------------------------------------------

		#if defined(_WINDOWS)
		// Convert ASCII path to a wide character path and load file into segment
		unsigned short  widepath[256];
		unsigned short* w = widepath;
		char*           x = fullpath;
		while (*x) (*w++ = *x++);
		*w = 0;
		HRESULT result = _audio.loader8->LoadObjectFromFile (CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, widepath, (LPVOID*) &soundlist[i].data);
		#endif

		#if defined(_XBOX)
		// Load file into segment
		HRESULT result = _audio.loader8->LoadObjectFromFile (CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, fullpath, (LPVOID*) &soundlist[i].data);
		#endif

		// If file loaded then initialize segment
		if (!FAILED(result))
		{
			// Set looping parameter for segment
			if (soundlist[i].flags & ZGFAUDIO_FLAG_LOOPING) ((IDirectMusicSegment8*) soundlist[i].data)->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
			else                                           ((IDirectMusicSegment8*) soundlist[i].data)->SetRepeats(0);

			// Download the segment
			result = ((IDirectMusicSegment8*) soundlist[i].data)->Download (_audio.performance8);
			//assert (result == S_OK);	// can fail if waveform was already loaded in another sound list
		}
		else
		{
			// Load failed so make sure data pointer is clear
			soundlist[i].data = 0;
		}
	}
	#endif


	//----------------------------
	// Load PS2 files
	//----------------------------

	#if defined(_PS2)
	// Wait until the SPU transfer channel is available
	while (sceSdRemote (1, rSdVoiceTransStatus, 0, SD_TRANS_STATUS_WAIT) == 0);

	// Load all files in sound list
	for (int i = 0; i < numsounds; i++)
	{
		//-----------------------------------------------------
		// Build a device compliant path to the specified file
		//-----------------------------------------------------

		// Combine device name, directory, filename, extension, and version number into a single path
		char fullpath[256];
		if (_audio.device == 0) strcpy (fullpath, "host0:");
		else                    strcpy (fullpath, "cdrom0:\\");
		strcat (fullpath, _audio.directory);
		strcat (fullpath, soundlist[i].filename);
		strcat (fullpath, ".vag");
		if (_audio.device != 0) strcat (fullpath, ";1");
		assert (strlen(fullpath) < sizeof(fullpath));

		// Apply formatting rules to path
		char* p = fullpath;
		while (*p != ':') p++;		// Skip device name
		if (_audio.device == 0) while (*p) {*p = (*p == '\\') ? '/' : *p; p++;}
		else                    while (*p) {*p = (*p == '/')  ? '\\': ((*p >= 'a') && (*p <= 'z')) ? *p - 0x20: *p; p++;}

		//-----------------------------------------------------
		// Read and process the VAG header
		//-----------------------------------------------------

		// Open the waveform file, read the 48-byte VAG header, and get the file size
		_vagheader vagheader;
		int handle = sceOpen (fullpath, SCE_RDONLY, 0);
		assert (handle >= 0);
		int bytesread = sceRead (handle, &vagheader, sizeof(vagheader));
		assert (bytesread == sizeof(vagheader));
		int filesize = sceLseek (handle, 0, SCE_SEEK_END);
		assert (filesize >= 0);
		int result = sceClose (handle);
		assert (result == 0);

		// The VAG header is in big-endian format so reverse the values we're interested in
		unsigned char *r;
		r = (unsigned char*) &vagheader.datasize;
		int datasize  = (*r << 24) + (*(r+1) << 16) + (*(r+2) << 8) + *(r+3);
		r = (unsigned char*) &vagheader.frequency;
		int frequency = (*r << 24) + (*(r+1) << 16) + (*(r+2) << 8) + *(r+3);

		// Validate VAG header data
		assert (vagheader.chunkid == 0x70474156);						// "VAGp"
		assert ((datasize + (int) sizeof(vagheader)) == filesize);

		// Store waveform frequency
		soundlist[i].frequency = frequency;

		//-----------------------------------------------------
		// Read and upload the waveform data
		//-----------------------------------------------------

		// Allocate a buffer on the IOP to receive the file data
		void* iopbuffer = sceSifAllocIopHeap (filesize);

		// Load the waveform file to the IOP buffer
		result = sceSifLoadIopHeap (fullpath, iopbuffer);
		assert (result == 0);

		// Upload the waveform data, excluding the 48-byte VAG header, to sound RAM
		sceSdRemote (1, rSdVoiceTrans, 0, SD_TRANS_MODE_WRITE + SD_TRANS_BY_IO, ((int)iopbuffer)+sizeof(vagheader), _audio.soundram, filesize-sizeof(vagheader));
		while (sceSdRemote (1, rSdVoiceTransStatus, 0, SD_TRANS_STATUS_WAIT) == 0);

		// Free the IOP buffer
		result = sceSifFreeIopHeap (iopbuffer);
		assert (result == 0);

		// Store sound RAM address for this waveform
		soundlist[i].data = _audio.soundram;

		// Increment and align sound RAM pointer
		_audio.soundram += (filesize - sizeof(vagheader));
		_audio.soundram  = (_audio.soundram + 15) & -16;

		// Error if we ran out of sound ram
		assert (_audio.soundram < 2*1024*1024);
	}
	#endif
}

/******************************************************************************
 Unload a list of sound files
 Note lists must be unloaded in the reverse order that they were loaded
******************************************************************************/
void ZgfAudioUnloadSounds (sound* soundlist, int sizeofsoundlist)
{
	// Calculate number of sounds in list
	int numsounds = sizeofsoundlist / sizeof(sound);

	//----------------------------
	// Unload Windows/XBOX files
	//----------------------------

	#if defined(_WINDOWS) || defined(_XBOX)
	// Force the cache to release its reference to segments
	HRESULT result = _audio.loader8->ClearCache(CLSID_DirectMusicSegment);
	assert (result == S_OK);

	// Unload each sound and release the reference to its segment
	for (int i = 0; i < numsounds; i++)
	{
		if (soundlist[i].data)
		{
			// Unload and release waveform
			HRESULT result = ((IDirectMusicSegment8*)soundlist[i].data)->Unload(_audio.performance8);
			assert (result == S_OK);
			int refcount = ((IDirectMusicSegment8*)soundlist[i].data)->Release();
			//assert (refcount == 0);	// can fail if sound just stopped playing

			// Clear sound data pointer
			soundlist[i].data = 0;
		}
	}
	#endif

	//----------------------------
	// Unload PS2 files
	//----------------------------

	#if defined(_PS2)
	// Move sound RAM pointer back to address of first waveform (byte address)
	_audio.soundram = soundlist[0].data;

	// Clear waveform data pointers (not entirely necessary but could be useful for debugging)
	for (int i = 0; i < numsounds; i++) soundlist[i].data = 0;
	#endif
}


/******************************************************************************
 Set the distance at which sound attenuates to 0 db
******************************************************************************/
void ZgfAudioSetListenerRange
(
	float range			// distance in world units at which sound attenuates to 0 db
)
{
	_audio.attenuationrange = range;
}


/******************************************************************************
 Set the master volume
******************************************************************************/
void ZgfAudioSetMasterVolume
(
	float volume		// volume (0.0f to 1.0f)
)
{
	//--------------------------------
	// Set Windows/XBOX master volume
	//--------------------------------

	#if defined(_WINDOWS) || defined(_XBOX)
	// Convert volume to attenuation. The maximum attenuation settable for the audio path is 9600db,
	// however if the volume is not 0, then we range from -2400db to 0db as this seems to give values
	// that sound about right.
	int attenuation;
	if (volume == 0) attenuation = -9600;
	else			 attenuation = (int) ((volume - 1.0f) * 2400.0f);

	// Set the volume for the audio path
	_audio.path->SetVolume (attenuation, 0);
	#endif

	//--------------------------------
	// Set PS2 master volume
	//--------------------------------

	#if defined(_PS2)
	// Set the master volume on both cores
	sceSdRemote (1, rSdSetParam, SD_P_MVOLL + 0, (int) (volume * 16383.0f));
	sceSdRemote (1, rSdSetParam, SD_P_MVOLR + 0, (int) (volume * 16383.0f));
	sceSdRemote (1, rSdSetParam, SD_P_MVOLL + 1, (int) (volume * 16383.0f));
	sceSdRemote (1, rSdSetParam, SD_P_MVOLR + 1, (int) (volume * 16383.0f));
	#endif
}


/******************************************************************************
 Play a sound
******************************************************************************/
int ZgfAudioPlaySound		// returns channel sound is playing on, or -1 if playback failed
(
	sound* sounddata,	// ptr to sound data
	float x,			// X position in world units
	float y,			// Y position in world units
	float z				// Z position in world units
)
{
	// Verify parameters
	assert (sounddata);

	// Exit if there is no sound data for this sound
	if (sounddata->data == NULL) return -1;

	// If single instance flag is set then check active voices and exit if this sound is already playing
	if (sounddata->flags & ZGFAUDIO_FLAG_SINGLE)
	{
		for (int i = 0; i < NUMCHANNELS; i++) if (_audio.channels[i].sounddata == sounddata) {return i;}
	}

	// Find a free channel, exit if none is available
	int channel = -1;
	{for (int i = 0; i < NUMCHANNELS; i++) if (_audio.channels[i].sounddata == 0) {channel = i; break;}}
	if (channel == -1) return -1;

	// Record the sound for this channel
	_audio.channels[channel].sounddata = sounddata;

	// Calulate the length of the position vector
	float length = sqrtf((x*x)+(y*y)+(z*z));

	// Calculate volume (0.0f to 1.0f) as the length of the position vector relative to the attenuation range
	float volume = (_audio.attenuationrange - length) / _audio.attenuationrange;
	if (volume < 0.0f) volume = 0.0f;

	// Apply to base volume of the sound
	volume *= sounddata->volume;

	// Calculate pan as the cosine of the vector angle on the XZ plane
	float pan;
	if (length) pan = x/length;
	else        pan = 0.0f;

	// Get the base pitch of the sound
	float pitch = sounddata->pitch;

	//----------------------------------------
	// Play waveform on Windows/XBOX
	//----------------------------------------

	#if defined(_WINDOWS)
	// Play the segment containing the requested sound
	HRESULT result = _audio.performance8->PlaySegmentEx ((IDirectMusicSegment8*) sounddata->data, NULL, NULL, DMUS_SEGF_SECONDARY, 0, &_audio.channels[channel].state, NULL, NULL);
	assert (result == S_OK);

	// Find the sound buffer being used for this sound
	IDirectMusicSegmentState8* state8;
	result = _audio.channels[channel].state->QueryInterface (IID_IDirectMusicSegmentState8, (void**) &state8);
	assert (result == S_OK);
	int refcount = state8->AddRef();
	result = state8->GetObjectInPath (0, DMUS_PATH_BUFFER, 0, GUID_All_Objects, 0, IID_IDirectSoundBuffer8, (void**) &_audio.channels[channel].buffer8);
	assert (result == S_OK);
	refcount = state8->Release();

	// Set volume, pan, and pitch
	ZgfAudioSetVolume (channel, volume);
	ZgfAudioSetPan    (channel, pan);
	ZgfAudioSetPitch  (channel, pitch);
	#endif

	#if defined(_XBOX)
	// Play the segment containing the requested sound
	HRESULT result = _audio.performance8->PlaySegmentEx ((IDirectMusicSegment8*) sounddata->data, NULL, NULL, DMUS_SEGF_SECONDARY, 0, &_audio.channels[channel].state, NULL, NULL);
	assert (result == S_OK);
	#endif

	//----------------------------------------
	// Play waveform on PS2
	//----------------------------------------

	#if defined(_PS2)
	// Determine the core, voice, and voice bit for this channel
	int core  = channel & 1;
	int voice = channel/2;
	int bit   = 1 << voice;

	// Set the waveform address for the voice
	sceSdRemote (1, rSdSetAddr, SD_VA_SSA + channel, sounddata->data);

	// Clear the voice endpoint bit
	int endpoints = sceSdRemote (1, rSdGetSwitch, SD_S_ENDX + core);
	endpoints &= ~bit;
	int result = sceSdRemote (1, rSdSetSwitch, SD_S_ENDX + core, endpoints);

	// Set volume, pan, and pitch
	_audio.channels[channel].volume = volume;
	_audio.channels[channel].pan    = pan;
	_setvolumepan(channel);
	ZgfAudioSetPitch  (channel, pitch);

	// Key on the voice
	result = sceSdRemote (1, rSdSetSwitch, SD_S_KON + core, bit);
	#endif


	// Return the channel number for this sound
	return channel;
}


/******************************************************************************
 Set the position of a channel
******************************************************************************/
void ZgfAudioSetPosition
(
	int   channel,		// channel number
	float x,			// X position in world units
	float y,			// Y position in world units
	float z				// Z position in world units
)
{
	// Validate parameters
	if (channel == -1) return;
	assert (channel < NUMCHANNELS);

	// Calulate the length of the position vector
	float length = sqrtf((x*x)+(y*y)+(z*z));

	// Calculate volume (0.0f to 1.0f) as the length of the position vector relative to the attenuation range
	float volume = (_audio.attenuationrange - length) / _audio.attenuationrange;
	if (volume < 0.0f) volume = 0.0f;

	// Apply to base volume of the sound
	volume *= _audio.channels[channel].sounddata->volume;

	// Calculate pan as the cosine of the vector angle on the XZ plane
	float pan;
	if (length) pan = x/length;
	else        pan = 0.0f;

	// Get the base pitch of the sound
	float pitch = _audio.channels[channel].sounddata->pitch;

	// If doppler flag is set for this sound then adjust the pitch as necessary
	if (_audio.channels[channel].sounddata->flags & ZGFAUDIO_FLAG_DOPPLER)
	{
		// Calculate distance of sound at last position
		float lastx      = _audio.channels[channel].x;
		float lasty      = _audio.channels[channel].y;
		float lastz      = _audio.channels[channel].z;
		float lastlength = sqrtf((lastx*lastx)+(lasty*lasty)+(lastz*lastz));
		if (lastlength < 1.0f) lastlength = 1.0f;

		// Calculate the relative change in distance
		float change = 1.0f + ((lastlength - length) / lastlength);
		if (change > 2.0f) change = 2.0f;
		if (change < 0.5f) change = 0.5f;

		// Modify the pitch in proportion to the change in distance
		pitch *= change;
	}

	// Set volume pan and pitch of sound
	ZgfAudioSetVolume (channel, volume);
	ZgfAudioSetPan    (channel, pan);
	ZgfAudioSetPitch  (channel, pitch);

	// Keep a copy of the position
	_audio.channels[channel].x = x;
	_audio.channels[channel].y = y;
	_audio.channels[channel].z = z;
}


/******************************************************************************
 Set the volume of a channel
******************************************************************************/
void ZgfAudioSetVolume
(
	int   channel,		// channel number
	float volume		// volume (0.0f to 1.0f)
)
{
	// Validate parameters
	if (channel == -1) return;
	assert (channel < NUMCHANNELS);

	// Record the volume for this channel
	_audio.channels[channel].volume = volume;

	//-------------------------------
	// Set Windows/XBOX volume
	//-------------------------------

	#if defined(_WINDOWS)
	// Convert volume to attenuation. The maximum attenuation settable for the sound buffer is 10,000db,
	// however if the volume is not 0, then we range from -2400db to 0db as this seems to give values
	// that sound about right.
	int attenuation;
	if (volume == 0) attenuation = -10000;
	else			 attenuation = (int) ((volume - 1.0f) * 2400.0f);

	// Set the volume of the sound buffer for this instance
	HRESULT result = _audio.channels[channel].buffer8->SetVolume (attenuation);
	assert (result == S_OK);
	#endif

	//-------------------------------
	// Set PS2 volume
	//-------------------------------

	#if defined(_PS2)
	// Update hardware settings for volume and pan
	_setvolumepan(channel);
	#endif
}


/******************************************************************************
 Set the pan of a currently playing instance
******************************************************************************/
void ZgfAudioSetPan
(
	int   channel,		// channel number
	float pan			// pan (-1.0f to 1.0f)
)
{
	// Validate parameters
	if (channel == -1) return;
	assert (channel < NUMCHANNELS);

	// Record the pan for this channel
	_audio.channels[channel].pan = pan;

	//-------------------------------
	// Set Windows/XBOX pan
	//-------------------------------

	#if defined(_WINDOWS)
	// Convert volume to attenuation. The maximum attenuation settable for the sound buffer is 10,000db,
	// however range from -2400db to +2400db as this seems to give values that sound about right.
	int attenuation = (int) (pan * 2400.0f);

	// Set the volume of the sound buffer for this instance
	HRESULT result = _audio.channels[channel].buffer8->SetPan (attenuation);
	assert (result == S_OK);
	#endif

	//-------------------------------
	// Set PS2 pan
	//-------------------------------

	#if defined(_PS2)
	// Update hardware settings for volume and pan
	_setvolumepan(channel);
	#endif
}


/******************************************************************************
 Set the pitch of a channel
******************************************************************************/
void ZgfAudioSetPitch
(
	int   channel,		// channel number
	float pitch			// pitch (0.0f to 4.0f)
)
{
	// Validate parameters
	if (channel == -1) return;
	assert (channel < NUMCHANNELS);

	// Record the pitch for this channel
	_audio.channels[channel].pitch = pitch;

	//-------------------------------
	// Set Windows/XBOX pitch
	//-------------------------------

	#if defined(_WINDOWS)
	// Get the waveform frequency from the sound buffer
	WAVEFORMATEX waveformat;
	HRESULT result = _audio.channels[channel].buffer8->GetFormat (&waveformat, sizeof(waveformat), NULL);
	int frequency = waveformat.nSamplesPerSec;

	// Calculate new frequency
	frequency = (int) (pitch * (float) frequency);

	// Write new frequency to sound buffer
//	result = _audio.channels[channel].buffer8->SetFrequency (frequency);
	#endif

	//-------------------------------
	// Set PS2 pitch
	//-------------------------------

	#if defined(_PS2)
	// Get the recorded frequency of the waveform that's playing
	int frequency = _audio.channels[channel].sounddata->frequency;

	// Correct pitch for recorded frequency
	float correction = (float) frequency / 48000.0f;
	pitch *= correction;

	// Write new pitch to voice
	sceSdRemote (1, rSdSetParam, SD_VP_PITCH + channel, (short) (pitch * 4096.0f));
	#endif
}


/******************************************************************************
 Write PS2 volume and pan information to voice
******************************************************************************/
#if defined(_PS2)
void _setvolumepan
(
	int	channel		// channel number
)
{
	// Retrieve volume and pan
	float volume = _audio.channels[channel].volume;
	float pan    = _audio.channels[channel].pan;

	// Calculate separate pan values for left and right outputs
	float rightpan = ((pan + 1.0f) / 2.0f);
	float leftpan  = 1.0f - rightpan;

	// Calculate volumes for left and right outputs
	int rightvolume = (int) (rightpan * volume * 16383.0f);
	int leftvolume  = (int) (leftpan  * volume * 16383.0f);

	// Write left\right volume values for this voice
	sceSdRemote (1, rSdSetParam, SD_VP_VOLL + channel, leftvolume);
	sceSdRemote (1, rSdSetParam, SD_VP_VOLR + channel, rightvolume);
}
#endif

/******************************************************************************
 Stop a channel
******************************************************************************/
void ZgfAudioStopSound
(
	int channel			// channel number
)
{
	// Validate parameters
	if (channel == -1) return;
	assert (channel < NUMCHANNELS);

	// Stop channel
	if (_audio.channels[channel].sounddata)
	{
		//-------------------------
		// Stop Windows/XBOX sound
		//-------------------------

		#if defined(_WINDOWS) || defined(_XBOX)
		// Stop segment playback
		HRESULT result = _audio.performance8->StopEx (_audio.channels[channel].state, 0, 0);
		assert (result == S_OK);

		// Dereference the state and buffer objects
		int refcount = _audio.channels[channel].state->Release();
		    refcount = _audio.channels[channel].buffer8->Release();
		#endif

		//-------------------------
		// Stop PS2 sound
		//-------------------------

		#if defined(_PS2)
		// Determine the core, voice, and voice bit for this channel
		int core  = channel & 1;
		int voice = channel/2;
		int bit   = 1 << voice;

		// Key off the specified voice
		sceSdRemote (1, rSdSetSwitch, SD_S_KOFF + core, bit);
		#endif

		// Indicate that channel is no longer in use
		_audio.channels[channel].sounddata = 0;
	}
}


/******************************************************************************
 Stop all sound
******************************************************************************/
void ZgfAudioStopAll (void)
{
	//------------------------------
	// Stop all Windows/XBOX sounds
	//------------------------------
	#if defined(_WINDOWS) || defined(_XBOX)
	for (int i = 0; i < NUMCHANNELS; i++) ZgfAudioStopSound(i);
	#endif

	//------------------------------
	// Stop all PS2 sounds
	//------------------------------
	#if defined(_PS2)
	// Key off all voices on both cores
	sceSdRemote (1, rSdSetSwitch, SD_S_KOFF + 0, 0x00ffffff);
	sceSdRemote (1, rSdSetSwitch, SD_S_KOFF + 1, 0x00ffffff);

	// Clear all channels
	for (int i = 0; i < NUMCHANNELS; i++) _audio.channels[i].sounddata = 0;
	#endif
}

/******************************************************************************
 Fade out all sound
******************************************************************************/
void ZgfAudioFade
(
	float fadetime
)
{
	_audio.fadetime   = fadetime;
	_audio.fadelength = fadetime;
}


