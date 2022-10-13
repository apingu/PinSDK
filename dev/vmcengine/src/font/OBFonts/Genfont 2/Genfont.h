// GenFont.h : main header file for the GENFONT application
//

#if !defined(AFX_GENFONT_H__2701BA94_8ABF_11D3_8376_0000E840A4CA__INCLUDED_)
#define AFX_GENFONT_H__2701BA94_8ABF_11D3_8376_0000E840A4CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGenFontApp:
//

class CGenFontApp : public CWinApp
{
public:
	CGenFontApp();

// Overrides
	//{{AFX_VIRTUAL(CGenFontApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGenFontApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_GENFONT_H__2701BA94_8ABF_11D3_8376_0000E840A4CA__INCLUDED_)
