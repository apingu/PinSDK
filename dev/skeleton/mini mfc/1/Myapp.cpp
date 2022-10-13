
// MyApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MyApp.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyApp

BEGIN_MESSAGE_MAP(CMyApp, CWinApp)
	//{{AFX_MSG_MAP(CMyApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyApp construction

CMyApp::CMyApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMyApp object

CMyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMyApp initialization

BOOL CMyApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	CMainFrame* pMainFrame = new CMainFrame;
	ASSERT(pMainFrame);

//  if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
//      return FALSE;
    if (!pMainFrame->Create())
		return FALSE;

	pMainFrame->Initialize();

	pMainFrame->ShowWindow(SW_SHOW);
	pMainFrame->UpdateWindow();

	m_pMainWnd = pMainFrame;

	return TRUE;
}

// App command to run the dialog
void CMyApp::OnAppAbout()
{
	CDialog(IDD_ABOUTBOX).DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMyApp commands
