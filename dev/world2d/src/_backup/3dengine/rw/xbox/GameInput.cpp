/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: gameInput.cpp
 * Original Author: Mark Harmon
 ********************************************************************/
//#include <xtl.h>

#include "../skeleton.h"
#include "../input.h"
#include "../entity/ZBCamera.h"
#include "../entity/vehicle/ZBLocalVehicle.h"
#include "../gui.h"

/*****************************************************************************/
// External Variables
/*****************************************************************************/
extern ZBCamera *gCamera;
extern ZBLocalVehicle *gMyVehicle;
extern ZBWorld *gWorld;
extern GUI *gGameHUD;

/*****************************************************************************/
// Global Variables
/*****************************************************************************/
GameInputData gGameInputData;



static void toggleChatVisibility(BOOL bShowChat)
{
  RsGlobal.bShowChat = bShowChat;
  GUIElement *ge = gGameHUD->GetElement(_T("chatWindow"));
  ge->ToggleVisibility(RsGlobal.bShowChat);
  ge = gGameHUD->GetElement(_T("chatEditBox"));
  ge->ToggleVisibility(RsGlobal.bShowChat);
}

/*******************************************************************************/
static RsEventStatus giHandleKeyDown(RsKeyStatus *keyStatus)
{

  switch(keyStatus->keyCharCode)
  {
  case rsUP:
    gGameInputData.accel = 1;
    return rsEVENTPROCESSED;

  case rsDOWN:
    gGameInputData.reverse = 1;
    return rsEVENTPROCESSED;

  case rsLEFT:
    gGameInputData.turn = -1;
    return rsEVENTPROCESSED;

  case rsRIGHT:
    gGameInputData.turn = 1;
    return rsEVENTPROCESSED;

  case rsF8:
    gCamera->IncrementViewMode();
    return rsEVENTPROCESSED;

  case rsF2:
    RsGlobal.bShowFPS = !RsGlobal.bShowFPS;
    return rsEVENTPROCESSED;

  case rsSPACE:
    gGameInputData.brake = 1;
    return rsEVENTPROCESSED;

  case rsLCTRL:
    gGameInputData.fireMain = 1;
    return rsEVENTPROCESSED;

  case rsLSHIFT:
    gGameInputData.fireSecond = 1;
    return rsEVENTPROCESSED;

  case rsPLUS:
    gGameInputData.devData0 = 1;
    return rsEVENTPROCESSED;

  case rsMINUS:
    gGameInputData.devData0 = -1;
    return rsEVENTPROCESSED;

  case rsF7:
    gGameInputData.devData1 = 1;
    return rsEVENTPROCESSED;
  }

  return rsEVENTNOTPROCESSED;
}


static RsEventStatus giHandlePADDown(RsPadButtonStatus *padStatus)
{ 
  switch(padStatus->padButtons)
  {
	case rsPADDPADUP:
    gGameInputData.accel = 1;
    return rsEVENTPROCESSED;

	case rsPADDPADDOWN:
    gGameInputData.reverse = 1;
    return rsEVENTPROCESSED;

  case rsPADDPADLEFT:
    gGameInputData.turn = -1;
    return rsEVENTPROCESSED;

  case rsPADDPADRIGHT:
    gGameInputData.turn = 1;
    return rsEVENTPROCESSED;

  case rsPADBUTTON1:
	case rsPADBUTTON7:
    gGameInputData.fireMain = 1;
    return rsEVENTPROCESSED;

  case rsPADBUTTON2:
	case rsPADBUTTON8:
    gGameInputData.fireSecond = 1;
    return rsEVENTPROCESSED;

  case rsPADBUTTON3:
    gGameInputData.devData0 = 1;
    return rsEVENTPROCESSED;

  case rsPADBUTTON4:
    gGameInputData.devData0 = -1;
    return rsEVENTPROCESSED;

  case rsPADBUTTON5:
    gGameInputData.brake = 0;
    return rsEVENTPROCESSED;

  case rsPADBUTTON6:
    return rsEVENTPROCESSED;

	case rsPADBUTTONA1:
    gGameInputData.devData1 = 1;
    return rsEVENTPROCESSED;

  case rsPADBUTTONA2:
    return rsEVENTPROCESSED;

	case rsPADSTART:
		toggleChatVisibility(!RsGlobal.bShowChat);
    return rsEVENTPROCESSED;

  case rsPADSELECT:
    RsGlobal.bShowFPS = !RsGlobal.bShowFPS;
    gCamera->IncrementViewMode();
    return rsEVENTPROCESSED;
  }

  return rsEVENTNOTPROCESSED;
}


static RsEventStatus giHandleKeyUp(RsKeyStatus *keyStatus)
{
  switch(keyStatus->keyCharCode)
  {
  case rsUP:
    gGameInputData.accel = 0;
    return rsEVENTPROCESSED;

  case rsDOWN:
    gGameInputData.reverse = 0;
    return rsEVENTPROCESSED;

  case rsLEFT:
    if(gGameInputData.turn == -1) gGameInputData.turn = 0;
    else gGameInputData.turn = 1;
    return rsEVENTPROCESSED;

  case rsRIGHT:
    if(gGameInputData.turn == 1) gGameInputData.turn = 0;
    else gGameInputData.turn = -1;
    return rsEVENTPROCESSED;

  case rsSPACE:
    gGameInputData.brake = 0;
    return rsEVENTPROCESSED;

  case rsLCTRL:
    gGameInputData.fireMain = 0;
    return rsEVENTPROCESSED;

  case rsLSHIFT:
    gGameInputData.fireSecond = 0;
    return rsEVENTPROCESSED;

  case rsPLUS:
  case rsMINUS:
    gGameInputData.devData0 = 0;
    return rsEVENTPROCESSED;

  case rsF7:
    gGameInputData.devData1 = 0;
    return rsEVENTPROCESSED;
  }

  return rsEVENTNOTPROCESSED;
}


static RsEventStatus giHandlePADUp(RsPadButtonStatus *padStatus)
{ 
	switch(padStatus->padButtons)
  {
  case rsPADDPADUP:
    gGameInputData.accel = 0;
    return rsEVENTPROCESSED;

  case rsPADDPADDOWN:
    gGameInputData.reverse = 0;
    return rsEVENTPROCESSED;

  case rsPADDPADLEFT:
    if(gGameInputData.turn == -1) gGameInputData.turn = 0;
    else gGameInputData.turn = 1;
    return rsEVENTPROCESSED;

  case rsPADDPADRIGHT:
    if(gGameInputData.turn == 1) gGameInputData.turn = 0;
    else gGameInputData.turn = -1;
    return rsEVENTPROCESSED;

  case rsPADBUTTON1:
	case rsPADBUTTON7:
    gGameInputData.fireMain = 0;
    return rsEVENTPROCESSED;

  case rsPADBUTTON2:
	case rsPADBUTTON8:
    gGameInputData.fireSecond = 0;
    return rsEVENTPROCESSED;

  case rsPADBUTTON3:
    gGameInputData.devData0 = 0;
    return rsEVENTPROCESSED;

  case rsPADBUTTON4:
    gGameInputData.devData1 = 0;
    return rsEVENTPROCESSED;

  case rsPADBUTTON5:
    gGameInputData.brake = 0;
    return rsEVENTPROCESSED;

  case rsPADBUTTON6:
    return rsEVENTPROCESSED;

	case rsPADBUTTONA1:
    return rsEVENTPROCESSED;

  case rsPADBUTTONA2:
    return rsEVENTPROCESSED;

	case rsPADSTART:
    gGameInputData.brake = 0;
    return rsEVENTPROCESSED;

  case rsPADSELECT:
    return rsEVENTPROCESSED;

  }

  return rsEVENTNOTPROCESSED;
}


static RsEventStatus giHandlePADLeftStick(RwV2d *padStatus)
{
	float x = -(padStatus->x);
	float y = -(padStatus->y);

	if (padStatus->y > 0)
    gGameInputData.turn = 1;	
	else
    gGameInputData.turn = -1;

	int loop = 0;

	if (padStatus->y < 0.1)
	{
		gGameInputData.accel = (RwInt8)-(55*padStatus->y);
		//gGameInputData.accel = 1;
		gGameInputData.reverse = 0;
		loop = 1;
	}
	else if (padStatus->y > 0.1)
	{
		gGameInputData.accel = 0;
		gGameInputData.reverse = (RwInt8)(padStatus->y);
		loop = 2;
	}
	else
	{
		gGameInputData.accel = 0;
		gGameInputData.reverse = 0;
		loop = 3;
	}

	if (padStatus->x > .1) gGameInputData.turn = -1;
	else if (padStatus->x < -.1) gGameInputData.turn = 1;
	else gGameInputData.turn = 0;

	char temp[255];
	sprintf(temp, "X=%f Y=%f Loop=%f accel=%f turn=%i\n", padStatus->x, padStatus->y, loop, gGameInputData.accel, gGameInputData.turn);
	OutputDebugString(temp);

	return rsEVENTPROCESSED;
}


static RsEventStatus giHandlePADLeftStickReset(RwV2d *padStatus)
{
	float x = padStatus->x;
	float y = padStatus->y;

	return rsEVENTPROCESSED;
}

/*******************************************************************************/
static RsEventStatus giKeyboardHandler(RsEvent event, void *param)
{
  /*...then the application events, if necessary...*/
  switch( event )
  {
    case rsKEYDOWN:
    {
      return giHandleKeyDown((RsKeyStatus *)param);
    }

    case rsKEYUP:
    {
      return giHandleKeyUp((RsKeyStatus *)param);
    }

    default:
    {
      return rsEVENTNOTPROCESSED;
    }
  }
}

static RsEventStatus giPADHandler(RsEvent event, void *param)
{
	switch ( event )
	{
		case rsPADBUTTONDOWN:
    {
      return giHandlePADDown((RsPadButtonStatus *)param);
    }

    case rsPADBUTTONUP:
    {
      return giHandlePADUp((RsPadButtonStatus *)param);
    }
		case rsPADANALOGUELEFT:
    {
      return giHandlePADLeftStick((RwV2d *)param);
    }

    case rsPADANALOGUELEFTRESET:
    {
      return giHandlePADLeftStickReset((RwV2d *)param);
    }
    default:
    {
      return rsEVENTNOTPROCESSED;
    }
	}
}


/*******************************************************************************/
BOOL AttachGameInput(void)
{
  RsInputDeviceAttach(rsPAD, giPADHandler);
  memset(&gGameInputData,0,sizeof(gGameInputData));
	gInputMode = IM_GAME;

  return TRUE;
}

/*******************************************************************************/
