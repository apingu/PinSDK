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
#ifndef CLIENTAPP_H
#define CLIENTAPP_H


#include <vector>
#include <callback/GameStateCallBack.h>

using namespace std;


//login fucntion
int LoginServer( char* username, char* password, int Times = 3 );
//find USERNAME and PASSWORD keyword
int LoginServer( int Times = 3 );

int LogoutServer( char* name );




//choice game role
//int GetAllCharacter( 

int CreateCharacter( char* name, 
					 void* displayProp=NULL, unsigned int displaySize=0,
					 void* statusProp=NULL, unsigned int statusSize=0
					 );

int SelectCharacter( char* name,
					 void* displayProp=NULL, unsigned int displaySize=0,
					 void* statusProp=NULL, unsigned int statusSize=0
					 );

int DeleteCharacter( char* name );
int DeleteAllCharacter( void );



//register game call back
int RegisterCallback( GameStateCallback* Gamecallback );


//
int EntityJoinGame( int entityid );


//
int SendStats( char* buf, int len );








#endif

