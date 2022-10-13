
/****************************************************************************
 *
 * xbox.c
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/* D3D8 Libraries */
#include <xtl.h>
#include <d3d8.h>

#include <stdio.h>

#include "rwcore.h"
#include "../platform.h"
#include "../skeleton.h"
//#include "mouse.h"
//#include "../vecfont.h"
//#include <XbApp.h>
//#include <XbFont.h>
//#include "virtualKeyboard.h"
#include "../gui.h"
#include "../input.h"

#include "../entity/ZBCamera.h"

// Globals
extern ZBCamera *gCamera;
extern GameInputData gGameInputData;

int charChoice = 0;

#ifdef RWSPLASH
/* Splash screen */
#include "splash.h"
#endif /* RWSPLASH */

/*--- Macro Definitions ---*/

#define JOYRANGE            (float)32767.5     /* (65536 - 1) / 2 */
#define JOYDEADZONE         (float)0.3

#define JOYSKELRANGE        (float)(1)
#define JOYRANGESCALE       (float)(JOYSKELRANGE / (float)(JOYRANGE))

#define JOYRANGECONVERT(_d)  \
    ( ((-(float)(_d)) + ((float)0.5)) * JOYRANGESCALE)

extern RwV2d DisplayPos[4];

/*--- Constants ---*/

/*--- Local Structure Definitions ---*/
//CXBFont    m_FontLatin;        // Font renderer for most keys

typedef struct _padStatus padStatus;
struct _padStatus
{
    HANDLE      device;         /* Device handle for the gamepad */
    UINT32    oldButtonState; /* Buttons changed since last poll */
};

/*--- Static ---*/

extern char gUserName[128];

static RwChar       MediaPath[] = "D:\\";
static padStatus    Pad = {NULL, 0};

static RwTextureCallBackRead    DefaultTextureReadCallBack;

/*
 *****************************************************************************
 */
#ifdef RWMETRICS
void
psMetricsRender(RsVectorFont *vecFont,
                RwV2d *pos,
                RwMetrics *metrics __RWUNUSED__)
{
    RwXboxMetrics   *xboxMetrics;

    xboxMetrics = (RwXboxMetrics *)metrics->devSpecificMetrics;

    /* check z compression ratio */
    {
        RwChar message[256];
        D3DTILE PrimaryDepthBufferTile;
        DWORD startTag, endTag, NumberCompressedTags;

        D3DDevice_GetTile( 1, &PrimaryDepthBufferTile );

        startTag = PrimaryDepthBufferTile.ZStartTag;
        endTag = D3DTILE_ZENDTAG( &PrimaryDepthBufferTile );

        NumberCompressedTags = D3DDevice_GetTileCompressionTags( startTag, endTag );

		sprintf(message,"percent z tags compressed %d \n", 100 * NumberCompressedTags / (endTag - startTag + 1));

        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
    }
}
#endif /* RWMETRICS */


/*
 *****************************************************************************
 */
#ifdef RWSPLASH
BOOL
psDisplaySplashScreen(BOOL state __RWUNUSED__)
{
    return TRUE;
}
#endif /* RWSPLASH */


/*
 *****************************************************************************
 */
void
psWindowSetText(const RwChar *text __RWUNUSED__)
{
}

/*
 *****************************************************************************
 */
void
psWarningMessage(const RwChar *message )
{
    OutputDebugString(message);
    OutputDebugString(RWSTRING("\n"));
}


/*
 *****************************************************************************
 */
void
psErrorMessage(const RwChar *message)
{
	OutputDebugString(message);
	OutputDebugString(RWSTRING("\n"));
// For XBOX Display a screeen with a selector
  static GUI *charGUI = NULL;
  static BOOL bFirst = TRUE;
  static GUIFont *font;
	RwRGBA RwBlack = {255,255,255,0};

  static float lastSpeed;
  RwChar caption[128];

  gCamera->ClearBgrd(&RwBlack);

  if(gCamera->BeginUpdate())
  {
		GUIBackground *bgrd;
		charGUI = new GUI(2, gCamera);
		bgrd = new GUIBackground(charGUI);

		if(!bgrd->LoadSetTexture(RWSTRING("d:\\"), RWSTRING("d:\\resources\\xbox\\ErrorPage")))		
			return;	

		font = new GUIFont(charGUI, 1);
		if(!font->LoadFontMetric1(RWSTRING("gui/font/"),
															RWSTRING("ChromosomeLight16")))
			return;
		font->SetFontHeight(0.1f);
		charGUI->BuildAll();   	
		charGUI->Render();

		rwsprintf(caption, RWSTRING("Attention:"), RsGlobal.version);
		font->AddStringPos(caption, (float).1, (float).8);

		font->SetFontHeight(0.08f);
		rwsprintf(caption, RWSTRING(message), RsGlobal.version);
		font->AddStringPos(caption, (float).05, (float).5);
		

	  gCamera->EndUpdate();
  }

  gCamera->ShowRaster();

	Sleep(1500);

}

/*
 *****************************************************************************
 */
void
psDebugMessageHandler(RwDebugType type __RWUNUSED__, const RwChar *str)
{
    OutputDebugString(str);
    OutputDebugString(RWSTRING("\n"));
}


/*
 *****************************************************************************
 */
void
psMouseSetVisibility(BOOL visible __RWUNUSED__)
{
}


/*
 *****************************************************************************
 */
void
psMouseSetPos(RwV2d *pos __RWUNUSED__)
{
}


/*
 *****************************************************************************
 */
RwImage* 
psGrabScreen(RwCamera *camera)
{
    RwRaster *camRas;
    INT32 width, height;
    RwImage *image;

    camRas = RwCameraGetRaster(camera);

    if (camRas)
    {
        width = RwRasterGetWidth(camRas);
        height = RwRasterGetHeight(camRas);
        image = RwImageCreate(width, height, 32);

        if (image)
        {
            RwImageAllocatePixels(image);
            RwImageSetFromRaster(image, camRas);

            return (image);
        }
        else
        {
            return (NULL);
        }
    }
    else
    {
        return (NULL);
    }
}


/*
 *****************************************************************************
 */
RwMemoryFunctions *
psGetMemoryFunctions()
{
    return (NULL);
}


/*
 *****************************************************************************
 */
BOOL
psInstallFileSystem(void)
{
    return (TRUE);
}

/*
 *****************************************************************************
 */

static RwTexture *
XboxTextureReadCallBack(const RwChar *name, const RwChar *maskName)
{
	
    RwTexture   *texture = NULL;
/*
    texture = RwXboxDDSTextureRead(name, maskName);
    if (!texture)
    {
        return (DefaultTextureReadCallBack(name, maskName));
    }

    return (texture);
*/
		return (DefaultTextureReadCallBack(name, maskName));
}


/*
 *****************************************************************************
 */
BOOL
psNativeTextureSupport(void)
{
    DefaultTextureReadCallBack = RwTextureGetReadCallBack();
    RwTextureSetReadCallBack(XboxTextureReadCallBack);

    return (TRUE);
}


/*
 *****************************************************************************
 */
BOOL
psInitialize(void)
{
    return (TRUE);
}


/*
 *****************************************************************************
 */
void
psTerminate(void)
{
}


/*
 *****************************************************************************
 */
BOOL
psAlwaysOnTop(BOOL AlwaysOnTop __RWUNUSED__)
{
    return TRUE;
}


/*
 *****************************************************************************
 */
UINT32 psTimer(void)
{
    UINT32 time;

    time = (UINT32)timeGetTime();

    return time;
}


/*
 *****************************************************************************
 */
RwChar *
psPathnameCreate(const RwChar *srcBuffer)
{
    RwChar *dstBuffer;
    RwChar *charToConvert;
    INT32 length = MAX_PATH;

    length = rwstrlen(srcBuffer) + rwstrlen(MediaPath) + 1;

    // First duplicate the string 
    dstBuffer = (RwChar*)RwOsGetMemoryInterface()->rwmalloc(sizeof(RwChar) * length);
    
    if (dstBuffer)
    {
        /* Drop ./,.\\ */

        if (*srcBuffer == '.')
        {
            srcBuffer++;
            length--;
        }

        if (*srcBuffer == '/')
        {
            srcBuffer++;
            length--;
        }

        if (*srcBuffer == '\\')
        {
            srcBuffer++;
            length--;
        }

        if (*srcBuffer == ':')
        {
            srcBuffer++;
            length--;
        }

        /* Media path */
        if (strncmp(srcBuffer, MediaPath, rwstrlen(MediaPath)) != 0)
        {
            rwstrcpy(dstBuffer, MediaPath);
        }
        else
        {
            /* already has media path */
            rwstrcpy(dstBuffer, "");
        }

        /* Append path */
        rwstrcat(dstBuffer, srcBuffer);

        /* 
         * Convert all /s into \s 
         */
        while ((charToConvert = rwstrchr(dstBuffer, '/')))
        {
            *charToConvert = '\\';
        }
    }

    return(dstBuffer);
}


/*
 *****************************************************************************
 */
void
psPathnameDestroy(RwChar *buffer)
{
    if (buffer)
    {
        RwFree(buffer);
    }
}


/*
 *****************************************************************************
 */
RwChar
psPathGetSeparator(void)
{
    return '\\';
}


/*
 *****************************************************************************
 */
void
psCameraShowRaster(RwCamera *camera)
{
    RwCameraShowRaster(camera, NULL, rwRASTERFLIPWAITVSYNC);
}


/*
 *****************************************************************************
 */
BOOL
psSelectDevice(BOOL useDefault)
{
    if(!useDefault)
    {
        INT32     vidMode, numVidModes;
        RwVideoMode vidModemInfo;

        numVidModes = RwEngineGetNumVideoModes();

        /* Add the available video modes to the dialog */
        for (vidMode = 0; vidMode < numVidModes; vidMode++)
        {
            RwEngineGetVideoModeInfo(&vidModemInfo, vidMode);

            if ((vidModemInfo.width == 640) &&
                (vidModemInfo.height == 480) &&
                (vidModemInfo.depth == 32) &&
                (vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE))
            {
                RsGlobal.maximumWidth = vidModemInfo.width;
                RsGlobal.maximumHeight = vidModemInfo.height;

                return (TRUE);
            }
        }
    }
    else
    {
        RwVideoMode vidModemInfo;

        RwEngineGetVideoModeInfo(&vidModemInfo,
                                 RwEngineGetCurrentVideoMode());
        RsGlobal.maximumWidth = vidModemInfo.width;
        RsGlobal.maximumHeight = vidModemInfo.height;
    }

    return (TRUE);
}


/*
 *****************************************************************************
 */
void
padOpen(void)
{
    DWORD                   deviceMask = 0;
    XDEVICE_PREALLOC_TYPE   deviceTypes[] =
    {
       {XDEVICE_TYPE_GAMEPAD, 1}
    };

    XInitDevices(1, deviceTypes);

    /* PAUSE - for XInitDevices before calling XGetDevices */
    {
        volatile UINT32 i;

        for (i = 0; i < 0x0FFFFFFF; i++) {}
    }

    /* Blank the GamePads */
    memset(&Pad, 0, sizeof(padStatus));

    /* Get a mask of all currently available devices */
    deviceMask = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    /* Open port 0 */
    if (deviceMask & XDEVICE_PORT0_MASK)
    {
        Pad.device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0,
                                XDEVICE_NO_SLOT, NULL);
    }
}


/*
 *****************************************************************************
 */
static UINT32
remapButtons(WORD dButtons, BYTE *aButtons)
{
    UINT32 rsButtons;

    rsButtons = 0;

    rsButtons |= (dButtons & XINPUT_GAMEPAD_START) ? rsPADSTART : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_BACK) ? rsPADSELECT : 0;

    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_UP) ? rsPADDPADUP : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? rsPADDPADDOWN : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? rsPADDPADLEFT : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? rsPADDPADRIGHT : 0;

    /*
     * Convert the analogue buttons to digital
     */

    /* A */
    if (aButtons[XINPUT_GAMEPAD_A] >= 0x80)
    {
        rsButtons |= rsPADBUTTON1;
    }

    /* B */
    if (aButtons[XINPUT_GAMEPAD_B] >= 0x80)
    {
        rsButtons |= rsPADBUTTON2;
    }
    
    /* X */
    if (aButtons[XINPUT_GAMEPAD_X] >= 0x80)
    {
        rsButtons |= rsPADBUTTON3;
    }

    /* Y */
    if (aButtons[XINPUT_GAMEPAD_Y] >= 0x80)
    {
        rsButtons |= rsPADBUTTON4;
    }

    /* Black */
    if (aButtons[XINPUT_GAMEPAD_BLACK] >= 0x80)
    {
        rsButtons |= rsPADBUTTON5;
    }

    /* White */
    if (aButtons[XINPUT_GAMEPAD_WHITE] >= 0x80)
    {
        rsButtons |= rsPADBUTTON6;
    }

    /* Left trigger */
    if (aButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] >= 0x80)
    {
        rsButtons |= rsPADBUTTON7;
    }

    /* Right trigger */
    if (aButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] >= 0x80)
    {
        rsButtons |= rsPADBUTTON8;
    }

    return (rsButtons);
}



/*
 *****************************************************************************
 */
void XboxTestForQuitState(XINPUT_STATE *PadState)
{
	/*
	 *    test whether the left trigger, right trigger, and
	 *    black buttons are depressed
	 */
    if ( ( PadState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  >= 0x80 ) &&   /* left trigger down  */
		 ( PadState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] >= 0x80 ) &&   /* right trigger down */
		 ( PadState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]         >= 0x80 ) )    /* black button down  */
	{
        RsEventHandler(rsQUITAPP, 0);
	}

	return;
}



/*
 *****************************************************************************
 */
void
padUpdate(void)
{
    /* Read the input state for pad0 */
    if (Pad.device)
    {
        XINPUT_STATE        state;
        RsPadButtonStatus   padButtonsDown;
        RsPadButtonStatus   padButtonsUp;
        UINT32            buttons;
        RwV2d               delta;
        static BOOL       zeroLeftStick = FALSE;
        static BOOL       zeroRightStick = FALSE;

        /* Get the input state fpr Pad0 */
        XInputGetState(Pad.device, &state);

		/*
		 *    test for quit state
		 *    terminate if found
		 */
		XboxTestForQuitState(&state);


        /*
         * Buttons
         */

        /* Remap the buttons */
        buttons = remapButtons(state.Gamepad.wButtons, state.Gamepad.bAnalogButtons);

        /* Set the padID to 0 */
        padButtonsDown.padID = padButtonsUp.padID = 0;

        /* Find out what buttons were released */
        padButtonsUp.padButtons = Pad.oldButtonState & ~buttons;

        /* Find out what buttons were pressed */
        padButtonsDown.padButtons = ~Pad.oldButtonState & buttons;

        /* If any buttons just went down, or came up, message the app */
        if (padButtonsDown.padButtons)
        {
            RsPadEventHandler(rsPADBUTTONDOWN, &padButtonsDown);
        }
    
        if (padButtonsUp.padButtons)
        {
            RsPadEventHandler(rsPADBUTTONUP, &padButtonsUp);
        }

        /* Save buttons so we know what's changed next time */
        Pad.oldButtonState = buttons;

        /*
         * Sticks
         */

        /* Manage the left analogue sticks */
        delta.x = JOYRANGECONVERT(state.Gamepad.sThumbLX);
        if (RwFabs(delta.x) < JOYDEADZONE)
        {
            delta.x = (float)0.0;
        }

        delta.y = JOYRANGECONVERT(state.Gamepad.sThumbLY);
        if (RwFabs(delta.y) < JOYDEADZONE)
        {
            delta.y = (float)0.0;
        }

        if ((0 == delta.x) && (0 == delta.y))
        {
            if (!zeroLeftStick)
            {
                RsPadEventHandler(rsPADANALOGUELEFT, &delta);
                zeroLeftStick = TRUE;
            }
        }
        else
        {
            RsPadEventHandler(rsPADANALOGUELEFT, &delta);
            zeroLeftStick = FALSE;
        }

	    /* Manage the right analogue sticks */
        delta.x = JOYRANGECONVERT(state.Gamepad.sThumbRX);
        if (RwFabs(delta.x) < JOYDEADZONE)
        {
            delta.x = (float)0.0;
        }

        delta.y = JOYRANGECONVERT(state.Gamepad.sThumbRY);
        if (RwFabs(delta.y) < JOYDEADZONE)
        {
            delta.y = (float)0.0;
        }

        if ((0 == delta.x) && (0 == delta.y))
        {
            if (!zeroRightStick)
            {
                RsPadEventHandler(rsPADANALOGUERIGHT, &delta);
                zeroRightStick = TRUE;
            }
        }
        else
        {
            RsPadEventHandler(rsPADANALOGUERIGHT, &delta);
            zeroRightStick = FALSE;
        }
    }
}

BOOL psCharacterSelectScreen(void* charInfo)
{
	return TRUE;
}

BOOL psCharacterCreateScreen(void* charInfo)
{
  ZBMasterVehicleMsg *master = (ZBMasterVehicleMsg*)charInfo;
  
  strcpy(master->props.name, gUserName);

	return TRUE;
}


/*
 *****************************************************************************
 */
int
main(void)
{
    RwXboxDeviceConfig  deviceConfig;

		OutputDebugString("Starting XZonaBattle running on XBox\n");

    /* These are the default setting just to demonstrate functionality.
     * If no RwXboxDeviceConfig struct is passed these default values
     * are used
     */
    deviceConfig.multiSample = D3DMULTISAMPLE_NONE;
    deviceConfig.zBufferFormat = D3DFMT_D24S8; //D3DFMT_LIN_X8R8G8B8; //D3DFMT_D24S8;
    deviceConfig.presentFlags = 0; //D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
    deviceConfig.refreshRateInHz = D3DPRESENT_RATE_DEFAULT;

//		sprintf(message,"Starting XZonaBattle\n");

    /*
     * Initialize the platform independent data 
     * This will in turn initialise the platform specific data
     */
    if (RsEventHandler(rsINITIALIZE, NULL) == rsEVENTERROR)
    {
        return FALSE;
    }

    /* Initialize the 3D (RenderWare) components of the app. */
    if (rsEVENTERROR == RsEventHandler(rsRWINITIALIZE, &deviceConfig))
    {
        RsEventHandler(rsTERMINATE, NULL);
        return FALSE;

    }

    /*
     * Open the pad
     */
    padOpen();


    /*
     * Force a camera resize event
     */
    {
       RwRect              r;

        r.x = 0;
        r.y = 0;
        r.w = RsGlobal.maximumWidth;
        r.h = RsGlobal.maximumHeight;

        RsEventHandler(rsCAMERASIZE, &r);
    }

    /*
     * Enter the message processing loop.
     */
    while (!RsGlobal.bQuit)
    {
        /* Process the pad */
        padUpdate();

         RsEventHandler(rsIDLE, NULL);
    }

    /*
     * Tidy up the 3D (RenderWare) components of the application.
     */
    RsEventHandler(rsRWTERMINATE, NULL);

    /* free the platform dependent data */
    RsEventHandler(rsTERMINATE, NULL);
    
	/*
	 *    return to the dashboard or you get a silly debug message
	 */
	XLaunchNewImage( NULL, NULL );

    return 0;
}

void psShowLoadingScreen(RwChar* message, RwChar* title)
{

}

/*void psShowLoadingScreen(RwChar* message, RwChar* title)
{

}
*/

void psCloseLoadingScreen()
{

}

int psPlayerLoginScreen(void* loginInfo)
{
// For XBOX Display a screeen with a selector
  static GUI *charGUI = NULL;
  static BOOL bFirst = TRUE;
  static GUIFont *font;
	RwRGBA RwBlack = {0,0,0,255};

  static float lastSpeed;
  RwChar caption[128];

  gCamera->ClearBgrd(&RwBlack);

  if(gCamera->BeginUpdate())
  {
		GUIBackground *bgrd;

		charGUI = new GUI(3, gCamera);

		bgrd = new GUIBackground(charGUI);

		if(!bgrd->LoadSetTexture(RWSTRING("d:\\"), RWSTRING("d:\\resources\\xbox\\ZBMenu")))		
			return FALSE;

		font = new GUIFont(charGUI, 1);
		if(!font->LoadFontMetric1(RWSTRING("gui/font/"),
															RWSTRING("ChromosomeLight16")))
			return FALSE;

		font->SetFontHeight(0.09f);

		charGUI->BuildAll();   
		charGUI->Render();

		font->SetAlignment(4);
		font->SetAlignment(1);
		//RwV2d fontPos = {0.1f, 0.85f}; delme
		font->SetPosition(0.1f, 0.85f); 

		rwsprintf(caption, RWSTRING("Select a Character"), RsGlobal.version);
		font->AddStringLine(caption);
		font->SetFontHeight(0.07f);
		rwsprintf(caption, RWSTRING("Press X for MrBlue"));
		font->AddStringLine(caption);
		rwsprintf(caption, RWSTRING("Press Y for MrYellow"));
		font->AddStringLine(caption);
		rwsprintf(caption, RWSTRING("Press B for MrRed"));
		font->AddStringLine(caption);
		rwsprintf(caption, RWSTRING("Press A for MrGreen"));
		font->AddStringLine(caption);

	  gCamera->EndUpdate();
  }

  gCamera->ShowRaster();

  AttachGameInput();
	char buf[255];
  while(1)
	{
		padUpdate();

		if (gGameInputData.fireMain		== 1)		charChoice = 1;
		if (gGameInputData.fireSecond	== 1)		charChoice = 2;
		if (gGameInputData.devData0		== 1)		charChoice = 3;
		if (gGameInputData.devData0		== -1)	charChoice = 4;
		if(charChoice > 0)		
		{
			sprintf(buf, "fireMain=%d\n", charChoice);
			OutputDebugString(buf);
			break;
		}
	}

	gCamera->BeginUpdate();

	RsLoginInfo* tmpLoginInfo = (RsLoginInfo*)loginInfo;

	switch(charChoice)
	{
		case 1:
			rwsprintf(caption, RWSTRING("Character MrGreen(A)"));
			strcpy(tmpLoginInfo->name, "MrGreen");
			break;
		case 2:
			rwsprintf(caption, RWSTRING("Character MrRed(B)"));
			strcpy(tmpLoginInfo->name, "MrRed");
			break;
		case 3:
			rwsprintf(caption, RWSTRING("Character MrBlue(X)"));
			strcpy(tmpLoginInfo->name, "MrBlue");
			break;
		case 4:
			rwsprintf(caption, RWSTRING("Character MrYellow(Y)"));
			strcpy(tmpLoginInfo->name, "MrYellow");
			break;
	}

	font->SetPosition(0.1f, 0.65f); 

	font->AddStringLine(RWSTRING("You Have Selected the"));
	font->AddStringLine(caption);
	gCamera->EndUpdate();
  gCamera->ShowRaster();

  if(RsTimer() > 1000)
  {
    gCamera->ClearBgrd(&RwBlack);
    delete charGUI;
//    gTaskState = TS_INITGAME;
  }
	
	strcpy(tmpLoginInfo->pass, "pass");
	return TRUE;
}

RwChar* psResourcePathCreate(const RwChar *srcBuffer)
{
  RwChar *dstBuffer;
  RwChar *charToConvert;
  //INT32 bufSize;
  INT32 length;

  //bufSize = sizeof(RwChar) * (rwstrlen(srcBuffer) + 
  //                             rwstrlen(RsGlobal.resPath)) + 2;

  length = rwstrlen(srcBuffer) + rwstrlen(MediaPath) + rwstrlen(RsGlobal.resPath) + 1;


  // First duplicate the string 
  dstBuffer = (RwChar*)RwOsGetMemoryInterface()->rwmalloc(sizeof(RwChar) * length);

  if (dstBuffer)
  {
      /* Drop ./,.\\ */

      if (*srcBuffer == '.')
      {
          srcBuffer++;
          length--;
      }

      if (*srcBuffer == '/')
      {
          srcBuffer++;
          length--;
      }

      if (*srcBuffer == '\\')
      {
          srcBuffer++;
          length--;
      }

      if (*srcBuffer == ':')
      {
          srcBuffer++;
          length--;
      }

      /* Media path */
      if (strncmp(srcBuffer, MediaPath, rwstrlen(MediaPath)) != 0)
      {
          rwstrcpy(dstBuffer, MediaPath);
      }
      else
      {
          /* already has media path */
          rwstrcpy(dstBuffer, "");
      }

      /* Append path */
			rwstrcat(dstBuffer, RsGlobal.resPath);
      rwstrcat(dstBuffer, srcBuffer);

      /* 
       * Convert all /s into \s 
       */
      while ((charToConvert = rwstrchr(dstBuffer, '/')))
      {
          *charToConvert = '\\';
      }
  }


  return dstBuffer;
}