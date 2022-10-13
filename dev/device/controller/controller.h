//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <pdef.h>
#include "Gamepad.h"
#include "Keyboard.h"


class CPGamepad
{
public:

	DWORD    m_Buttons[GAMEPAD_MAXBUTTONS];

public:

	CPGamepad();
	CPGamepad(const char* filename){ Load( filename ); }
	~CPGamepad();


	int   Load( const char* filename );
	int   Save( const char* filename );

	void  SetMapping( DWORD mapping, DWORD real );

	DWORD GetButton( DWORD button );

    inline Init( int window, int instance )
	{ gamepad_init(window, instance); }

	inline void  Deinit( void )
	{ gamepad_deinit(); }

	inline void  Update( void )
	{ gamepad_update(); }

	inline float Getbutton(int gamepad, int button )
	{ return gamepad_getbutton(GetButton( gamepad ), button); }

	inline int   Testbutton(int gamepad, int button, int state)
	{ return gamepad_testbutton(GetButton( gamepad ), button, state); } 

	inline float Getstickx(int gamepad, int stick)
	{ return gamepad_getstickx(GetButton( gamepad ), stick); }

	
	inline float Getsticky(int gamepad, int stick)
	{ return gamepad_getsticky(GetButton( gamepad ), stick); }

	inline float Getstickangle(int gamepad, int stick)
	{ return gamepad_getstickangle(GetButton( gamepad ), stick ); }

    inline float Getstickmagnitude (int gamepad, int stick)
	{ return gamepad_getstickmagnitude(GetButton( gamepad ), stick ); }

};

/*
class CPKeyboardMap
{
public:

	DWORD    m_Buttons[KEY_MAXBUTTONS];

public:

	CPGamepadMap();
	~CPGamepadMap();

	DWORD Button( DWORD button );

	void  SetMapping( DWORD mapping, DWORD real );

	int   Load( const char* filename );
	int   Save( const char* filename );

};
*/



#endif //CONTROLLER_H
