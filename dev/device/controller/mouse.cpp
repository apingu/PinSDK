/*
 ZGF Mouse module
 (c) 2002 Zona Inc. All rights reserved.
 Support available from http://www.zona.net and mailto:support@zona.net
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

// Windows APIs
#if defined(_WINDOWS)
#define  DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#endif

// Standard C APIs
#include <assert.h>
#include <string.h>

// Module API
#include "..\include\zgfmouse.h"


/******************************************************************************
 Data and Declarations
******************************************************************************/

static struct {
	#if defined(_WINDOWS)
	LPDIRECTINPUT8		 di8;			// Pointer to Direct Input 8 object
	LPDIRECTINPUTDEVICE8 device;		// Pointer to mouse device
	#endif
	float				 deltax;
	float				 deltay;
	float				 deltaz;
	unsigned char        button[4];
} _mouse;


/******************************************************************************
 Initialize the mouse module
******************************************************************************/
void ZgfMouseInit
(
	int window,				// application window handle
	int instance			// application instance handle
)
{
	// Clear mouse variables
	memset (&_mouse, 0, sizeof(_mouse));

	#if defined(_WINDOWS)
	// Create the direct input object
	HRESULT result = DirectInput8Create ((HINSTANCE) instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &_mouse.di8, NULL);
	assert (result == DI_OK);

	// Create the mouse device
	result = _mouse.di8->CreateDevice(GUID_SysMouse, &_mouse.device, NULL); 
	if (result == DI_OK)
	{
		// Set the data format for the mouse
		result = _mouse.device->SetDataFormat(&c_dfDIMouse);
		assert (result == DI_OK);

		// Set the cooperative level for the mouse
		result = _mouse.device->SetCooperativeLevel((HWND) window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
		assert (result == DI_OK);
	}

	// Perform an initial update
	ZgfMouseUpdate();
	#endif
}


/******************************************************************************
 Terminate the mouse module
******************************************************************************/
void ZgfMouseTerm (void)
{
	#if defined(_WINDOWS)
	// Release the mouse device
	if (_mouse.device)
	{
		// Unacquire the mouse device
		HRESULT result = _mouse.device->Unacquire();	// This function call can fail, but that's okay

		// Release the mouse device
		int refcount = _mouse.device->Release();
		assert (refcount == 0);
	}

	// Release the direct input object
	int refcount = _mouse.di8->Release();
	assert (refcount == 0);
	#endif
}


/******************************************************************************
 Latch current state if mouse
******************************************************************************/
void ZgfMouseUpdate (void)
{
	#if defined(_WINDOWS)

	// Acquire the mouse and get its state
	DIMOUSESTATE state;
	HRESULT result = _mouse.device->Acquire();
	if (FAILED(result))
	{
		// We don't have the mouse, so clear its state
		memset (&state, 0, sizeof(state));
	}
	else
	{
		// Read the mouse device raw state
		result = _mouse.device->GetDeviceState(sizeof(state),&state); 
		assert (result == DI_OK);
	}

	// Process mouse axes
	_mouse.deltax = ((float) state.lX) / 100.0f;
	_mouse.deltay = ((float) state.lY) / 100.0f;
	_mouse.deltaz = ((float) state.lZ) / 100.0f;

	// Process mouse buttons
	for (int i = 0; i < 4; i++)
	{
		_mouse.button[i] = ((_mouse.button[i] ^ state.rgbButtons[i]) >> 1) + state.rgbButtons[i];
	}
	#endif
}


/******************************************************************************
 Get change in mouse X position
******************************************************************************/
float ZgfMouseGetDeltaX (void)
{
	return _mouse.deltax;
}


/******************************************************************************
 Get change in mouse Y position
******************************************************************************/
float ZgfMouseGetDeltaY (void)
{
	return _mouse.deltay;
}


/******************************************************************************
 Get change in mouse Z position (wheel)
******************************************************************************/
float ZgfMouseGetDeltaZ (void)
{
	return _mouse.deltaz;
}


/******************************************************************************
 Test state of a mouse button
******************************************************************************/
int ZgfMouseTestButton	// Returns TRUE if button is in state tested for
(
	int button,			// button to test
	int state			// state to test for
)
{
	// Validate parameters
	assert (button < ZGFMOUSE_MAXBUTTONS);

	// Bits 6 and 7 of the input state are a mask
	// Bits 14 and 15 of the input state are the desired button state after the mask
	// Return TRUE if the button state matches the requested state
	return ((_mouse.button[button] & state) == (state >> 8));
}
