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

#ifndef CLIENTCALLBACK_H
#define CLIENTCALLBACK_H


#include <vector>
#include <callback/GameStateCallBack.h>


class TraZonaClientCallback : public GameStateCallback
{

public:

	// this game state callback is designed to receive multiple, 1 byte messages,
	// as sent in the main() function below from the ZonaServices::SendGameState()
	// method
    void onReceivedGameStateMsg(int entityId , char* stateData, short dataSize)
	{
		
		//MSG_UPDATE *state = (MSG_UPDATE*)stateData;
		//printf("Received senderId=%i entityId=%i X=%f Y=%f\n", entityId, state->entityId, state->posX, state->posY);
/*
		for (int i = 0; i<totalPacketSize; i++)
		{
			int value;
			value = entity_state[i];
			printf("%d%s", value, (i<totalPacketSize-1) ? ", " : "\n");
		} 
*/	
		return;
	}
	void onEntityJoinedSphere(ZonaClientEntity *zce)
	{


	}

	void onEntityDepartedSphere(int entityId)
	{

	}


	void onNotifyChildDelete(ZonaClientEntity *zce, bool success)
	{


	}

	void onNotifyEntityUpdate(ZonaClientEntity *zce, int flag)
	{

	}


	void onNotifyEntityPropertyUpdate(ZonaClientEntity *zce, int propertyType,
												char *propertyUpdate, short size)
	{


	}

    void  onPlayerReset(int resetAction , int resetReason)
	{

	}
	

	void onNotifyEntityExit(ZonaClientEntity *zce)
	{


	}

	ZonaClientEntity *onCreateNewClientEntity()
	{
		return NULL;
	}

	ZonaClientCharacter *onCreateNewClientCharacter()
	{
		return NULL;
	}


};


#endif