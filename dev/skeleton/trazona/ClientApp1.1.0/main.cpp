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
//#include <PWindow.h>
#include <windows.h>
#include "ClientApp.h"

#include "..\\Comm\\ConfigReader.h"
#include <ZonaServices.h>
#include "ClientCallBack.h"

//extern TraZonaClientCallback gGameCallback;

TraZonaClientCallback gGameCallback;



//ZonaServices gZonaServer; 	// instantiate the ZonaServices object

char gErrMessage[256];


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	//TZMessageCreature gmsg;
	//gmsg.fMoveState.fPos.x = 100.0f;
	//gmsg.fMoveState.fPos.y = 100.0f;
	//gmsg.fMoveState.fPos.z = 100.0f;


	if( LoginServer() == -1 )
	    sprintf(gErrMessage, " login server failure !!! " );

	//if( LoginServer( "testclient1", "testclient1" ) == -1 )
		//sprintf(gErrMessage, " login server failure !!! " );

	//if( CreateCharacter( "Apingu" ) != 1 )
	//	sprintf(gErrMessage, " create character failure !!! " );

	DeleteAllCharacter();


	int eid = -1;
    char *charname = CConfigReader::GetStringOnce("clientdef.cfg", "CHARNAME");
	if( charname != NULL )
	{
		CreateCharacter( charname, (void*)&gErrMessage, 256 );
		eid = SelectCharacter( charname );
	    if( eid == -1 )
		{
		    sprintf(gErrMessage, " select character failure !!! " );
			return -1;
		}
        delete [] charname;
	}
	

	RegisterCallback( &gGameCallback );

	EntityJoinGame( eid );

	//EntityJoinGame( entityid );

	char msg[255] = {"test12345"};

	for( int i= 0; i < 20; i++ )
	{
		SendStats( msg, strlen( msg ) );
		Sleep( 5000 );
	}


	//GetAllCharacter();
	/*
	ZonaServices zonaServices; 	// instantiate the ZonaServices object
	GameCharacterPtrVector characters;
	GameCharacter *character;

	char state[255] = {"test12345"};
	int ret;


	//zonaServices.setGameVersion("MO V0.1", strlen("MO V0.1")+1);
	zonaServices.setGameVersion("MO V0.1", strlen("MO V0.1")+1);

	int ret_code = zonaServices.login( "http://localhost:2000", "testclient1",  "testclient1", true);

	if ( ( ret_code != ERR_SUCCESS) ) {
		printf("Error logging onto Terazona server. exiting.\n");
		exit(ret_code);
	}

	wchar_t name[255];
	wcscpy(name, zonaServices.getLoginName());
	wprintf(L"\nMy name is %s\n", name);

	// register the game state callback object
	// (defined above) to receive the messages
	//zonaServices.registerGameStateCallback(new TrackerClient());

	// This example currently uses the ZonaBattle server side plugin 
	// for creating and accessing character data. The user will have
	// to use characters already created in ZonaBattle in order to find
	// existing characters for this example.
	CreateCharacter( "Pin" );
	zonaServices.getCharacters(characters);
	if ( characters.getSize() <= 0 ) {
		printf("No Characters found.\n");
		exit(1);
	}

	character = characters[0];
	zonaServices.selectCharacter(character->id);

	//Define unique entity ID
	int entityId = character->id;

	// enter the player into the spehere with a property, sizeof(property)
	// the property could be could be any data, here it is the string "this"
	zonaServices.enterCharacter();

	// subscribe to the game state messages using the gamestate callback object
	// (defined above) in order to process messages from other clients
	// Message types flags are defined in ZonaGlobalConstants.h as the following
	//		do not receive any messages				--- ZONA_MSG_NONE
	//		receive enter & exit spheres updates	--- ZONA_MSG_SPHERE_UPDATES
	//		receive game states						--- ZONA_MSG_GAMESTATE
	//		receive game events						--- ZONA_MSG_GAMEEVENT
	//		receive all messages					--- ZONA_MSG_ALL
	zonaServices.subscribeToGameStateMsgs(character->id,ZONA_MSG_ALL);




	// Send 100 messages to the Zona Game State Server.
	// In this case, we're sending information that looks like 2d positional data.
	while(1)
	{ 
		ret = zonaServices.sendGameStateMsg(state, strlen(state)); 
		if( ret == ERR_SUCCESS )
			break;
			// wait for half a second so the demonstration doesn't end at once
			//Sleep(msgFreq); 
	} 
	//printf("\nExiting example\n");
	//Sleep(200);

	// remove the player from the sphere
	zonaServices.exitCharacter();

	// remove the player from the game
	zonaServices.logoff();
	*/

    return 1;
}