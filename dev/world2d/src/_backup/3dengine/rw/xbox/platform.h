
/****************************************************************************
 *
 * platform.h
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

#ifndef PLATFORM_H
#define PLATFORM_H

#include "rwcore.h"
#include "vecfont.h"
#include <xbdm.h>

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern void psWindowSetText(const RwChar *text);
extern void psErrorMessage(const RwChar *text);
extern void psWarningMessage(const RwChar *text);
extern void psDebugMessageHandler(RwDebugType type,
                                  const RwChar *str);

extern UINT32 psTimer(void);

extern RwImage* psGrabScreen(RwCamera *camera);


extern RwChar *psPathnameCreate(const RwChar *srcBuffer);
extern void psPathnameDestroy(RwChar *buffer);
extern RwChar psPathGetSeparator(void);

extern BOOL psInitialize(void);
extern void   psTerminate(void);
extern BOOL psAlwaysOnTop( BOOL AlwaysOnTop );
extern void psCameraShowRaster(RwCamera *camera);
extern void psMouseSetVisibility(BOOL visible);
extern void psMouseSetPos(RwV2d *pos);

extern BOOL psSelectDevice(BOOL useDefault);

/* return memory function overload if required */
extern RwMemoryFunctions *psGetMemoryFunctions(void);

/* install the platform specific file system */
extern BOOL psInstallFileSystem(void);

/* Render platform specific metrics */
extern void psMetricsRender(struct RsVectorFont *vecFont,
                            RwV2d *pos,
                            RwMetrics *metrics);

/* Handle native texture support */
extern BOOL psNativeTextureSupport(void);

/*  A microsecond timer */
extern RwUInt64 psMicroTimer(void);

#if (defined(__R5900__))
extern void skyOverideIOPPath(char *newPath);
#endif /* (defined(__R5900__)) */

#ifdef RWSPLASH
extern BOOL   psDisplaySplashScreen(BOOL state);
#endif

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* PLATFORM_H */
