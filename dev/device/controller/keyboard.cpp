/*
 Keyboard module
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
#define DEBUG_KEYBOARD
#include <xtl.h>
#endif

// PS2 APIs
#if defined(_PS2)
#include <libusbkb.h>
#endif

// Standard C APIs
#include <assert.h>
#include <string.h>

// Application APIs
#include ".\keyboard.h"

/******************************************************************************
 Data and Declarations
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/

// Configuration
#define MAXBUFFEREDINPUT (256)				// Maximum length of buffered input
#define REPEATSTART      (0.5f)				// Initial delay before key repeat
#define REPEATINTERVAL   (0.1f)				// Delay between recurring repeats

// Key mapping table
#define BACK  (KEY_CONTROL_BACKSPACE)		// Convenience labels
#define TAB   (KEY_CONTROL_TAB)
#define ENTER (KEY_CONTROL_ENTER)
#define UP    (KEY_CONTROL_UP)
#define DOWN  (KEY_CONTROL_DOWN)
#define LEFT  (KEY_CONTROL_LEFT)
#define RIGHT (KEY_CONTROL_RIGHT)


#if defined(_WINDOWS)
struct {unsigned char keycode; char ascii; char shiftascii;} keyxlat[] = {
	DIK_ESCAPE,			0,		0,			// KEY_ESC
	DIK_F1,				0,		0,			// KEY_F1
	DIK_F2,				0,		0,			// KEY_F2
	DIK_F3,				0,		0,			// KEY_F3
	DIK_F4,				0,		0,			// KEY_F4
	DIK_F5,				0,		0,			// KEY_F5
	DIK_F6,				0,		0,			// KEY_F6
	DIK_F7,				0,		0,			// KEY_F7
	DIK_F8,				0,		0,			// KEY_F8
	DIK_F9,				0,		0,			// KEY_F9
	DIK_F10,			0,		0,			// KEY_F10
	DIK_F11,			0,		0,			// KEY_F11
	DIK_F12,			0,		0,			// KEY_F12
	DIK_F13,			0,		0,			// KEY_F13   (NEC PC-98 Japanese keyboard)
	DIK_F14,			0,		0,			// KEY_F14   (NEC PC-98 Japanese keyboard)
	DIK_F15,			0,		0,			// KEY_F15   (NEC PC-98 Japanese keyboard)
	DIK_SYSRQ,			0,		0,			// KEY_SYSRQ
	DIK_SCROLL,			0,		0,			// KEY_SCROLLLOCK
	DIK_PAUSE,			0,		0,			// KEY_PAUSE

	DIK_GRAVE,			'`',	'~',		// KEY_GRAVE
	DIK_1,				'1',	'!',		// KEY_1
	DIK_2,				'2',	'@',		// KEY_2
	DIK_3,				'3',	'#',		// KEY_3
	DIK_4,				'4',	'$',		// KEY_4
	DIK_5,				'5',	'%',		// KEY_5
	DIK_6,				'6',	'^',		// KEY_6
	DIK_7,				'7',	'&',		// KEY_7
	DIK_8,				'8',	'*',		// KEY_8
	DIK_9,				'9',	'(',		// KEY_9
	DIK_0,				'0',	')',		// KEY_0
	DIK_MINUS,			'-',	'_',		// KEY_MINUS
	DIK_EQUALS,			'=',	'+',		// KEY_EQUALS
	DIK_BACK,			BACK,	BACK,		// KEY_BACKSPACE

	DIK_TAB,			TAB,	TAB,		// KEY_TAB
	DIK_Q,				'q',	'Q',		// KEY_Q
	DIK_W,				'w',	'W',		// KEY_W
	DIK_E,				'e',	'E',		// KEY_E
	DIK_R,				'r',	'R',		// KEY_R
	DIK_T,				't',	'T',		// KEY_T
	DIK_Y,				'y',	'Y',		// KEY_Y
	DIK_U,				'u',	'U',		// KEY_U
	DIK_I,				'i',	'I',		// KEY_I
	DIK_O,				'o',	'O',		// KEY_O
	DIK_P,				'p',	'P',		// KEY_P
	DIK_LBRACKET,		'[',	'{',		// KEY_LEFTBRACKET
	DIK_RBRACKET,		']',	'}',		// KEY_RIGHTBRACKET
	DIK_BACKSLASH,		'\\',	'|',		// KEY_BACKSLASH

	DIK_CAPITAL,		0,		0,			// KEY_CAPSLOCK
	DIK_A,				'a',	'A',		// KEY_A
	DIK_S,				's',	'S',		// KEY_S
	DIK_D,				'd',	'D',		// KEY_D
	DIK_F,				'f',	'F',		// KEY_F
	DIK_G,				'g',	'G',		// KEY_G
	DIK_H,				'h',	'H',		// KEY_H
	DIK_J,				'j',	'J',		// KEY_J
	DIK_K,				'k',	'K',		// KEY_K
	DIK_L,				'l',	'L',		// KEY_L
	DIK_SEMICOLON,		';',	':',		// KEY_SEMICOLON
	DIK_APOSTROPHE,		'\'',	'"',		// KEY_APOSTROPHE
	DIK_RETURN,			ENTER,	ENTER,		// KEY_ENTER

	DIK_LSHIFT,			0,		0,			// KEY_LEFTSHIFT
	DIK_Z,				'z',	'Z',		// KEY_Z
	DIK_X,				'x',	'X',		// KEY_X
	DIK_C,				'c',	'C',		// KEY_C
	DIK_V,				'v',	'V',		// KEY_V
	DIK_B,				'b',	'B',		// KEY_B
	DIK_N,				'n',	'N',		// KEY_N
	DIK_M,				'm',	'M',		// KEY_M
	DIK_COMMA,			',',	'<',		// KEY_COMMA
	DIK_PERIOD,			'.',	'>',		// KEY_PERIOD
	DIK_SLASH,			'/',	'?',		// KEY_FORWARDSLASH
	DIK_RSHIFT,			0,		0,			// KEY_RIGHTSHIFT

	DIK_LCONTROL,		0,		0,			// KEY_LEFTCTRL
	DIK_LWIN,			0,		0,			// KEY_LEFTWIN
	DIK_LMENU,			0,		0,			// KEY_LEFTALT
	DIK_SPACE,			' ',	' ',		// KEY_SPACE
	DIK_RMENU,			0,		0,			// KEY_RIGHTALT
	DIK_RWIN,			0,		0,			// KEY_RIGHTWIN
	DIK_APPS,			0,		0,			// KEY_APPLICATIONS
	DIK_RCONTROL,		0,		0,			// KEY_RIGHTCTRL

	DIK_INSERT,			0,		0,			// KEY_INSERT
	DIK_HOME,			0,		0,			// KEY_HOME
	DIK_PRIOR,			0,		0,			// KEY_PAGEUP
	DIK_DELETE,			0,		0,			// KEY_DELETE
	DIK_END,			0,		0,			// KEY_END
	DIK_NEXT,			0,		0,			// KEY_PAGEDOWN

	DIK_UP,				UP,		UP,			// KEY_UP
	DIK_DOWN,			DOWN,	DOWN,		// KEY_DOWN
	DIK_LEFT,			LEFT,	LEFT,		// KEY_LEFT
	DIK_RIGHT,			RIGHT,	RIGHT,		// KEY_RIGHT

	DIK_NUMLOCK,		0,		0,			// KEY_NUMLOCK
	DIK_DIVIDE,			'/',	'/',		// KEY_NUMPADDIVIDE
	DIK_MULTIPLY,		'*',	'*',		// KEY_NUMPADMULTIPLY
	DIK_SUBTRACT,		'-',	'-',		// KEY_NUMPADSUBTRACT
	DIK_ADD,			'+',	'+',		// KEY_NUMPADADD
	DIK_NUMPADENTER,	ENTER,	ENTER,		// KEY_NUMPADENTER
	DIK_DECIMAL,		'.',	'.',		// KEY_NUMPADDECIMAL	// Keep together for Numpad processing
	DIK_NUMPAD0,		'0',	'0',		// KEY_NUMPAD0			// "
	DIK_NUMPAD1,		'1',	'1',		// KEY_NUMPAD1			// "
	DIK_NUMPAD2,		'2',	'2',		// KEY_NUMPAD2			// "
	DIK_NUMPAD3,		'3',	'3',		// KEY_NUMPAD3			// "
	DIK_NUMPAD4,		'4',	'4',		// KEY_NUMPAD4			// "
	DIK_NUMPAD5,		'5',	'5',		// KEY_NUMPAD5			// "
	DIK_NUMPAD6,		'6',	'6',		// KEY_NUMPAD6			// "
	DIK_NUMPAD7,		'7',	'7',		// KEY_NUMPAD7			// "
	DIK_NUMPAD8,		'8',	'8',		// KEY_NUMPAD8			// "
	DIK_NUMPAD9,		'9',	'9',		// KEY_NUMPAD9			// "
	DIK_NUMPADCOMMA,	',',	',',		// KEY_NUMPADCOMMA  (NEC PC-98 Japanese keyboard)
	DIK_NUMPADEQUALS,	'=',	'=',		// KEY_NUMPADEQUALS (NEC PC-98 Japanese keyboard)
	DIK_ABNT_C1,		'/',	'/',		// KEY_ABNT_C1 (Brazilian keyboard)
	DIK_ABNT_C2,		'.',	'.',		// KEY_ABNT_C2 (Brazilian keyboard)

	DIK_WAKE,			0,		0,			// KEY_WAKE
	DIK_SLEEP,			0,		0,			// KEY_SLEEP
	DIK_POWER,			0,		0,			// KEY_POWER

	DIK_MAIL,			0,		0,			// KEY_MAIL
	DIK_CALCULATOR,		0,		0,			// KEY_CALCULATOR
	DIK_MYCOMPUTER,		0,		0,			// KEY_MYCOMPUTER

	DIK_MEDIASELECT,	0,		0,			// KEY_MEDIASELECT
	DIK_MEDIASTOP,		0,		0,			// KEY_MEDIASTOP
	DIK_PLAYPAUSE,		0,		0,			// KEY_PLAYPAUSE
	DIK_NEXTTRACK,		0,		0,			// KEY_NEXTTRACK
	DIK_PREVTRACK,		'^',	'^',		// KEY_PREVTRACK  (also Circumflex on Japanese keyboard)

	DIK_MUTE,			0,		0,			// KEY_MUTE
	DIK_VOLUMEUP,		0,		0,			// KEY_VOLUMEUP
	DIK_VOLUMEDOWN,		0,		0,			// KEY_VOLUMEDOWN

	DIK_WEBHOME,		0,		0,			// KEY_WEBHOME
	DIK_WEBFORWARD,		0,		0,			// KEY_WEBFORWARD
	DIK_WEBBACK,		0,		0,			// KEY_WEBBACK
	DIK_WEBREFRESH,		0,		0,			// KEY_WEBREFRESH
	DIK_WEBFAVORITES,	0,		0,			// KEY_WEBFAVORITES
	DIK_WEBSEARCH,		0,		0,			// KEY_WEBSEARCH
	DIK_WEBSTOP,		0,		0,			// KEY_WEBSTOP

	DIK_COLON,			':',	':',		// KEY_COLON     (Japanese keyboard)
	DIK_CONVERT,		0,		0,			// KEY_CONVERT   (Japanese keyboard)
	DIK_KANA,			0,		0,			// KEY_KANA      (Japanese keyboard)
	DIK_KANJI,			0,		0,			// KEY_KANJI     (Japanese keyboard)
	DIK_AT,				0,		0,			// KEY_AT        (Japanese keyboard)
	DIK_AX,				0,		0,			// KEY_AX        (Japanese keyboard)
	DIK_NOCONVERT,		0,		0,			// KEY_NOCONVERT (Japanese keyboard)
	DIK_UNLABELED,		0,		0,			// KEY_UNLABELED (Japanese keyboard)
	DIK_YEN,			0,		0,			// KEY_YEN       (Japanese keyboard)
	DIK_STOP,			0,		0,			// KEY_STOP      (NEC PC-98 Japanese keyboard)
	DIK_UNDERLINE,		'_',	'_',		// KEY_UNDERLINE (NEC PC-98 Japanese keyboard)
	DIK_OEM_102,		0,		0,			// KEY_OEM_102   (British and German keyboards)
};
#endif


#if defined(_XBOX)
struct {unsigned char keycode; char ascii; char shiftascii;} keyxlat[] = {
	VK_ESCAPE,				0,		0,			// KEY_ESC
	VK_F1,					0,		0,			// KEY_F1
	VK_F2,					0,		0,			// KEY_F2
	VK_F3,					0,		0,			// KEY_F3
	VK_F4,					0,		0,			// KEY_F4
	VK_F5,					0,		0,			// KEY_F5
	VK_F6,					0,		0,			// KEY_F6
	VK_F7,					0,		0,			// KEY_F7
	VK_F8,					0,		0,			// KEY_F8
	VK_F9,					0,		0,			// KEY_F9
	VK_F10,					0,		0,			// KEY_F10
	VK_F11,					0,		0,			// KEY_F11
	VK_F12,					0,		0,			// KEY_F12
	VK_F13,					0,		0,			// KEY_F13   (NEC PC-98 Japanese keyboard)
	VK_F14,					0,		0,			// KEY_F14   (NEC PC-98 Japanese keyboard)
	VK_F15,					0,		0,			// KEY_F15   (NEC PC-98 Japanese keyboard)
	VK_PRINT,				0,		0,			// KEY_SYSRQ
	VK_SCROLL,				0,		0,			// KEY_SCROLLLOCK
	VK_PAUSE,				0,		0,			// KEY_PAUSE

	VK_OEM_3,				'`',	'~',		// KEY_GRAVE
	'1',					'1',	'!',		// KEY_1
	'2',					'2',	'@',		// KEY_2
	'3',					'3',	'#',		// KEY_3
	'4',					'4',	'$',		// KEY_4
	'5',					'5',	'%',		// KEY_5
	'6',					'6',	'^',		// KEY_6
	'7',					'7',	'&',		// KEY_7
	'8',					'8',	'*',		// KEY_8
	'9',					'9',	'(',		// KEY_9
	'0',					'0',	')',		// KEY_0
	VK_OEM_MINUS,			'-',	'_',		// KEY_MINUS
	VK_OEM_PLUS,			'=',	'+',		// KEY_EQUALS
	VK_BACK,				BACK,	BACK,		// KEY_BACKSPACE

	VK_TAB,					TAB,	TAB,		// KEY_TAB
	'Q',					'q',	'Q',		// KEY_Q
	'W',					'w',	'W',		// KEY_W
	'E',					'e',	'E',		// KEY_E
	'R',					'r',	'R',		// KEY_R
	'T',					't',	'T',		// KEY_T
	'Y',					'y',	'Y',		// KEY_Y
	'U',					'u',	'U',		// KEY_U
	'I',					'i',	'I',		// KEY_I
	'O',					'o',	'O',		// KEY_O
	'P',					'p',	'P',		// KEY_P
	VK_OEM_4,				'[',	'{',		// KEY_LEFTBRACKET
	VK_OEM_6,				']',	'}',		// KEY_RIGHTBRACKET
	VK_OEM_5,				'\\',	'|',		// KEY_BACKSLASH

	VK_CAPITAL,				0,		0,			// KEY_CAPSLOCK
	'A',					'a',	'A',		// KEY_A
	'S',					's',	'S',		// KEY_S
	'D',					'd',	'D',		// KEY_D
	'F',					'f',	'F',		// KEY_F
	'G',					'g',	'G',		// KEY_G
	'H',					'h',	'H',		// KEY_H
	'J',					'j',	'J',		// KEY_J
	'K',					'k',	'K',		// KEY_K
	'L',					'l',	'L',		// KEY_L
	VK_OEM_1,				';',	':',		// KEY_SEMICOLON
	VK_OEM_7,				'\'',	'"',		// KEY_APOSTROPHE
	VK_RETURN,				ENTER,	ENTER,		// KEY_ENTER

	VK_LSHIFT,				0,		0,			// KEY_LEFTSHIFT
	'Z',					'z',	'Z',		// KEY_Z
	'X',					'x',	'X',		// KEY_X
	'C',					'c',	'C',		// KEY_C
	'V',					'v',	'V',		// KEY_V
	'B',					'b',	'B',		// KEY_B
	'N',					'n',	'N',		// KEY_N
	'M',					'm',	'M',		// KEY_M
	VK_OEM_COMMA,			',',	'<',		// KEY_COMMA
	VK_OEM_PERIOD,			'.',	'>',		// KEY_PERIOD
	VK_OEM_2,				'/',	'?',		// KEY_FORWARDSLASH
	VK_RSHIFT,				0,		0,			// KEY_RIGHTSHIFT

	VK_LCONTROL,			0,		0,			// KEY_LEFTCTRL
	VK_LWIN,				0,		0,			// KEY_LEFTWIN
	VK_LMENU,				0,		0,			// KEY_LEFTALT
	VK_SPACE,				' ',	' ',		// KEY_SPACE
	VK_RMENU,				0,		0,			// KEY_RIGHTALT
	VK_RWIN,				0,		0,			// KEY_RIGHTWIN
	VK_APPS,				0,		0,			// KEY_APPLICATIONS
	VK_RCONTROL,			0,		0,			// KEY_RIGHTCTRL

	VK_INSERT,				0,		0,			// KEY_INSERT
	VK_HOME,				0,		0,			// KEY_HOME
	VK_PRIOR,				0,		0,			// KEY_PAGEUP
	VK_DELETE,				0,		0,			// KEY_DELETE
	VK_END,					0,		0,			// KEY_END
	VK_NEXT,				0,		0,			// KEY_PAGEDOWN

	VK_UP,					UP,		UP,			// KEY_UP
	VK_DOWN,				DOWN,	DOWN,		// KEY_DOWN
	VK_LEFT,				LEFT,	LEFT,		// KEY_LEFT
	VK_RIGHT,				RIGHT,	RIGHT,		// KEY_RIGHT

	VK_NUMLOCK,				0,		0,			// KEY_NUMLOCK
	VK_DIVIDE,				'/',	'/',		// KEY_NUMPADDIVIDE
	VK_MULTIPLY,			'*',	'*',		// KEY_NUMPADMULTIPLY
	VK_SUBTRACT,			'-',	'-',		// KEY_NUMPADSUBTRACT
	VK_ADD,					'+',	'+',		// KEY_NUMPADADD
	0,						ENTER,	ENTER,		// KEY_NUMPADENTER
	VK_DECIMAL,				'.',	'.',		// KEY_NUMPADDECIMAL	// Keep together for Numpad processing
	VK_NUMPAD0,				'0',	'0',		// KEY_NUMPAD0			// "
	VK_NUMPAD1,				'1',	'1',		// KEY_NUMPAD1			// "
	VK_NUMPAD2,				'2',	'2',		// KEY_NUMPAD2			// "
	VK_NUMPAD3,				'3',	'3',		// KEY_NUMPAD3			// "
	VK_NUMPAD4,				'4',	'4',		// KEY_NUMPAD4			// "
	VK_NUMPAD5,				'5',	'5',		// KEY_NUMPAD5			// "
	VK_NUMPAD6,				'6',	'6',		// KEY_NUMPAD6			// "
	VK_NUMPAD7,				'7',	'7',		// KEY_NUMPAD7			// "
	VK_NUMPAD8,				'8',	'8',		// KEY_NUMPAD8			// "
	VK_NUMPAD9,				'9',	'9',		// KEY_NUMPAD9			// "
	0,						',',	',',		// KEY_NUMPADCOMMA  (NEC PC-98 Japanese keyboard)
	VK_OEM_NEC_EQUAL,		'=',	'=',		// KEY_NUMPADEQUALS (NEC PC-98 Japanese keyboard)
	0,						'/',	'/',		// KEY_ABNT_C1 (Brazilian keyboard)
	0,						'.',	'.',		// KEY_ABNT_C2 (Brazilian keyboard)

	0,						0,		0,			// KEY_WAKE
	VK_SLEEP,				0,		0,			// KEY_SLEEP
	0,						0,		0,			// KEY_POWER

	VK_LAUNCH_MAIL,			0,		0,			// KEY_MAIL
	VK_LAUNCH_APP1,			0,		0,			// KEY_CALCULATOR
	VK_LAUNCH_APP2,			0,		0,			// KEY_MYCOMPUTER

	VK_LAUNCH_MEDIA_SELECT,	0,		0,			// KEY_MEDIASELECT
	VK_MEDIA_STOP,			0,		0,			// KEY_MEDIASTOP
	VK_MEDIA_PLAY_PAUSE,	0,		0,			// KEY_PLAYPAUSE
	VK_MEDIA_NEXT_TRACK,	0,		0,			// KEY_NEXTTRACK
	VK_MEDIA_PREV_TRACK,	'^',	'^',		// KEY_PREVTRACK  (also Circumflex on Japanese keyboard)

	VK_VOLUME_MUTE,			0,		0,			// KEY_MUTE
	VK_VOLUME_UP,			0,		0,			// KEY_VOLUMEUP
	VK_VOLUME_DOWN,			0,		0,			// KEY_VOLUMEDOWN

	VK_BROWSER_HOME,		0,		0,			// KEY_WEBHOME
	VK_BROWSER_FORWARD,		0,		0,			// KEY_WEBFORWARD
	VK_BROWSER_BACK,		0,		0,			// KEY_WEBBACK
	VK_BROWSER_REFRESH,		0,		0,			// KEY_WEBREFRESH
	VK_BROWSER_FAVORITES,	0,		0,			// KEY_WEBFAVORITES
	VK_BROWSER_SEARCH,		0,		0,			// KEY_WEBSEARCH
	VK_BROWSER_STOP,		0,		0,			// KEY_WEBSTOP

	0,						':',	':',		// KEY_COLON     (Japanese keyboard)
	VK_CONVERT,				0,		0,			// KEY_CONVERT   (Japanese keyboard)
	VK_KANA,				0,		0,			// KEY_KANA      (Japanese keyboard)
	VK_KANJI,				0,		0,			// KEY_KANJI     (Japanese keyboard)
	0,						0,		0,			// KEY_AT        (Japanese keyboard)
	VK_OEM_AX,				0,		0,			// KEY_AX        (Japanese keyboard)
	VK_NONCONVERT,			0,		0,			// KEY_NOCONVERT (Japanese keyboard)
	0,						0,		0,			// KEY_UNLABELED (Japanese keyboard)
	0,						0,		0,			// KEY_YEN       (Japanese keyboard)
	0,						0,		0,			// KEY_STOP      (NEC PC-98 Japanese keyboard)
	0,						'_',	'_',		// KEY_UNDERLINE (NEC PC-98 Japanese keyboard)
	VK_OEM_102,				0,		0,			// KEY_OEM_102   (British and German keyboards)
};
#endif


#if defined(_PS2)
struct {unsigned char keycode; char ascii; char shiftascii;} keyxlat[] = {
	{USBKEYC_ESC,				0,		0},			// KEY_ESC
	{USBKEYC_F1,				0,		0},			// KEY_F1
	{USBKEYC_F2,				0,		0},			// KEY_F2
	{USBKEYC_F3,				0,		0},			// KEY_F3
	{USBKEYC_F4,				0,		0},			// KEY_F4
	{USBKEYC_F5,				0,		0},			// KEY_F5
	{USBKEYC_F6,				0,		0},			// KEY_F6
	{USBKEYC_F7,				0,		0},			// KEY_F7
	{USBKEYC_F8,				0,		0},			// KEY_F8
	{USBKEYC_F9,				0,		0},			// KEY_F9
	{USBKEYC_F10,				0,		0},			// KEY_F10
	{USBKEYC_F11,				0,		0},			// KEY_F11
	{USBKEYC_F12,				0,		0},			// KEY_F12
	{0,							0,		0},			// KEY_F13   (NEC PC-98 Japanese keyboard)
	{0,							0,		0},			// KEY_F14   (NEC PC-98 Japanese keyboard)
	{0,							0,		0},			// KEY_F15   (NEC PC-98 Japanese keyboard)
	{USBKEYC_PRINTSCREEN,		0,		0},			// KEY_SYSRQ
	{USBKEYC_SCROLL_LOCK,		0,		0},			// KEY_SCROLLLOCK
	{USBKEYC_PAUSE,				0,		0},			// KEY_PAUSE

	{USBKEYC_ACCENT_CIRCONFLEX_106,'`',	'~'},		// KEY_GRAVE
	{USBKEYC_1,					'1',	'!'},		// KEY_1
	{USBKEYC_2,					'2',	'@'},		// KEY_2
	{USBKEYC_3,					'3',	'#'},		// KEY_3
	{USBKEYC_4,					'4',	'$'},		// KEY_4
	{USBKEYC_5,					'5',	'%'},		// KEY_5
	{USBKEYC_6,					'6',	'^'},		// KEY_6
	{USBKEYC_7,					'7',	'&'},		// KEY_7
	{USBKEYC_8,					'8',	'*'},		// KEY_8
	{USBKEYC_9,					'9',	'('},		// KEY_9
	{USBKEYC_0,					'0',	')'},		// KEY_0
	{USBKEYC_MINUS,				'-',	'_'},		// KEY_MINUS
	{USBKEYC_EQUAL_101,			'=',	'+'},		// KEY_EQUALS
	{USBKEYC_BS,				BACK,	BACK},		// KEY_BACKSPACE

	{USBKEYC_TAB,				TAB,	TAB},		// KEY_TAB
	{USBKEYC_Q,					'q',	'Q'},		// KEY_Q
	{USBKEYC_W,					'w',	'W'},		// KEY_W
	{USBKEYC_E,					'e',	'E'},		// KEY_E
	{USBKEYC_R,					'r',	'R'},		// KEY_R
	{USBKEYC_T,					't',	'T'},		// KEY_T
	{USBKEYC_Y,					'y',	'Y'},		// KEY_Y
	{USBKEYC_U,					'u',	'U'},		// KEY_U
	{USBKEYC_I,					'i',	'I'},		// KEY_I
	{USBKEYC_O,					'o',	'O'},		// KEY_O
	{USBKEYC_P,					'p',	'P'},		// KEY_P
	{USBKEYC_LEFT_BRACKET_101,	'[',	'{'},		// KEY_LEFTBRACKET
	{USBKEYC_RIGHT_BRACKET_101,	']',	'}'},		// KEY_RIGHTBRACKET
	{USBKEYC_BACKSLASH_101,		'\\',	'|'},		// KEY_BACKSLASH

	{USBKEYC_CAPS_LOCK,			0,		0},			// KEY_CAPSLOCK
	{USBKEYC_A,					'a',	'A'},		// KEY_A
	{USBKEYC_S,					's',	'S'},		// KEY_S
	{USBKEYC_D,					'd',	'D'},		// KEY_D
	{USBKEYC_F,					'f',	'F'},		// KEY_F
	{USBKEYC_G,					'g',	'G'},		// KEY_G
	{USBKEYC_H,					'h',	'H'},		// KEY_H
	{USBKEYC_J,					'j',	'J'},		// KEY_J
	{USBKEYC_K,					'k',	'K'},		// KEY_K
	{USBKEYC_L,					'l',	'L'},		// KEY_L
	{USBKEYC_SEMICOLON,			';',	':'},		// KEY_SEMICOLON
	{USBKEYC_QUOTATION_101,		'\'',	'"'},		// KEY_APOSTROPHE
	{USBKEYC_ENTER,				ENTER,	ENTER},		// KEY_ENTER

	{0,							0,		0},			// KEY_LEFTSHIFT
	{USBKEYC_Z,					'z',	'Z'},		// KEY_Z
	{USBKEYC_X,					'x',	'X'},		// KEY_X
	{USBKEYC_C,					'c',	'C'},		// KEY_C
	{USBKEYC_V,					'v',	'V'},		// KEY_V
	{USBKEYC_B,					'b',	'B'},		// KEY_B
	{USBKEYC_N,					'n',	'N'},		// KEY_N
	{USBKEYC_M,					'm',	'M'},		// KEY_M
	{USBKEYC_COMMA,				',',	'<'},		// KEY_COMMA
	{USBKEYC_PERIOD,			'.',	'>'},		// KEY_PERIOD
	{USBKEYC_SLASH,				'/',	'?'},		// KEY_FORWARDSLASH
	{0,							0,		0},			// KEY_RIGHTSHIFT

	{0,							0,		0},			// KEY_LEFTCTRL
	{0,							0,		0},			// KEY_LEFTWIN
	{0,							0,		0},			// KEY_LEFTALT
	{USBKEYC_SPACE,				' ',	' '},		// KEY_SPACE
	{0,							0,		0},			// KEY_RIGHTALT
	{0,							0,		0},			// KEY_RIGHTWIN
	{USBKEYC_APPLICATION,		0,		0},			// KEY_APPLICATION
	{0,							0,		0},			// KEY_RIGHTCTRL

	{USBKEYC_INSERT,			0,		0},			// KEY_INSERT
	{USBKEYC_HOME,				0,		0},			// KEY_HOME
	{USBKEYC_PAGE_UP,			0,		0},			// KEY_PAGEUP
	{USBKEYC_DELETE,			0,		0},			// KEY_DELETE
	{USBKEYC_END,				0,		0},			// KEY_END
	{USBKEYC_PAGE_DOWN,			0,		0},			// KEY_PAGEDOWN

	{USBKEYC_UP_ARROW,			UP,		UP},		// KEY_UP
	{USBKEYC_DOWN_ARROW,		DOWN,	DOWN},		// KEY_DOWN
	{USBKEYC_LEFT_ARROW,		LEFT,	LEFT},		// KEY_LEFT
	{USBKEYC_RIGHT_ARROW,		RIGHT,	RIGHT},		// KEY_RIGHT

	{USBKEYC_NUM_LOCK,			0,		0},			// KEY_NUMLOCK
	{USBKEYC_KPAD_SLASH,		'/',	'/'},		// KEY_NUMPADDIVIDE
	{USBKEYC_KPAD_ASTERISK,		'*',	'*'},		// KEY_NUMPADMULTIPLY
	{USBKEYC_KPAD_MINUS,		'-',	'-'},		// KEY_NUMPADSUBTRACT
	{USBKEYC_KPAD_PLUS,			'+',	'+'},		// KEY_NUMPADADD
	{USBKEYC_KPAD_ENTER,		ENTER,	ENTER},		// KEY_NUMPADENTER
	{USBKEYC_KPAD_PERIOD,		'.',	'.'},		// KEY_NUMPADDECIMAL	// Keep together for Numpad processing
	{USBKEYC_KPAD_0,			'0',	'0'},		// KEY_NUMPAD0			// "
	{USBKEYC_KPAD_1,			'1',	'1'},		// KEY_NUMPAD1			// "
	{USBKEYC_KPAD_2,			'2',	'2'},		// KEY_NUMPAD2			// "
	{USBKEYC_KPAD_3,			'3',	'3'},		// KEY_NUMPAD3			// "
	{USBKEYC_KPAD_4,			'4',	'4'},		// KEY_NUMPAD4			// "
	{USBKEYC_KPAD_5,			'5',	'5'},		// KEY_NUMPAD5			// "
	{USBKEYC_KPAD_6,			'6',	'6'},		// KEY_NUMPAD6			// "
	{USBKEYC_KPAD_7,			'7',	'7'},		// KEY_NUMPAD7			// "
	{USBKEYC_KPAD_8,			'8',	'8'},		// KEY_NUMPAD8			// "
	{USBKEYC_KPAD_9,			'9',	'9'},		// KEY_NUMPAD9			// "
	{0,							',',	','},		// KEY_NUMPADCOMMA  (NEC PC-98 Japanese keyboard)
	{0,							'=',	'='},		// KEY_NUMPADEQUALS (NEC PC-98 Japanese keyboard)
	{0,							'/',	'/'},		// KEY_ABNT_C1 (Brazilian keyboard)
	{0,							'.',	'.'},		// KEY_ABNT_C2 (Brazilian keyboard)

	{0,							0,		0},			// KEY_WAKE
	{0,							0,		0},			// KEY_SLEEP
	{0,							0,		0},			// KEY_POWER

	{0,							0,		0},			// KEY_MAIL
	{0,							0,		0},			// KEY_CALCULATOR
	{0,							0,		0},			// KEY_MYCOMPUTER

	{0,							0,		0},			// KEY_MEDIASELECT
	{0,							0,		0},			// KEY_MEDIASTOP
	{0,							0,		0},			// KEY_PLAYPAUSE
	{0,							0,		0},			// KEY_NEXTTRACK
	{0,							'^',	'^'},		// KEY_PREVTRACK  (also Circumflex on Japanese keyboard)

	{0,							0,		0},			// KEY_MUTE
	{0,							0,		0},			// KEY_VOLUMEUP
	{0,							0,		0},			// KEY_VOLUMEDOWN

	{0,							0,		0},			// KEY_WEBHOME
	{0,							0,		0},			// KEY_WEBFORWARD
	{0,							0,		0},			// KEY_WEBBACK
	{0,							0,		0},			// KEY_WEBREFRESH
	{0,							0,		0},			// KEY_WEBFAVORITES
	{0,							0,		0},			// KEY_WEBSEARCH
	{0,							0,		0},			// KEY_WEBSTOP

	{USBKEYC_COLON_106,			':',	':'},		// KEY_COLON     (Japanese keyboard)
	{0,							0,		0},			// KEY_CONVERT   (Japanese keyboard)
	{USBKEYC_KANA,				0,		0},			// KEY_KANA      (Japanese keyboard)
	{USBKEYC_106_KANJI,			0,		0},			// KEY_KANJI     (Japanese keyboard)
	{0,							0,		0},			// KEY_AT        (Japanese keyboard)
	{0,							0,		0},			// KEY_AX        (Japanese keyboard)
	{0,							0,		0},			// KEY_NOCONVERT (Japanese keyboard)
	{0,							0,		0},			// KEY_UNLABELED (Japanese keyboard)
	{USBKEYC_YEN_106,			0,		0},			// KEY_YEN       (Japanese keyboard)
	{0,							0,		0},			// KEY_STOP      (NEC PC-98 Japanese keyboard)
	{0,							'_',	'_'},		// KEY_UNDERLINE (NEC PC-98 Japanese keyboard)
	{0,							0,		0},			// KEY_OEM_102   (British and German keyboards)
};
#endif


// Keyboard variables
static struct {
	//------------------------
	// Platform specific data
	//------------------------
	#if defined(_WINDOWS)
	LPDIRECTINPUT8		 di8;						// Pointer to Direct Input 8 object
	LPDIRECTINPUTDEVICE8 device;					// Pointer to keyboard device
	#endif
	#if defined(_XBOX) && defined(_DEBUG)
	int                  attached;					// Bits 0-3 track ports with an attached keyboard
	HANDLE               handles[4];				// Handle for each open keyboard
	#endif
	#if defined(_PS2)
	int					 numconnections;			// Max number of connected keyboards
	USBKBDATA_t			 data;						// Data from USB keyboard
	#endif
	//------------------------
	// Common data
	//------------------------
	unsigned char		 state[KEY_MAXBUTTONS];		// State byte for each key
	float				 repeat[KEY_MAXBUTTONS];	// Repeat time for each key
	int					 capslock;					// CAPSLOCK state
	int					 numlock;					// NUMLOCK state
	char				 buffer[MAXBUFFEREDINPUT];	// Buffered input
	int					 bufferindex;				// Current position in buffered input
} _keyboard;

/******************************************************************************
 Initialize keyboard
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void keyboard_init
(
	int window,			// application window handle
	int instance		// application instance handle
)
{
	// Clear keyboard variables
	memset (&_keyboard, 0, sizeof(_keyboard));

	//------------------------------------
	// Initialize Windows keyboard
	//------------------------------------

	#if defined(_WINDOWS)
	// Create the direct input object
	HRESULT result = DirectInput8Create ((HINSTANCE) instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &_keyboard.di8, NULL);
	assert (result == DI_OK);

	// Create the keyboard device
	result = _keyboard.di8->CreateDevice(GUID_SysKeyboard, &_keyboard.device, NULL); 
	if (result == DI_OK)
	{
		// Set the data format for the keyboard
		result = _keyboard.device->SetDataFormat(&c_dfDIKeyboard); 
		assert (result == DI_OK);

		// Set the cooperative level for the keyboard
		result = _keyboard.device->SetCooperativeLevel((HWND) window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
		assert (result == DI_OK);
	}

	// Initialize toggles
	BYTE data[256];
	GetKeyboardState (data);
	_keyboard.capslock = data[VK_CAPITAL] & 1;
	_keyboard.numlock  = data[VK_NUMLOCK] & 1;
	#endif

	//------------------------------------
	// Initialize XBOX keyboard (in Debug build only)
	//------------------------------------
	#if defined(_XBOX) && defined(_DEBUG)
	XINPUT_DEBUG_KEYQUEUE_PARAMETERS parameters = {
		XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN + XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP,	// Event flags
		256,																	// Queue size
		1,																		// Repeat delay
		1																		// Repeat interval
	};

	// Initialize keyboard event queue
	XInputDebugInitKeyboardQueue (&parameters);

	// Enumerate attached keyboards
	_keyboard.attached = XGetDevices (XDEVICE_TYPE_DEBUG_KEYBOARD);

	// Iterate through ports and initialize all attached keyboards
	for (int i = 0; i < 4; i++)
	{
		if (_keyboard.attached & (1 << i))
		{
			_keyboard.handles[i] = XInputOpen(XDEVICE_TYPE_DEBUG_KEYBOARD, i, XDEVICE_NO_SLOT, NULL);
		}
	}
	#endif

	//------------------------------------
	// Initialize PS2 keyboard
	//------------------------------------

	#if defined(_PS2)
	// Initialize keyboard
	sceUsbKbInit(&_keyboard.numconnections);

	// Set repeat parameters to repeat key every frame if it is pressed.
	// This is the only way to get raw state information every frame.
	sceUsbKbSetRepeat (0, 1, 1);

	// Tell keyboard to return raw USB key codes
	sceUsbKbSetCodeType (0, USBKB_CODETYPE_RAW);

	// Start asynchronous read
	sceUsbKbRead (0, &_keyboard.data);
	#endif

	//------------------------------------
	// Common initialization
	//------------------------------------

	// Set key repeat delays
	for (int i = 0; i < KEY_MAXBUTTONS; i++) _keyboard.repeat[i] = REPEATSTART;

}


/******************************************************************************
 Latch keyboard state
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void keyboard_update (float elapsedtime)
{
	#if defined(_XBOX) && defined(_RELEASE)
	    return;
    #endif
	// The update code for each platform fills out the following raw key map with the
	// raw state of each key. A common update then follows that processes the raw key
	// map to record the state of each key and add ascii keystrokes to the input buffer.
	unsigned char rawkeymap[256];
	memset(rawkeymap, 0, sizeof(rawkeymap));
	int alt   = 0;					// Tracks state of Alt key
	int shift = 0;					// Tracks state of Shift key

	//------------------------------------
	// Update Windows keyboard
	//------------------------------------

	#if defined(_WINDOWS)
	// Acquire the keyboard and read raw data
	HRESULT result = _keyboard.device->Acquire();
	if (FAILED(result))
	{
		// We don't have the keyboard device, so clear its state
		memset(&rawkeymap, 0, sizeof(rawkeymap));
	}
	else
	{
		// Read the keyboard device raw data
		result = _keyboard.device->GetDeviceState(sizeof(rawkeymap), rawkeymap); 
		assert (result == DI_OK);
	}

	// Get state of the alt and shift keys
	alt   = rawkeymap[DIK_LMENU]  | rawkeymap[DIK_RMENU];
	shift = rawkeymap[DIK_LSHIFT] | rawkeymap[DIK_RSHIFT];

	// If numlock is off, shadow main keyboard keys from numerical keypad
	if (!_keyboard.numlock)
	{
		rawkeymap[DIK_HOME]   |= rawkeymap[DIK_NUMPAD7];
		rawkeymap[DIK_UP]     |= rawkeymap[DIK_NUMPAD8];
		rawkeymap[DIK_PRIOR]  |= rawkeymap[DIK_NUMPAD9];
		rawkeymap[DIK_LEFT]   |= rawkeymap[DIK_NUMPAD4];
		rawkeymap[DIK_RIGHT]  |= rawkeymap[DIK_NUMPAD6];
		rawkeymap[DIK_END]    |= rawkeymap[DIK_NUMPAD1];
		rawkeymap[DIK_DOWN]   |= rawkeymap[DIK_NUMPAD2];
		rawkeymap[DIK_NEXT]   |= rawkeymap[DIK_NUMPAD3];
		rawkeymap[DIK_INSERT] |= rawkeymap[DIK_NUMPAD0];
		rawkeymap[DIK_DELETE] |= rawkeymap[DIK_DECIMAL];
	}
	#endif

	//------------------------------------
	// Update XBOX keyboard (in Debug build only)
	//------------------------------------
	#if defined(_XBOX) && defined(_DEBUG)
	if( _keyboard.attached == 0 )
		return;
	// Get insertions and removals since last update
	DWORD insertions;
	DWORD removals;
	XGetDeviceChanges (XDEVICE_TYPE_DEBUG_KEYBOARD, &insertions, &removals);

	// Close any keyboards that were removed
	for (int j = 0; j < 4; j++)
	{
		if (removals && (1 << j))
		{
			_keyboard.attached &= ~(1 << j);
			XInputClose (_keyboard.handles[j]);
		}
	}

	// Open any keyboards that were inserted
	for (int k = 0; k < 4; k++)
	{
		if (insertions && (1 << k))
		{
			_keyboard.attached  |= (1 << k);
			_keyboard.handles[k] = XInputOpen(XDEVICE_TYPE_DEBUG_KEYBOARD, k, XDEVICE_NO_SLOT, NULL);
		}
	}

	// If there's a keyboard attached then read and process the keystrokes
	if (_keyboard.attached)
	{
		XINPUT_DEBUG_KEYSTROKE key;
		while (XInputDebugGetKeystroke (&key) == ERROR_SUCCESS)
		{
			// Update raw keymap with key event
			if (key.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP) rawkeymap[key.VirtualKey] = 0x00;
			else                                               rawkeymap[key.VirtualKey] = 0x80;

			// Keep track of last state of alt and shift keys
			alt   = key.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_ALT;
			shift = key.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_SHIFT;
		}
	}
	#endif

	//------------------------------------
	// Update PS2 keyboard
	//------------------------------------

	#if defined(_PS2)
	// Get result of last read
	int result;
	int status = sceUsbKbSync(USBKB_NO_WAIT, &result);

	// Exit if read has not completed
	if (status == USBKB_EXEC) return;

	// If there was a read error then try again
	if ((status != USBKB_OK) || (result != USBKB_OK))
	{
		sceUsbKbRead (0, &_keyboard.data);
		return;
	}

	// Parse all keys into a raw key map
	memset (rawkeymap, 0, sizeof(rawkeymap));
	for (int i = 0; i < _keyboard.data.len; i++)
	{
		rawkeymap[_keyboard.data.keycode[i] & 0xff] = 0x80;
	}

	// Get state of alt and shift keys
	alt   = _keyboard.data.mkey & 0x44;
	shift = _keyboard.data.mkey & 0x22;

	// Start read for next update
	sceUsbKbRead (0, &_keyboard.data);
	#endif

	//------------------------------------
	// Common update
	//------------------------------------

	// Update the state of each key
	for (int i = 0; i < KEY_MAXBUTTONS; i++)
	{
		// Look up the device key code, skip if this key is not supported
		int devicecode = keyxlat[i].keycode;
		if (devicecode == 0) continue;

		// Set bit 7 of the state if the key is down
		// Set bit 6 of the state if the key position changed since the last update
		// Bits 0-5 will hold last 6 frames worth of changed bits
		_keyboard.state[i] ^=  rawkeymap[devicecode];
		_keyboard.state[i] >>= 1;
		_keyboard.state[i] |=  rawkeymap[devicecode];

		// Do key repeat
		if (_keyboard.state[i] & 0x80)
		{
			_keyboard.repeat[i] -= elapsedtime;
			if (_keyboard.repeat[i] <= 0)
			{
				_keyboard.state[i]  |= 0x40;
				_keyboard.repeat[i]  = REPEATINTERVAL;
			}
		}
		else _keyboard.repeat[i] = REPEATSTART;

		// If the key has just been pressed then do some processing on it.
		// Note, we ensure the changed bits from the last 2 cycles are clear in order to
		// prevent letters repeating accidentally on PS2. This is a side-effect of the way
		// we read raw data from  the keyboard by telling keys to repeat every 1 VSync.
		if ((_keyboard.state[i] & 0xf0) == 0xc0)
		{
			// Toggle CAPSLOCK and NUMLOCK
			if (i == KEY_CAPSLOCK) {_keyboard.capslock ^= 1; continue;}
			if (i == KEY_NUMLOCK)  {_keyboard.numlock  ^= 1; continue;}

			// Add valid ascii character to our buffered input
			if (_keyboard.bufferindex < (MAXBUFFEREDINPUT-1))
			{
				// Ignore ENTER key if ALT is pressed
				if ((alt) && (i == KEY_ENTER)) continue;

				// If NUMLOCK is off filter out digits from numerical keypad
				if ((!_keyboard.numlock) && ((i >= KEY_NUMPADDECIMAL) && (i <= KEY_NUMPAD9))) continue;

				// Look up the ascii code
				char c = shift ? keyxlat[i].shiftascii : keyxlat[i].ascii;

				// Invert alphabet character if CAPSLOCK is on
				if ((_keyboard.capslock) && (((c >='a') && (c <= 'z')) || ((c >='A') && (c <= 'Z')))) c ^= 0x20;

				// Add character to our buffered input
				if (c != 0) _keyboard.buffer[_keyboard.bufferindex++] = c;
			}
		}
	}
}


/******************************************************************************
 Shutdown keyboard
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
void keyboard_deinit (void)
{
	//------------------------------------
	// Terminate Windows keyboard
	//------------------------------------

	#if defined(_WINDOWS)
	// Release the keyboard device
	HRESULT result;
	int     refcount;
	if (_keyboard.device)
	{
		// Unacquire the keyboard device
		result = _keyboard.device->Unacquire();	// This function call can fail, but that's okay

		// Release the keyboard device
		refcount = _keyboard.device->Release();
		assert (refcount == 0);
	}

	// Release the direct input object
	refcount = _keyboard.di8->Release();
	assert (refcount == 0);
	#endif

	//------------------------------------
	// Terminate XBOX keyboard (in Debug build only)
	//------------------------------------
	#if defined(_XBOX) && defined(_DEBUG)
	// Iterate through ports and close all attached keyboards
	for (int i = 0; i < 4; i++)
	{
		if (_keyboard.attached & (1 << i)) XInputClose(_keyboard.handles[i]);
	}
	#endif

	//------------------------------------
	// Terminate PS2 keyboard
	//------------------------------------

	#if defined(_PS2)
	// Release the keyboard device
	sceUsbKbEnd();
	#endif
}


/******************************************************************************
 Test state of a button
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
int keyboard_testkey	// returns TRUE if button is in state being tested for
(
	int key,			// key to test
	int state			// state to test for
)
{
	// Validate parameters
	assert (key < KEY_MAXBUTTONS);

	// Test if key is in desired state
	return ((_keyboard.state[key] & state) == (state >> 8));
}

/******************************************************************************
 Get buffered input
 (c) 2002 David O'Connor (procode@pacbell.net). All rights reserved.
******************************************************************************/
char* keyboard_getbufferedinput (void)	// Returns pointer to buffered input
{
	// Terminate buffered input
	_keyboard.buffer[_keyboard.bufferindex] = 0;

	// Flush buffered input
	_keyboard.bufferindex = 0;

	// Return buffered input
	return _keyboard.buffer;
}
