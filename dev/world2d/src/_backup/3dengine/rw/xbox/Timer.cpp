/*********************************************************************
 * Copyright (c) 2001, Mark Harmon All Rights Reserved
 *
 * Filename: Timer.cpp
 * Original Author: Mark Harmon
 ********************************************************************/

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

//#include <mmsystem.h>
#include <DSound.h>
#include "Timer.h"

// constructor
Timer::Timer (void)
{
	m_nIDTimer = NULL;
}


// Destructor
Timer::~Timer (void)
{
	if(m_nIDTimer) timeKillEvent(m_nIDTimer);
}


// Create
BOOL Timer::Create (UINT nPeriod, UINT nRes, DWORD dwUser, TIMERCALLBACK pfnCallback)
{
	BOOL bRtn = TRUE;    // assume success

	if(!pfnCallback || (nPeriod <= 10) || (nPeriod < nRes)) return FALSE;

	m_nPeriod = nPeriod;
	m_nRes = nRes;
	m_dwUser = dwUser;
	m_pfnCallback = pfnCallback;

	m_nIDTimer = timeSetEvent(m_nPeriod, m_nRes, TimeProc, (DWORD) this, TIME_PERIODIC);

	if(m_nIDTimer == NULL) bRtn = FALSE;

	return (bRtn);
}


// Timer proc for multimedia timer callback set with timeSetTime().
//
// Calls procedure specified when Timer object was created. The 
// dwUser parameter contains "this" pointer for associated Timer object.
// 
void CALLBACK Timer::TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// dwUser contains ptr to Timer object
	Timer * ptimer = (Timer *) dwUser;

	// Call user-specified callback and pass back user specified data
	(ptimer->m_pfnCallback) (ptimer->m_dwUser);
}
