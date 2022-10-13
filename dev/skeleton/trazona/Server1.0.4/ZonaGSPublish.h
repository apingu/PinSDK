/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: ZonaGSPublish.h
 * Original Author: Adam Lang
 ********************************************************************/
#ifndef ZONAGSPUBLISH_H
#define ZONAGSPUBLISH_H

#include <stdlib.h>
#include <stdio.h>

#include "ZonaGSValidate.h" // Shared Memory mgmt

#pragma warning(disable: 4786) 

typedef unsigned char byte; 

/*****************************************************************************/
/** Message will be published to all the entities
	within the sphere.*/
#define PUB_OP_DEFAULT    0

/** Message will be discarded and not published.*/
#define PUB_OP_DISCARD    1

/** In addition to the message being published to all the entities
    within the sphere it is also sent back to the entity.
*/
#define PUB_OP_CORRECTION 2

/** The quality of the message QOM_3 being the highest quality.*/
#define PUB_QOM_0         0
#define PUB_QOM_1         4
#define PUB_QOM_2         8
#define PUB_QOM_3        12

/** Message will be published directly to recepient
	entity and only to the entity.
	The region id will *not* be updated for the entity
	even if the flag PUB_OP_UPDATE_RGN is set.
*/
#define PUB_OP_DIRECT	  16

/** Flag to inform the server to update the region of the entity
    during a plublish*/
#define PUB_OP_UPDATE_RGN 32


/** To publish non-entity specific data to all entities
	in a specified region. Entity Id should be -1.*/
#define PUB_OP_REGION     64

/*****************************************************************************/

// Publish functions


// Removed on 07/23
//////////////////////////////////////////////////////////////////////////////*
/**
* Creates a new entity in the database.
*
* @param entityType		Type of the new entity
* @param ownerEntityId	Id of the owner entity, 0 in case of root
* @param entityData		Initial entity record for the entity
* @param size			Size of entity record
*
* @return				Id of the newly created entity
*
int createEnity(short entityType, int ownerEntityId,
			    byte *entityData, short size);

*////////////////////////////////////////////////////////////////////////////////

/**
* Returns string containing version
* in format of major.minor.patch (build#)
*
* @param versionStr memory must be allocated by user (at least 32 8bit chars)
*
* @return			void
*/
void getZAFLibVersion(char* versionStr);

/**
* Deletes the entity permanently from the database.
*
* @param entityId	Id of the entity that needs to be deleted
*
* @return			True, if successful
*/
bool deleteEntity(int entityId);


/**
* Saves the state of the entity to the database.
*
* @param entityId	Id of the entity that needs to be saved
*
* @return			True, if successful
*/
bool saveEntity(int entityId);


/**
* Publishes the character information.
* This method should be used during character creation and prior to enterCharacter.
* The server will update it's cache and the database.
* The client updating the character will receive type and entity record updates.
* 
*
* @param type			Updated type of the character
* @param name           Updated name of the character
* @param len            Length of the updated character name
* @param er				Updated Entity Record for the character
* @param erSize			Length of the updated entity record
* @param pp				Updated private properties for the character
* @param ppSize			Length of the updated private properties
*
* @return				True, if successful
*/
bool publishCharacter(short type,
						byte *name, short len,
					    byte *er, short erSize,
					    byte *pp, short ppSize);

/**
* Publish a game state message to all the entities in the entity's sphere
* Only those clients subscribed to receive game states will receive the messages.
* Please see subscribeTOGameStateMsgs(...) in the client side API.
* However, messages published with PUB_OP_DIRECT will
* still be received by clients even with no subscription.
* 
*
* @param entityId		Id of the entity for which the state needs to be published
* @param entityRegionId	Id of the entity's region
* @param operation		Type of operation that needs to be performed during publish. one or more operations can be specified by OR-ing PUB_OP_XXX
* @param gsm			Game state message that needs to be published
* @param size			Length of game state message
*
* @return				True, if successful
*/
bool publishGameStateMsg(int   entityId,
						 int entityRegionId, 
						 int operation, 
						 byte *gsm,  short size);

/**
* Publish a game state message to the entity only. It is received by the client hosting the entity.
* Messages published by this method are received by clients even if the
* clients have not subscribed to game state messages.
*
* @param recipientEntityId		Id of the recipient entity
* @param qom                    Quality of message, PUB_QOM_[0,1,2,3]
* @param gsm					Game state message that needs to be published
* @param size					Length of gsm
*
* @return						True, if successful
*/
bool publishGameStateMsgTo(int recipientEntityId,	
						   int qom, 
						   byte *gsm, short size);

/**
* Updates the server cache with the new display properties as well as 
* updates all clients within the entity's sphere via updateDisplayProperties(...).
* UpdateDisplayProperty is called in those servers that the entity is replicated on.
*
* @param entityId			Id of the entity whose display properties needs to be updated
* @param displayProperties	Updated display properties of the entity
* @param size				Length of display properties
* @param serverUpdateOnly	If true, only server cache is updated and is not published to clients.
*
* @return					True, if successful
*/
bool publishDisplayProperties(int entityId,	
						byte *displayProperties,
						short size, bool serverUpdateOnly);

/**
* Updates the server cache with the new entity record as well as 
* updates the data base of the persist flag is set.
* UpdateEntityRecord is called in those servers that the entity is replicated on.
*
* @param entityId		Id of the entity whose entity record needs to be updated
* @param entityRecord	Updated entity properties of the entity
* @param size			Length of entity record
* @param persist		If this flag is set, the entity record
*						is also updated to the database
* @param sendToClient   if true, the client that owns entityId will receive updateEntityRecord callback
*
* @return				True, if successful
*/
bool publishEntityRecord(int entityId,	
						byte *entityRecord,
						short size,
						bool persist,
						bool sendToClient);

/**
* Updates the server cache with the new private properties as well as 
* updates the data base of the persist flag is set.
* UpdatePrivateProperties is called in those servers that the entity is replicated on.
*
* @param entityId			Id of the entity whose entity record needs to be updated
* @param privateProperties	Updated entity properties of the entity
* @param size				Length of private properties
* @param persist			If this flag is set, the private properties
*							is also updated to the database
*
* @return					True, if successful
*/
bool publishPrivateProperties(int entityId,	
							  byte *privateProperties,
							  short size,
							  bool persist);


/**
* Get time since character has logged on
*
* @param anEntityId	of the character
*
* @return	milliseconds since user has logged on
*  return 0 if error
*/
int getMillsSinceLogin(int anEntityId);


/**
* Transfers the ownership of an entity to a new parent
*
* @param entityId		id of the entity whose ownership needs changed
* @param curParentId	id of the curent parent
* @param newParentId	id of the new owner entity
*
* @return error code, 0 if no error
*/
int switchParentEntity(int entityId, int curParentId, int newParentId);


/**************************************************************** 
 * The following functions used for shared memory access between 
 * java and plugin. 
*****************************************************************/


/**
* Get Current entity state record
*
* @return	pointer to the current state record structure
*
*/
ValidatedStateRecord*	getCurrentStateRecord();


/**
* Get pointer to a new entity state record allocated in shared memory
*
* @param length			Length of the entityState
* @param entityDataPtr	Pointer to memory where entity state can be placed
*
* @return	pointer to the new state record structure
*
*/
ValidatedStateRecord*	getNewStateRecord(int length, byte* entityDataPtr);


////////////////// Timer Event Functions ////////////////////////

/**
* Called to set a timer event associated with an Entity. 
* When the timer event is due, the onEntityTimerEvent callback is fired.
*
* @param entityId		Id of the entity associated with the event
* @param delay			The millis, from current time, at which to schedule the event.
*								
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
* @param period			No of times that this event should be fired.
*						-1 indicates infinite repeating executions.
*						The event would fire at regular intervals determined by the value of
*						the 'delay' parameter.
*
* @return				EventId if successful, else -1
*/
int setEntityLevelTimerEvent(int entityId, long delay, byte* eventData, short size, int period);


/**
* Called to set a timer event for a Region .
* When the timer event is due, the onRegionTimerEvent callback is fired.
*
* @param regionId		Id of the region associated with the event
* @param delay			The millis, from current time, at which to schedule the event.
*								
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
* @param period			No of times that this event should be fired.
*						-1 indicates infinite repeating executions.
*						The event would fire at regular intervals determined by the value of
*						the 'delay' parameter.	  
*
* @return				EventId if successful, else -1
*/
int setRegionLevelTimerEvent(int regionId, long delay, byte* eventData, short size, int period);


/**
* Called to set a timer event for the local server .
* When the timer event is due, the onGssTimerEvent callback is fired.
*
* @param delay			The millis, from current time, at which to schedule the event.
*								
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
* @param period			No of times that this event should be fired.
*						-1 indicates infinite repeating executions.
*						The event would fire at regular intervals determined by the value of
*						the 'delay' parameter.
*
* @return				EventId if successful, else -1
*/
int setGSSLevelTimerEvent(long delay, byte* eventData, short size, int period);


/**
* Called to set a Global timer event. This event will be fired across all active GSS Servers
* at the time specified
* When the timer event is due, the onEWorldTimerEvent callback is fired.
*
* @param delay			The millis, from current time, at which to schedule the event.
*								
* @param eventData		Event Data associated with the event
* @param size			Size of Event Data packet
* @param period			No of times that this event should be fired.
*						-1 indicates infinite repeating executions.
*						The event would fire at regular intervals determined by the value of
*						the 'delay' parameter.
*
* @return				EventId if successful, else -1
*/
int setWorldLevelTimerEvent(long delay, byte* eventData, short size, int period);


/**
* Called to cancel a previously set timer event.
* Recurring timers as well as as single event timers would never execute 
* again after they have been cancelled. 
*  
* @param eventId		The id of the event to be cancelled.
*								
* @return				True if successful.
*/
bool cancelTimerEvent(int eventId);



#endif

