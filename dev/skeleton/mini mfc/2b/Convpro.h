// Convpro.h : main header file for the CONVPRO application
//

#if !defined(AFX_CONVPRO_H__9D04E376_6CF8_11D3_B2F4_0000E840A4CA__INCLUDED_)
#define AFX_CONVPRO_H__9D04E376_6CF8_11D3_B2F4_0000E840A4CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CConvproApp:
// See Convpro.cpp for the implementation of this class
//

class CConvproApp : public CWinApp
{
public:
	CConvproApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConvproApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CConvproApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVPRO_H__9D04E376_6CF8_11D3_B2F4_0000E840A4CA__INCLUDED_)
