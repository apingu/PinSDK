// MyApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MyApp.h"

#include "MainFrm.h"
#include "ChildFrm.h"

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
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif


    CMDIFrameWnd* pFrame = new CMainFrame;
    ASSERT(pFrame);
    
    m_pMainWnd = pFrame;

    // create main MDI frame window
    if (!pFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;

    pFrame->ShowWindow(m_nCmdShow);
    pFrame->UpdateWindow();

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMyApp message handlers

int CMyApp::ExitInstance() 
{
    return CWinApp::ExitInstance();
}

void CMyApp::OnAppAbout()
{
    CDialog(IDD_ABOUTBOX).DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMyApp message handlers

