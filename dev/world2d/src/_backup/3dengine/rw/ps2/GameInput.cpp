/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: gameInput.cpp
 * Original Author: Mark Harmon
 ********************************************************************/
#include "../skeleton.h"
#include "../input.h"
#include "../entity/ZBCamera.h"
#include "../ZBWorld.h"
#include "../gui/gui.h"


/*****************************************************************************/
// External Variables
/*****************************************************************************/
extern ZBCamera *gCamera;
extern ZBWorld *gWorld;
extern GUI *gGameHUD;

/*****************************************************************************/
// Global Variables
/*****************************************************************************/
GameInputData gGameInputData;


static void toggleChatVisibility(BOOL bShowChat)
{
  RsGlobal.bShowChat = bShowChat;
  GUIElement *ge = gGameHUD->GetElement("chatWindow");
  ge->ToggleVisibility(RsGlobal.bShowChat);
  ge = gGameHUD->GetElement("chatEditBox");
  ge->ToggleVisibility(RsGlobal.bShowChat);
}

/*******************************************************************************/
static RsEventStatus giHandleKeyDown(RsKeyStatus *keyStatus)
{
  
  switch(keyStatus->keyCharCode)
  {
    // accelerate
  case rsUP:
    gGameInputData.accel = 1;
    return rsEVENTPROCESSED;

    // reverse
  case rsDOWN:
    gGameInputData.reverse = 1;
    return rsEVENTPROCESSED;

    // turn left
  case rsLEFT:
    gGameInputData.turn = -1;
    return rsEVENTPROCESSED;

    // turn right
  case rsRIGHT:
    gGameInputData.turn = 1;
    return rsEVENTPROCESSED;

    // show debug output
  case rsF2:
    RsGlobal.bShowFPS = !RsGlobal.bShowFPS;
    return rsEVENTPROCESSED;

    // show wireframe models
  case rsF3:
    RsGlobal.bShowWireframe = !RsGlobal.bShowWireframe;
    if(RsGlobal.bShowWireframe) gWorld->ResizeIm3DVertexArray();
    return rsEVENTPROCESSED;

    // change camera mode
  case rsF8:
    gCamera->IncrementViewMode();
    return rsEVENTPROCESSED;

    // brake
  case rsSPACE:
    gGameInputData.brake = 1;
    return rsEVENTPROCESSED;

   // fire main weapon
  case rsLCTRL:
    gGameInputData.fireMain = 1;
    return rsEVENTPROCESSED;

   // fire secondary weapon
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

    // enter chat mode NOT LOCKED.
  case rsENTER:
    toggleChatVisibility(TRUE);
    // chance to chat input handler
    AttachChatInput(FALSE);
    // clear game input so that keys are locked
    memset(&gGameInputData, 0, sizeof(GameInputData));
    return rsEVENTPROCESSED;

    // Enter chat mode LOCKED
  case 't':
    toggleChatVisibility(TRUE);
    // chance to chat input handler
    AttachChatInput(TRUE);
    // clear game input so that keys are locked
    memset(&gGameInputData, 0, sizeof(GameInputData));
    return rsEVENTPROCESSED;

    // toggle chat window visibility
  case 'c':
    toggleChatVisibility(!RsGlobal.bShowChat);
    return rsEVENTPROCESSED;

    // quit app
  case 'x':
    // Send a quit message - this allows app to do stuff
//    RsEventHandler(rsQUITAPP, NULL);
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


/*******************************************************************************/
BOOL AttachGameInput(void)
{
  RsInputDeviceAttach(rsKEYBOARD, giKeyboardHandler);
  memset(&gGameInputData,0,sizeof(gGameInputData));

	gLastInputMode = gInputMode;
  gInputMode = IM_GAME;

  return TRUE;
}

/*******************************************************************************/
