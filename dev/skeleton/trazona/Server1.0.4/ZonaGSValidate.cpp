
#include <stdio.h>
#include <vector>
#include <iostream>
#include <windows.h>

#include "ZonaGSValidate.h"
#include "ZonaGSPublish.h"

static byte g_currentRegionId = 0;
const int dimension=10;

bool initialization()
{
	printf("GSValidate: entering initialization\n");
	fflush(stdout);

	return true;
}


bool destruction()
{
	return true;
}


bool exitEntity(int entityId)
{
	return true;
}


void validateGameStateMsg(int eid, byte* stateData, short stateDataSize)
{
//	publishGameStateMsg(eid, 0, CNVNC_CVRG_DEFAULT, PUB_OP_DEFAULT|PUB_OP_CORRECTION, stateData, stateDataSize);
	publishGameStateMsg(eid, 0, PUB_QOM_2, stateData, stateDataSize);
}


void initializeRegions()
{
}


int* getAllRegionIds(unsigned int *numIds)
{
	int *allRegionIds = NULL;

	allRegionIds = new int[1];
	allRegionIds[0] = g_currentRegionId;
	*numIds = 1;

	return allRegionIds;
}


int* getRegionNeighbors(int regionId, byte regionType, unsigned int *numIds)
{
	*numIds = 0;
	int *allRegionIds = NULL;

	return allRegionIds;
}

int getRegionIdForEntityState(byte* entityState)
{

    return g_currentRegionId;
} 

int  getRegionIdForEntityRecord(byte* masterEntityState)
{
	return 0;
}

byte *extractDisplayProperties(byte* masterEntityState, short* length)
{
	return NULL;
}

byte *getEntityRecord(int entityId, short* length)
{
	return NULL;
}

bool placeEntityInRegion(int regionId, int entityId, byte *entityRecord, short size, int userRole) 
{
	return true;
}

bool logoutEntity(int senderId, int entityId, int ownerId)
{
	return true;
}

// Messges for region monitor
int getMapDimensionX()
{
	return dimension;
}


int getMapDimensionY()
{
	return dimension;
}

int getMapDimensionZ()
{
	return 0;
}

int	 loginEntity(int entityId, byte* masterEntityState, short stateSize)
{
	return 0;
}

bool logoutEntity(int entityId)
{
	saveEntity(entityId);
	return true;
}

bool logoutChildEntity(int childEntityId)
{
	return true;
}

bool enterEntity(int entityId, short entityType, int ownerId,
				 byte* masterEntityState, short stateSize,
				 byte* privateProps, short ppSize,
				 bool isEntityMaster, int userRole)
{
	return true;
}


void updateEntityMode(int entityId, bool isMasterEntity)
{
	return;
}

int  getRegionIdForEntityState(byte* entityState, int size)
{
	return true;
}

short getEntityType(int entityId, byte *masterEntityState, int dataLen)
{
	return ET_DEFAULT;
}



bool validateCreateCharacter(int characId, short newType, 
					   byte *characName, short nameLen,
					   byte *newCharacEntityRec, short newSize)
{
	publishCharacter(newType,
					characName, nameLen,
					newCharacEntityRec, newSize,
					NULL, 0); // no private properties
	return true;
}



bool validateUpdateCharacter(int characId, short newType, 
					   byte *characName, short nameLen,
					   byte *prevCharacEntityRec, short prevSize,
					   byte *newCharacEntityRec, short newSize)
{
	publishCharacter(newType,
					characName, nameLen,
					newCharacEntityRec, newSize,
					NULL, 0); // no private properties
	return true;
}

bool validateEntityTypeUpdate(int entityId, short entityType)
{
	return true;
}

void updateGameStateMsg(int eid, byte *gsm, short size)
{
}

void updateDisplayProperties(int entityId, byte *dp, short size)
{
}

void notifyEntityRecordUpdate(int entityId, byte *er, short size)
{
}

void updateEntityType(int entityId, short newType)
{
}

bool validateCreateChildEntity(int parentId, int entityId, short type, byte* er, int size)
{
	return true; // no validation yet.
}

bool validateDeleteChildEntity(int entityId, int parentId)
{
	return true;
}

void onEntityTimerEvent(int eventId, int entityId, byte* eventData, short size)
{
}

void onRegionTimerEvent(int eventId, int regionId, byte* eventData, short size)
{
}

void onGSSTimerEvent(int eventId, byte* eventData, short size)
{
}

void onWorldTimerEvent(int eventId, byte* eventData, short size)
{
}

bool validateTransferEntity(int entityId, int currentParentId, int newParentId)
{
	return true;
}

void updateTransferEntity(int entityId, int currentParentId, int newParentId)
{
}

int getRegionCount()
{
	return 1;
}



bool placeEntityInRegion(int regionId, int entityId,
						 byte *entityRecord, short size,
						 byte *privateProps, short ppSize,
						 int userRole)
{
  return true;
}

void updatePrivateProperties(int entityId, byte* pp, short size)
{

}

byte* getReferenceGamestate(int entityId, short* length)
{
	return NULL;
}

char* getGamePluginVersion()
{
	return NULL;
}

bool validateClientVersion(byte* clientVersion, short size)
{
	if ( strcmp((char *)clientVersion, "TrackerClient") == 0 )
		return true;
	else
		return false;
}

bool validateDeleteCharacter(int characId, byte *characEntityRec, int size)
{
	return true;
}

void onLoggoutEntityTimerEvent(int eventId, int entityId, int ownerId, short entityType,
						byte *er, short erSize,
						byte *pp, short ppSize,
						byte* eventData, short eventDataSize) {
}

