// MfcDialog.h : main header file for the MFCDIALOG application
//

#if !defined(AFX_MFCDIALOG_H__F1B0134E_00F5_4473_B94F_8F2086EE7FCE__INCLUDED_)
#define AFX_MFCDIALOG_H__F1B0134E_00F5_4473_B94F_8F2086EE7FCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfcDialogApp:
// See MfcDialog.cpp for the implementation of this class
//

class CMfcDialogApp : public CWinApp
{
public:
	CMfcDialogApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcDialogApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMfcDialogApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCDIALOG_H__F1B0134E_00F5_4473_B94F_8F2086EE7FCE__INCLUDED_)
