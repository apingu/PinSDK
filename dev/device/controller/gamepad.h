/*
 Gamepad module
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

#ifndef _GAMEPAD_H
#define _GAMEPAD_H

// Button names
enum {
	GAMEPAD_TOP,			// Windows 4, XBOX Y, PS2 Triangle, GameCube Y
	GAMEPAD_BOTTOM,			// Windows 2, XBOX A, PS2 X,        GameCube A
	GAMEPAD_LEFT,			// Windows 1, XBOX X, PS2 Square,   GameCube B
	GAMEPAD_RIGHT,			// Windows 3, XBOX B, PS2 Circle,   GameCube X
	GAMEPAD_START,			// Start button (all platforms)
	GAMEPAD_SELECT,			// Select button (except GameCube)
	GAMEPAD_L1,				// Left  upper shoulder (except GameCube)
	GAMEPAD_R1,				// Right upper shoulder (all platforms)
	GAMEPAD_L2,				// Left  lower shoulder (except XBOX)
	GAMEPAD_R2,				// Right lower shoulder (except XBOX)
	GAMEPAD_DPADLEFT,		// Directional pad left  button (all platforms)
	GAMEPAD_DPADRIGHT,		// Directional pad right button (all platforms)
	GAMEPAD_DPADUP,			// Directional pad up    button (all platforms)
	GAMEPAD_DPADDOWN,		// Directional pad down  button (all platforms)
	GAMEPAD_LEFTSTICK,		// Left  analog stick button (PS2 and XBOX only)
	GAMEPAD_RIGHTSTICK,		// Right analog stick button (PS2 and XBOX only)
	GAMEPAD_BLACK,			// Black button (XBOX only)
	GAMEPAD_WHITE,			// White button (XBOX only)
	GAMEPAD_MAXBUTTONS
};

// Button states
enum {
	GAMEPAD_STATE_UP       = 0x0080,	// Button is currently up
	GAMEPAD_STATE_DOWN     = 0x8080,	// Button is currently down
	GAMEPAD_STATE_RELEASED = 0x40c0,	// Button was released since last update
	GAMEPAD_STATE_PRESSED  = 0xc0c0,	// Button was pressed since last update
};

// Analog stick names
enum {
	GAMEPAD_STICK_DIRECTIONAL,	// Directional pad simulating an analog stick
	GAMEPAD_STICK_LEFT,			// Left analog stick
	GAMEPAD_STICK_RIGHT,		// Right analog stick
	GAMEPAD_MAXSTICKS
};

// Pad functions
void  gamepad_init	            (int window, int instance);
void  gamepad_deinit            (void);
void  gamepad_update            (void);
float gamepad_getbutton         (int gamepad, int button);
int   gamepad_testbutton        (int gamepad, int button, int state);
float gamepad_getstickx         (int gamepad, int stick);
float gamepad_getsticky         (int gamepad, int stick);
float gamepad_getstickangle     (int gamepad, int stick);
float gamepad_getstickmagnitude (int gamepad, int stick);

#endif
