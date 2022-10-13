/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: ps2.cpp
 * Original Author: 
 ********************************************************************/

#include "rwcore.h"

#include "ZGlobal.h"


// to be implemented


//void psPathnameDestroy(RwChar *buffer)
//{
//  if(buffer) RwFree(buffer);
//}

//void psErrorMessage(const RwChar *message)
//{
//}



//void psWarningMessage(const RwChar *message)
//{
//}


//RwChar psPathGetSeparator(void)
//{
//  return '\\';
//}


//UINT32 psTimer(void)
//{
//  UINT32 time = 0;
//
//	// fixme
//  return time;
//}

//BOOL psInstallFileSystem(void)
//{
//
//  return TRUE;
//}


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
}

BOOL psCharacterSelectScreen(void* charInfo)
{
}

BOOL psCharacterCreateScreen(void* charInfo)
{
}


