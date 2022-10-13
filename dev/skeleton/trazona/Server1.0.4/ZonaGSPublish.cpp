// ZonaGSPublish.cpp
//
//
//    TraZona App Skel
//
//
//
//
//
//
//
//
//                                       2003/1/28
//                                                     Apingu
//

#include "ZonaGSPublish.h"


// Returns string containing version in format of major.minor.patch (build#). 
void getZAFLibVersion (char *versionStr)
{

}

// Deletes the entity permanently from the database. 
bool deleteEntity (int entityId) 
{
	return true;
}

// Saves the state of the entity to the database. 
bool saveEntity (int entityId) 
{
	return true;
}
 
// Publishes the character information. 
bool publishCharacter (short type, byte *name, short len, byte *er, short erSize, byte *pp, short ppSize) 
{
	return true;
}
 
// Publish a game state message to all the entities in the entity's sphere Only those clients subscribed to receive game states will receive the messages. 
bool publishGameStateMsg (int entityId, int entityRegionId, int operation, byte *gsm, short size) 
{
	return true;
}
 
// Publish a game state message to the entity only. 
bool publishGameStateMsgTo (int recipientEntityId, int qom, byte *gsm, short size) 
{
	return true;
}
 
// Updates the server cache with the new display properties as well as updates all clients within the entity's sphere via updateDisplayProperties(...). 
bool publishDisplayProperties (int entityId, byte *displayProperties, short size, bool serverUpdateOnly) 
{
	return true;
}
 
// Updates the server cache with the new entity record as well as updates the data base of the persist flag is set. 
bool publishEntityRecord (int entityId, byte *entityRecord, short size, bool persist, bool sendToClient) 
{
	return true;
}
 
// Updates the server cache with the new private properties as well as updates the data base of the persist flag is set. 
bool publishPrivateProperties (int entityId, byte *privateProperties, short size, bool persist) 
{
	return true;
}
 
// Get time since character has logged on. 
int getMillsSinceLogin (int anEntityId) 
{
	return 1;
}
 
// Transfers the ownership of an entity to a new parent Currently not implemented. 
int switchParentEntity (int entityId, int curParentId, int newParentId) 
{
	return 1;
}
 
// Get Current entity state record. 
ValidatedStateRecord * getCurrentStateRecord () 
{
	ValidatedStateRecord *temp = NULL;
	return temp;
}
 
// Get pointer to a new entity state record allocated in shared memory. 
ValidatedStateRecord * getNewStateRecord (int length, byte *entityDataPtr) 
{
	ValidatedStateRecord *temp = NULL;
	return temp;
}
 
// Called to set a timer event associated with an Entity. 
int setEntityLevelTimerEvent (int entityId, long delay, byte *eventData, short size, int period) 
{
	return 1;
}
 
// Called to set a timer event for a Region . 
int setRegionLevelTimerEvent (int regionId, long delay, byte *eventData, short size, int period) 
{
	return 1;
}
 
// Called to set a timer event for the local server . 
int setGSSLevelTimerEvent (long delay, byte *eventData, short size, int period) 
{
	return 1;
}
 
// Called to set a Global timer event. 
int setWorldLevelTimerEvent (long delay, byte *eventData, short size, int period) 
{
	return 1;
}
 
// Called to cancel a previously set timer event. 
bool cancelTimerEvent (int eventId) 
{
	return true;
}
