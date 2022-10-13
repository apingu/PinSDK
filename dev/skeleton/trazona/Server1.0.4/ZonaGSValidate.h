/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: ZonaGSValidate.h
 * Original Author: Adam Lang
 ********************************************************************/
#ifndef ZONAGSVALIDATE_H
#define ZONAGSVALIDATE_H

#pragma warning(disable: 4786) 

#include <stdlib.h>

typedef unsigned char byte; 

/*****************************************************************************/
extern const short COSMIC_SPHERE;
extern const short SPHERE_2D_SPATIAL;
extern const short SPHERE_3D_SPATIAL;
extern const short JNI_SPHERE_3D_SPATIAL;
extern const short PLUGIN_SPHERE;

extern const int NUM_REGIONS;

/*****************************************************************************/


/** Default entity type */
static int ET_DEFAULT		 = 0;
/** Entity is not perceived by other entities, but will see other entities */
static int ET_IS_HIDDEN      = 1;
/** Entity's region id is always assumed to be that of its parent's. Child entities only. */
static int ET_IS_ATTACHED    = 2;

static int ET_RESERVED_2     = 4;
static int ET_RESERVED_3     = 8;
static int ET_RESERVED_4     = 16;
static int ET_RESERVED_5     = 32;
static int ET_RESERVED_6     = 64;
static int ET_RESERVED_7     = 128;
static int ET_RESERVED_8     = 256;

/******************* USER ROLE CONSTANTS ********************************/

// Indicates an unknown user
#define UNKNOWN      0x00000000;

// Indicates the system administrator
#define SYSTEMADMIN  0x00000001;

// Indicates an overlord
#define OVERLORD     0x00000002;

// Indicates the Gamemaster. 
#define GAMEMASTER   0x00000004;

// Indicates a Non Plying character.
#define NPC          0x00000008;

// Indicates a game player
#define PLAYER       0x00000010;

/*****************************************************************************/



// Validation functions implemented by server side developers using ZAF

/**
* Called once during server startup.
*
*/
bool initialization();


/**
* Called once during server shutdown.
*
*/
bool destruction();



/*************Entity State Management functions*************/


/**
* Called when the client initiates enterCharacter().
*
* @param entityId		Id of the entering entity
* @param entityRecord	Last saved entity record of the entity
* @param size			Size of entity record being passed in
*
* @return				Should return the region id that the entity should be placed in
*/
int	 loginEntity(int entityId, byte* entityRecord, short size);


/**
* Called when the client initiates exitCharacter().
*
* @param entityId			Id of the exiting entity
*
* @return					Should return true, if successful
*/
bool logoutEntity(int entityId);


/**
* Called by the server to inform the plugin that it should start tracking the entity.
*
* @param entityId		Id of the entity
* @param entityType		Type of the entity
* @param ownerEntityId	Entity's parent Id in the entity hierarchy
* @param entityRecord	Last known entity record of the entity
* @param size			Length of the entity record
* @param privateProps   Private properties of the entity
* @param ppSize         Length of private properties
* @param isMasterEntity	Is set to true, if the entity should be considered as a master
* @param userRole		Role of the player that owns the entity
*
* @return				Should return true, if successful
*/
bool enterEntity(int entityId, short entityType, int ownerEntityId, 
                 byte* entityRecord, short size,
				 byte* privateProps, short ppSize,
				 bool isMasterEntity, int userRole);


/**
* Called the server to inform the plugin that it should no longer track the entity
*
* @param entityId			Id of the entity
*
* @return					Should return true, if successful
*/
bool exitEntity(int entityId);


/**
* Called during initial placement.
*
* @param regionId		Region id in which the entity must be placed
* @param entityId		Id of the entity
* @param entityRecord	Last known entity record of an entity
* @param size			Length of entity record
* @param privateProps   Private properties for the entity
* @param ppSize         Lenght of private properties
* @param userRole		Role of the player that owns the entity
*
* @return					True, if successful
*/
bool placeEntityInRegion(int regionId, int entityId,
						 byte* entityRecord, short size,
						 byte* privateProps, short ppSize,
						 int userRole);

/**
* Called to update the state of the entity.
*
* @param entityId		Id of the Entity for which the state needs to be set
* @param isMasterEntity	True, if the entity should be treated as a master
*
*/
void updateEntityMode(int entityId, bool isMasterEntity);


/**
* Extract properties that should be used during enterSphere(...) to clients.
*
* @param entityRecord	Last known state of an entity
* @param length			Length of entity record call,
*                       on return length should be populated with
*						length of the returned display properties
*
* @return				Should return the properties of the entity
*						The returned pointer will be deleted by ZAF.
*/
byte* extractDisplayProperties(byte* entityRecord, short* length);

/**
* Should reference/complete game state (rgs) be used during enterSphere(...) to clients.
*
* @param entityId	Id of the entity for which the rgs should be returned
* @param length		**Should return the length of returning rgs**
*
* @return			Should return the game state of the entity
*					The returned pointer will be deleted by ZAF.
*/
byte* getReferenceGamestate(int entityId, short* length);


/**
* Get entity record of an entity.
*
* @param entityId	Id of the entity
* @param size		Size of entity record on return
*
* @return			Should return entity record.
*					The returned pointer will be deleted by ZAF.
*/
byte* getEntityRecord(int entityId, short* size);


/**
* Called when server receives new data from the client.
*
* @param entityId	Id of the entity sending the game state
* @param gsm		Game state msg received by the server from the client
* @param size		Length of the state data
*
*/
void validateGameStateMsg(int entityId, byte* gsm, short size);



/////////////////////////////////////////////////////////////
//
/*********** DEPRECATED - 10/21 *****************************
*
*
* Called when server receives entity record is being updated.
* Usually by the game master. The implementation may publish
* validated entity record back to the server.
*
* @param entityId	Id of the entity sending the game state
* @param er 		New updated entity record
* @param size		Length of the entity record
*
*/
//void validateEntityRecord(int entityId, byte* er, short size);

//////////////////////////////////////////////////////////////






/**
* Called to update the neighboring servers after the master 
* server publishes the validated game state message. 
*
* @param entityId	Id of the entity updating the game state
* @param gsm		Validated game state msg published by the master server
* @param size		Length of the state data
*
*/
void updateGameStateMsg(int entityId, byte* gsm, short size);

/**
* Called to update the display properties for an entity
*
* @param entityId	Id of the entity for which the entity record was updated
* @param dp			The entity record
* @param size		Length of display properties
*
*/
void updateDisplayProperties(int entityId, byte* dp, short size);


/**
* Called to notify update of the entity record for an entity
*
* @param entityId	Id of the entity for which the entity record was updated
* @param er			The updated entity record
* @param size		Length of the entity record
*
*/
void notifyEntityRecordUpdate(int entityId, byte* er, short size);

/**
* Called to update the entity's private properties
*
* @param entityId	Id of the entity for which the private properties was updated
* @param pp			The updated private properties
* @param size		Length of the private properties
*
*/
void updatePrivateProperties(int entityId, byte* pp, short size);

/**
* Called to update the entity type for an entity. This is called
* after the master server has validated the entity type update.
* see validateEntityTypeUpdate(...)
*
* @param entityId	Id of the entity for which the entity record was updated
* @param entityType	The new entity type
*
*/
void updateEntityType(int entityId, short entityType);


/************* Region Management functions *************/


/**
* Called once during sever startup.
*
*/
void initializeRegions();


/**
* The server lazily loads the neighbors for each region. For each region its called at most once.
*
* @param regionId		The id of the region whose neighbors are being requested
* @param regionCoverage	The coverage around the region that must be included
* @param numIds			The number of regions ids that being returned
*
* @return				Return an array of all neighboring regions
*						The returned pointer will be deleted by ZAF.
*/
int* getRegionNeighbors(int regionId, byte regionCoverage,
                        unsigned int* numIds);


/** removed on 07/25
*
* Called to find the region of an entity state.
*
* @param entityState	Entity state from which the region id needs to be determined
* @param size			Size of entityState
*
* @return				Region Id calculated from the entity state
*
int  getRegionIdForEntityState(byte* entityState, int size);
**************************************************************************/




/**
* Called to find the region of a entity from its entity record.
*
* @param entityRecord	Entity record from which the region id needs to be determined
*
* @return				Region Id calculated from entity record
*/
int  getRegionIdForEntityRecord(byte* entityRecord);



/************* Methods added to facilitate Region Probe utility *************/


/**
* Called once during server startup for region visualization.
*
*
* @return			Should return the number of regions along the x-axis
*							Width/horizontal/Left to Right (Perspective: 2-D topview)
*/
int  getMapDimensionX();


/**
* Called once during server startup for region visualization.
*
*
* @return			Should return the number of regions along the y-axis
*							Height/vertical/Top to Bottom (Perspective: 2-D topview)
*/
int  getMapDimensionY();


/**
* Called once during server startup for region visualization.
*
*
* @return			Should return the number of regions along the z-axis (Depth)
*/
int  getMapDimensionZ();

/**
* Called when a client invokes acquireChildEntity(...)/releaseChileEntity(...)
* This method is called on the GSS, where the new parent is a master.
*
* @param entityId			id of the child that is being transfered
* @param currentParentId	id of the current parent of the child entity
* @param newParentId		id of the new parent
*
* @return					should return true, only if it was successfully validated
*/
bool validateTransferEntity(int entityId, int currentParentId, int newParentId);

/**
* Called to update the ownership of an entity.
*
* @param entityId			id of the child that is being transfered
* @param currentParentId	id of the current parent of the child entity
* @param newParentId		id of the new parent
*
* @return					should return true, only if it was successfully validated
*/
void updateTransferEntity(int entityId, int currentParentId, int newParentId);

/**
* Called when a client invokes createChildEntity(...). [Un-implemented]
*
* @param parentId		Id of the entity creating a new child entity
* @param entityId		Id of the new entity being created
* @param type			Type of the new entityId
* @param er				Initial entity record of the child entity
* @param size			Size of the entity record
*
* @return				Should return true, only if it was successfully validated
*/
bool validateCreateChildEntity(int parentId, int entityId, short type, byte* er, int size);

/**
* Called when a client invokes destroyChild(...). [Un-implemented]
*
* @param entityId		id of the child that is being destroyed
* @param parentId		id of the parent destroying the child entity
*
* @return				should return true, only if it was successfully validated
*/
bool validateDeleteChildEntity(int entityId, int parentId);


/**
* Called during character creation.
*
* @param characId				Id of the Character that is being updated
* @param type					Type of character being updated to
* @param characterName          Name of the character
* @param len                    Length of the character name
* @param newCharacEntityRec		New entity record of the character
* @param newSize				New size of entity record
*
*/
bool validateCreateCharacter(int characId, short type,
					   byte *characterName, short len,
					   byte *newCharacEntityRec, short newSize);



/**
* Called during character update.
*
* @param characId				Id of the Character that is being updated
* @param type					Type of character being updated to
* @param characterName          Name of the character
* @param len                    Length of the character name
* @param prevCharacEntityRec	Previous entity record of the character
* @param prevSize				Previous size of entity record
* @param newCharacEntityRec		New entity record of the character
* @param newSize				New size of entity record
*
*/
bool validateUpdateCharacter(int characId, short type,
					   byte *characterName, short len,
					   byte *prevCharacEntityRec, short prevSize,
					   byte *newCharacEntityRec, short newSize);


/**
* Called during character deletion
*
* @param characId				Id of the Character that is being deleted
* @param characEntityRec		Entity record of the Character that is being deleted
* @param size					Size of the entity record
*/
bool validateDeleteCharacter(int characId, byte *characEntityRec, int size);

/**
* Called when the client updates the entity type using updateEntityType(...).
*
* @param entityId		Id of the entity
* @param newType		New type of the entity
*
* @return				should return true, only if it was successfully validated
*/
bool validateEntityTypeUpdate(int entityId, short newType);


//////////////// Version Control Functions //////////////////////

/**
* This function should return a string that marks the version of 
* client server side game code.
*
* @return				should return string of game version
*/
char* getGamePluginVersion();

/**
* Called when the client logs in to verify client version.
* The client will not be allowed to login to the server, if returned false.
* @param clientVersion  String of version information send at login by client
* @param size			Size of clientVersion
*
* @return				should return true, only if it was successfully validated
*/
bool validateClientVersion(byte* clientVersion, short size);




////////////////// Event Timer Functions ////////////////////////

/**
* Called when a timer event for an Entity is fired.
* This event would have been previously set by setEntityLevelTimerEvent.
*
* Note: This callback is only fired if the entity is a master Entity
*
* @param eventId		Id of the event being fired.
* @param entityId		Id of the entity associated with the event
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
*
* @return				void
*/
void onEntityTimerEvent(int eventId, int entityId, byte* eventData, short size);

/**
* Called when a timer event for an Entity that already has logged out is fired.
* This event would have been previously set by setEntityLevelTimerEvent.
*
*
* @param eventId		Id of the event being fired.
* @param entityId		Id of the entity associated with the event
* @param ownerId		Id of the entity's parent
* @param entityType     Type of the entity
* @param er             Entity record of the entity from the database
* @param erSize         Length of entity record
* @param pp             Private properties of the entity from the database
* @param ppSize         Length of private properties
* @param eventData		Event Data associated with the event
* @param eventDataSize  Size of Event Data packet
*
* @return				void
*/
void onLoggoutEntityTimerEvent(int eventId, int entityId, int ownerId, short entityType,
						byte *er, short erSize,
						byte *pp, short ppSize,
						byte* eventData, short eventDataSize);


/**
* Called when timer event for Region is fired.
* This event would have been previously set by setRegionLevelTimerEvent.
*
* Note: This callback is only fired if the Region is owned by this GSS
*
* @param eventId		Id of the event being fired.
* @param regionId		Id of the region associated with the event
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
*
* @return				void
*/
void onRegionTimerEvent(int eventId, int regionId, byte* eventData, short size);


/**
* Called when timer event for local GSS server is fired.
* This event would have been previously set by setGssLevelTimerEvent.
*
* @param eventId		Id of the event being fired.
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
*
* @return				void
*/
void onGSSTimerEvent(int eventId, byte* eventData, short size);


/**
* Called when a Global/World timer event in fired. This event will be fired in all 
* the active GSS currently operating in the system	
* This event would have been previously set by setWorldLevelTimerEvent.
*
* @param eventId		Id of the event being fired.
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
*
* @return				void
*/
void onWorldTimerEvent(int eventId, byte* eventData, short size);


/**
* Called at time of server startup. This function returns the total no of 
* regions in the map to the server. 
*
* Note: The REGION IDS have to be numbered from 0 to (TOTAL NO. OF REGIONS - 1)
* The server needs this sequential numbering for efficient message distributionn.
*
* @return				The total number of Regions in the map.
*/
int getRegionCount(void);




/**
* Structure used to represent a validated state record.
*
*
*/
struct ValidatedStateRecord{
	int		recordType;		 // This value should never be modified  
	int     entityId;		 // The entity id for corresponding to state update
	int     entityType;		 // The entity type
    int     entityRegionId ; // The region id of the above entity
	int		stateDataOffset; // The offset for reading / writing state data.
							 // This value should never be modified
    int		operation ;		 // The operation ( bit combinations ) for this entity state
    int		stateDataLen ;	 // The length of the state data to be published
							 // This value should not be modified if it has been      
							 // obtained by a call to getCurrentStateRecord.  

};

#endif