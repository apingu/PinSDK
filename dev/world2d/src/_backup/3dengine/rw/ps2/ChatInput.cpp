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
ChatInputData gChatInputData;


/*******************************************************************************/
static RsEventStatus ciHandleKeyDown(RsKeyStatus *keyStatus)
{
  switch(keyStatus->keyCharCode)
  {
  case rsBACKSP:
    gChatInputData.keyInput[gChatInputData.size++] = CKI_BACKSPACE;
    return rsEVENTPROCESSED;

  case rsENTER:
    gChatInputData.keyInput[gChatInputData.size++] = CKI_ENTER;
    return rsEVENTPROCESSED;

  case rsESC:
    gChatInputData.keyInput[gChatInputData.size++] = CKI_CANCEL;
    gChatInputData.bLocked = FALSE;
    return rsEVENTPROCESSED;

  default:
    gChatInputData.keyInput[gChatInputData.size] = CKI_CHAR_INPUT;
    gChatInputData.charInput[gChatInputData.size++] = keyStatus->keyChar[0];
    return rsEVENTPROCESSED;
  }

  return rsEVENTNOTPROCESSED;
}

/*******************************************************************************/
static RsEventStatus ciHandleKeyUp(RsKeyStatus *keyStatus)
{
//  gChatInputData.keyInput[gChatInputData.size++] = CKI_NOINPUT;
  return rsEVENTPROCESSED;
}

/*******************************************************************************/
static RsEventStatus ciKeyboardHandler(RsEvent event, void *param)
{
  /*...then the application events, if necessary...*/
  switch( event )
  {
    case rsKEYDOWN:
      return ciHandleKeyDown((RsKeyStatus *)param);

    case rsKEYUP:
      return ciHandleKeyUp((RsKeyStatus *)param);

    default:
      return rsEVENTNOTPROCESSED;
  }
}

/*****************************************************************************/
BOOL AttachChatInput(BOOL bLock)
{
  RsInputDeviceAttach(rsKEYBOARD, ciKeyboardHandler);
  memset(&gChatInputData, 0, sizeof(gChatInputData));
	
  gLastInputMode = gInputMode;
  gInputMode = IM_CHAT;
  gChatInputData.bLocked = bLock;
  RsGlobal.bShowChat = TRUE; // just a precaution. set elsewhere too.
  
  return TRUE;
} 

/*****************************************************************************/
