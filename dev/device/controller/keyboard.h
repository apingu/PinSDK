/*
 Keyboard module
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

// Key identifiers
enum {
	KEY_ESC,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,				// On NEC PC-98 Japanese keyboard 
	KEY_F14,				// On NEC PC-98 Japanese keyboard 
	KEY_F15,				// On NEC PC-98 Japanese keyboard 
	KEY_SYSRQ,
	KEY_SCROLLLOCK,
	KEY_PAUSE,

	KEY_GRAVE,				// Grave accent (`)  
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_MINUS,
	KEY_EQUALS,
	KEY_BACKSPACE,

	KEY_TAB,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_U,
	KEY_I,
	KEY_O,
	KEY_P,
	KEY_LEFTBRACKET,
	KEY_RIGHTBRACKET,
	KEY_BACKSLASH,

	KEY_CAPSLOCK,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,	
	KEY_ENTER,

	KEY_LEFTSHIFT,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_N,
	KEY_M,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_FORWARDSLASH,
	KEY_RIGHTSHIFT,

	KEY_LEFTCTRL,
	KEY_LEFTWIN,
	KEY_LEFTALT,
	KEY_SPACE,
	KEY_RIGHTALT,
	KEY_RIGHTWIN,
	KEY_APPLICATION,
	KEY_RIGHTCTRL,

	KEY_INSERT,
	KEY_HOME,
	KEY_PAGEUP,
	KEY_DELETE,
	KEY_END,
	KEY_PAGEDOWN,

	KEY_UP,					// Up arrow
	KEY_DOWN,				// Down arrow
	KEY_LEFT,				// Left arrow
	KEY_RIGHT,				// Right arrow

	KEY_NUMLOCK,
	KEY_NUMPADDIVIDE,
	KEY_NUMPADMULTIPLY,
	KEY_NUMPADSUBTRACT,
	KEY_NUMPADADD,
	KEY_NUMPADENTER,
	KEY_NUMPADDECIMAL,
	KEY_NUMPAD0,
	KEY_NUMPAD1,
	KEY_NUMPAD2,
	KEY_NUMPAD3,
	KEY_NUMPAD4,
	KEY_NUMPAD5,
	KEY_NUMPAD6,
	KEY_NUMPAD7,
	KEY_NUMPAD8,
	KEY_NUMPAD9,
	KEY_NUMPADCOMMA,		// On numeric keypad of NEC PC-98 Japanese keyboard 
	KEY_NUMPADEQUALS,		// On numeric keypad of NEC PC-98 Japanese keyboard 
	KEY_ABNT_C1,			// On numeric pad of Brazilian keyboards 
	KEY_ABNT_C2,			// On numeric pad of Brazilian keyboards 

	KEY_WAKE,
	KEY_SLEEP,
	KEY_POWER,

	KEY_MAIL,
	KEY_CALCULATOR,
	KEY_MYCOMPUTER,

	KEY_MEDIASELECT,
	KEY_MEDIASTOP,
	KEY_PLAYPAUSE,
	KEY_NEXTTRACK,
	KEY_PREVTRACK,			// Also circumflex on Japanese keyboard

	KEY_MUTE,
	KEY_VOLUMEUP,
	KEY_VOLUMEDOWN,

	KEY_WEBHOME,
	KEY_WEBFORWARD,
	KEY_WEBBACK,
	KEY_WEBREFRESH,
	KEY_WEBFAVORITES,	
	KEY_WEBSEARCH,
	KEY_WEBSTOP,

	KEY_COLON,				// On Japanese keyboard 
	KEY_CONVERT,			// On Japanese keyboard 
	KEY_KANA,				// On Japanese keyboard 
	KEY_KANJI,				// On Japanese keyboard 
	KEY_AT,					// On Japanese keyboard 
	KEY_AX,					// On Japanese keyboard 
	KEY_NOCONVERT,			// On Japanese keyboard 
	KEY_UNLABELED,			// On Japanese keyboard 
	KEY_YEN,				// On Japanese keyboard 
	KEY_STOP,				// On NEC PC-98 Japanese keyboard 
	KEY_UNDERLINE,			// On NEC PC-98 Japanese keyboard 
	KEY_OEM_102,			// On British and German keyboards 
	KEY_MAXBUTTONS
};

// Key states
enum {
	KEY_STATE_UP       = 0x0080,	// Button is currently up
	KEY_STATE_DOWN     = 0x8080,	// Button is currently down
	KEY_STATE_RELEASED = 0x40c0,	// Button was released since last update
	KEY_STATE_PRESSED  = 0xc0c0,	// Button was pressed since last update
};

// Control codes in buffered input
#define KEY_CONTROL_END       (0x00)
#define KEY_CONTROL_BACKSPACE (0x08)
#define KEY_CONTROL_TAB       (0x09)
#define KEY_CONTROL_ENTER	  (0x0d)
#define KEY_CONTROL_RIGHT     (0x1c)
#define KEY_CONTROL_LEFT      (0x1d)
#define KEY_CONTROL_UP        (0x1e)
#define KEY_CONTROL_DOWN      (0x1f)

// Keyboard functions
void  keyboard_init               (int window, int instance);
void  keyboard_update             (float elapsedtime);
void  keyboard_deinit             (void);
int   keyboard_testkey            (int key, int state);
char* keyboard_getbufferedinput   (void);

#endif