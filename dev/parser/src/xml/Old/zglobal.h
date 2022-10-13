/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: ZBGlobal.h
 * Original Author: Mark Harmon
 ********************************************************************/
#ifndef ZBGLOBAL_H
#define ZBGLOBAL_H

#include <stdio.h>
//#ifndef UNIX /* #ifdef WIN32 */
#if defined(_WINDOWS)
#include <tchar.h>
#else
/*typedef wchar_t     TCHAR;*/
typedef char     TCHAR;
typedef unsigned long DWORD;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#endif
//#include <wchar.h>

/** The maximum number of characters for player's name */
enum{MAX_PLAYERNAMELENGTH=32};


#define Clamp(x, min, max)  x = (x<min  ? min : x<max ? x : max);

#if !defined(UINT16)
typedef short           INT16;
typedef unsigned short  UINT16;
typedef int             INT32;
typedef unsigned int    UINT32;
#ifndef UNIX /* #ifdef WIN32 */
typedef int             BOOL;
#else
typedef char            BOOL;
#endif

#define MAX_PATH        260
#endif

#if !defined(INT8)
typedef signed char     INT8;
typedef unsigned char   UINT8;
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

extern int atow(char *strA, wchar_t *strW, int lenW);
extern int wtoa(wchar_t *strW, char *strA, int lenA);


#endif