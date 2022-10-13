/*
 Timer module
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

// Windows APIs
#if defined(_WINDOWS)
#define  WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// XBOX APIs
#if defined(_XBOX)
#include <xtl.h>
#endif

// PS2 APIs
#if defined(_PS2)
#include <eeregs.h>
#endif

// Standard C APIs
#include <assert.h>

// Application APIs
#include "ztimer.h"


/******************************************************************************
 Data and Declarations
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/

// Timer variables
#if defined(_WINDOWS) || defined(_XBOX)
static struct {
	LARGE_INTEGER base;
	float		  frequency;
} _timer;
#endif

// global elapsed time and frame counter
float elapsedtime;
int   currentframe;

float gCurrentTime;
/******************************************************************************
 Initialize timer
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void timer_init (void)
{
	timer_reset();
	currentframe = 1;
}


/******************************************************************************
 Reset the timer
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void timer_reset (void)
{
#if defined(_WINDOWS) || defined(_XBOX)
	// Store the frequency of the performance counter
	LARGE_INTEGER frequency;
	int result = QueryPerformanceFrequency (&frequency);
	assert (result);
	_timer.frequency = (float) frequency.LowPart;

	// Store the current value of the performance counter
	result = QueryPerformanceCounter (&_timer.base);
	assert (result);
#endif

#if defined(_PS2)
	// Start timer 0 with a frequency of 0.586Mhz (300Mhz/256)
	*T0_MODE  = 0x00000082;
	*T0_COUNT = 0x00000000;
#endif

	// Clear the elapsed time
	elapsedtime  = 0.0f;

    // TODO: Alex, reset?? init??
    gCurrentTime = 0.f;
}

/******************************************************************************
 Latch hardware timer and calculate elapsed time since last latch
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void timer_update
(
	int paused			// TRUE if application is paused
)
{
#if defined(_WINDOWS) || defined(_XBOX)
	// Get the current value of the performance counter
	LARGE_INTEGER time;
	int result = QueryPerformanceCounter (&time);
	assert (result);

	// Calculate time elapsed in seconds
	LARGE_INTEGER delta;
	delta.QuadPart = time.QuadPart - _timer.base.QuadPart;
	elapsedtime = ((float) delta.LowPart) / _timer.frequency;

	// Save the current performance counter
	_timer.base.QuadPart = time.QuadPart;
#endif

#if defined(_PS2)
	// Get the current timer value (lower 16 bits hold timer, upper 16 bits are fixed at 0)
	unsigned int count = *T0_COUNT;

	// Calculate time elapsed in seconds
	elapsedtime = ((float) count) / 585938.0f;

	// Reset the timer
	*T0_COUNT = 0x00000000;
#endif

	// Ensure elapsed time never exceeds 0.2 seconds (prevents glithcing when debugging)
	if (elapsedtime > 0.2f) elapsedtime = 0.2f;

	// Update the frame counter
	if (!paused)
    {
        gCurrentTime += elapsedtime;
        currentframe++;
    }

	// If we're paused then no time has elapsed
	if (paused)  elapsedtime = 0;
}


/******************************************************************************
 INLINE: Get time in seconds between calls to timer_update()
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
float _timer_getelapsed (void);

/******************************************************************************
 INLINE: Get number of game frames since initialization
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
float _timer_getframe (void);

/******************************************************************************
 INLINE: Get current time since initialization
 // TODO: Alex, the mechanism not tested.
******************************************************************************/
float _timer_getcurrent (void);
