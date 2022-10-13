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


/*****************************************************************************/
// Global Variables
/*****************************************************************************/
MenuInputData gMenuInputData;


/*******************************************************************************/
static RsEventStatus miHandleKeyDown(RsKeyStatus *keyStatus)
{
  switch(keyStatus->keyCharCode)
  {
  case rsUP:
    gMenuInputData.direction = DI_UP;
    return rsEVENTPROCESSED;

  case rsDOWN:
    gMenuInputData.direction = DI_DOWN;
    return rsEVENTPROCESSED;

  case rsLEFT:
    gMenuInputData.direction = DI_LEFT;
    return rsEVENTPROCESSED;

  case rsRIGHT:
    gMenuInputData.direction = DI_RIGHT;
    return rsEVENTPROCESSED;
  }

  return rsEVENTNOTPROCESSED;
}

/*******************************************************************************/
static RsEventStatus miHandleKeyUp(RsKeyStatus *keyStatus)
{
  switch(keyStatus->keyCharCode)
  {
  case rsUP:
  case rsDOWN:
  case rsLEFT:
  case rsRIGHT:
    gMenuInputData.direction = DI_NONE;
    return rsEVENTPROCESSED;
  }

  return rsEVENTNOTPROCESSED;
}

/*******************************************************************************/
static RsEventStatus miKeyboardHandler(RsEvent event, void *param)
{
  /*...then the application events, if necessary...*/
  switch( event )
  {
    case rsKEYDOWN:
      return miHandleKeyDown((RsKeyStatus *)param);

    case rsKEYUP:
      return miHandleKeyUp((RsKeyStatus *)param);

    default:
      return rsEVENTNOTPROCESSED;
  }
}

/*****************************************************************************/
BOOL AttachMenuInput(void)
{
  RsInputDeviceAttach(rsKEYBOARD, miKeyboardHandler);
  memset(&gMenuInputData, 0, sizeof(gMenuInputData));

	gLastInputMode = gInputMode;
  gInputMode = IM_MENU;

  return TRUE;
} 

/*****************************************************************************/
