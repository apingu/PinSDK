/*
 ZGF Mouse module
 (c) 2002 Zona Inc. All rights reserved.
 Support available from http://www.zona.net and mailto:support@zona.net
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

#ifndef _ZGFMOUSE_H
#define _ZGFMOUSE_H

// Mouse buttons
enum {
	ZGFMOUSE_LEFT,
	ZGFMOUSE_RIGHT,
	ZGFMOUSE_CENTER,
	ZGFMOUSE_MAXBUTTONS
};

// Mouse button states
enum {
	ZGFMOUSE_STATE_UP       = 0x0080,	// Button is currently up
	ZGFMOUSE_STATE_DOWN     = 0x8080,	// Button is currently down
	ZGFMOUSE_STATE_RELEASED = 0x40c0,	// Button was released since last update
	ZGFMOUSE_STATE_PRESSED  = 0xc0c0,	// Button was pressed since last update
};

// Mouse functions
void  ZgfMouseInit       (int window, int instance);
void  ZgfMouseUpdate     (void);
void  ZgfMouseTerm       (void);
float ZgfMouseGetDeltaX  (void);
float ZgfMouseGetDeltaY  (void);
float ZgfMouseGetDeltaZ  (void);
int   ZgfMouseTestButton (int button, int state);

#endif