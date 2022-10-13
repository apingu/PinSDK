/****************************************************************************
 *
 * sky.c
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

#define IOPPATH c:/usr/local/sce/iop


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>

#include <sys/errno.h>

#include <rwcore.h>
#include "rtcharse.h"
#include "rtbmp.h"
#include "skeleton.h"
#include "camera.h"
#include "platform.h"
#include "vecfont.h"
#include "skyfs.h"

#include "eeregs.h"
#include "sifdev.h"
#include "sifrpc.h"
#include "libcdvd.h"

#include <ConfigReader.h>
#include <ZonaBattle/messageType.h>


BOOL Initialize(void);
BOOL Initialize3D(void *param);
void OnCameraSize(RwRect* rect);
void Idle(void);


int _read_from_cd;
int gUserNumber = -1;

typedef RwChar      SkyString[256];

typedef struct _RwSkyState RwSkyState;
struct _RwSkyState
{
    volatile unsigned long sweHighCount;
    int                 skyTimerHandlerHid;
    int                 keyboardHandle;
    char               *iopPathOverride;
    RwMemoryFunctions   bugWorkAround;
    SkyString           elfDir;
};

static RwSkyState   SkyState;


/*
 *****************************************************************************
 */
static int
TimerHandler(int ca)
{
    if ((ca == INTC_TIM0) && (*T0_MODE & 0x800))
    {
        *T0_MODE |= 0x800;

        SkyState.sweHighCount += 0x10000;
    }

    /* This is a TRC requirement at the moment */
    ExitHandler();

    return 0;
}


/*
 *****************************************************************************
 */
static void        *
private_malloc(size_t s)
{
    /* Since memalign() is broken, we fake it here */
    void               *newMem, *alignedMem;

    newMem = malloc(s + 64);
    if (newMem)
    {
        alignedMem = (void *) (((RwUInt32) newMem + 64) & ~63);

        /* Norty - assume sizeof(void **) == sizeof(RwUInt32) */
        ((void **) alignedMem)[-1] = newMem;
        ((RwUInt32 *) alignedMem)[-2] = s;
    }
    else
    {
        alignedMem = NULL;
    }

    return alignedMem;
}

/*
 *****************************************************************************
 */
static void
private_free(void *alignedMem)
{
    /* Since memalign() is broken, we fake it here */
    free(((void **) alignedMem)[-1]);

    return;
}

/*
 *****************************************************************************
 */
static void *
private_realloc(void *oldMem, size_t newSize)
{
    /* Since memalign() is broken, we fake it here */
    void               *newMem;

    newMem = private_malloc(newSize);
    if (newMem)
    {
        RwUInt32            oldSize = ((RwUInt32 *) oldMem)[-2];

        if (oldSize < newSize)
        {
            memcpy(newMem, oldMem, oldSize);
        }
        else
        {
            memcpy(newMem, oldMem, newSize);
        }

        private_free(oldMem);
    }

    return newMem;
}

/*
 *****************************************************************************
 */
static void *
private_calloc(size_t a, size_t b)
{
    /* Since memalign() is broken, we fake it here */
    size_t              realSize = a * b;
    void               *newMem;

    newMem = private_malloc(realSize);
    if (newMem)
    {
        memset(newMem, 0, realSize);
    }

    return newMem;
}

/*
 *****************************************************************************
 */
void
psMouseSetVisibility(RwBool visible)
{
    return;
}

/*
 *****************************************************************************
 */
void
psMouseSetPos(RwV2d * pos)
{
    return;
}

/*
 *****************************************************************************
 */
void
psWindowSetText(const RwChar * text)
{
    RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__, text);

    return;
}

/*
 *****************************************************************************
 */
void
psErrorMessage(const RwChar * message)
{
    RwDebugSendMessage(rwDEBUGERROR, __FUNCTION__, message);

    return;
}

/*
 *****************************************************************************
 */
void
psWarningMessage(const RwChar * message)
{
    RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__, message);

    return;
}

/*
 *****************************************************************************
 */
void
psCameraShowRaster(RwCamera *camera)
{
    RwCameraShowRaster(camera, NULL, 0);

    return;
}


/*
 *****************************************************************************
 */
RwImage *
psGrabScreen(RwCamera * camera)
{
    RwImage            *result = (RwImage *) NULL;
    RwRaster           *camRas;
    RwInt32             width, height, depth;
    RwImage            *image;
    RwUInt8            *oldCamPixels, *pixels;

    camRas = RwCameraGetRaster(camera);

    if (camRas)
    {
        width = RwRasterGetWidth(camRas);
        height = RwRasterGetHeight(camRas);
        depth = RwRasterGetDepth(camRas) >> 3;

        /*
         * On PS2, we must give the Camera a data area.
         * Ordinarily, PS2 camera rasters have no in memory image
         * Under normal conditions it is impossible to lock the camera
         * raster on PS2 as its a real performance problem. However,
         * by setting the cpPixel pointer the driver will assume that
         * you know what you are doing and are willing to take the hit
         */

        /* We know that malloc aligns on qw */
        oldCamPixels = camRas->cpPixels;

        pixels = (RwUInt8 *) malloc(width * height * depth);

        if ((pixels))
        {
            camRas->cpPixels = pixels;
            /* Lock the camera raster for read. This will do what ever is */
            /* required to get the data into memory */

            /* The PS2 driver doesn't do this for you */
            if (!RwRasterLock(camRas, 0, rwRASTERLOCKREAD))
            {
                camRas->cpPixels = oldCamPixels;
                free(pixels);
            }

            image = RwImageCreate(width, height, 32);
            if (image)
            {
                RwImageAllocatePixels(image);
                RwImageSetFromRaster(image, camRas);

                RwRasterUnlock(camRas);
                camRas->cpPixels = oldCamPixels;
                free(pixels);

                result = (image);
            }
            else
            {
                RwRasterUnlock(camRas);
                camRas->cpPixels = oldCamPixels;
                free(pixels);

                result = (RwImage *) NULL;
            }
        }
        else
        {
            result = (RwImage *) NULL;
        }

    }
    else
    {
        result = (RwImage *) NULL;
    }

    return result;
}

/*
 *****************************************************************************
 */
void
psDebugMessageHandler(RwDebugType type, const RwChar * str)
{
    switch (type)
    {
#if (defined(COLOR))
        case rwDEBUGASSERT:   /* red */
            printf("\033[31m%s\033[0m\n", str);
            break;
        case rwDEBUGERROR:    /* bold red */
            printf("\033[31;1m%s\033[0m\n", str);
            break;
        case rwDEBUGMESSAGE:  /* blue */
            printf("\033[34m%s\033[0m\n", str);
            break;
#endif /* (defined(COLOR)) */

        default:
            printf("%s\n", str);
    }

    return;
}

/*
 *****************************************************************************
 */
RwUInt32
psTimer(void)
{
    unsigned long       high0, low0, high1, low1;

    high0 = SkyState.sweHighCount;
    low0 = *T0_COUNT;
    high1 = SkyState.sweHighCount;
    low1 = *T0_COUNT;

    if (high0 == high1)
    {
        return ((RwUInt32) ((high0 | (low0 & 0xffff)) / 9216));
    }
    else
    {
        return ((RwUInt32) ((high1 | (low1 & 0xffff)) / 9216));
    }
}

/*
 *****************************************************************************
 */
RwUInt64
psMicroTimer(void)
{
    unsigned long       high0, low0, high1, low1;

    high0 = SkyState.sweHighCount;
    low0 = *T0_COUNT;
    high1 = SkyState.sweHighCount;
    low1 = *T0_COUNT;

    if (high0 == high1)
    {
        return (((RwUInt64) (high0 | (low0 & 0xffff))) * 1000) / 9216;
    }
    else
    {
        return (((RwUInt64) (high1 | (low1 & 0xffff))) * 1000) / 9216;
    }
}

/*
 *****************************************************************************
 */
RwChar *
psPathnameCreate(const RwChar * pathname)
{
    if (pathname)
    {
        char               *dstBuffer;

        dstBuffer = (char *)
            RwMalloc(sizeof(RwChar) * (rwstrlen(pathname) + 1));

        if (dstBuffer)
        {
            char               *charToConvert;

            strcpy(dstBuffer, pathname);

            /* Convert a path for use on PSX 2.
             * Convert all \s and :s into /s */
            while ((charToConvert = rwstrchr(dstBuffer, '\\')))
            {
                *charToConvert = '/';
            }

            while ((charToConvert = rwstrchr(dstBuffer, ':')))
            {
                *charToConvert = '/';
            }
        }

        return dstBuffer;
    }

    return 0;
}

/*
 *****************************************************************************
 */
void
psPathnameDestroy(RwChar * pathname)
{
    if (pathname)
    {
//        RwFree(pathname);

        pathname = 0;
    }

    return;
}

/*
 *****************************************************************************
 */
RwChar
psPathGetSeparator(void)
{
#ifdef CDROM
    return '\\';
#else
    return '/';
#endif /* CDROM */
}

/*
 *****************************************************************************
 */
void
skyOverideIOPPath(char *newPath)
{
    /*
     * Stealth function to overide IOP paths, I use strdup
     * and free in case this is called before RenderWare is
     * initialised
     */
    if (SkyState.iopPathOverride)
    {
        free(SkyState.iopPathOverride);
    }

    if (newPath)
    {
        SkyState.iopPathOverride = strdup(newPath);
    }
    else
    {
        SkyState.iopPathOverride = 0;
    }

    return;
}

/*
 *****************************************************************************
 */
RwBool
psAlwaysOnTop(RwBool AlwaysOnTop __RWUNUSED__)
{
    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
psSelectDevice(RwBool useDefault)
{

	return TRUE;
}

/*
 *****************************************************************************
 */
RwMemoryFunctions*
psGetMemoryFunctions(void)
{
    /* Since memalign() is broken, we fake it here */

    /* We return a work around for malloc alignment on EB-2000S */
    /* One day, this may not be necessary */
    SkyState.bugWorkAround.rwmalloc = private_malloc;
    SkyState.bugWorkAround.rwfree = private_free;
    SkyState.bugWorkAround.rwrealloc = private_realloc; /* Hmm, this might need a fix too */
    SkyState.bugWorkAround.rwcalloc = private_calloc;

    /* Appears fixed in the 0909 flash so return NULL */
    /* return(&SkyState.bugWorkAround); */
    return 0;
}

/*
 *****************************************************************************
 */
RwBool
psInstallFileSystem(void)
{
    if (SkyInstallFileSystem(NULL))
    {
        RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__,
                           "sky file system installed");
        return TRUE;
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
psNativeTextureSupport(void)
{
    return TRUE;
}

/*
 *****************************************************************************
 */
void
psTerminate(void)
{
    return;
}

/*
 *****************************************************************************
 */
RwBool
psInitialize(void)
{
    return TRUE;
}





/*
 *****************************************************************************
 */

#define DEVICE (0)
#define MEDIA  (SCECdCD)			// PS2 media type, SCECdCD = CD-ROM, SCECdDVD = DVD-ROM

#include "file.h"
#include "pad.h"
#include "audio.h"

int
main(int argc, char *argv[])
{
	// Initialize communication with the IOP
	sceSifInitRpc(0);

	if (argc>1){
		gUserNumber = atoi(argv[1]);
	}

	// Reboot IOP and load IOP modules
	if (DEVICE == 0)
	{
		// Reboot IOP with image from host
		while (!sceSifRebootIop("host0:modules/ioprp243.img"));
		while (!sceSifSyncIop());
		sceSifInitRpc(0);
		sceFsReset();

		// Load modules from host
		while (sceSifLoadModule("host0:modules/sio2man.irx", 0, NULL) < 0);
		while (sceSifLoadModule("host0:modules/padman.irx",  0, NULL) < 0);
		while (sceSifLoadModule("host0:modules/libsd.irx",   0, NULL) < 0);
		while (sceSifLoadModule("host0:modules/sdrdrv.irx",  0, NULL) < 0);
		while (sceSifLoadModule("host0:modules/usbd.irx",    0, NULL) < 0);
		while (sceSifLoadModule("host0:modules/usbkb.irx",   0, NULL) < 0);

		// Set flag for ICE library
		_read_from_cd = 0;
	}
	else
	{
		// Initialize CD/DVD drive
		sceCdInit (SCECdINIT);
		sceCdMmode(MEDIA);

		// Reboot IOP with image from CD/DVD-ROM
		while (!sceSifRebootIop("cdrom0:\\MODULES\\IOPRP243.IMG;1"));
		while (!sceSifSyncIop());
		sceSifInitRpc(0);
		sceFsReset();

		// Re-initialize CD/DVD drive
		sceCdInit (SCECdINIT);
		sceCdMmode(MEDIA);

		// Load modules from CD/DVD-ROM
		while (sceSifLoadModule("cdrom0:\\MODULES\\SIO2MAN.IRX;1", 0, NULL) < 0);
		while (sceSifLoadModule("cdrom0:\\MODULES\\PADMAN.IRX;1",  0, NULL) < 0);
		while (sceSifLoadModule("cdrom0:\\MODULES\\LIBSD.IRX;1",   0, NULL) < 0);
		while (sceSifLoadModule("cdrom0:\\MODULES\\SDRDRV.IRX;1",  0, NULL) < 0);
		while (sceSifLoadModule("cdrom0:\\MODULES\\USBD.IRX;1",    0, NULL) < 0);
		while (sceSifLoadModule("cdrom0:\\MODULES\\USBKB.IRX;1",   0, NULL) < 0);

		// Set flag for ICE library
		_read_from_cd = 1;
	}

	// Initialize IOP memory allocation
    sceSifInitIopHeap();

    SkyState.skyTimerHandlerHid = -1;
    SkyState.keyboardHandle = -1;
    SkyState.iopPathOverride = 0;
    SkyState.elfDir[0] = '\0';
    SkyState.skyTimerHandlerHid = AddIntcHandler(INTC_TIM0, TimerHandler, 0);
    SkyState.sweHighCount = 0;
    *T0_COUNT = 0;
    *T0_COMP = 0;
    *T0_HOLD = 0;
    *T0_MODE = 0x281;
    EnableIntc(INTC_TIM0);

	Initialize();
	Initialize3D(NULL);
    RwRect              r;
    r.x = 0;
    r.y = 0;
    r.w = RsGlobal.maximumWidth;
    r.h = RsGlobal.maximumHeight;
	OnCameraSize(&r);

	RpSkySelectTrueTSClipper(TRUE);

//	file_init(DEVICE);
	pad_init(0, 0);
	audio_init(0);
	audio_loadsounds();

    while (1)
    {
		pad_update();
		audio_update();
		Idle();
    }

    return 0;
}


RwChar* psResourcePathCreate(const RwChar *srcBuffer)
{
	return (RwChar*) srcBuffer;
}



void psShowLoadingScreen(RwChar* message, RwChar* title)
{
}

void psCloseLoadingScreen()
{
}

BOOL psPlayerLoginScreen(void* loginInfo)
{
	char userToken[128];
	char passToken[128];

	RsLoginInfo *login = (RsLoginInfo *)loginInfo;

	if (gUserNumber!=-1){
		sprintf(userToken, "USERNAME%d", gUserNumber);
		sprintf(passToken, "PASSWORD%d", gUserNumber);
	}else{
		sprintf(userToken, "USERNAME");
		sprintf(passToken, "PASSWORD");
	}
  
	
	RwChar *username = CConfigReader::GetStringOnce("zbattle.cfg",userToken);
  RwChar *password = CConfigReader::GetStringOnce("zbattle.cfg",passToken);


  if (username){
		strcpy(login->name, username);
		delete [] username;
	}

	if (password){
		strcpy(login->pass, password);
		delete [] password;
	}
	return true;
}

BOOL psCharacterSelectScreen(void* charInfo)
{
}

BOOL psCharacterCreateScreen(void* charInfo)
{
 	char charToken[128];

	if (gUserNumber!=-1){
		sprintf(charToken, "CHARACTER%d", gUserNumber);
	}else{
		sprintf(charToken, "CHARACTER");
	}
 
	ZBMasterVehicleMsg *master = (ZBMasterVehicleMsg *)charInfo;

  RwChar *charname = CConfigReader::GetStringOnce("zbattle.cfg", charToken);

	if (charname){
		strcpy(master->props.name, charname);	 
		delete [] charname;
	}else{
		strcpy(master->props.name, "test player");	 
	}

	return true;
}