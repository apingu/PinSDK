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
#include "Controller.h"
#include <ctype.h>
#include <PGSPFile.h>


//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
CPGamepad::CPGamepad()
{
    m_Buttons[GAMEPAD_TOP]        = GAMEPAD_TOP;
	m_Buttons[GAMEPAD_BOTTOM]     = GAMEPAD_BOTTOM;
	m_Buttons[GAMEPAD_LEFT]       = GAMEPAD_LEFT;
	m_Buttons[GAMEPAD_RIGHT]      = GAMEPAD_RIGHT;
	m_Buttons[GAMEPAD_START]      = GAMEPAD_START;
	m_Buttons[GAMEPAD_SELECT]     = GAMEPAD_SELECT;
	m_Buttons[GAMEPAD_L1]         = GAMEPAD_L1;
	m_Buttons[GAMEPAD_R1]         = GAMEPAD_R1;
	m_Buttons[GAMEPAD_L2]         = GAMEPAD_L2;
	m_Buttons[GAMEPAD_R2]         = GAMEPAD_R2;
	m_Buttons[GAMEPAD_DPADLEFT]   = GAMEPAD_DPADLEFT;
	m_Buttons[GAMEPAD_DPADRIGHT]  = GAMEPAD_DPADRIGHT;
	m_Buttons[GAMEPAD_DPADUP]     = GAMEPAD_DPADUP;
	m_Buttons[GAMEPAD_DPADDOWN]   = GAMEPAD_DPADDOWN;
	m_Buttons[GAMEPAD_LEFTSTICK]  = GAMEPAD_LEFTSTICK;
	m_Buttons[GAMEPAD_RIGHTSTICK] = GAMEPAD_RIGHTSTICK;
	m_Buttons[GAMEPAD_BLACK]      = GAMEPAD_BLACK;
	m_Buttons[GAMEPAD_WHITE]      = GAMEPAD_WHITE;
}


//=============================================================================
//  Name : distructure
//  Desc :
//=============================================================================
CPGamepad::~CPGamepad()
{


}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void CPGamepad::SetMapping( DWORD mapping, DWORD real )
{
	m_Buttons[ mapping ] = real;
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
DWORD CPGamepad::GetButton( DWORD button )
{
	return m_Buttons[ button ];
}


char* GetButtonText( DWORD bid, char* text )
{
    switch( bid )
	{
	case GAMEPAD_TOP:        strcpy( text, "GAMEPAD_TOP" );       return text;
	case GAMEPAD_BOTTOM:     strcpy( text, "GAMEPAD_BOTTOM" );    return text;
	case GAMEPAD_LEFT:       strcpy( text, "GAMEPAD_LEFT" );      return text;
	case GAMEPAD_RIGHT:      strcpy( text, "GAMEPAD_RIGHT" );     return text;
	case GAMEPAD_START:      strcpy( text, "GAMEPAD_START" );     return text;
	case GAMEPAD_SELECT:     strcpy( text, "GAMEPAD_SELECT" );    return text;
	case GAMEPAD_L1:         strcpy( text, "GAMEPAD_L1" );        return text;
	case GAMEPAD_R1:         strcpy( text, "GAMEPAD_R1" );        return text;
	case GAMEPAD_L2:         strcpy( text, "GAMEPAD_L2" );        return text;
	case GAMEPAD_R2:         strcpy( text, "GAMEPAD_R2" );        return text;
	case GAMEPAD_DPADLEFT:   strcpy( text, "GAMEPAD_DPADLEFT" );  return text;
	case GAMEPAD_DPADRIGHT:  strcpy( text, "GAMEPAD_DPADRIGHT" ); return text;
	case GAMEPAD_DPADUP:     strcpy( text, "GAMEPAD_DPADUP" );    return text;
	case GAMEPAD_DPADDOWN:   strcpy( text, "GAMEPAD_DPADDOWN" );  return text;
	case GAMEPAD_LEFTSTICK:  strcpy( text, "GAMEPAD_LEFTSTICK" ); return text;
	case GAMEPAD_RIGHTSTICK: strcpy( text, "GAMEPAD_RIGHTSTICK" );return text;
	case GAMEPAD_BLACK:      strcpy( text, "GAMEPAD_BLACK" );     return text;
	case GAMEPAD_WHITE:      strcpy( text, "GAMEPAD_WHITE" );     return text;
	}

	return text;
}



///////////////////////////////////////////////////////////////////////////////
//
class GamepadScript : public CPGSPFile
{
public:

	CPGamepad * control;


	void OnInitiate( void )
	{
		AddDefineValue( "GAMEPAD_TOP", GAMEPAD_TOP );
		AddDefineValue( "GAMEPAD_BOTTOM", GAMEPAD_BOTTOM );
		AddDefineValue( "GAMEPAD_LEFT", GAMEPAD_LEFT );
		AddDefineValue( "GAMEPAD_RIGHT", GAMEPAD_RIGHT );
		AddDefineValue( "GAMEPAD_START", GAMEPAD_START );
		AddDefineValue( "GAMEPAD_SELECT", GAMEPAD_SELECT );
		AddDefineValue( "GAMEPAD_L1", GAMEPAD_L1 );
		AddDefineValue( "GAMEPAD_R1", GAMEPAD_R1 );
		AddDefineValue( "GAMEPAD_L2", GAMEPAD_L2 );
		AddDefineValue( "GAMEPAD_L2", GAMEPAD_L2 );
		AddDefineValue( "GAMEPAD_DPADLEFT", GAMEPAD_DPADLEFT );
		AddDefineValue( "GAMEPAD_DPADRIGHT", GAMEPAD_DPADRIGHT );
		AddDefineValue( "GAMEPAD_DPADUP", GAMEPAD_DPADUP );
		AddDefineValue( "GAMEPAD_DPADDOWN", GAMEPAD_DPADDOWN );
		AddDefineValue( "GAMEPAD_LEFTSTICK", GAMEPAD_LEFTSTICK );
		AddDefineValue( "GAMEPAD_RIGHTSTICK", GAMEPAD_RIGHTSTICK );
		AddDefineValue( "GAMEPAD_BLACK", GAMEPAD_BLACK );
		AddDefineValue( "GAMEPAD_WHITE", GAMEPAD_WHITE );
		AddDefineValue( "TOP", GAMEPAD_TOP );
		AddDefineValue( "BOTTOM", GAMEPAD_BOTTOM );
		AddDefineValue( "LEFT", GAMEPAD_LEFT );
		AddDefineValue( "RIGHT", GAMEPAD_RIGHT );
		AddDefineValue( "START", GAMEPAD_START );
		AddDefineValue( "SELECT", GAMEPAD_SELECT );
		AddDefineValue( "L1", GAMEPAD_L1 );
		AddDefineValue( "R1", GAMEPAD_R1 );
		AddDefineValue( "L2", GAMEPAD_L2 );
		AddDefineValue( "L2", GAMEPAD_L2 );
		AddDefineValue( "DPADLEFT", GAMEPAD_DPADLEFT );
		AddDefineValue( "DPADRIGHT", GAMEPAD_DPADRIGHT );
		AddDefineValue( "DPADUP", GAMEPAD_DPADUP );
		AddDefineValue( "DPADDOWN", GAMEPAD_DPADDOWN );
		AddDefineValue( "LEFTSTICK", GAMEPAD_LEFTSTICK );
		AddDefineValue( "RIGHTSTICK", GAMEPAD_RIGHTSTICK );
		AddDefineValue( "BLACK", GAMEPAD_BLACK );
		AddDefineValue( "WHITE", GAMEPAD_WHITE );

	}

	/*
	DWORD GetVard( const char* value )
	{
		if ( ( strcmp( value, "GAMEPAD_TOP" ) == 0 ) || 
		     ( strcmp( value, "TOP" ) == 0 ) )
		    return GAMEPAD_TOP;

		else if ( ( strcmp( value, "GAMEPAD_BOTTOM" ) == 0 ) ||
			 ( strcmp( value, "BOTTOM" ) == 0 ) )
			return GAMEPAD_BOTTOM;

		else if ( ( strcmp( value, "GAMEPAD_LEFT" ) == 0 ) ||
		     ( strcmp( value, "LEFT" ) == 0 ) )
			return GAMEPAD_LEFT;
				else if ( ( strcmp( value, "GAMEPAD_RIGHT" ) == 0 ) ||
			 ( strcmp( value, "RIGHT" ) == 0 ) )
		    return GAMEPAD_RIGHT;

		else if ( ( strcmp( value, "GAMEPAD_START" ) == 0 ) ||
			 ( strcmp( value, "START" ) == 0 ) )
			return GAMEPAD_START;

		else if ( ( strcmp( value, "GAMEPAD_SELECT" ) == 0 ) ||
			 ( strcmp( value, "SELECT" ) == 0 ) )
			return GAMEPAD_SELECT;

		else if ( ( strcmp( value, "GAMEPAD_L1" ) == 0 ) ||
			 ( strcmp( value, "L1" ) == 0 ) )
			return GAMEPAD_L1;

		else if ( ( strcmp( value, "GAMEPAD_R1" ) == 0 ) ||
			 ( strcmp( value, "R1" ) == 0 ) )
			return GAMEPAD_R1;

		else if ( ( strcmp( value, "GAMEPAD_L2" ) == 0 ) ||
			 ( strcmp( value, "L2" ) == 0 ) )
			return GAMEPAD_L2;

		else if ( ( strcmp( value, "GAMEPAD_R2" ) == 0 ) ||
			 ( strcmp( value, "R2" ) == 0 ) )
			return GAMEPAD_R2;

		else if( ( strcmp( value, "GAMEPAD_DPADLEFT" ) == 0 ) ||
			( strcmp( value, "DPADLEFT" ) == 0 ) )
			return GAMEPAD_DPADLEFT;

		else if ( ( strcmp( value, "GAMEPAD_DPADRIGHT" ) == 0 ) ||
			 ( strcmp( value, "DPADRIGHT" ) == 0 ) )
			return GAMEPAD_DPADRIGHT;

		else if ( ( strcmp( value, "GAMEPAD_DPADUP" ) == 0 ) ||
			 ( strcmp( value, "DPADUP" ) == 0 ) )
			return GAMEPAD_DPADUP;

		else if ( ( strcmp( value, "GAMEPAD_DPADDOWN" ) == 0 ) ||
			 ( strcmp( value, "DPADDOWN" ) == 0 ) )
			return GAMEPAD_DPADDOWN;

		else if ( ( strcmp( value, "GAMEPAD_LEFTSTICK" ) == 0 ) ||
			 ( strcmp( value, "LEFTSTICK" ) == 0 ) )
			return GAMEPAD_LEFTSTICK;

		else if ( ( strcmp( value, "GAMEPAD_RIGHTSTICK" ) == 0 ) ||
			( strcmp( value, "RIGHTSTICK" ) == 0 ) )
			return GAMEPAD_RIGHTSTICK;

		else if ( ( strcmp( value, "GAMEPAD_BLACK" ) == 0 ) ||
			 ( strcmp( value, "BLACK" ) == 0 ) )
			return GAMEPAD_BLACK;

		else if ( ( strcmp( value, "GAMEPAD_WHITE" ) == 0 ) ||
			 ( strcmp( value, "WHITE" ) == 0 ) )
			return GAMEPAD_WHITE;

		else
			return 0;
	}
	*/

	
	void* OnCreateObject( const char* name )
	{
		if( strcmp( name, "GAMEPAD" ) == 0 )
		{
			return control;
		}
		if( strcmp( name, "BUTTON") == 0 )
		{
			return control;
		}
		return NULL;
	}

    void OnReadAttribute( const char* name,
                          void* obj,
                          char* attribute,
                          char* value )
	{
		if( ( strcmp( name, "BUTTON") == 0 ) &&
			( strcmp( GetParent(), "GAMEPAD" ) == 0 ) )
		{
			control->SetMapping( GetVard( attribute ), GetVard( value ) );
			return;
		}

		return;

	}

	void OnCloseObject( const char* name, void* obj )
	{


	}
};


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
int CPGamepad::Load( const char* filename )
{
	GamepadScript sf;
	sf.control = this;
	return sf.Load( filename );
}



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
int CPGamepad::Save( const char* filename )
{
	GamepadScript writer;

	char buf[255];

	writer.Open( filename );

	writer.CreateObject("GAMEPAD");
	writer.CreateObject("BUTTON");

	writer.SetReadAttribute( "TOP", GetButtonText(m_Buttons[0], buf) );
	writer.SetReadAttribute( "BOTTOM", GetButtonText(m_Buttons[1], buf) );
	writer.SetReadAttribute( "LEFT", GetButtonText(m_Buttons[2], buf) );
	writer.SetReadAttribute( "RIGHT", GetButtonText(m_Buttons[3], buf) );
	writer.SetReadAttribute( "START", GetButtonText(m_Buttons[4], buf) );
	writer.SetReadAttribute( "SELECT", GetButtonText(m_Buttons[5], buf) );
	writer.SetReadAttribute( "L1", GetButtonText(m_Buttons[6], buf) );
	writer.SetReadAttribute( "R1", GetButtonText(m_Buttons[7], buf) );
	writer.SetReadAttribute( "L2", GetButtonText(m_Buttons[8], buf) );
	writer.SetReadAttribute( "R2", GetButtonText(m_Buttons[9], buf) );
	writer.SetReadAttribute( "DPADLEFT", GetButtonText(m_Buttons[10], buf) );
	writer.SetReadAttribute( "DPADRIGHT", GetButtonText(m_Buttons[11], buf) );
	writer.SetReadAttribute( "DPADUP", GetButtonText(m_Buttons[12], buf) );
	writer.SetReadAttribute( "DPADDOWN", GetButtonText(m_Buttons[13], buf) );
	writer.SetReadAttribute( "LEFTSTICK", GetButtonText(m_Buttons[14], buf) );
	writer.SetReadAttribute( "RIGHTSTICK", GetButtonText(m_Buttons[15], buf) );
	writer.SetReadAttribute( "BLACK", GetButtonText(m_Buttons[16], buf) );
	writer.SetReadAttribute( "WHITE", GetButtonText(m_Buttons[17], buf) );

	writer.CloseObject();
	writer.CloseObject();

	return 1;

}
