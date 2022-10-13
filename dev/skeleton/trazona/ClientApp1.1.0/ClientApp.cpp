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
#include <ZonaServices.h>
#include "uncode.h"
#include "ClientApp.h"
#include "..\\Comm\\ConfigReader.h"


ZonaServices          gZonaServer;
char Cfgfname[256] = "clientdef.cfg";


ZonaClientCharacter* getClientCharacter( char* name )
{
    ZonaClientCharacterPtrVector characters;
    int zdbrval = gZonaServer.getCharacters(characters); 
    int size = characters.getSize();

    // no characters to select so return FALSE
    if(size == 0) return NULL;

	ZonaClientCharacter *character;
	
    // check to see if any characters with data blobs exist and store them
    for(int i = 0; i < size; i++)
    {
        character = characters[i];

	    char aname[255];
        wtoa( (unsigned short*)character->getName(), aname, 255);
      	  
	    if( strcmp( aname, name ) == 0 )
		    return character;
	}

	return NULL;
}


//=============================================================================
//
//
//=============================================================================
int LoginServer( char* username, char* password, int Times )
{
	char Gver[255];
	char Host[255];
	char Port[255];

	//get game version
	char *gver = CConfigReader::GetStringOnce(Cfgfname,"GAMEVER");
	if( gver != NULL )
	{
	    strcpy( Gver, gver );
	    delete [] gver;
	}

	//get host and port define
    char *host = CConfigReader::GetStringOnce(Cfgfname,"HOST");
	if( host != NULL )
	{
        strcpy( Host, host );
        delete [] host;
	}

	char *port = CConfigReader::GetStringOnce(Cfgfname,"PORT");
	if( port != NULL )
	{
		strcpy( Port, port );
	    delete [] port;
	}

    gZonaServer.setGameVersion( Gver, strlen(Gver) + 1 );
    // try to login
	for( int i = 0; i < Times; i++ )
	{
		int ret = gZonaServer.login(Host, username, password);
        if( ret == ERR_SUCCESS )
		    return 1;
		Sleep( 100 );
	}

	return 0;
}


//=============================================================================
//
//
//=============================================================================
int LoginServer( int Times )
{
	char Username[255];
	char Password[255];

	//try to read configure file
	char* username = CConfigReader::GetStringOnce(Cfgfname,"USERNAME");
	strcpy( Username, username );
	delete [] username;

	//try to read configure file
	char* password = CConfigReader::GetStringOnce(Cfgfname,"PASSWORD");
	strcpy( Password, password );
	delete [] password;

	return LoginServer( Username, Password, Times );
}


//=============================================================================
//
//
//=============================================================================
int LogoutServer( char* name )
{
	getClientCharacter( name )->exit();
	gZonaServer.logoff();
	return 1;
}



//=============================================================================
//
//
//=============================================================================
int CreateCharacter( char* name, void* displayProp, unsigned int displaySize,
					             void* statusProp,  unsigned int statusSize )
{
    // create the character
    //if(!psCharacterCreateScreen((RwChar*)master)) return FALSE;
    wchar_t wname[255];
    atow(name, wname, strlen(name)+1);

	ZonaClientCharacter character;
	character.setName(wname);

	//Setup public state properties of that character
	character.setPublicDisplayProperty((char*)&displayProp, displaySize);
	character.setPublicStateProperty((char*)&statusProp, statusSize);
	
	// Create a new character
	if( gZonaServer.createCharacter(character) != ERR_SUCCESS )
	    return -1;

    return character.getId();
}


 //=============================================================================
//
//
//=============================================================================
int SelectCharacter( char* name, void* displayProp, unsigned int displaySize,
					             void* statusProp,  unsigned int statusSize )
{
    ZonaClientCharacterPtrVector characters;
    int zdbrval = gZonaServer.getCharacters(characters); 
    int size = characters.getSize();

    // no characters to select so return FALSE
    if(size == 0) return -1;

	ZonaClientCharacter *character = getClientCharacter( name );
	if( character != NULL )
	{
	    if( character->select() == ERR_SUCCESS )
		{
	  	    if( displayProp != NULL )
				character->setPublicDisplayProperty( (char*)&displayProp, displaySize);
			if( statusProp != NULL )
				character->setPublicStateProperty( (char*)statusProp, statusSize );

			if( ( displayProp != NULL ) || ( statusProp != NULL ) )
			{
				if( character->update() != ERR_SUCCESS )
					return -1;
			}

		    return character->getId();
		}
		return -1;
	}

	/*	
    // check to see if any characters with data blobs exist and store them
    for(int i = 0; i < size; i++)
    {
        character = characters[i];

	    char aname[255];
        wtoa( (unsigned short*)character->getName(), aname, 255);
      	  
	    if( strcmp( aname, name ) == 0 )
		{
		    int entityid = character->getId();
  		    if( character->select() == ERR_SUCCESS )
			{
	  		    if( initdata != NULL )
				{
	                // update the characters persistant data
			        character->setPublicStateProperty( (char*)initdata, datasize );
                    if( character->update() != ERR_SUCCESS )
                        return -1;
				}
		        return character->getId();
			}
		    return -1;
		}
	}
	*/

	return -1;
}


//=============================================================================
//
//
//=============================================================================
int DeleteAllCharacter( void )
{
    ZonaClientCharacterPtrVector characters;
    if( !gZonaServer.getCharacters(characters) )
		return -1;

    for( int i = 0; i < characters.getSize(); i++ )
	{
		ZonaClientCharacter *character;
        // check to see if any characters with data blobs exist and store them
        character = characters[i];
        // for now there are no child nodes of a character, so if a
        // character with an empty data blob is found then remove it from
        // the database
        gZonaServer.deleteCharacter( character->getId() );
    }
	return 1;
}


/*
//=============================================================================
//
//
//=============================================================================
int GetAllCharacter( void )
{
    GameCharacterPtrVector characters;
    GameCharacter *character;
	if( !gZonaServer.getCharacters(characters) )
		return -1;

    for( int i = 0; i < characters.getSize(); i++ )
	{
        // check to see if any characters with data blobs exist and store them
        character = characters[i];
        // for now there are no child nodes of a character, so if a
        // character with an empty data blob is found then remove it from
        // the database
        //gZonaServer.deleteCharacter(character->id);
    }

}
*/


//=============================================================================
//
//
//=============================================================================
int RegisterCallback( GameStateCallback* Gamecallback )
{
	gZonaServer.setGameStateCallback( Gamecallback );
    return 1;
	//return 0;
}


//=============================================================================
//
//
//=============================================================================
int EntityJoinGame( int entityid )
{
 	if( gZonaServer.monitorGameState() == ERR_SPHERE ) 
		return FALSE;

 	if( gZonaServer.subscribeToGameStateMsgs( entityid, ZONA_MSG_ALL) == ERR_SPHERE ) 
		return FALSE;

	return TRUE;
}



//=============================================================================
//
//
//=============================================================================
int SendStats( char* buf, int len )
{
	if( gZonaServer.sendGameStateMsg( buf, len ) == ERR_SUCCESS )
	    return 1;
	return 0;
}