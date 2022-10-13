
/*********************************************************************
 * Copyright (c) 2002, Zona, Inc. All Rights Reserved 
 * Please visit http://www.zona.net 
 *											
 * Filename: ZonaGSValidate.cpp (For TrackerClient_ServerPlugin)
 *											
 * Original Author: Adam Lang
 *																		
 ********************************************************************/
#include <stdio.h>
#include <vector>
#include <iostream>
#include <windows.h>


#include <ZonaGSValidate.h>
#include <ZonaGSPublish.h>
//#include <Debug.h>
#define DebugOutPutf(s, t)       printf(s)

#include "TeraZonaEntityMgr.h"

char AppName[255] = "Terazona Game Server App";
char Cfgfname[255] = "serverdef.cfg";

static byte g_currentRegionId = 0;

//in TeraZona the world axis is X-Y
//but 3D world is X-Z
int gDimensionCX=10;
int gDimensionCY=1;
int gDimensionCZ=10; 

int gMapSizeCX=1;
int gMapSizeCY=1;
int gMapSizeCZ=1; 

CEntityMgr gEntityMgr;

#define regioncount()        ( gDimensionCX * gDimensionCY * gDimensionCZ )



//////////////////////////////////////////////////////////////////////
// tell me how to get position for package
//#include "..\\Comm\\Message.h"
//#include "..\\Comm\\tzmessage.h"
//#define MoveStateMessage( e ) ( ((TZMessageCreature*)(e))->fMoveState.fPos )
#define MoveStateMessage( e ) ( ((TZMessageCreatureMove*)(e))->fPropData.fPos )
#define getPositionX( e )    (0)//( ((TZMessageCreatureMove*)(e))->fPropData.fPos.x )
#define getPositionY( e )    (0)//( ((TZMessageCreatureMove*)(e))->fPropData.fPos.y )
#define getPositionZ( e )    (0)//( ((TZMessageCreatureMove*)(e))->fPropData.fPos.z )
#define IsMoveStateType( e ) (1)//( ((TZMessage*)e)->fType == TZMessage::kCreatureMoveState )


//#define EntityX( tz )  ((NetMessage*)tz)->x
//#define EntityY( tz )  ((NetMessage*)tz)->y
//#define EntityZ( tz )  ((NetMessage*)tz)->z




//=============================================================================
//  Name : 
//  Desc : this is not a zona function
//=============================================================================
int getRegionIdByDim( int x, int y, int z )
{
	//printf( "[CX : %d][CY : %d][CZ : %d]\n", x, y, z );
	//fflush( stdout );
	//TeraZona world axis is x-y so we must transform to x-y
	int i = ( z * gDimensionCX ) + x;
	if( i < 0 )
		int g = 0;


	return ( z * gDimensionCX ) + x;
}

//=============================================================================
//  Name : 
//  Desc : this is not a zona function
//=============================================================================
int getRegionIdByPos( float x, float y, float z )
{
	//offset to map center
	//if( x != 0 )
	//	int kkk = 0;
	x = x+(gMapSizeCX>>1);
	//y = y+(gMapSizeCY>>1);
	z = z+(gMapSizeCZ>>1);

	//	
	int gcx = gMapSizeCX/gDimensionCX;
	int gcy = gMapSizeCY/gDimensionCY;
	int gcz = gMapSizeCZ/gDimensionCZ;

	//printf( "[MX : %d][MY : %d][MZ : %d]", x, y, z );
	//fflush( stdout );

// By Gary Changt, 2/28/2003 - Y should always be 0, based on the current assumption. 
// It would become negative, if the character's goes underground where Y is negative

	return getRegionIdByDim( ((int)x)/gcx, 0, ((int)z)/gcz );
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
int getRegionIdForEntityRecord(byte* masterEntityState)
{
	//********TODO:**********************************************
	//DebugOutPutf(" getRegionIdForEntityRecord ", "Terazona" );
	return getRegionIdByPos( getPositionX( masterEntityState ),
		                     getPositionY( masterEntityState ),
				             getPositionZ( masterEntityState ) );
}

//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onInitialization()
{
	
    DebugOutPutf("===================== initialization =============================\n", "Terazona" );
    printf("==================================================================\n");
	printf("=  TraZona Application Server: Ver:0.1                           =\n");
	printf("=                              for Terazona 1.1                  =\n");
	printf("=                                                        Apingu  =\n");
	printf("==================================================================\n");

	DebugOutPutf("--------------------- initialization ---------------------\n", "Terazona" );


	/*
	char *gver = CConfigReader::GetStringOnce(Cfgfname,"GAMEVER");
	if( gver != NULL )
	{
	    strcpy( AppName, gver );
	    delete [] gver;
		printf( "Game version : %s\n", AppName );
	}
	else
		printf( "Can't read Game version : %s\n", AppName );

	//get map region x count
	char *mrcx = CConfigReader::GetStringOnce(Cfgfname,"MAPDIMCX");
	if( mrcx != NULL )
	{
		gDimensionCX = atoi( mrcx );
		delete [] mrcx;
		printf("map dim cx : %d\n", gDimensionCX );
	}

	//get map region y count
	char *mrcy = CConfigReader::GetStringOnce(Cfgfname,"MAPDIMCY");
	if( mrcy != NULL )
	{
	    gDimensionCY = atoi( mrcy );
		delete [] mrcy;
		printf("map dim cy : %d\n", gDimensionCY );
	}

	//get map region z count
	char *mrcz = CConfigReader::GetStringOnce(Cfgfname,"MAPDIMCZ");
	if( mrcz != NULL )
	{
	    gDimensionCZ = atoi( mrcz );
	    delete [] mrcz;
		printf("map dim cz : %d\n", gDimensionCZ );
	}

	char *mscx = CConfigReader::GetStringOnce(Cfgfname,"MAPSIZECX");
	if( mscx != NULL )
	{
	    gMapSizeCX = atoi( mscx );
	    delete [] mscx;
		printf("map size cx : %d\n", gMapSizeCX );
	}

	char *mscy = CConfigReader::GetStringOnce(Cfgfname,"MAPSIZECY");
	if( mscy != NULL )
	{
	    gMapSizeCY = atoi( mscy );
	    delete [] mscy;
		printf("map size cy : %d\n", gMapSizeCY );
	}

	char *mscz = CConfigReader::GetStringOnce(Cfgfname,"MAPSIZECZ");
	if( mscz != NULL )
	{
	    gMapSizeCZ = atoi( mscz );
	    delete [] mscz;
		printf("map size cz : %d\n", gMapSizeCZ );
	}
	*/
	DebugOutPutf("----------------------------------------------------\n", "Terazona" );

	fflush(stdout);

	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
ZonaServerEntity *onCreateNewServerEntity()
{
	return NULL;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
ZonaServerCharacter *onCreateNewServerCharacter()
{
	return NULL;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onDestruction()
{
	DebugOutPutf(" destruction ", "Terazona" );
    DebugOutPutf(" ======================================= ", "Terazona" );

	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
int onEntityJoinedGame(ZonaServerEntity *entity)
{
	//DebugOutPutf(" loginEntity ", "Terazona" );
	printf( "loginEntity.......\n" );


	//add a new entity to entitymgr
	int regionID = getRegionIdByPos( getPositionX( entity->getPublicStateProperty() ),
		                             getPositionY( entity->getPublicStateProperty() ),
									 getPositionZ( entity->getPublicStateProperty() ) );//getRegionIdForEntityRecord( MoveState( masterEntityState ) );


	//entity->setRegionId( regionID );
	printf( "Login a entity in Region ID %d\n", regionID );
	fflush(stdout);

	return regionID;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onExitEntity(ZonaServerEntity *entity)
{
	gEntityMgr.DelEntity( entity->getId() );
	printf(" exitEntity ........ " );
	printf( "Online entity count %d\n", gEntityMgr.EntitySize() );
	fflush( stdout );

	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onValidateEntityPropertyUpdate(ZonaServerEntity *entity, int propertyFlag,
								byte *property, short size)
{

}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onNotifyEntityUpdate(ZonaServerEntity *entity,int updateFlag) 
{

}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onValidateGameStateMsg(ZonaServerEntity *entity, byte* stateData,
							short stateDataSize)
{


	CPMessage* recv = new CPMessage;
	recv->New( stateData, stateDataSize );


	CEntity* e = gEntityMgr.GetEntity( entity->getId() );
	

	e->putMessage( recv );
	CPMessage* send = e->getMessage();

	//NULL message
	if( send == NULL )
		return;

	//if message type is moving message
	if( IsMoveStateType( send->buf() ) )
		e->setRId( getRegionIdForEntityRecord( send->buf() ) );
	
	//int rID = getRegionIdForEntityRecord(stateData);
	if( publishGameStateMsgInSphere( e->getRId(), PUB_QOM_2, PUB_OP_CLIENTS, send->buf(), send->size() ) == false )
	//if( publishGameStateMsg(eid, getRegionIdForEntityRecord( stateData ), PUB_QOM_2|PUB_OP_UPDATE_RGN, stateData, stateDataSize) == false )
	{
		printf(" publishGameStateMsg failure " );
		fflush(stdout);
	}

}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onInitializeRegions()
{
	printf(" got initialize regions");
}



//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
int* onGetRegionNeighbors(int regionId, byte regionType, unsigned int *numIds)
{
	// NOTE: according to Prash, regionCoverage can be ignored and that coverage
    //       should be assumed to be a 3x3 region area centered on the region
    //       specified by regionId.
	//DebugOutPutf(" getRegionNeighbors ", "Terazona" );
	int d_x[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };
	int d_y[8] = { -1,  0,  1, 1, 1, 0, -1, -1 };

	int regionCoverage = 3;
	*numIds = 0;

	//********************TODO***************************
	UINT16 numRgns = regionCoverage*regionCoverage;
	int *rgns = new int[numRgns];
    
	short counter = 0;
    //Vector3 pos(0,0,0);
    //int regRad = floor(regionCoverage / 2);
	//printf( "getRegionNeighbors : " );

    for(int j = 0; j < regionCoverage; j++ )
	{
        for(int i = 0; i < regionCoverage; i++ )
		{
			int newRegionId = regionId + ( d_x[counter] * gDimensionCX ) + d_y[counter];
			//if( ( newRegionId < 0 ) || ( newRegionId == regionId ) )
			if( ( newRegionId < 0 ) || ( newRegionId > regioncount() ) )//try to test add self region
				continue;
			rgns[counter] = newRegionId;
            counter++;
			//printf( "| %d ", newRegionId );
		}
	}
	//printf("\n");
	//fflush(stdout);

	int* allRegionIds = new int[counter];
	memcpy( allRegionIds, rgns, sizeof(int) * counter );

	delete [] rgns;

    *numIds = counter;
    return allRegionIds;
	//return allRegionIds;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onPlaceEntityInRegion(ZonaServerEntity *entity, int regionId) 
{
	printf(" On place entity in region ");
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onEntityDepartedGame(ZonaServerEntity *entity)
{
	DebugOutPutf(" logoutEntity ", "Terazona" );
	entity->save( true );
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Messges for region monitor
//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
int onGetMapDimensionX()
{
	char msg[255];
	sprintf( msg, " getMapDimensionX = %d", gDimensionCX );
	DebugOutPutf(msg, "Terazona" );
	return gDimensionCX;
	//return dimension;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
int onGetMapDimensionY()
{
	//for 3D map the y must be tranform to z
	char msg[255];
	sprintf( msg, " getMapDimensionZ = %d", gDimensionCY );
	DebugOutPutf(msg, "Terazona" );
	return gDimensionCZ;
	//return dimension;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
int onGetMapDimensionZ()
{
	//for 3D map the z must be tranform to y
	char msg[255];
	sprintf( msg, " getMapDimensionY = %d", gDimensionCZ );
	DebugOutPutf(msg, "Terazona" );
	return gDimensionCY;
	//return 0;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onEnterEntity(ZonaServerEntity *entity)
{
	//DebugOutPutf(" enterEntity ", "Terazona" );
	printf("=======================enterEntity===========================\n" );
	
	gEntityMgr.AddEntity( entity );//(byte*)entity->getPublicDisplayProperty(), entity->getPublicDisplayPropertyLength() );

	//TZPropCreature* state = (TZPropCreature*)masterEntityState;
	int regionID = getRegionIdByPos( getPositionX( entity->getPublicStateProperty() ),
		                             getPositionY( entity->getPublicStateProperty() ),
									 getPositionZ( entity->getPublicStateProperty() ) );//getRegionIdForEntityRecord( MoveState( masterEntityState ) );

	CEntity* e = gEntityMgr.GetEntity( entity->getId() );
	e->setRId( regionID );

	printf(" enterEntity on region Id %d........ ", regionID );
	printf( "Online entity count %d\n", gEntityMgr.EntitySize() );
	fflush( stdout );

	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onUpdateEntityMode(int entityId, bool isMasterEntity)
{
	return;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateCreateCharacter(ZonaServerCharacter *newCharacter)
{
	printf(" on validate create character ");
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateUpdateCharacter(ZonaServerCharacter *newCharacter,
							   ZonaServerCharacter *previousCharacter)
{
	printf(" on validate update character ");
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateEntityTypeUpdate(ZonaServerEntity *entity, int newType)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onNotifyGameStateMsgUpdate(ZonaServerEntity *entity, byte* gsm, short size)
{
	
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onUpdateEntityType(int entityId, int newType)
{
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateCreateChildEntity(ZonaServerEntity *newChildEntity)
{
	return true; // no validation yet.
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateDeleteChildEntity(ZonaServerEntity *newChildEntity)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onEntityTimerEvent(int eventId, ZonaServerEntity *entity, byte* eventData, short size)
{
	printf(" On entity TIMER hello %s", (char*)eventData );

}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onRegionTimerEvent(int eventId, int regionId, byte* eventData, short size)
{
	printf(" On entity TIMER hello %s", (char*)eventData );
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onGSSTimerEvent(int eventId, byte* eventData, short size)
{
	printf(" On entity TIMER hello %s", (char*)eventData );
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onWorldTimerEvent(int eventId, byte* eventData, short size)
{
	printf(" On entity TIMER hello %s", (char*)eventData );
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateTransferEntity(int entityId, int currentParentId, int newParentId)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onNotifyTransferEntity(int childEntityId, int prevParentId, int newParentId) {
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
int onGetRegionCount()
{
	DebugOutPutf(" getRegionCount ", "Terazona" );
	//printf( "getRegionCount : %d ", gDimensionCX * gDimensionCY * gDimensionCZ );
	//fflush(stdout);

	return regioncount();
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
char* onGetGamePluginVersion()
{
	return "TrackerClient";
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateClientVersion(byte* clientVersion, short size)
{
	printf( "Terazona:validateClientVersion\n " );
	if ( strcmp( (char*)clientVersion, AppName ) == 0 )
	{
		printf(" Validate on version : %s", (char*)clientVersion );
		fflush(stdout);
		return true;
	}
	else
	{
		printf(" Validate fail on version : %s", (char*)clientVersion );
		fflush(stdout);
		return false;
	}

	
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateDeleteCharacter(ZonaServerCharacter *character)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onLoggedOutEntityTimerEvent(int eventId, ZonaServerEntity *entity,
							   byte* eventData, short eventDataSize)
{

}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateCreateGuild(int guildId, byte *creator, int size)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateDeleteGuild(int guildId, byte *requestor, int size)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateAddMemberToGuild(int guildId, byte *newMember, int size)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateRemoveMemberFromGuild(int guildId, byte *guildMember, int size)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateAddModerator(int guildId, byte *newModerator, int size)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
bool onValidateRemoveModerator(int guildId, byte *moderator, int size)
{
	return true;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onNotifyPropertyUpdate(const char* key, char* value, int size)
{
	printf(" Notify Property update for %s ", key);
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void onNotifyPropertyDelete(const char* key)
{
	printf(" Notify Property deleted  %s ", key);
}


