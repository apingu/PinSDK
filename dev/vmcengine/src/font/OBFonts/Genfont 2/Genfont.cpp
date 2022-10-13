// GenFont.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GenFont.h"
#include "GenFontDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif

/////////////////////////////////////////////////////////////////////////////
// CGenFontApp

BEGIN_MESSAGE_MAP(CGenFontApp, CWinApp)
    //{{AFX_MSG_MAP(CGenFontApp)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenFontApp construction

CGenFontApp::CGenFontApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGenFontApp object

CGenFontApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGenFontApp initialization

BOOL CGenFontApp::InitInstance()
{
    // Standard initialization

#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    CGenFontDlg dlg;

    m_pMainWnd = &dlg;
    int  nResponse = dlg.DoModal();

    if (nResponse == IDOK)
    {
    }
    else if (nResponse == IDCANCEL)
    {
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    // application, rather than start the application's message pump.
    return FALSE;
}
