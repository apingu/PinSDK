/*
 Gamepad module
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

// Windows APIs
#if defined(_WINDOWS)
#define  DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#endif

// XBOX APIs
#if defined(_XBOX)
#include <xtl.h>
#endif

// PS2 APIs
#if defined(_PS2)
#include <eetypes.h>
#include <libpad.h>
#endif

// Standard C APIs
#include <assert.h>
#include <string.h>
#include <math.h>

// Application APIs
#include ".\gamepad.h"


/******************************************************************************
 Data and Declarations
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/

// Windows Configuration
#if defined(_WINDOWS)
#define GAMEPAD_MAXPADS    (4)		// Maximum number of gamepads
#endif

// XBOX Configuration
#if defined(_XBOX)
#define GAMEPAD_MAXPADS    (4)		// Maximum number of gamepads
#endif

// PS2 Configuration
#if defined(_PS2)
#define GAMEPAD_MAXPADS    (2)		// Maximum number of gamepads
#endif

// Constants
#define PI (3.141592654f)

// Button object
typedef struct {
	float	      pressure;		// Current pressure on button (0.0 to 1.0)
	unsigned char state;		// Bit 7 = Button down, Bit 6 = Button state changed
} _button;

// Stick object
typedef struct {
	float	x;				// Stick x position (-1.0 to 1.0)
	float	y;				// Stick y position (-1.0 to 1.0)
	float	angle;			// Stick angle      (0.0  to 1.0)
	float	magnitude;		// Stick magnitude  (0.0  to 1.0)
} _stick;

// Gamepad object
typedef struct
{
	_stick	sticks[GAMEPAD_MAXSTICKS];		// state data for each stick
	_button	buttons[GAMEPAD_MAXBUTTONS];	// state data for each button
} gamepad;

// Angle lookup for directional pad angle calculation
static float anglelookup[] = {
	0.000f,		// No direction pressed
	0.250f,		// ---D (down)
	0.750f,		// --U- (up)
	0.000f,		// --UD (not possible)
	0.000f,		// -R-- (right)
	0.125f,		// -R-D (right and down)
	0.875f,		// -R-U (right and up)
	0.000f,		// -RUD (not possible)
	0.500f,		// L--- (left)
	0.375f,		// L--D (left and down)
	0.625f,		// L-U- (left and up)
	0.000f,		// L-UD (not possible)
	0.000f,		// LR-- (not possible)
	0.000f,		// LR-D (not possible)
	0.000f,		// LRU- (not possible)
	0.000f		// LRUD (not possible)
};

// Gamepad variables
static struct {
	//------------------------
	// Platform specific data
	//------------------------
	#if defined(_WINDOWS)
	LPDIRECTINPUT8		 di8;						// Pointer to Direct Input 8 object
	LPDIRECTINPUTDEVICE8 devices[GAMEPAD_MAXPADS];	// Pointer to each gamepad device
	#endif
	#if defined(_XBOX)
	int                  attached;					// Bits 0-3 track ports with an attached gamepad
	HANDLE               handles[GAMEPAD_MAXPADS];	// Handle for each open gamepad
	#endif
	#if defined(_PS2)
	unsigned char		 dma[GAMEPAD_MAXPADS][scePadDmaBufferMax * sizeof(u_long128)];	// DMA buffer for each gamepad device (must be 64-byte aligned)
	#endif
	//------------------------
	// Common data
	//------------------------
	int					 numdevices;				// Number of attached gamepad devices
	gamepad				 pads[GAMEPAD_MAXPADS];		// Gamepad objects
} _gamepad
#if defined(_PS2)
__attribute__((aligned(64)))		// Ensure 64-byte alignment for DMA buffer
#endif
;


// Forward references
void _updatebutton (_button* button, int pressure);
#if defined(_WINDOWS)
BOOL CALLBACK _axiscallback   (const DIDEVICEOBJECTINSTANCE* instance, LPVOID param);
BOOL CALLBACK _devicecallback (const DIDEVICEINSTANCE* instance,       LPVOID window);
#endif


/******************************************************************************
 Initialize gamepads
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void gamepad_init
(
	int window,				// application window handle
	int instance			// application instance handle
)
{
	// Clear gamepad variables
	memset (&_gamepad, 0, sizeof(_gamepad));

	//----------------------------------------
	// Initialize windows gamepads
	//----------------------------------------

	#if defined(_WINDOWS)
	// Create the direct input object
	HRESULT result = DirectInput8Create ((HINSTANCE) instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &_gamepad.di8, NULL);
	assert (result == DI_OK);

	// Enumerate and create game controller devices
	result = _gamepad.di8->EnumDevices(DI8DEVCLASS_GAMECTRL, _devicecallback, (void*) window, DIEDFL_ATTACHEDONLY);
	assert (result == DI_OK);
	#endif

	//----------------------------------------
	// Initialize XBOX gamepads
	//----------------------------------------

	#if defined(_XBOX)
	// Enumerate attached gamepads
	_gamepad.attached = XGetDevices (XDEVICE_TYPE_GAMEPAD);

	// Iterate through ports and initialize all attached gamepads
	for (int i = 0; i < GAMEPAD_MAXPADS; i++)
	{
		if (_gamepad.attached & (1 << i))
		{
			_gamepad.handles[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
		}
	}

	// Set number of attached gamepad devices
	_gamepad.numdevices = GAMEPAD_MAXPADS;
	#endif

	//----------------------------------------
	// Initialize PS2 gamepads
	//----------------------------------------

	#if defined(_PS2)
	// Initialize the pad library
	scePadInit(0);

	// Open each pad to enable DMA of pad data
	scePadPortOpen (0, 0, (u_long128*) &_gamepad.dma[0][0]);
	scePadPortOpen (1, 0, (u_long128*) &_gamepad.dma[1][0]);

	// Set number of attached gamepad devices
	_gamepad.numdevices = GAMEPAD_MAXPADS;
	#endif
}


/******************************************************************************
 Shutdown gamepads
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void gamepad_deinit (void)
{
	//----------------------------------------
	// Shut down windows gamepads
	//----------------------------------------

	#if defined(_WINDOWS)
	// Release the pad devices
	for (int i = 0; i < _gamepad.numdevices; i++)
	{
		// Unacquire the device (no assert is required since this function can legally fail)
		HRESULT result = _gamepad.devices[i]->Unacquire();

		// Release the device
		int refcount = _gamepad.devices[i]->Release();
		assert (refcount == 0);
	}

	// Release the direct input object
	int refcount = _gamepad.di8->Release();
	assert (refcount == 0);
	#endif

	//----------------------------------------
	// Shut down XBOX gamepads
	//----------------------------------------
	#if defined(_XBOX)
	// Close all attached gamepads
	for (int i = 0; i < GAMEPAD_MAXPADS; i++)
	{
		if (_gamepad.attached & (1 << i)) XInputClose(_gamepad.handles[i]);
	}
	#endif

	//----------------------------------------
	// Shut down PS2 gamepads
	//----------------------------------------

	#if defined(_PS2)
	// Close each pad
	scePadPortClose (0, 0);
	scePadPortClose (1, 0);

	// Terminate pad library
	scePadEnd();
	#endif
}


/******************************************************************************
 Latch gamepad state
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void gamepad_update (void)
{
	//---------------------------------------------------
	// Process insertion and removal of gamepads on XBOX
	//---------------------------------------------------

#if defined(_XBOX)
	// Get insertions and removals since last update
	DWORD insertions;
	DWORD removals;
	XGetDeviceChanges (XDEVICE_TYPE_GAMEPAD, &insertions, &removals);

	// Close any gamepads that were removed
	for (int j = 0; j < GAMEPAD_MAXPADS; j++)
	{
		if (removals && (1 << j))
		{
			_gamepad.attached &= ~(1 << j);
			XInputClose (_gamepad.handles[j]);
		}
	}

	// Open any gamepads that were inserted
	for (int k = 0; k < GAMEPAD_MAXPADS; k++)
	{
		if (insertions && (1 << k))
		{
			_gamepad.attached  |= (1 << k);
			_gamepad.handles[k] = XInputOpen(XDEVICE_TYPE_GAMEPAD, k, XDEVICE_NO_SLOT, NULL);
		}
	}
#endif


	//--------------------------------------------
	// Read and process state for each gamepad
	//--------------------------------------------

	// Loop for each gamepad
	for (int i = 0; i < _gamepad.numdevices; i++)
	{
		//---------------------------------
		// Read gamepad state
		//---------------------------------
#if defined (_XBOX)
		if( _gamepad.handles[i] == NULL )
			continue;
#endif

		#if defined(_WINDOWS)

        // TODO: Alex, need error prevent
        HRESULT result = _gamepad.devices[i]->Poll();

		// Acquire the gamepad. Skip the gamepad if there's a problem.
		result = _gamepad.devices[i]->Acquire();
		if (FAILED(result)) {memset(&_gamepad.pads[i], 0, sizeof(gamepad)); continue;}

		// Read the gamepad state.  Skip the gamepad if there's a problem.
		DIJOYSTATE2 state;
		result = _gamepad.devices[i]->GetDeviceState(sizeof(DIJOYSTATE2), &state);
		if (FAILED(result)) {memset(&_gamepad.pads[i], 0, sizeof(gamepad)); continue;}
		#endif

		#if defined(_XBOX)
		// Test if gamepad is attached to this port. Skip the gamepad if not attached
		if ((_gamepad.attached & (1 << i)) == 0) {memset(&_gamepad.pads[i], 0, sizeof(gamepad)); continue;}

		// Read the gamepad state.  Skip the gamepad if there's a problem.
		XINPUT_STATE state;
		DWORD result = XInputGetState (_gamepad.handles[i], &state);
		if (result != ERROR_SUCCESS) {memset(&_gamepad.pads[i], 0, sizeof(gamepad)); continue;}
		#endif

		#if defined(_PS2)
		// Read the gamepad attachment state.  Skip the gamepad if there's a problem.
		if (scePadGetState(i, 0) != scePadStateStable) {memset(&_gamepad.pads[i], 0, sizeof(gamepad)); continue;}

		// If the gamepad is not in DUALSHOCK mode then we put it in DUALSHOCK mode
		// We do this every update as a gamepad can be added or removed at any time
		if (scePadInfoMode (i, 0, InfoModeCurID, -1) != 0x07)
		{
			int nummodes = scePadInfoMode (i, 0, InfoModeIdTable, -1);
			for (int j = 0; j < nummodes; j++)
			{
				// If mode 0x07 is available (DUALSHOCK), then use it
				if (scePadInfoMode (i, 0, InfoModeIdTable, j) == 0x07)
				{
					// Set and lock DUALSHOCK mode
					scePadSetMainMode (i, 0, j, 3);
					while (scePadGetReqState(i, 0) == scePadReqStateBusy);

					// Enable pressure information to be received from gamepad
					scePadEnterPressMode (i, 0);
					while (scePadGetReqState(i, 0) == scePadReqStateBusy);
				}
			}
		}

		// Read the gamepad state. Skip the gamepad if there's a problem
		unsigned char state[32];
		scePadRead (i, 0, state);
		if (state[0] == 0xff) {memset(&_gamepad.pads[i], 0, sizeof(gamepad)); continue;}

		// Extract controller type and the amount of data received
		int type       = (state[1] & 0xf0);
		int datalength = (state[1] & 0x0f) * 2;

		// Ensure the gamepad is a Digital controller, Analog joystick, DUALSHOCK or DUALSHOCK2. If not we skip it.
		if ((type != 0x40) && (type != 0x50) && (type != 0x70)) {memset(&_gamepad.pads[i], 0, sizeof(gamepad)); continue;}
		#endif


		//---------------------------------
		// Process left analog stick
		//---------------------------------

		// Read analog position from gamepad state
		#if defined(_WINDOWS)
		float x = ((float) state.lX) / 1000.0f;
		float y = ((float) state.lY) / 1000.0f;
		#endif
		#if defined(_XBOX)
		float x = ((float) state.Gamepad.sThumbLX) / 32768.0f;
		float y = ((float) state.Gamepad.sThumbLY) / 32768.0f;
		#endif
		#if defined(_PS2)
		// Read analog position only if it is provided by this controller type
		float x,y;
		if (datalength >= 6) {x = (float) (state[6] - 128) / 128.0f; y = (float) (state[7] - 128) / 128.0f;}
		else                 {x = 0.0f;                              y = 0.0f;}
		#endif

		// Enforce a 20% dead zone for the left stick
		if ((x > -0.20f) && (x < 0.20f) && (y > -0.20f) && (y < 0.20f)) {x = 0; y = 0;}

		// Calculate a vector for the left stick
		float angle     = atan2f (y,x) / (PI*2);
		float magnitude = sqrtf  ((x*x) + (y*y)) / 1.414214f;
		if (angle < 0.0f) angle += 1.0f;

		// Store left stick state
		_gamepad.pads[i].sticks[GAMEPAD_STICK_LEFT].x         = x;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_LEFT].y         = y;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_LEFT].angle     = angle;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_LEFT].magnitude = magnitude;


		//---------------------------------
		// Process right analog stick
		//---------------------------------

		// Read analog position from gamepad state
		#if defined(_WINDOWS)
		x = ((float) state.lRz) / 1000.0f;
		y = ((float) state.lZ)  / 1000.0f;
		#endif
		#if defined(_XBOX)
		x = ((float) state.Gamepad.sThumbRX) / 32768.0f;
		y = ((float) state.Gamepad.sThumbRY) / 32768.0f;
		#endif
		#if defined(_PS2)
		// Read analog position only if it is provided by this controller type
		if (datalength >= 6) {x = (float) (state[4] - 128) / 128.0f; y = (float) (state[5] - 128) / 128.0f;}
		else                 {x = 0.0f;                              y = 0.0f;}
		#endif

		// Enforce a 20% dead zone for the right stick
		if ((x > -0.20f) && (x < 0.20f) && (y > -0.20f) && (y < 0.20f)) {x = 0; y = 0;}

		// Calculate a vector for the right stick
		angle     = atan2f (y,x) / (PI*2);
		magnitude = sqrtf ((x*x) + (y*y)) / 1.414214f;
		if (angle < 0.0f) angle += 1.0f;

		// Store right stick state
		_gamepad.pads[i].sticks[GAMEPAD_STICK_RIGHT].x         = x;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_RIGHT].y         = y;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_RIGHT].angle     = angle;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_RIGHT].magnitude = magnitude;


		//---------------------------------
		// Process buttons
		//---------------------------------

		#if defined(_WINDOWS)
		// Process regular buttons
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_LEFT],   (state.rgbButtons[0]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_BOTTOM], (state.rgbButtons[1]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_RIGHT],  (state.rgbButtons[2]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_TOP],    (state.rgbButtons[3]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_L1],     (state.rgbButtons[4]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_R1],     (state.rgbButtons[5]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_L2],     (state.rgbButtons[6]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_R2],     (state.rgbButtons[7]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_START],  (state.rgbButtons[8]) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_SELECT], (state.rgbButtons[9]) ? 255 : 0);

		// Process directional pad buttons
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADLEFT],  (state.rglSlider[0] < -500) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADRIGHT], (state.rglSlider[0] >  500) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADUP],    (state.lRx          < -500) ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADDOWN],  (state.lRx          >  500) ? 255 : 0);
		#endif

		#if defined(_XBOX)
		// Process digital buttons
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADUP],     (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)     ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADDOWN],   (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)   ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADLEFT],   (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)   ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADRIGHT],  (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)  ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_START],      (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)       ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_SELECT],     (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)        ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_LEFTSTICK],  (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)  ? 255 : 0);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_RIGHTSTICK], (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 255 : 0);

		// Process analog buttons
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_BOTTOM], state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_RIGHT],  state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_LEFT],   state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X]);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_TOP],    state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y]);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_BLACK],  state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_WHITE],  state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE]);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_L1],     state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_R1],     state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]);
		#endif

		#if defined(_PS2)
		// Get digital state of buttons
		short int buttons = state[3] + (state[2] << 8);

		// If pressure information is available then use it
		if (datalength >= 18)
		{
			// Process button states using pressure information
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADRIGHT], state[8] );
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADLEFT],  state[9] );
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADUP],    state[10]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADDOWN],  state[11]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_TOP],       state[12]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_RIGHT],     state[13]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_BOTTOM],    state[14]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_LEFT],      state[15]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_L1],        state[16]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_R1],        state[17]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_L2],        state[18]);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_R2],        state[19]);
		}
		else
		{
			// Process button states using digital button bits
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADLEFT],  buttons & 0x8000 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADDOWN],  buttons & 0x4000 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADRIGHT], buttons & 0x2000 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_DPADUP],    buttons & 0x1000 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_LEFT],      buttons & 0x0080 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_BOTTOM],    buttons & 0x0040 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_RIGHT],     buttons & 0x0020 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_TOP],       buttons & 0x0010 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_R1],        buttons & 0x0008 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_L1],        buttons & 0x0004 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_R2],        buttons & 0x0002 ? 0 : 255);
			_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_L2],        buttons & 0x0001 ? 0 : 255);
		}

		// These buttons are digital only (no pressure information)
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_START],      buttons & 0x0800 ? 0 : 255);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_LEFTSTICK],  buttons & 0x0400 ? 0 : 255);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_RIGHTSTICK], buttons & 0x0200 ? 0 : 255);
		_updatebutton (&_gamepad.pads[i].buttons[GAMEPAD_SELECT],     buttons & 0x0100 ? 0 : 255);
		#endif


		//----------------------------------------------------------
		// Create a simulated analog stick from the directional pad
		//----------------------------------------------------------

		// Calculate a position and a vector for the d-pad
		int flag = 0;
		if (_gamepad.pads[i].buttons[GAMEPAD_DPADLEFT].pressure)  flag++;
		flag <<= 1;
		if (_gamepad.pads[i].buttons[GAMEPAD_DPADRIGHT].pressure) flag++;
		flag <<= 1;
		if (_gamepad.pads[i].buttons[GAMEPAD_DPADUP].pressure)    flag++;
		flag <<= 1;
		if (_gamepad.pads[i].buttons[GAMEPAD_DPADDOWN].pressure)  flag++;
		angle     = anglelookup[flag];
		x         = cosf(angle * (PI*2));
		y         = sinf(angle * (PI*2));
		magnitude = sqrtf ((x*x) + (y*y)) / 1.414214f;

		// Store the d-pad position and vector as an analog stick
		_gamepad.pads[i].sticks[GAMEPAD_STICK_DIRECTIONAL].x		 = x; 
		_gamepad.pads[i].sticks[GAMEPAD_STICK_DIRECTIONAL].y		 = y;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_DIRECTIONAL].angle     = angle;
		_gamepad.pads[i].sticks[GAMEPAD_STICK_DIRECTIONAL].magnitude = magnitude;
	}
}

/******************************************************************************
 Set button state
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void _updatebutton
(
	_button* button,		// ptr to button object
	int      pressure		// current pressure on button (0 to 255)
)
{
	// Apply 5% dead zone to pressure
	if (pressure < 12) pressure = 0;

	// Determine raw button state
	unsigned char state = 0x00;
	if (pressure != 0) state = 0x80;

	// Determine gated button state
	if ((pressure == 0) && (button->pressure != 0.0f)) state |= 0x40;
	if ((pressure != 0) && (button->pressure == 0.0f)) state |= 0x40;

	// Store state and pressure for button
	button->state    = state;
	button->pressure = (float) pressure / 255.0f;
}


/******************************************************************************
 Get button pressure
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
float gamepad_getbutton		// Returns current pressure on button (0.0f to 1.0f)
(
	int gamepad,			// gamepad number
	int button				// button number
)
{
	// If either gamepad or button is invalid then return 0
	if ((gamepad >= GAMEPAD_MAXPADS) || (button >= GAMEPAD_MAXBUTTONS)) return 0.0f;

	// Return button pressure
	return _gamepad.pads[gamepad].buttons[button].pressure;
}


/******************************************************************************
 Test state of a button
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
int gamepad_testbutton		// Returns TRUE if button is in state tested for
(
	int gamepad,			// gamepad number
	int button,				// button number
	int state				// state to test for
)
{
	// If either gamepad or button is invalid then return 0
	if ((gamepad >= GAMEPAD_MAXPADS) || (button >= GAMEPAD_MAXBUTTONS)) return 0;

	// Bits 6 and 7 of the input state are a mask
	// Bits 14 and 15 of the input state are the desired button state after the mask
	// Return TRUE if the button state matches the requested state
	return ((_gamepad.pads[gamepad].buttons[button].state & state) == (state >> 8));
}


/******************************************************************************
 Get X position of a stick
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
float gamepad_getstickx		// Returns current x position of stick (-1.0f to 1.0f)
(
	int gamepad,			// gamepad number
	int stick				// stick number
)
{
	// If either gamepad or stick is invalid then return 0
	if ((gamepad >= GAMEPAD_MAXPADS) || (stick >= GAMEPAD_MAXSTICKS)) return 0.0f;

	// Return stick X position
	return _gamepad.pads[gamepad].sticks[stick].x;
}


/******************************************************************************
 Get Y position of a stick
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
float gamepad_getsticky		// Returns current y position of stick (-1.0f to 1.0f)
(
	int gamepad,			// gamepad number
	int stick				// stick number
)
{
	// If either gamepad or stick is invalid then return 0
	if ((gamepad >= GAMEPAD_MAXPADS) || (stick >= GAMEPAD_MAXSTICKS)) return 0.0f;

	// Return stick Y position
	return _gamepad.pads[gamepad].sticks[stick].y;
}


/******************************************************************************
 Get angle of a stick vector
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
float gamepad_getstickangle	// Returns current angle of stick vector (0.0f to 1.0f)
(
	int gamepad,			// gamepad number
	int stick				// stick number
)
{
	// If either pad or stick is invalid then return 0
	if ((gamepad >= GAMEPAD_MAXPADS) || (stick >= GAMEPAD_MAXSTICKS)) return 0.0f;

	// Return stick angle
	return _gamepad.pads[gamepad].sticks[stick].angle;
}


/******************************************************************************
 Get magnitude of stick vector
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
float gamepad_getstickmagnitude	// Returns current magnitude of stick vector (0.0f to 1.0f)
(
	int gamepad,				// gamepad number
	int stick					// stick number
)
{
	// If either pad or stick is invalid then return 0
	if ((gamepad >= GAMEPAD_MAXPADS) || (stick >= GAMEPAD_MAXSTICKS)) return 0.0f;

	// Return stick magnitude
	return _gamepad.pads[gamepad].sticks[stick].magnitude;
}


/******************************************************************************
 Callback for windows game device axis enumeration
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
#if defined(_WINDOWS)
BOOL CALLBACK _axiscallback							
(
	const DIDEVICEOBJECTINSTANCE* instance,		// ptr to instance id for axis
	LPVOID                        param			// parameter (unused)
)
{
	HRESULT result;
	DIPROPRANGE diprg; 

	// Set axis to return minimum of -1000 and maximum of +1000
	diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	diprg.diph.dwHow        = DIPH_BYID; 
	diprg.diph.dwObj        = instance->dwType; 
	diprg.lMin              = -1000; 
	diprg.lMax              = +1000; 
	result = _gamepad.devices[_gamepad.numdevices]->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert (result == DI_OK);

	// Continue enumeration
	return DIENUM_CONTINUE;
}
#endif

/******************************************************************************
 Callback for windows game device enumeration
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
#if defined(_WINDOWS)
BOOL CALLBACK _devicecallback	// Returns DIENUM_CONTINUE to continue or DIENUM_STOP when done
(
	const DIDEVICEINSTANCE* instance,	// ptr to instance id for pad device
	LPVOID                  window		// window handle
)
{
	HRESULT result;

	// Create a device for the pad
	result = _gamepad.di8->CreateDevice(instance->guidInstance, &_gamepad.devices[_gamepad.numdevices], NULL);
	assert (result == DI_OK);

	// Set the data format for the pad device
	result = _gamepad.devices[_gamepad.numdevices]->SetDataFormat(&c_dfDIJoystick2);
	assert (result == DI_OK);

	// Set the cooperative level for the pad device
	#if defined(_DEBUG)
	result = _gamepad.devices[_gamepad.numdevices]->SetCooperativeLevel((HWND) window, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); 
	#else
	result = _gamepad.devices[_gamepad.numdevices]->SetCooperativeLevel((HWND) window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
	#endif
	assert (result == DI_OK);

	// Enumerate the device axes and set their properties
	result = _gamepad.devices[_gamepad.numdevices]->EnumObjects(_axiscallback, (HWND) window, DIDFT_AXIS);
	assert (result == DI_OK);

	// Continue unless we've reached the maximum number of controllers we care about
	_gamepad.numdevices++;
	if (_gamepad.numdevices == GAMEPAD_MAXPADS) return DIENUM_STOP;
	else                                        return DIENUM_CONTINUE;
}
#endif